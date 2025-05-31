//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TEtherCATRT.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   TwinCat2
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#pragma hdrstop

#include <math.h>

#include "TEtherCATRT.h"
#include "TemplateLogs.h"
#include "TEventDispatcher.h"
#include "TTimePosition.h"
#include "TExpGroup.h"
#include "TDataExt.h"
#include "defines.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

// Evitar los problemas de llamadas desde diferentes threads
//#include "critical.h"
//TCriSection tcs;

//-----------------------------------------------------------------------------
// Datos para la callback
TEtherCATRT      *ethCAT              = NULL;

// Callback con la ejecución de la tarea de IO de control de válvulas, trigger y stroBe
// La llamamos a cada tick ( 250 us)
// ToDo: En lugar de una callback deberíamos usar un evento. De esa forma
// evitaríamos problemas de multiples threads, ya que tocando estos TDatas, están
// el thread del módulo y el creado para el temporizador
void CALLBACK TEtherCATRT::TimerProc( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {


   if( ethCAT /*&& ethCAT->Status() >= ready*/)                               {
      if( uID == ethCAT->idTimerUpdate)                                       {

         // actualizar configuración en EtherCAT
         ethCAT->SetCfg();

         // contemplar el caso de time < ethCAT->m_lastUpdate, que quire decir que el contador ha dado la vuelta
         unsigned int _time = timeGetTime();
         if (  _time < ethCAT->m_lastUpdate  || (_time - ethCAT->m_lastUpdate) > 1000) {
            ethCAT->m_lastUpdate = timeGetTime();  // se pone en Update, pero como esto entra tan rápido...
            //si hace mas de un segundo que no se ejecuta el Update
            ethCAT->Update();
         }
      }
   }
}
//------------------------------------------------------------------------------

// Constructor
//
TEtherCATRT::TEtherCATRT( wchar_t *_name, TProcess *parent)
      :  TIORealTime(_name, parent),
         TEtherCAT( this)                                                    {

  QueryPerformanceFrequency( &m_freq);

   m_initializing = false;
   m_reading      = false;
   // Puntero a la dirección de EtherCAT ( para la callback)
   ethCAT      = this;

   // Configuración de la tarea de IO
   m_IOConnected          = false;
   idTimerUpdate          = NULL;

   // Enganchamos a la notificacion los parámetros que no cuelgan de ninguna manager hijo
   DataModEventPost    = DataModifiedPost;


   //---- TDatas no cuelgan de ningun manager hijo
   new TData( this, L"TraceFile2",              &m_traceFile,             0,     0,    1,    NULL,  (FLAG_LEVEL_3 | FLAG_SAVE_NO_VALUE | FLAG_NOTIFY ), L"for test purposes");
   new TData( this, L"Unlock trigger",          &m_unlock,                1,     0,    1,    NULL,  (FLAG_LEVEL_0 | FLAG_SAVE |  FLAG_NOTIFY), L"Advance index trigger when no space left in buffer");
   new TData( this, L"TimeSync",                &m_timeSyncMs,            5000,  500,  30000,NULL,  (FLAG_LEVEL_0 | FLAG_SAVE |  FLAG_NOTIFY), L"Time in ms between syncronization procedures");
   new TData( this, L"RepetFailEjection",       &m_maxEjectFail,          1,     1,    10,   NULL,  (FLAG_LEVEL_0 | FLAG_SAVE |  FLAG_NOTIFY), L"Number of ejection fails in the MyPeriodic periode to show the Fualty");
   new TData( this, L"RepetFailTrigger",        &m_maxTriggerError,       10,    1,    1000, NULL,  (FLAG_LEVEL_0 | FLAG_SAVE |  FLAG_NOTIFY), L"Number of trigger error in the MyPeriodic periode to show the Fualty");

   new TData( this, L"RestartTwincatIfError",   &m_restartTwincatError,   0,     0,    1,    NULL,   FLAG_NORMAL, L"If there are a error with cominication, do a system restart");

   pEnableEjection      = new TData( this, L"EnableEjection",  &m_enableEjection,  1, 0, 1,     NULL, (FLAG_LEVEL_3 | FLAG_SAVE  | FLAG_NOTIFY), L"For testing purposes");


   new TDataByte( this, L"Status",                 &eth.status,               0, 0, 3,                (FLAG_LEVEL_0 | FLAG_SAVE_NO_VALUE | FLAG_RDONLY | FLAG_NOTIFY));
   new TDataByte( this, L"Mode",                   &eth.mode,                 0, L"Auto=0;Manual=1",  FLAG_NORMAL);
   new TDataByte( this, L"Divisor EndTrigStrob",   &eth.divisorEncTrigStrob,  1, 1, 10,               FLAG_NORMAL, L"Divisor para atender el encoder, el triger y el strobe. 1 -> 250 us, 2 -> 500 us,...");
   new TDataByte( this, L"Divisor Expulsión",      &eth.divisorExp,           1, 1, 10,               FLAG_NORMAL, L"Divisor para atender el buffer de PutExp y las órdenes. 1-> 250 us, 2 -> 500 us,....");
   new TDataWord( this, L"Numero Terminales",      &eth.numTerminales,        0, 0, 9999,             (FLAG_LEVEL_0 | FLAG_SAVE_NO_VALUE | FLAG_RDONLY | FLAG_NOTIFY));
   new TDataWord( this, L"Fallo Terminales",       &eth.falloTerminales,      0, 0, 9999,             (FLAG_LEVEL_0 | FLAG_SAVE_NO_VALUE | FLAG_RDONLY | FLAG_NOTIFY));
   new TDataByte( this, L"Master OK",              &eth.masterOk,  1,                                 (FLAG_LEVEL_0 | FLAG_SAVE_NO_VALUE | FLAG_RDONLY | FLAG_NOTIFY));

   //---- TDatas del encoder
   TDataManager *mngrBaseTime = new TDataManager(L"BaseTime", this);
   mngrBaseTime->DataModEventPost          = DataModifiedPost;
   new TDataByte(  mngrBaseTime, L"Origen Pulsos",      &eth.baseTime.cfg.Sync,              0, L"ENCODER 1012=0;TIMER=1;ENCODER 5151=2", FLAG_NORMAL);
   new TDataInt64( mngrBaseTime, L"Contador",           &eth.baseTime.data.counter.QuadPart,                                              (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE ));
   new TDataInt64( mngrBaseTime, L"Timer 100ns",        &eth.baseTime.data.time100ns.QuadPart,                                            (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE));
   new TDataDWord( mngrBaseTime, L"Timer",              &eth.baseTime.data.timer,            0, 0, 0xFFFFFFFF,                            (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE ));
   new TDataInt64( mngrBaseTime, L"Last ReSync",        &eth.baseTime.data.lastReSync.QuadPart,                                           (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE) , L"Valor de counter en el último instante de resincronización");
   new TDataDWord( mngrBaseTime, L"Timer Last ReSync",  &eth.baseTime.data.timerLastReSync,  0, 0, 0xFFFFFFFF,                            (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE ), L"Valor de timer en el último instante de resincronización");


   //---- TDatas de trigger
   mngrTrigger = new TDataManager( L"Trigger" , this);
   pTrigToConfig      = new TData( mngrTrigger, L"Trigger to config",     &m_trigToConfig,  1, 1, NUM_TRIG ,   NULL,  FLAG_NORMAL & ~FLAG_SAVE, L"trigger number to show in interface");

   mngrTrigger->DataModEventPost    = DataModifiedPost;

   new TDataByte( mngrTrigger, L"Trigger Enabled",    &trigCfg.enabled,          0,                         (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE ));  //en otra variable diferente para poder hacer el MyGo y MyStop correctamente
   new TDataByte( mngrTrigger, L"Resincronizar con",  &trigCfg.reSync,           0, L"Nada=0;Base de tiempos=1;Fotocélula 1=2;Fotocélula 2=3;Fotocélula 3=4;Fotocélula 4=5;Fotocélula 5=6;Fotocélula 6=7;Fotocélula 7=8;Fotocélula 8=9",  (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE));
   new TData(     mngrTrigger, L"Cycle Time",         (int*)&trigCfg.cycleTime,  0, 0, 10000000, L"%.2f",   (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE),  L"Tiempo en ms entre disparos de trigger en modo autómatico");
   new TDataByte( mngrTrigger, L"Triger Time",        &trigCfg.triggerTime,      1, 1, 255,                 (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE),  L"Tiempo en ms de duración de la señal de trigger (independiente de la de strobe)");
   new TDataByte( mngrTrigger, L"OffsetPulse",        &trigCfg.offsetPulse,      0, 0, 255,                 (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE),  L"Offset en pulsos respecto al instante de resincronización");
   new TDataByte( mngrTrigger, L"OffsetTime",         &trigCfg.offsetTime,       0, 0, 255,                 (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE),  L"Offset en tiempo para adelantar respecto a la señal de trigger");
   new TDataByte( mngrTrigger, L"Invert",             &trigCfg.invert,           0,                         (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE));
   new TDataByte( mngrTrigger, L"Control Enabled",    &trigCfg.ctrlEnabled,      0,                         (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE));
   new TDataByte( mngrTrigger, L"Put Trigger",        &trigCfg.putTrigger,       0,                         (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE));
   new TDataInt64(mngrTrigger, L"Next",               &trigData.next.QuadPart,                              (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
   new TDataInt64(mngrTrigger, L"Last",               &trigData.last.QuadPart,                              (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
   new TDataDWord(mngrTrigger, L"TotalCyles",         &trigData.totalCycles,     0, 0, 0xFFFFFFFF,          (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE), L"Disparos totales de este trigger");
   new TDataDWord(mngrTrigger, L"Cyles",              &trigData.totalCycles,     0, 0, 0xFFFFFFFF,          (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE), L"Disparos desde la última resincronización");


  //-- Buffer con últimos sincronismos
   TDataManager *mngrTrigCtrl   = new TDataManager( L"Control Trigger", mngrTrigger, FLAG_NORMAL);
   mngrTrigCtrl->DataModEventPost   = DataModifiedPost;

   TDataManager *mngrLastSinc   = new TDataManager( L"Last Sinc",   mngrTrigCtrl);
   mngrLastSinc->DataModEventPost   = DataModifiedPost;

   TDataManager *mngrLastStrob  = new TDataManager( L"Last Strobe", mngrTrigCtrl);
   mngrLastStrob->DataModEventPost  = DataModifiedPost;

   TDataManager *mngrLastTime   = new TDataManager( L"Last Time",   mngrTrigCtrl);
   mngrLastTime->DataModEventPost   = DataModifiedPost;

   TDataManager *mngrFrameCount = new TDataManager( L"Frame Count", mngrTrigCtrl);
   mngrFrameCount->DataModEventPost  = DataModifiedPost;

   new TDataByte( mngrTrigCtrl, L"Indice read",    &trigCfg.ctrl_i_r,   0, 0, NTRIG-1, (FLAG_LEVEL_0 | /*FLAG_RDONLY  | */FLAG_SAVE_NO_VALUE));
   new TDataByte( mngrTrigCtrl, L"Indice write",   &trigData.ctrl_i_w,  0, 0, NTRIG-1, (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
   for( int j = 0; j < NTRIG; j++)                             {
      new TDataInt64(   mngrLastSinc,     String(String("LastSinc")     + String(j+1)).c_str(),  &trigData.lastSinc[j].QuadPart,                (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
      new TDataInt64(   mngrLastTime,     String(String("LastTime HW")  + String(j+1)).c_str(),  &trigData.lastTime[j].QuadPart,                (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
      new TDataByte(    mngrLastStrob,    String(String("LastStrobe")   + String(j+1)).c_str(),  &trigData.lastStrobe[j],     0, 0, 255,        (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
      new TDataDWord(   mngrFrameCount,   String(String("FrameCount")   + String(j+1)).c_str(),  &trigData.frameCount[j],     0, 0, 0xFFFFFFFF, (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
   }

   pTrigConfiguration = new TData ( mngrTrigger, L"Trigger Configuration",  (void*)  &eth.trig.cfg,  sizeof( eth.trig.cfg) ,  (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE ),   L"binary data with trigger configuration", NULL, false);
   for ( int i=0; i< NUM_TRIG; i++)                                         {
      m_mutexIR[i]   = CreateMutex( NULL, false, NULL);
      eth.trig.cfg[i].Init()  ; //default values for trigger configuration, becuase constructor of TData overrides with 0x00 in previous sentence
  }

   //---- TDatas de los Strobe

   mngrStrobe = new TDataManager( L"Strobe", this);
   mngrStrobe->DataModEventPost     = DataModifiedPost;

   pStrobeToConfig         = new TData( mngrStrobe, L"Strobe to config",         &m_strobToConfig,  1, 1, NUM_STROB ,   NULL,  FLAG_NORMAL & ~FLAG_SAVE, L"strobe number to show in interface");

   new TDataByte( mngrStrobe, L"enabled",                &strobCfg.enabled,      0,                (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE));
   new TDataByte( mngrStrobe, L"Trigger",                &strobCfg.trigger,      1, 1, NUM_TRIG,   (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE), L"Trigger al que vamos asociados");
   new TDataByte( mngrStrobe, L"OffsetTime",             &strobCfg.offsetTime,   0, 0, 255,        (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE), L"Offset en tiempo del strobe para adelantar la señal de strobe respecto al trigger");
   new TDataByte( mngrStrobe, L"Time",                   &strobCfg.strobeTime,   1, 1, 20,         (FLAG_LEVEL_0 | FLAG_NOTIFY  | FLAG_SAVE_NO_VALUE), L"Duración del strobe (en función del divisor)");
   new TDataByte( mngrStrobe, L"Tamaño secuencia",       &strobCfg.tamSec,       1, 1, 8,          (FLAG_LEVEL_0 | FLAG_NOTIFY  | FLAG_SAVE_NO_VALUE), L"Número de bits válidas habilitados en la secuencia");
   pStrobSequence = new TDataArrayOfBytes<TAM_SEC>( 1,   mngrStrobe, L"Secuencia", strobCfg.sequence, (FLAG_LEVEL_0 | FLAG_NOTIFY  | FLAG_SAVE_NO_VALUE),    L"Secuencia binaria de cuando se dispara o no este strobe");


   new TDataByte(  mngrStrobe, L"Tipo strobe",           &strobCfg.strobeType,   0, L"Visible=0;NIR=1;Rayos X=2;No predefinido(3)=3;No predefinido(4)=4;No predefinido(5)=5;No predefinido(6)=6;No predefinido(7)=7;No predefinido(8)=8;",  (FLAG_LEVEL_0 | FLAG_NOTIFY  | FLAG_SAVE_NO_VALUE));
   new TDataByte(  mngrStrobe, L"Resinc. secuencia con", &strobCfg.reSync,       0, L"Nada=0;Base de tiempos=1;Fotocélula 1=2;Fotocélula 2=3;Fotocélula 3=4;Fotocélula 4=5;Fotocélula 5=6,Fotocélula 6=7,Fotocélula 7=8,Fotocélula 8=9",  (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE));
   new TDataInt64( mngrStrobe, L"Next",                  &strobData.next.QuadPart,                 (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
   new TDataInt64( mngrStrobe, L"Last",                  &strobData.last.QuadPart,                 (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
   new TDataInt64( mngrStrobe, L"LastNextTrig",          &strobData.lastNextTrig.QuadPart,         (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
   new TDataByte(  mngrStrobe, L"SeqIndex",              &strobData.seqIndex,    0, 0, 8,          (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));

   pStrobeConfiguration    = new TData ( mngrStrobe, L"Strobe Configuration",  (void*)  &eth.strob.cfg,  sizeof( eth.strob.cfg) ,  (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE ),   L"binary data with strobe configuration", NULL, false);
   for ( int i=0; i<NUM_STROB; i++)
      eth.strob.cfg[i].Init(); //initialize strobe configuration with default data, becuase constructor of TData overrides with 0x00 in previous sentence

   //---- Buffer para poner expulsiones
   TDataManager *mngrPutExp  = new TDataManager( L"PutExp", this, FLAG_NORMAL);
   mngrPutExp->DataModEventPost           = DataModifiedPost;

   new TDataByte( mngrPutExp, L"Indice read",    &eth.putExp.data.i_r,  0, 0, MAX_EXP-1, (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
   new TDataByte( mngrPutExp, L"Indice write",   &eth.putExp.cfg.i_w,   0, 0, MAX_EXP-1, (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));


   if(!m_diagnoseMngr)
      m_diagnoseMngr = new TDataManager( LBL_DIAG_STATUS, this);
   // Estos TDatas indicarán el estado normal del funcionamiento de la máquina
   // Comunes a todos los driver
   new TData( m_diagnoseMngr,   L"DriverInstalado",         &m_drvInstalled,     1,      0,    1,          NULL,    FLAG_NORMAL2);
   new TData( m_diagnoseMngr,   L"TriggerHabilitado",       &m_trigHabil,        1,      0,    1,          NULL,    FLAG_NORMAL2);
   new TData( m_diagnoseMngr,   L"OrigenPulsos",            &m_origPulse,        0,      L"Encoder;Timer",          FLAG_NORMAL2);
   new TData( m_diagnoseMngr,   L"ModoExpulsion",           &m_modoExpul,        1,      L"Manual;Automatico",      FLAG_NORMAL2);
   new TData( m_diagnoseMngr,   L"MinimoPulsosEncoder",     &m_minPulsEnc,       1400,   1,    100000,     NULL,    FLAG_NORMAL2);
   new TData( m_diagnoseMngr,   L"MaximoPulsosEncoder",     &m_maxPulsEnc,       2300,   1,    100000,     NULL,    FLAG_NORMAL2);
   new TData( m_diagnoseMngr,   L"MinimoPulsosTimer",       &m_minPulsEnc,       2000,   1,    100000,     NULL,    FLAG_NORMAL2);
   new TData( m_diagnoseMngr,   L"MaximoPulsosTimer",       &m_maxPulsEnc,       5000,   1,    100000,     NULL,    FLAG_NORMAL2);
   new TData( m_diagnoseMngr,   L"NúmeroTerminales",        &m_numTerminales,    16,     0,    9999,       NULL,    FLAG_NORMAL2);
   new TData( m_diagnoseMngr,   L"VerificarVueltaCompleta", &m_vueltaCompleta,   1,      0,    1,          NULL,    FLAG_NORMAL2);
   new TData( m_diagnoseMngr,   L"TriggerCicleTime",        &m_triggerCicleTime, 500,    0,    10000,      NULL,    FLAG_NORMAL2);
   new TData( m_diagnoseMngr,   L"Pulsos por Vuelta",       &m_pulsosPorVuelta,  4096,   1,    9999,       NULL,    FLAG_NORMAL2);



   TDataManager   *bufferExpMngr = new TDataManager( L"BufferedEjection", this);
   new TData( bufferExpMngr, L"EnableBuffered",           &m_enableBufferedExp,  0,        0,       1,       NULL,    FLAG_NORMAL2, L"Enable buffered sending of ejection orders");
   new TData( bufferExpMngr, L"PLCbufferSize",            &m_PLCBufferSize,      1000,   100,  100000,   L"0x%x",    FLAG_NORMAL2, L"Ejection buffer size in PLC");
   new TData( bufferExpMngr, L"CheckEjectionPeriode",     &m_ejectionPeriode,      10,   10,  1000,       NULL,    FLAG_NORMAL2, L"( in ms) Periode for checking ejection pending list");

   QueryPerformanceFrequency( &m_freq);

   //Arracancamos
   IOOpen();
   if ( !IsRunning())                                                         {
      Sleep(500); // Esperamos un tiempo por si no esta arrancado el Twincat
      IOOpen( );  // Intentamos reconectar
   }


//   if ( !IsRunning())                                                         {
//      TFormWaitDrv *DlgWaitDriver = new TFormWaitDrv( NULL, this);
//      if( DlgWaitDriver->ShowModal() == mrOk)
//         // Tras la espera, si NO se ha pulsado cancelar, intentamos reconectar
//         IOOpen( );
//      delete DlgWaitDriver;
//   }



   m_mutex2                   = CreateMutex( NULL, false, NULL);

   DataModEventPost           = DataModifiedPost;
   m_priority                 = 0; //for MyInit to be called in correct sequence
   m_updating                 = false;
   m_dispatchControlEnabled   = true;

   m_offset1970               = 0;
   m_SyncLastTime             = 0;
   m_SyncDeriva               = 0;                 // deriva de temporizadores

   m_protect                  = false;
   m_lastUpdate               = 0;
   m_lastSyncTime             = 0;
   m_lastCheckPending         = 0;
   m_lastFaultyTime           = 0;
   m_lastLatePutExp           = 0;
   m_counterSyncTooLate       = 0;
   m_lastNoRoomPutExp         = 0;
   m_numRepEjectFail          = 0;
   m_numRepTriggerError       = 0;

   m_lastSystemTime           = 0;
   m_lastEthTime              = 0;
   m_validCount               = 0;

}

void TEtherCATRT::IOOpen()                                                    {

   IOOpenParam( DRIVER_ETHERCAT_PORTNUMBER, (void **)&pOutputs, sizeof(Driver_EtherCAT_Outputs), (void **)&pInputs, sizeof(Driver_EtherCAT_Inputs));
}

//------------------------------------------------------------------------------

// Destructor
//
TEtherCATRT::~TEtherCATRT()                                           {


   KillTimers();
   ::Sleep( 200);
   m_IOConnected = false;

   for( int j = 0; j < NUM_TRIG; j++)
      CloseHandle(  m_mutexIR[j]);


}


//------------------------------------------------------------------------------

/**
   PutExp @see TIORealTime::PutExp

*/
int  TEtherCATRT::PutExp( S_EXP *exp, int num, bool ejectImmediatly)          {

   if ( num==0)
      return 0;

   if ( !m_IOConnected)                                                       {
      return 0;      //no despachamos evento porque ya deberia haber un faulty
   }

   if ( !m_enableEjection)                                                    {
      return 0;
   }


   bool  late = false,
		 noroom = false;

   Update();


   if ( !m_enableBufferedExp || ejectImmediatly)                              {
      //si no está habilitada la buffered ejection, entonces tratar de enviar todos
	  //al PLC. NO hay que bloquear el mutex
	  int cont = MyPutExp( exp, num, late, noroom);
	  MyPostPutExp( cont, late, noroom);
      return cont;
   }
   //primero revisar la lista, antes de intentar enviar los que nos acaban de llegar
   int dispatched =  CheckEjectionList( late, noroom);
   //Si está habilitada la buffered ejection, entonces primero hay que
   //limpiar el array que nos han pasado, borrando del mismo
   //los que se han de guardar en la lista, porque su instante de expulsion está tan lejano
   //en el tiempo que no cabe en el buffer
   S_EXP *pExp = exp,
         *pEnd = exp + num;

//         #ifdef _DEBUG
//         FILE *fitx = fopen( "ejection.txt","a");
//         fprintf( fitx, "PutExp: num=%d,\n", num);
//         #endif

   while ( pExp != pEnd )                                                     {
	  __int64 diff =  pExp->syncCount + pExp->delay  - eth.baseTime.data.counter.QuadPart;
//			#ifdef _DEBUG
//            fprintf( fitx, "syncCount + delay =%Ld, eth.baseTime.data.counter.QuadPart=%Ld, diff = %Ld\n",
//                               pExp->syncCount + pExp->delay,  eth.baseTime.data.counter.QuadPart, diff);
//			#endif
      if ( diff <= 0 || diff  >= (__int64) m_PLCBufferSize)                   {
         if ( diff <=0)                                                       {
            //demasiado tarde
            late = true;

//            #ifdef _DEBUG
//            fprintf( fitx, "late\n");
//            #endif
         }
         else                                                                 {

//            #ifdef _DEBUG
//            fprintf( fitx, "diff  >= m_PLCBufferSize\n");
//            #endif

            //No se puede despachar todavia
            //guardar en la lista, los mas recientes al final
            m_listExp.push_back( *pExp);
         }
         //borrar del array
         if ( pEnd != pExp -1)                                                {
            //cuidado, no utilizar un memcpy
            memmove( pExp, pExp +1, (pEnd - pExp -1) * sizeof( S_EXP));
         }
         pEnd -= 1;
         //decrementar contador de elementos en el array
         num--;
      }
      else
         pExp++;
   }

//         #ifdef _DEBUG
//         fprintf( fitx, "num=%d\n", num);
//         fclose( fitx);
//         #endif


   //ya hemos purgado el array. Enviar lo que queda en el mismo al PLC
   if ( num >0)
      dispatched +=  MyPutExp( exp, num, late, noroom);
   //si ha habido problemas, notificar
   MyPostPutExp( dispatched, late, noroom);

   return  dispatched;
}

//------------------------------------------------------------------------------
/**
   CheckEjectionList
   Este metodo chequea la lista de expulsiones pendientes y envia al PLC aquellas
   que caben en el bufer temporal del PLC
   @return el numero de expulsiones que se han podido pasar al PLC
*/
int TEtherCATRT::CheckEjectionList( bool &late, bool &noroom)                 {

   int cont = 0;

   if ( m_listExp.empty())
      return 0;

   int num = 0; //numero de expulsiones que se escriben en el array de expulsiones a pasar al PLC

         //tomar puntero para recorrer el array
         S_EXP *pExp = m_ejectionArray;
         //calcular el espacio que nos queda en el buffer de intercambio, para no extraer de la lista más de lo que cabe
         int room;
         if ( eth.putExp.data.i_r > eth.putExp.cfg.i_w)
            room = eth.putExp.data.i_r - eth.putExp.cfg.i_w;
         else
            room = eth.putExp.data.i_r + MAX_EXP - eth.putExp.cfg.i_w;

         //recorreremos la lista, de los mas antiguos a los mas recientes
         std::list<S_EXP>::iterator it;
         it = m_listExp.begin();
         while ( it != m_listExp.end() )                                      {

            __int64 diff =  ( *it).syncCount + (*it).delay  - eth.baseTime.data.counter.QuadPart;

            if ( diff  >= (__int64) m_PLCBufferSize)
               break;
            if ( diff < 0)                                                     {
               //si la expulsion es tardia, indicarlo y no pasarla al array
               late = true;
            }
            else                                                              {
               //poner la expulsion en el buffer de expulsiones
               *pExp = *it;
               pExp++;
               cont++;
               //comprobar si ya hemos llenado demasiadas, teniendo en cuenta el tamaño del area de intercambio
               if ( cont >= room)                                             {
                  //si no hay mas espacio, indicarlo y parar de procesar la lista
                  noroom = true;
                  break;
               }
            }
            //borrar el elemento de la lista avanzando el iterador
            it = m_listExp.erase( it);
         }

         //no llamamos a PutExp, que vuelve a hacer un update, sino a MyPutExp
         if ( cont)
            num = MyPutExp( m_ejectionArray, cont, late, noroom);

   //si no hemos tenido ningun problema con noroom, entonces actualizar el tiempo
   //si hemos tenido problemas de espacio, entonces NO lo actualizamos, para que
   //se reintente cuanto antes
   if ( noroom==false)
      m_lastCheckPending = timeGetTime();
   return num;
}

//------------------------------------------------------------------------------

/**
   MyPutExp
   Este metodo escribe las expulsiones que se le pasan en un array en el buffer de intercambio con el PLC
   @param exp es el array con las ordenes de expulsion
   @param num es la cantidad de ordenes de expulsion en el array
   @param late es un parametro de salida, que se establece a true si alguna de esas ordenes de expulsion ha superado el contador de tiempo del PLC
   @param noroom es un parametro de salida, que se establece a true si alguna de las ordenes de expulsion no se ha podido
   enviar al PLC por falta de espacio en el buffer de intercambio de expulsiones ( con el PLC)
   @return el numero de expulsiones que ha podido escribir en el buffer de intercambio, que serán las que se le han pasado
   en el array menos las tardias y las que no han podido escribirse por falta de espacio en el buffer de intercambio
*/
int TEtherCATRT::MyPutExp( S_EXP *exp, int num, bool &late,  bool &noroom)    {

   if ( !m_IOConnected)
      return 0;      //no despachamos evento porque ya deberia haber un faulty

   int contador;

   // Añadimos una expulsión al buffer de putExp
   for( int i = 0; i < num; ++i)                                              {

      if (  eth.mode == MODE_AUTO               &&
            eth.baseTime.data.counter.QuadPart  &&
            exp[i].syncCount + exp[i].delay <= eth.baseTime.data.counter.QuadPart){

         late = true;
         continue;
      }

      // Comprobamos si hay sitio
      if (  (eth.putExp.cfg.i_w < MAX_EXP-1   && eth.putExp.cfg.i_w + 1!= eth.putExp.data.i_r) ||
			(eth.putExp.cfg.i_w == MAX_EXP-1  && eth.putExp.data.i_r != 0))   {

// Don't use, because if is disabled the counters are incremented. ( CyclesValves.txt)
//         if ( !m_enableEjection)
//            exp[i].state = 0;

		 memcpy( &eth.putExp.cfg.buffer[eth.putExp.cfg.i_w], &exp[i], sizeof( exp[i]));

         if( ++eth.putExp.cfg.i_w >= MAX_EXP)
            eth.putExp.cfg.i_w   = 0;

         contador++;
      }
      else                                                                    {
         noroom = true;
         break;
      }
   }

   return contador;
}

//------------------------------------------------------------------------------
/*
   MyPostPutExp
   Codigo a ejecutar despues de escribir ordenes de expulsion en el buffer de intercambio con el PLC
   @param int es el numero de ordenes de expulsion que se han escrito en el buffer de intercambio
   @param late indica si alguna orden de expulsion no se ha escrito por ser tardia
   @param noroom indica si alguna orden de expulsion no se ha escrito por no haber espacio en el area de intercambio
*/
void TEtherCATRT::MyPostPutExp( int num, bool late, bool noroom)              {
   //si se ha escrito alguna orden de expulsion en el área de intercambio, entonces
   //enviar los datos modificados al PLC
   if( num > 0)
      SetCfg();

   //si ha habido alguna orden de expulsion tardia, indicar
   if ( late)                                                                 {
      LOG_WARN1( LoadText( L"EV_PUTEXP_LATE"));
      if( ++m_numRepEjectFail > m_maxEjectFail)                               {
         m_lastLatePutExp = LT::Time::GetTime();
         SetFaultyFlag(FAULTY_FAIL_PUTEXP,2);
      }
   }

   //si ha habido falta de espacio, indicar
   if ( noroom)                                                               {
	  LOG_WARN1( LoadText( L"EV_PUTEXP_NO_ROOM"));
      if( ++m_numRepEjectFail > m_maxEjectFail)                               {
		 m_lastNoRoomPutExp = LT::Time::GetTime();
         SetFaultyFlag(FAULTY_FAIL_PUTEXP,2);
      }
   }
}

//------------------------------------------------------------------------------
// PutTrigger
//
bool  TEtherCATRT::PutTrigger(unsigned char t)                                {

   eth.trig.cfg[t].putTrigger   = true;
   SetCfg();
   return true;
}

//------------------------------------------------------------------------------
// SetMode
//
bool  TEtherCATRT::SetMode( unsigned char mode)                               {

   eth.mode             = mode;
   SetCfg();
   return true;
}

//------------------------------------------------------------------------------
// EnableSinc
//
bool  TEtherCATRT::MyInit()                                                     {

   m_lastFaultyTime           = 0;
   m_lastLatePutExp           = 0;
   m_lastNoRoomPutExp         = 0;
   m_counterSyncTooLate       = 0;

   m_lastSystemTime           = 0;
   m_lastEthTime              = 0;
   m_validCount               = 0;

   // trigger number to show in interface at start
   pTrigToConfig->SetAsInt( 1);

   //Trozo de codigo añadido para sincronizar relojes
//   m_fullCountTime   = false;
//
//   m_i_w_CountTime   = 0;

// static faultyConnect = false;

   SetCfg();
//   ::Sleep( MS_UPDATE);
//
//   Update();
   ::Sleep( MS_SYNC_CLOCK);
   Update();
   //Fin Trozo de codigo añadido para sincronizar relojes

   // Avisamos de cambios de version
   WORD cfgVersion = pCodeVersion->AsInt();
   CheckVersion( cfgVersion, eth.codeVersion);

   if (! m_IOConnected)                                                       {
      SetFaultyFlag( FAULTY_ETHERCAT_NOT_CONNECTED, 2);
      DISPATCH_EVENT( WARN_CODE, L"EV_ETHERCAT_NOT_CONNECTED" , L"", this);
      LOG_WARN1(LoadText(L"EV_ETHERCAT_NOT_CONNECTED"));
      return false;
   }
   else                                                                       {
      ResetFaultyFlag(FAULTY_ETHERCAT_NOT_CONNECTED);
      if ( m_enableEjection == 0)                                             {
         SetFaultyFlag( FAULTY_EJECTION_DISABLED, 1);
         DISPATCH_EVENT( WARN_CODE, L"EV_EJECTION_DISABLED" , L"", this);
         LOG_WARN1(LoadText(L"EV_EJECTION_DISABLED"));
      }
      else
         ResetFaultyFlag(FAULTY_EJECTION_DISABLED);
   }


   ::Sleep( 10);

   eth.status   = ready;

   SetCfg();
   Update();
   m_dispatchControlEnabled = true;
   //limpiar la lista de expulsiones pendientes
   m_listExp.clear();
   return TIORealTime::MyInit();
}

//------------------------------------------------------------------------------
//
bool  TEtherCATRT::MyGo()                                                       {
   eth.status   = running;
   SetCfg();
   return TIORealTime::MyGo();
}

//------------------------------------------------------------------------------
//
bool  TEtherCATRT::MyStop()                                                   {

   bool  ret = TIORealTime::MyStop();

   if (ret)                                                                   {
      eth.status = ready;

      ResetFaultyFlag( FAULTY_INDEX_TRIGG_ADVANCED);
      ResetFaultyFlag( FAULTY_EXECUTE_TIME);
      ResetFaultyFlag( FAULTY_IREAD_EQUALS_IWRITE);
      ResetFaultyFlag( FAULTY_FAIL_PUTEXP );

      SetCfg();
   }
   else                                                                       {
      LOG_ERROR2(L"EV_FAIL_STOP_ETHERCAT_REAL_TIME",GetLangName());
      DISPATCH_EVENT (ERROR_CODE,L"EV_FAIL_STOP_ETHERCAT_REAL_TIME", GetLangName(),this);
      return false;
   }

   return ret;
}

//-----------------------------------------------------------------------------
// Read configuration
//
bool TEtherCATRT::Read( LT::TStream *stream, TDataInterface *intf, bool infoModif, bool *forceSave ) {

   m_reading = true;
   bool b = TDataManagerControl::Read( stream, intf, infoModif, forceSave );

   Update();

   memcpy( &trigCfg,    &eth.trig.cfg[  m_trigToConfig-1],  sizeof( S_TRIG_CFG));
   memcpy( &trigData,   &eth.trig.data[ m_trigToConfig-1],  sizeof( S_TRIG_DATA));

   memcpy( &strobCfg,   &eth.strob.cfg[ m_strobToConfig-1], sizeof( S_STROB_CFG));
   memcpy( &strobData,  &eth.strob.data[m_strobToConfig-1], sizeof( S_STROB_DATA));

   pStrobSequence->SetUsedSize( strobCfg.tamSec);

   m_reading = false;
   return b;
}

//-----------------------------------------------------------------------------
// DataModifiedPost
//
void __fastcall TEtherCATRT::DataModifiedPost( TData* da)                     {

   // evitar que la función sea reentrante
   if (m_updating || da==NULL)   {
      return;
   }

   TIORealTime::DataModifiedPost( da);
   m_updating = true;


   // TDatas que no cuelgan de ningun datamanager
   if( da == pEnableEjection && m_enableEjection == 1)
      ResetFaultyFlag( FAULTY_EJECTION_DISABLED);
   if( da == pEnableEjection && m_enableEjection == 0)                        {
      SetFaultyFlag( FAULTY_EJECTION_DISABLED, 1);
      DISPATCH_EVENT( WARN_CODE, L" EV_EJECTION_DISABLED" , L"", this);
      LOG_WARN1(LoadText(L"EV_EJECTION_DISABLED"));
   }

   // Solo entraremos aquí, tras leer del registry
   if( !m_reading)                                                            {

      // si se ha cambiado el trigger a configurar, seleccionar su configuración
      if ( da == pTrigToConfig )                                              {
         memcpy( &trigCfg, &eth.trig.cfg[m_trigToConfig-1], sizeof( S_TRIG_CFG));
         if( mngrTrigger)                                                     {
            std::list<TData*>::iterator it = mngrTrigger->DataBegin();
            while (it != mngrTrigger->DataEnd())                              {
               if( (*it))
                  (*it)->UpdateFromMem();
               it++;
            }
         }

        m_updating = false;
        return;              //no queremos provocar un SetCfg
      }

      // si se ha modificado el strobe a configurar, seleccionar su configuración
      if ( da == pStrobeToConfig)                                             {
         memcpy( &strobCfg,   &eth.strob.cfg[m_strobToConfig-1],  sizeof( S_STROB_CFG));
         pStrobSequence->SetUsedSize( strobCfg.tamSec);
         if( mngrStrobe)                                                      {
            std::list<TData*>::iterator it = mngrStrobe->DataBegin();
            while (it != mngrStrobe->DataEnd())                               {
               if( (*it))
                  (*it)->UpdateFromMem();
               it++;
            }
         }
         m_updating = false;
         return;     //no queremos provocar un SetCfg
      }

      memcpy( &eth.trig.cfg[  m_trigToConfig-1],   &trigCfg,   sizeof( S_TRIG_CFG));
      memcpy( &eth.strob.cfg[ m_strobToConfig-1],  &strobCfg,  sizeof( S_STROB_CFG));
      if ( pStrobSequence)
         pStrobSequence->SetUsedSize( strobCfg.tamSec);

      // Update if we have modified a trigger or strobe value, UpdateFromMem
      if( da)                                                                 {
         if ( da->GetParent() == mngrTrigger && pTrigConfiguration)
            pTrigConfiguration->UpdateFromMem();
         if ( da->GetParent() == mngrStrobe && pStrobeConfiguration)
            pStrobeConfiguration->UpdateFromMem();
      }

      // update configuration to EtherCAT
      SetCfg();
   }

   m_updating = false;
}

//-----------------------------------------------------------------------------
// - Actualizar la información de EtherCAT a variables locales
// - ajustar offsets de tiempos (EtherCAT y local)
// - verificar si hay que desbloquear índices de triggers
//                                                  l
bool TEtherCATRT::Update( )                                                   {

   LARGE_INTEGER  counter0,
                  counter;

   __int64        ethTime;

   static int     errorCounter = 0;

   //--- coger tiempo local, el de EtherCAT y comprobar que no ha pasado muho tiempo
   QueryPerformanceCounter(&counter0);
   if (m_IOConnected==false )
      return false;

   if (TCatIoInputUpdate( DRIVER_ETHERCAT_PORTNUMBER ) != 0)                  {

      if (!m_restartTwincatError)
         return false;

      if ( errorCounter < 10)                                                 {
         errorCounter++;
         return false;
      }

      AmsAddr     Addr;
      PAmsAddr    pAddr = &Addr;
      AdsPortOpen();
      DISPATCH_EVENT( WARN_CODE, L"EV_RESTARTING_TWINCAT", NULL, this);
      LOG_WARN1(LoadText(L"EV_RESTARTING_TWINCAT") );
      if (!AdsGetLocalAddress(pAddr))                                         {
         // Reset todas las tareas
         int err;
         pAddr->port = AMSPORT_R0_PLC_RTS1;
         do                                                                   {

            err = AdsSyncWriteControlReq (pAddr, ADSSTATE_RESET, 0, 0, NULL);
            if( err == ADSERR_NOERR)                                          {
               DISPATCH_EVENT( INFO_CODE, L"EV_TASK_RESETED ", String((pAddr->port - AMSPORT_R0_PLC_RTS1)/10).c_str(), this);
               LOG_INFO2(LoadText(L"EV_TASK_RESETED"), String((pAddr->port - AMSPORT_R0_PLC_RTS1)/10).c_str() );
            }
            pAddr->port += 10;      // Cada PLC está separado por 10
         }while (err == ADSERR_NOERR || err == ADSERR_DEVICE_INVALIDSTATE);

         // Arrancamos todas las tareas
         pAddr->port = AMSPORT_R0_PLC_RTS1;
         do                                                                   {

            err = AdsSyncWriteControlReq (pAddr, ADSSTATE_RUN, 0, 0, NULL);
            if( err == ADSERR_NOERR)                                          {
               DISPATCH_EVENT( INFO_CODE, L"EV_TASK_RUNNED ", String((pAddr->port - AMSPORT_R0_PLC_RTS1)/10).c_str(), this);
               LOG_INFO2(LoadText(L"EV_TASK_RUNNED"), String((pAddr->port - AMSPORT_R0_PLC_RTS1)/10).c_str() );
            }
            pAddr->port += 10;      // Cada PLC está separado por 10
         }while (err == ADSERR_NOERR || err == ADSERR_DEVICE_INVALIDSTATE);

         // Reiniciamos TwinCAT
         pAddr->port = 10000;
         err = AdsSyncWriteControlReq (pAddr, ADSSTATE_RESET, 0, 0, NULL);
         if( err == ADSERR_NOERR)                                             {
            DISPATCH_EVENT( INFO_CODE, L"EV_TWINCAT_RESTARTED ", NULL, this);
            LOG_INFO1(LoadText(L"EV_TWINCAT_RESTARTED"));
         }
      }

      errorCounter = 0;
      return false;
   }

   // guardar en variable local tiempo de EtherCAT..., para posterior SyncTimers()
   memcpy( &eth.baseTime.data,   &pInputs->BASETIME_DATA, sizeof( eth.baseTime.data));

   QueryPerformanceFrequency( (LARGE_INTEGER*)&m_freq);
   QueryPerformanceCounter(&counter);

   ethTime =  eth.baseTime.data.time100ns.QuadPart;

   //--- copiamos información de EtherCAT a variables locales (data)
   memcpy( &eth.trig.data,       &pInputs->TRIG_DATA,     sizeof( eth.trig.data));
//   memcpy( &eth.baseTime.data,   &pInputs->BASETIME_DATA, sizeof( eth.baseTime.data));
   memcpy( &eth.strob.data,      &pInputs->STROB_DATA,    sizeof( eth.strob.data));
   memcpy( &eth.putExp.data,     &pInputs->PUTEXP_DATA,   sizeof( eth.putExp.data));
   memcpy( &trigData,            &eth.trig.data[m_trigToConfig-1],   sizeof( trigData));
   memcpy( &strobData,           &eth.strob.data[m_strobToConfig-1], sizeof( strobData));

   eth.numTerminales       = pInputs->numTerminales;
   eth.falloTerminales     = pInputs->falloTerminales;
   eth.masterOk            = pInputs->masterOk;
   eth.codeVersion         = pInputs->codeVersion;

   // guardar tiempo de ultima actualización
   m_lastUpdate            = timeGetTime();

   ethTime =  eth.baseTime.data.time100ns.QuadPart;

// Dani: No funciona bien
//   __int64 systemTime;
//   GetSystemTimeFromEthTime(ethTime, systemTime);
//   m_lastTimeAndQueryPerformaceCounter.systemTime = systemTime;
//   m_lastTimeAndQueryPerformaceCounter.queryPerformanceCounter = counter;
//   m_lastTimeAndQueryPerformaceCounter.ethCounter = eth.baseTime.data.counter;
//   m_lastTimeAndQueryPerformaceCounter.queryPerformanceFreq = m_freq;

   //--- protección de llamadas re entrantes (se dan!!)
   static bool dentro = false;
   if( dentro)                                                                {
      LOG_DEBUG1( L"DENTRO_UPDATE_ETH_DATA");
      return false;
   }
   dentro = true;

   //--- check if it's time to syncronize clocks
   __int64 time = LT::Time::GetTime( counter.QuadPart);
   if ( m_lastSyncTime == 0 || time - m_lastSyncTime > m_timeSyncMs * 1000 )  {

      // Check precision & call SyncTimers()
      __int64 precision = ((__int64)(counter.QuadPart - counter0.QuadPart))*1000000 / m_freq.QuadPart;
      if ( precision < 100 )                                                  {
         LOG_DEBUG1( L"Precision OK. SyncTimers");
         SyncTimers( counter0.QuadPart, ethTime);              // Sync timers (offsets)
         m_lastSyncTime = time;
      }
   }

   // en ocasiones, si se van disparando triggers, y no llegan imágenes, el
   // índice de escritura llega al de lectura y no se pueden continuar poniendo
   // señales de trigger desde el EtherCAT..., en este caso avanzamos el índice
   // de lectura de triggers, para desbloquear esta situación.
   // esto sólo se hace si está configurado de esta forma (m_unlock)
   if ( m_unlock && m_state.state == running)                                 {
      bool unlocked = false;
      for (int i=0; i < NUM_TRIG; i++)                                        {
         bool unlock = false;

         // si el índice de escritura es igual al de lectura
         if (eth.trig.data[i].ctrl_i_w == CIRC(eth.trig.cfg[i].ctrl_i_r-1,NTRIG)){
            // avanzar el índice de lectura
            if (WaitForSingleObject(m_mutexIR[i], 0) == WAIT_OBJECT_0)        {
               eth.trig.cfg[i].ctrl_i_r   = CIRC( eth.trig.cfg[i].ctrl_i_r+1, NTRIG);
               unlock                     = true;
               SetCfg();                           // pasar índice modificado a EtherCAT
               ReleaseMutex( m_mutexIR[i]);
            }
         }

         // si se ha movido algún índice para desbloquear los triggers, dejar registro
         if ( unlock)                                                         {
            wchar_t     indext[48];
            swprintf( indext, L"%s trigger %d", GetLangName(), i);
            LOG_WARN2( L"EV_INDEX_TRIGGER_ADVANCED", indext);
            DISPATCH_EVENT( WARN_CODE, L"EV_INDEX_TRIGGER_ADVANCED", indext, this);
            unlocked    = true;
         }
      }

      // si se ha desbloqueado algún índice, poner estado de faulty
      if (unlocked)                                                           {
         m_lastFaultyTime = LT::Time::GetTime();
         SetFaultyFlag( FAULTY_INDEX_TRIGG_ADVANCED, 1);
      }
   }

   dentro = false;
   return true;
}

// ------------------------------------------------------------------------------
// Función que es llamada de forma periódica, para detectar posibles fallos, etc...
//
void  TEtherCATRT::MyPeriodic( __int64 &time)                                 {

   TProcess::MyPeriodic( time);
   m_numRepEjectFail    = 0;
   m_numRepTriggerError = 0;
   if ( m_lastFaultyTime && time - m_lastFaultyTime > 10000000 )              {
      m_lastFaultyTime  = 0;
      ResetFaultyFlag(FAULTY_INDEX_TRIGG_ADVANCED);
   }

   if ( m_lastNoRoomPutExp && time - m_lastNoRoomPutExp > 5000000 )           {
      m_lastNoRoomPutExp = 0;
      ResetFaultyFlag( FAULTY_FAIL_PUTEXP );
   }
   if ( m_lastLatePutExp && time - m_lastLatePutExp > 5000000 )               {
      m_lastLatePutExp = 0;
      ResetFaultyFlag( FAULTY_FAIL_PUTEXP );
   }
}

// ------------------------------------------------------------------------------
//   in micro sec since 1970, ethTime in 100ns
bool TEtherCATRT::GetEthTimeFromSystemTime( __int64 systemTime, __int64 &ethTime){

   if ( WaitForSingleObject( m_mutex2, 500) == WAIT_OBJECT_0)                 {
         ethTime  =  m_offset1970            +                          // offset
                     10 * systemTime         +                          // tiempo pasado
                     10 *(systemTime - m_SyncLastTime) * m_SyncDeriva;  // corrección de deriva
         ReleaseMutex(m_mutex2);
         return true;
   }
   // aunque no consiga coger el mutex, calculamos el tiempo
   ethTime        =  m_offset1970            +                          // offset
                     10 * systemTime         +                          // tiempo pasado
                     10 *(systemTime - m_SyncLastTime) * m_SyncDeriva;  // corrección de deriva
   return false;
}

// ------------------------------------------------------------------------------
//
bool TEtherCATRT::GetSystemTimeFromEthTime( __int64 ethTime, __int64 &systemTime){

   if ( WaitForSingleObject( m_mutex2, 500) == WAIT_OBJECT_0)                 {
      systemTime  = (ethTime - m_offset1970 + 10*m_SyncDeriva*m_SyncLastTime +5) / (10 + m_SyncDeriva*10);
      ReleaseMutex(m_mutex2);
      return true;
   }
   // aunque no consiga coger el mutex, calculamos el tiempo
   systemTime     = (ethTime - m_offset1970 + 10*m_SyncDeriva*m_SyncLastTime +5) / (10 + m_SyncDeriva*10);
   return false;
}

// ------------------------------------------------------------------------------
// SetCfg, actualizar estados de la configuración del PLC
//
void TEtherCATRT::SetCfg()                                                    {

   if( m_IOConnected)                                                         {

      // Héctor 27/10/2010 Protección para no sobrepasar nunca el índice del buffer de expulsiones.
      // Relacionado con los cambios en las versiones 1.0.4, 2.0.3, y 3.0.3 del programa de driver de EtherCAT.
     if (*(WORD*)(&pOutputs->PUTEXP_CFG) >= MAX_EXP)
         *(WORD*)(&pOutputs->PUTEXP_CFG)  = MAX_EXP-1;

      // actualizar salidas
      memcpy( &pOutputs->BASETIME_CFG,  &eth.baseTime.cfg,  sizeof(eth.baseTime.cfg));
      memcpy( &pOutputs->PUTEXP_CFG,    &eth.putExp.cfg,    sizeof(eth.putExp.cfg));
      memcpy( &pOutputs->STROB_CFG,     &eth.strob.cfg,     sizeof(eth.strob.cfg));
      memcpy( &pOutputs->TRIG_CFG,      &eth.trig.cfg,      sizeof(eth.trig.cfg));

      // Escribimos el reset. En cualquier caso lo volvemos a poner a cero, ya que el PLC no puede
      pOutputs->status              = eth.status;
      pOutputs->mode                = eth.mode;
      pOutputs->divisorEncTrigStrob = eth.divisorEncTrigStrob;
      pOutputs->divisorExp          = eth.divisorExp;

      // Quitamos flag de puttriger si se ha activado alguno
      for( int i = 0; i < NUM_TRIG; ++i)
         eth.trig.cfg[i].putTrigger = 0;

      // start the I/O update and field bus cycle
      static int errorCounter = 0;

      if ( (TCatIoOutputUpdate( DRIVER_ETHERCAT_PORTNUMBER ) != 0) && m_restartTwincatError) {

         if (!m_restartTwincatError)
            return;

         if (errorCounter < 10000)                                            {
            errorCounter++;
            return;
         }

         AmsAddr     Addr;
         PAmsAddr    pAddr = &Addr;
         AdsPortOpen();
         DISPATCH_EVENT( WARN_CODE, L"EV_RESTARTING_TWINCAT", NULL, this);
         LOG_WARN1(LoadText(L"EV_RESTARTING_TWINCAT"));

         if (!AdsGetLocalAddress(pAddr))                                      {
            // Reset todas las tareas
            int err;
            pAddr->port = AMSPORT_R0_PLC_RTS1;
            do                                                                {

               err = AdsSyncWriteControlReq (pAddr, ADSSTATE_RESET, 0, 0, NULL);
               if( err == ADSERR_NOERR)                                       {
                  DISPATCH_EVENT( INFO_CODE, L"EV_TASK_RESETED ", String((pAddr->port - AMSPORT_R0_PLC_RTS1)/10).c_str(), this);
                  LOG_INFO2(LoadText(L"EV_TASK_RESETED"), String((pAddr->port - AMSPORT_R0_PLC_RTS1)/10).c_str() );
               }
               pAddr->port += 10;      // Cada PLC está separado por 10
            }while (err == ADSERR_NOERR || err == ADSERR_DEVICE_INVALIDSTATE);

            // Arrancamos todas las tareas
            pAddr->port = AMSPORT_R0_PLC_RTS1;
            do                                                                {

               err = AdsSyncWriteControlReq (pAddr, ADSSTATE_RUN, 0, 0, NULL);
               if( err == ADSERR_NOERR)                                       {
                  DISPATCH_EVENT( INFO_CODE, L"EV_TASK_RUNNED ", String((pAddr->port - AMSPORT_R0_PLC_RTS1)/10).c_str(), this);
                  LOG_INFO2(LoadText(L"EV_TASK_RUNNED"), String((pAddr->port - AMSPORT_R0_PLC_RTS1)/10).c_str() );
               }
               pAddr->port += 10;      // Cada PLC está separado por 10
            }while (err == ADSERR_NOERR || err == ADSERR_DEVICE_INVALIDSTATE);

            // Reiniciamos TwinCAT
            pAddr->port = 10000;
            err = AdsSyncWriteControlReq (pAddr, ADSSTATE_RESET, 0, 0, NULL);
            if( err == ADSERR_NOERR)                                          {
               DISPATCH_EVENT( INFO_CODE, L"EV_TWINCAT_RESTARTED ", NULL, this);
               LOG_INFO1(LoadText(L"EV_TWINCAT_RESTARTED"));
            }
         }

         errorCounter = 0;
      }

   }
}

//-----------------------------------------------------------------------------
//
bool TEtherCATRT::SetTimers()                                                 {

   if( idTimerUpdate)
      KillTimers();
   idTimerUpdate  = timeSetEvent( MS_UPDATE, 0, TimerProc, 0, TIME_PERIODIC|TIME_CALLBACK_FUNCTION);
   return idTimerUpdate;
}

//-----------------------------------------------------------------------------
//
void TEtherCATRT::KillTimers()                                                {
   if( idTimerUpdate)
      timeKillEvent( idTimerUpdate);
   idTimerUpdate = NULL;
}

//------------------------------------------------------------------------------

/**
   @param numTrigger is the number of the trigger where to get a information from (starting at 1)
   @param tmax is the desired maximum time for the trigger time. It is measured in microseconds from 1970
   @param capRTData returns Last Trigger, Last Strobe, and Last Synchronysm from the buffer
   @return  true when got a time from buffer of LastTimes lower than tmax
            in that case, the read index for the buffer is incremented
*/
bool TEtherCATRT::GetLastTrig( int numTrigger, __int64 tmax, s_captureRealTimeData &capRTData){

   // precondition
   if( numTrigger < 1 || numTrigger > NUM_TRIG)
      return false;

   // update info from EtherCAT to make sure we are working on the latest data
   Update();

   // take control of mutex
   if (WaitForSingleObject(m_mutexIR[numTrigger-1], 5) != WAIT_OBJECT_0)      {
      LOG_WARN2( m_name, L"Failed to obtain mutex for IR." );
      return false;
   }

   try                                                                        {
      unsigned char &i_r            = eth.trig.cfg[ numTrigger-1].ctrl_i_r;
      S_TRIG_DATA   &ctrlBinTrigger = eth.trig.data[numTrigger-1];
      int            i_w            = ctrlBinTrigger.ctrl_i_w;

      //--- comprobar que el control de trigger está habilitado en EtherCAT
      if (eth.trig.cfg[numTrigger-1].ctrlEnabled==0 && m_dispatchControlEnabled){
         m_dispatchControlEnabled   = false;
         LOG_WARN2( m_name, L"The GetLastTrigger is not enabled." );
         return false;
      }

      // if there is info available for at least one trigger
      if ((i_r < NTRIG) && (i_r != i_w))                                      {

         // convert "max" time
         __int64 t100ns;
         GetEthTimeFromSystemTime( tmax, t100ns);

         //--- check if current trigger is too late !!
         if (ctrlBinTrigger.lastTime[i_r].QuadPart > t100ns)                  {

            if (m_traceFile)                                                  {
               FILE *fitx=fopen("getTrigger.txt","a");
               fprintf( fitx, "num = %d. lastTime[i_r].QuadPart = %Ld, " ,
                  numTrigger,  ctrlBinTrigger.lastTime[i_r].QuadPart);
               fprintf( fitx, "t100ns = %Ld, ", t100ns);

               SYSTEMTIME systemTime;
               GetLocalTime( &systemTime);
               __int64 t;
               LT::Time::TimeStampFromLocalTime( &systemTime, &t);
               fprintf( fitx, "now ( lyratime * 10)=%Ld * 10 ( centenares de ns)\n", t);
               fclose(fitx);
            }
            return false;   //time greater then maximum
         }

         //--- at this point we have a valid trigger info, store info

         GetSystemTimeFromEthTime(ctrlBinTrigger.lastTime[  i_r].QuadPart, capRTData.time);
         capRTData.sinc       = ctrlBinTrigger.lastSinc[  i_r].QuadPart;
         capRTData.strobe     = ctrlBinTrigger.lastStrobe[i_r];
         capRTData.frameCount = ctrlBinTrigger.frameCount[i_r];
         capRTData.lastResync = eth.baseTime.data.lastReSync.QuadPart;

         if (m_traceFile)                                                     {
            FILE *fitx=fopen("getTrigger.txt","a");
            fprintf( fitx, "num = %d. lastTime[i_r].QuadPart = %Ld, capRTData.time=%Ld, capRTData.sinc=%Ld Lastcycle = %d\n" ,
               numTrigger,  ctrlBinTrigger.lastTime[i_r].QuadPart, capRTData.time, capRTData.sinc, eth.baseTime.data.lastReSync);
            fclose(fitx);
         }

         // select next trigger & update to EtherCAT
         if( ++i_r >= NTRIG)
            i_r = 0;
         SetCfg();

         // update visible i_r at the interface
         if( m_trigToConfig == numTrigger)
            trigCfg.ctrl_i_r = i_r;

         // reset faulty state
         ResetFaultyFlag(FAULTY_IREAD_EQUALS_IWRITE);
         return true;
      }
      else                                                                    {
         //--- no hay info de trigger disponibles...

         // comprobar que stamos en marcha. Si estamos parados no
         if (m_state.state == running )                                       {

            // set faulty state
            if ( ++m_numRepTriggerError > m_maxTriggerError)                  {
               SetFaultyFlag( FAULTY_IREAD_EQUALS_IWRITE,2);
            }
            else                                                              {
               // reset faulty state
               ResetFaultyFlag(FAULTY_IREAD_EQUALS_IWRITE);
            }

            wchar_t message[120];
            swprintf( message, L"%s: i_r=%d LastFrameCount: %d,%d,%d,%d,%d,%d,%d,%d",
               GetLangName(), i_r,
               ctrlBinTrigger.frameCount[0],
               ctrlBinTrigger.frameCount[1],
               ctrlBinTrigger.frameCount[2],
               ctrlBinTrigger.frameCount[3],
               ctrlBinTrigger.frameCount[4],
               ctrlBinTrigger.frameCount[5],
               ctrlBinTrigger.frameCount[6],
               ctrlBinTrigger.frameCount[7]);

            LOG_WARN2( L"EV_IREAD_EQUALS_IWRITE", message );

            if (m_traceFile)                                                  {
               FILE *fitx=fopen("getTrigger.txt","a");
               fprintf( fitx, "numTrigger = %d. Index read is equal that index write: i_r=%d, i_w = %d\n", numTrigger, i_r, i_w );
               fclose(fitx);
            }
         }
         return false;
     }
   } __finally                                                                {
      // release trigger
      ReleaseMutex( m_mutexIR[numTrigger-1]);
   }

   return false;
}
//-------------------------------------------------------------------------------


void TEtherCATRT::AddErrorText( std::wstring &errorText)                      {

   TIORealTime::AddErrorText( errorText);

   if ( (m_state.faultyFlags & FAULTY_ETHERCAT_NOT_CONNECTED) || (m_state.faultyFlags & (FAULTY_ETHERCAT_NOT_CONNECTED << 16)))   {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText( L"FAULTY_ETHERCAT_NOT_CONNECTED");
      errorText = errorText + L"\r\n";
   }
   if ( (m_state.faultyFlags & FAULTY_EJECTION_DISABLED) || (m_state.faultyFlags & (FAULTY_EJECTION_DISABLED << 16)))   {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText( L"FAULTY_ETHERCAT_EJECTION_DISABLED");
      errorText = errorText + L"\r\n";
   }
   if ( (m_state.faultyFlags & FAULTY_INDEX_TRIGG_ADVANCED) || (m_state.faultyFlags & (FAULTY_INDEX_TRIGG_ADVANCED << 16)))   {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText( L"FAULTY_INDEX_TRIGGER_ADVANCED");
      errorText = errorText + L"\r\n";
   }
   if ( (m_state.faultyFlags & FAULTY_EXECUTE_TIME) || (m_state.faultyFlags & (FAULTY_EXECUTE_TIME << 16)))   {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText( L"FAULTY_EXECUTE_TIME");
      errorText = errorText + L"\r\n";
   }
   if ( (m_state.faultyFlags & FAULTY_IREAD_EQUALS_IWRITE) || (m_state.faultyFlags & (FAULTY_IREAD_EQUALS_IWRITE << 16)))   {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText( L"FAULTY_IREAD_EQUALS_IWRITE");
      errorText = errorText + L"\r\n";
   }
   if ( (m_state.faultyFlags & FAULTY_FAIL_ENCODER) || (m_state.faultyFlags & (FAULTY_FAIL_ENCODER << 16)))   {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText( L"FAULTY_FAIL_ENCODER");
      errorText = errorText + L"\r\n";
   }
   if ( (m_state.faultyFlags & FAULTY_FAIL_PUTEXP) || (m_state.faultyFlags & (FAULTY_FAIL_PUTEXP << 16)))   {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText( L"FAULTY_FAIL_PUTEXP");
      errorText = errorText + L"\r\n";
   }
}
//------------------------------------------------------------------------------
/**
*/
bool TEtherCATRT::MyCheckDiagnose(e_diagnose_type type_diagnose, TBasicDiagnose *diag)    {


   wchar_t txt[256];
   swprintf(txt, L"\r\nETHERCAT\r\n");
   diag->Add( txt, wcslen( txt) * sizeof(wchar_t));

   //Check if driver is installed
   if (eth.masterOk == false)                                                 {
      swprintf(txt, L"\t%s\r\n", LoadText( L"MSG_ETHERCAT_NOT_CONNECTED"));
      diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
      swprintf(txt, L"\t\t\tNOT OK\r\n");
      diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
      diag->SetDiagStatus( FAIL_REAL_TIME);
   }
       //Check Versions
   if (pCodeVersion)                                                          {
      wchar_t str[128];
      swprintf(str,  L"\t%s:%d", LoadText( L"MSG_ETHERCAT_VERSION") ,pCodeVersion->AsInt());
      diag->Add( str, wcslen( str) * sizeof(wchar_t));
      if (pCodeVersion->AsInt() != eth.codeVersion)                           {
         swprintf(str, L"\t\t\tNOT OK\r\n");
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      else
         swprintf( str, L"\t\t\tOK\r\n");
      diag->Add( str, wcslen( str) * sizeof(wchar_t));
      }


   //Check Pulses origin
   if ((eth.baseTime.cfg.Sync == 2) || (eth.baseTime.cfg.Sync == 0))          {
      swprintf(txt, L"\t%s: ENCODER", LoadText( L"MSG_BASE_TIME"));
      diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
      wchar_t txt1[256];
      if (m_origPulse == 0) {
         swprintf(txt1, L"\t\t\tOK\r\n");
      }
      else                                                                    {
         swprintf(txt1, L"\t\t\tNOT OK\r\n");
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));
   }
   else if (eth.baseTime.cfg.Sync == 1)                                       {
      swprintf(txt, L"\t%s: TIMER", LoadText(L"MSG_BASE_TIME"));
      diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
      wchar_t txt1[256];
      if (m_origPulse == 1) {
         swprintf(txt1, L"\t\t\tOK\r\n");
      }
      else                                                                    {
         swprintf(txt1, L"\t\t\tNOT OK\r\n");
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));
   }

   //Check Ejection Mode
   if( eth.mode == MODE_AUTO)                                                 {
      swprintf(txt, L"\tEJECTION MODE: AUTO");
      diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
      wchar_t txt1[256];
      if (m_modoExpul == 1)                                                   {
         swprintf(txt1, L"\t\t\tOK\r\n");
      }
      else                                                                    {
         swprintf(txt1, L"\t\t\tNOT OK\r\n");
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));
   }
   else                                                                       {
      swprintf(txt, L"\t%s: MANUAL", LoadText( L"MSG_EJECTION_MODE"));
      diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
      wchar_t txt1[256];
      if (m_modoExpul == 0)                                                   {
         swprintf(txt1, L"\t\tOK\r\n");
      }
      else                                                                    {
         swprintf(txt1, L"\t\tNOT OK\r\n");
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));
   }
        //Check Trigger
   for( int i = 0; i < NUM_TRIG; ++i)                                         {
      if( eth.trig.cfg[i].enabled)                                            {
         wchar_t txt1[256];
         swprintf(txt1,  L"\tTrigger:%d : TriggerTime:%d, CicleTime:%d", i, eth.trig.cfg[i].triggerTime, eth.trig.cfg[i].cycleTime);
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

         if ( eth.trig.cfg[i].cycleTime != eth.trig.cfg[i].cycleTime)         {
            swprintf(txt1, L"\tNOT OK\r\n");
            diag->SetDiagStatus( FAIL_REAL_TIME);
         }
         else
            swprintf(txt1, L"\tOK\r\n");
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

      }
      else                                                                    {
         //trigger deshabilitado
//         wchar_t txt5[256];
//         swprintf(txt5,  L"\tTrigger:%d is disabled\r\n", i);
//         diag->Add( txt5, wcslen( txt5) * sizeof(wchar_t));
      }
   }
   //Check Strobe
   for( int i = 0; i < NUM_STROB; ++i)                                        {
      if( eth.strob.cfg[i].enabled)                                           {
         wchar_t txt1[256];
         swprintf(txt1,  L"\tStrobe:%d : StrobeTime:%d\r\n", i,  eth.strob.cfg[i].strobeTime);
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));
      }
      else                                                                    {
            //Strobe deshabilitado
//            wchar_t txt5[256];
//            swprintf(txt5,  L"\tStrobe:%d is disabled\r\n", i);
//            diag->Add( txt5, wcslen( txt5) * sizeof(wchar_t));
      }
   }

   // Cálculo de tiempos y prueba de cuadro eléctrico
   if (type_diagnose == 0)                                                    {

      s_test_driver res;
      TestDriver( res, 10000);

      //--- Funcionamiento de los terminales
      // Número de terminales configurados
      swprintf( txt, L"\t%s\t: %d ", LoadText( L"MSG_ETHERCAT_DEVICES"), res.numTerminales);
      diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
      if( m_numTerminales != res.numTerminales)                               {
         swprintf(txt, L"\t\t\tNOT OK\r\n");
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      else
         swprintf(txt, L"\t\t\tOK\r\n");
      diag->Add( txt, wcslen( txt) * sizeof(wchar_t));

      // Estado de terminales
      if( res.falloTerminales)                                                {
         wchar_t txt1[128];
         swprintf( txt1, L"\t%s\t: %s %d\r\n", LoadText( L"MSG_DEVICES"), LoadText(L"MSG_FAILURE_DEVICE") ,res.falloTerminales);
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      //-- Comprobamos pulsos de encoder
      if( (eth.baseTime.cfg.Sync == 2) || (eth.baseTime.cfg.Sync == 0))       {
         wchar_t txt1[128];
         swprintf( txt1, L"\t%s\t: %.1f\tpulses/s", LoadText( L"MSG_PULSE_FREQ"),  res.pulsEncoder);
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

         if(   res.pulsEncoder   < m_minPulsEnc || res.pulsEncoder   > m_maxPulsEnc) {
            swprintf(txt1, L"\t\tNOT OK\r\n");
            diag->SetDiagStatus( FAIL_REAL_TIME);
         }
         else
            swprintf(txt1, L"\t\tOK\r\n");
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

         swprintf( txt1, L"\t%s\t: %.f pulses", LoadText(L"MSG_PULSE_TURN"), res.pulsEncoderVC);
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

         // Comprobamos que entre vueltas completas tenemos los pulsos que tocan( máximo 4 pulsos de error)
         #define PULS_ERROR 4
         if( m_vueltaCompleta)                                                {
            int dif = ( (unsigned int)res.pulsEncoderVC - m_pulsosPorVuelta);
            dif = abs(dif);
            while (dif > m_pulsosPorVuelta - PULS_ERROR && dif < m_pulsosPorVuelta + PULS_ERROR)
               dif = abs(dif - m_pulsosPorVuelta);
            wchar_t txt2[128];

            if( res.pulsEncoderVC == 0 || dif >= PULS_ERROR)                  {
               swprintf(txt2, L"\t\t\tNOT OK\r\n");
               diag->SetDiagStatus( FAIL_REAL_TIME);
            }
            else
               swprintf( txt2, L"\t\t\tOK\r\n");
            diag->Add( txt2, wcslen( txt2) * sizeof(wchar_t));

         }
         else                                                                 {
            //if not check zero signal
            wchar_t txt2[128];
            swprintf( txt2, L"\r\n");
            diag->Add( txt2, wcslen( txt2) * sizeof(wchar_t));

         }
      }
      //-- Comprobamos pulsos de timer
      wchar_t str6[128];
         swprintf( str6, L"\t%s\t: %.1f\tpulses/s",  LoadText(L"MSG_TIMER_FREQ"),     res.pulsTimer);
      diag->Add( str6, wcslen( str6) * sizeof(wchar_t));
      wchar_t str7[128];
      if(  GetPulseOrigin() == PULSE_TIMER &&  (res.pulsEncoder   < m_minPulsEnc || res.pulsEncoder   > m_maxPulsEnc)) {
         swprintf(str7, L"\t\tNOT OK\r\n");
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      else
         swprintf(str7, L"\t\tOK\r\n");
      diag->Add( str7, wcslen( str7) * sizeof(wchar_t));
   }

   return true;

}
/**
*/
void TEtherCATRT::TestDriver( s_test_driver &res, int time)                   {

   double            ctime;
   LARGE_INTEGER     freq,
                     counter[2];
   S_ETHINFO         info[2];

   QueryPerformanceFrequency( &freq);
   memset( &res, 0x00, sizeof(res));

   // Cogemos parámetros temporales
   if( Update())                                  // Contadores de la máquina
      memcpy( &info[0], &eth, sizeof( eth));
   else                                                     {
      return;
   }

   QueryPerformanceCounter( &counter[0]);    // Tiempo actual de la máquina

   while(1)                                                                   {
      // coger valor de los contadores
      if( Update( ))                                                          {  // Contadores de la máquina
         QueryPerformanceCounter( &counter[1]);                // Tiempo actual de la máquina
         memcpy( &info[1], &eth, sizeof( eth));
      }

      ctime = (counter[1].QuadPart-counter[0].QuadPart)*1000./freq.QuadPart;

      if ( info[1].baseTime.data.lastReSync.QuadPart != info[0].baseTime.data.lastReSync.QuadPart) {
         res.pulsEncoderVC = info[1].baseTime.data.lastReSync.QuadPart - info[0].baseTime.data.lastReSync.QuadPart;
         info[0].baseTime.data.lastReSync.QuadPart = info[1].baseTime.data.lastReSync.QuadPart;
      }

      if( ctime > time)
         break;
      Sleep(100);
   }

   res.pulsTimer        = (info[1].baseTime.data.timer    - info[0].baseTime.data.timer)    *1000./ctime;
   res.pulsEncoder      = (info[1].baseTime.data.counter.QuadPart  - info[0].baseTime.data.counter.QuadPart)  *1000./ctime;

   // Comprobación de Terminales y Master
   res.numTerminales    = info[1].numTerminales;
   res.falloTerminales  = info[1].falloTerminales;
   res.masterOk         = info[1].masterOk;

}

//------------------------------------------------------------------------------
/** calculate offset between local and ethercat time...

   lo que hacemos es, cada vez que se llma esta función, guardamos el offset
   entre los contadores de EtherCAT y sistema, y cada al menos 10 segundos
   calculamos la deriva que hay entre ambos contadores.

   en base al offset y  deriva, se calcula la conversión de tiempos.
*/
void TEtherCATRT::SyncTimers( __int64 perfCounter,__int64 ethTime100ns)       {

   __int64        systemTime        =  LT::Time::GetTime( perfCounter),
                  systemTime100ns   =  systemTime * 10,
                  st;

   //--- comprobar error para generar traza si supera los 2 ms
   if( m_SyncLastTime)                                                        {
      GetSystemTimeFromEthTime( ethTime100ns, st);
      int   diff        = systemTime - st;

      if( abs( diff) > 2 * 1000)                                              {
         wchar_t message[256];
         swprintf(message, L"Time Sync Error (EtherCAT <-> System): dif = %.1f ms. Synced : %.1f Count: %d", diff/1000., (systemTime-m_SyncLastTime)/1000., m_validCount);
         LOG_INFO1(message);
         if( --m_validCount < 0)
            m_validCount  = 0;
      }
      else if( ++m_validCount > 100)
         m_validCount     = 100;
   }

   // sólo ajustamos si la sincronización es 100% válida, o ha fallado mucho
   if( m_validCount < 95 || m_validCount == 100)                              {
      if ( WaitForSingleObject( m_mutex2, 0) == WAIT_OBJECT_0)                {
         try                                                                  {

            //--- calculo deriva

            // si es la primera vez, guardar pareja de tiempos
            if( !m_lastSystemTime)                                            {
               m_lastEthTime    = ethTime100ns;
               m_lastSystemTime = systemTime100ns;
            }

            // calculamos deriva sólo si han pasado más de 10s
            else if( systemTime100ns - m_lastSystemTime > 10 * 1000 * 10)       {

               // diferencia de tiempo pasado en EtherCAT y System
               m_SyncDeriva   = 1.0 - (double)(systemTime100ns - m_lastSystemTime) / (double)(ethTime100ns - m_lastEthTime);

               m_lastEthTime    = ethTime100ns;
               m_lastSystemTime = systemTime100ns;
            }

            // recalcular offset de tiempos entre EtherCAT y System
            m_offset1970      = ethTime100ns - systemTime100ns;

            // guardar último instate ajustado
            m_SyncLastTime    = systemTime;
         }
         __finally                                                            {
            ReleaseMutex(m_mutex2);
         }
      }
   }
}

//------------------------------------------------------------------------------
/**
   MyProcess
   @see TIORealTime::MyProcess
*/
TProcessData *TEtherCATRT::MyProcess( TProcessData *processData)              {

   TProcessData *ret =  TIORealTime::MyProcess( processData);
   //si no esta habilitado
   if ( (!m_enableBufferedExp) ||  ( GetMode() == MODE_MANUAL))
      return ret;

   TExpGroup *expGroup = dynamic_cast<TExpGroup*>(processData);
   if ( expGroup==NULL)                                                       {
      //el procesamiento de los grupos de expulsion ya incluye una revision de la lista de expulsiones pendientes, con un unico SetCg
      //pero si llega por ejemplo una imagen o un grupo de objetos, entonces aprovechamos para volver a procesar
      //la lista de expulsiones pendientes
      unsigned int t = timeGetTime();  //tiempo actual  ms
      //contemplar el caso de t < m_lastCheckPending, lo cual quiere decir que el contador ha dado la vuelta
      //si ha pasado mas de la mitad del periodo requerido para el chequeo de la lista, lo hacemos
      if (  t < m_lastCheckPending || t - m_lastCheckPending > CheckEjectionPeriode() * 500)  {

         DispatchPendingEjections();
      }
   }

   return ret;
}





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
