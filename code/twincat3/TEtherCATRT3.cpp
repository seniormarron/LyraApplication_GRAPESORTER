//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TEtherCATRT3.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   TwinCat3
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#pragma hdrstop

#include <math.h>

#include "TEtherCATRT3.h"
#include "TemplateLogs.h"
#include "TEventDispatcher.h"
#include "TTimePosition.h"
#include "TExpGroup.h"
#include "TDataExt.h"
#include "defines.h"
#include "threadsDefs.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

// Evitar los problemas de llamadas desde diferentes threads
//#include "critical.h"
//TCriSection tcs;

//-----------------------------------------------------------------------------
//// Datos para la callback
//TEtherCATRT3      *ethCAT3              = NULL;
//
//// Callback con la ejecución de la tarea de IO de control de válvulas, trigger y stroBe
//// La llamamos a cada tick ( 250 us)
//// ToDo: En lugar de una callback deberíamos usar un evento. De esa forma
//// evitaríamos problemas de multiples threads, ya que tocando estos TDatas, están
//// el thread del módulo y el creado para el temporizador
//void CALLBACK TEtherCATRT3::TimerProc( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
//
//   if( ethCAT3 /*&& ethCAT->Status() >= ready*/)                               {
//      if( uID == ethCAT3->idTimerUpdate)                                       {
//
//         // actualizar configuración en EtherCAT
//         ethCAT3->SetCfg();
//
//         // contemplar el caso de time < ethCAT->m_lastUpdate, que quire decir que el contador ha dado la vuelta
//         unsigned int _time = timeGetTime();
//         if (  _time < ethCAT3->m_lastUpdate  || (_time - ethCAT3->m_lastUpdate) > 1000) {
//            ethCAT3->m_lastUpdate = timeGetTime();  // se pone en Update, pero como esto entra tan rápido...
//            //si hace mas de un segundo que no se ejecuta el Update
//            ethCAT3->Update();
//         }
//      }
//   }
//}

//-----------------------------------------------------------------------------
/**
*  Thread to send commands to the x-ray generator. All commands must be sent
*  through this so there aren`t access conflicts to the generator serial port.
*/
DWORD WINAPI TEtherCATRT3Thread(LPVOID owner)                                        {

   TEtherCATRT3      *ethCAT3  = (TEtherCATRT3*)owner;
   HANDLE            event     = ethCAT3->m_stopEvent;
   DWORD             ev;
   while((ev = WaitForSingleObject( event, ethCAT3->m_periode ))!= WAIT_OBJECT_0){
       switch (ev) {

          case WAIT_TIMEOUT:

          {
            int lastCode = 0;
            #ifndef _DEBUG
            try                                                               {
            #endif

               // actualizar configuración en EtherCAT
               ethCAT3->SetCfg();

               // contemplar el caso de time < ethCAT->m_lastUpdate, que quire decir que el contador ha dado la vuelta
               unsigned int _time = timeGetTime();
               if (  _time < ethCAT3->m_lastUpdate  || (_time - ethCAT3->m_lastUpdate) > 1000) {
                  ethCAT3->m_lastUpdate = _time;  // se pone en Update, pero como esto entra tan rápido...

                  int lastCode = 1;
                  //si hace mas de un segundo que no se ejecuta el Update

                  ethCAT3->Update();

               }

            #ifndef _DEBUG
            }
            __except(EXCEPTION_EXECUTE_HANDLER)                               {

               // Hardware Exceptions
               unsigned long code = GetExceptionCode();
//               if(lastCode == 0)
//               ExceptionHandler( code, pOmron, L"SetCfg");
//               if(lastCode == 1)
//               ExceptionHandler( code, pOmron, L"Update");
               wchar_t slastCode[10];
               swprintf( slastCode, L"%d", lastCode);
               DISPATCH_EVENT( WARN_CODE, L"TEtherCATRT3Thread except: lastCode=", slastCode, ethCAT3);
            }
            #endif
          }
       }
   }
   return 0;
}


//------------------------------------------------------------------------------

// Constructor
//
TEtherCATRT3::TEtherCATRT3( wchar_t *_name, TProcess *parent)
      :  TIORealTime(_name, parent), TEtherCAT3( this)                          {

   QueryPerformanceFrequency( &m_freq);

   m_initializing = false;
   m_reading      = false;

//   // Puntero a la dirección de EtherCAT ( para la callback)
//   ethCAT3      = this;

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
   new TData( this, L"RestartTwincatIfError",   &m_restartTwincatError,   0,     0,    1,    NULL,   FLAG_NORMAL, L"If there are a error with cominication, do a system restart");
   new TData( this, L"RepetFailTrigger",        &m_maxTriggerError,       10,    1,    1000, NULL,  (FLAG_LEVEL_0 | FLAG_SAVE |  FLAG_NOTIFY), L"Number of trigger error in the MyPeriodic periode to show the Fualty");

   pEnableEjection      = new TData( this, L"EnableEjection",  &m_enableEjection,  1, 0, 1,     NULL, (FLAG_LEVEL_3 | FLAG_SAVE  | FLAG_NOTIFY), L"For testing purposes");

   new TDataByte( this, L"Status",                 &eth.Cfg.status,               0, 0, 3,                (FLAG_LEVEL_0 | FLAG_SAVE_NO_VALUE | FLAG_RDONLY | FLAG_NOTIFY));
   new TDataByte( this, L"Mode",                   &eth.Cfg.mode,                 0, L"Auto=0;Manual=1",  FLAG_NORMAL);
   new TDataByte( this, L"Divisor EndTrigStrob",   &eth.Cfg.divisorEncTrigStrob,  1, 1, 10,               FLAG_NORMAL, L"Divisor para atender el encoder, el triger y el strobe. 1 -> 250 us, 2 -> 500 us,...");
   new TDataByte( this, L"Divisor Expulsión",      &eth.Cfg.divisorExp,           1, 1, 10,               FLAG_NORMAL, L"Divisor para atender el buffer de PutExp y las órdenes. 1-> 250 us, 2 -> 500 us,....");
   new TDataWord( this, L"Numero Terminales",      &eth.Data.numTerminales,       0, 0, 9999,             (FLAG_LEVEL_0 | FLAG_SAVE_NO_VALUE | FLAG_RDONLY | FLAG_NOTIFY));
   new TDataWord( this, L"Fallo Terminales",       &eth.Data.falloTerminales,     0, 0, 9999,             (FLAG_LEVEL_0 | FLAG_SAVE_NO_VALUE | FLAG_RDONLY | FLAG_NOTIFY));
   new TDataByte( this, L"Master OK",              &eth.Data.masterOk,            1,                      (FLAG_LEVEL_0 | FLAG_SAVE_NO_VALUE | FLAG_RDONLY | FLAG_NOTIFY));

   //---- TDatas del encoder
   TDataManager *mngrBaseTime = new TDataManager(L"BaseTime", this);
   mngrBaseTime->DataModEventPost = DataModifiedPost;
   new TDataByte(  mngrBaseTime, L"Origen Pulsos",      &eth.Cfg.baseTimeCfg.Sync,              0, L"ENCODER 1012=0;TIMER=1;ENCODER 5151=2", FLAG_NORMAL);
   new TDataInt64( mngrBaseTime, L"Contador",           &eth.Data.baseTimeData.counter.QuadPart,                                              (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE ));
   new TDataInt64( mngrBaseTime, L"Timer 100ns",        &eth.Data.baseTimeData.time100ns.QuadPart,                                            (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE));
   new TDataDWord( mngrBaseTime, L"Timer",              &eth.Data.baseTimeData.timer,            0, 0, 0xFFFFFFFF,                            (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE ));
   new TDataInt64( mngrBaseTime, L"Last ReSync",        &eth.Data.baseTimeData.lastReSync.QuadPart,                                           (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE) , L"Valor de counter en el último instante de resincronización");
   new TDataDWord( mngrBaseTime, L"Timer Last ReSync",  &eth.Data.baseTimeData.timerLastReSync,  0, 0, 0xFFFFFFFF,                            (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE ), L"Valor de timer en el último instante de resincronización");

   //---- TDatas de trigger
   mngrTrigger = new TDataManager( L"Trigger" , this);
   pTrigToConfig      = new TData( mngrTrigger, L"Trigger to config",     &m_trigToConfig,  1, 1, NUM_TRIG ,   NULL,  FLAG_NORMAL & ~FLAG_SAVE, L"trigger number to show in interface");

   mngrTrigger->DataModEventPost    = DataModifiedPost;

   new TDataByte( mngrTrigger, L"Trigger Enabled",    &trigCfg.enabled,          0,                         (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE ));  //en otra variable diferente para poder hacer el MyGo y MyStop correctamente
   new TDataByte( mngrTrigger, L"Resincronizar con",  &trigCfg.reSync,           0, L"Nada=0;Base de tiempos=1;Fotocélula 1=2;Fotocélula 2=3;Fotocélula 3=4;Fotocélula 4=5;Fotocélula 5=6;Fotocélula 6=7;Fotocélula 7=8;Fotocélula 8=9",  (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE));
   new TData    ( mngrTrigger, L"Cycle Time",         (int*)&trigCfg.cycleTime,  0, 0, 10000000, L"%.2f",   (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE),  L"Tiempo en pulsos entre disparos de trigger en modo autómatico");
   new TDataByte( mngrTrigger, L"Triger Time",        &trigCfg.triggerTime,      1, 1, 255,                 (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE),  L"Tiempo en ms de duración de la señal de trigger (independiente de la de strobe)");
   new TDataByte( mngrTrigger, L"OffsetPulse",        &trigCfg.offsetPulse,      0, 0, 255,                 (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE),  L"Offset en pulsos respecto al instante de resincronización");
   new TDataByte( mngrTrigger, L"offsetRepetition",   &trigCfg.offsetRepetition, 0, 0, 255,                 (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE),  L"Offset en tiempo para adelantar respecto a la señal de trigger");
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

  pTrigConfiguration = new TData ( mngrTrigger, L"Trigger Configuration",  (void*)  &eth.Cfg.trigCfg,  sizeof( eth.Cfg.trigCfg) ,  (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE ),   L"binary data with trigger configuration", NULL, false);
   for ( int i=0; i< NUM_TRIG; i++)                                         {
      m_mutexIR[i]   = CreateMutex( NULL, false, NULL);
      eth.Cfg.trigCfg[i].Init()  ; //default values for trigger configuration, becuase constructor of TData overrides with 0x00 in previous sentence
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


   new TDataByte ( mngrStrobe, L"Tipo strobe",           &strobCfg.strobeType,   0, L"Visible=0;NIR=1;Rayos X=2;No predefinido(3)=3;No predefinido(4)=4;No predefinido(5)=5;No predefinido(6)=6;No predefinido(7)=7;No predefinido(8)=8;",  (FLAG_LEVEL_0 | FLAG_NOTIFY  | FLAG_SAVE_NO_VALUE));
   new TDataByte ( mngrStrobe, L"Resinc. secuencia con", &strobCfg.reSync,       0, L"Nada=0;Base de tiempos=1;Fotocélula 1=2;Fotocélula 2=3;Fotocélula 3=4;Fotocélula 4=5;Fotocélula 5=6,Fotocélula 6=7,Fotocélula 7=8,Fotocélula 8=9",  (FLAG_LEVEL_0 | FLAG_NOTIFY | FLAG_SAVE_NO_VALUE));
   new TDataInt64( mngrStrobe, L"Next",                  &strobData.next.QuadPart,                 (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
   new TDataInt64( mngrStrobe, L"Last",                  &strobData.last.QuadPart,                 (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
   new TDataInt64( mngrStrobe, L"LastNextTrig",          &strobData.lastNextTrig.QuadPart,         (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
   new TDataByte ( mngrStrobe, L"SeqIndex",              &strobData.seqIndex,    0, 0, 8,          (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));

   pStrobeConfiguration    = new TData ( mngrStrobe, L"Strobe Configuration",  (void*)  &eth.Cfg.strobCfg,  sizeof( eth.Cfg.strobCfg) ,  (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE ),   L"binary data with strobe configuration", NULL, false);
   for ( int i=0; i<NUM_STROB; i++) {
      eth.Cfg.strobCfg[i].Init(); //initialize strobe configuration with default data, becuase constructor of TData overrides with 0x00 in previous sentence
   }
   //---- Buffer para poner expulsiones
   TDataManager *mngrPutExp  = new TDataManager( L"PutExp", this, FLAG_NORMAL);
   mngrPutExp->DataModEventPost           = DataModifiedPost;

   new TDataByte( mngrPutExp, L"Indice read",  &eth.Data.putExpData.i_r, 0, 0, MAX_EXP-1, (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));
   new TDataByte( mngrPutExp, L"Indice write", &eth.Cfg.putExpCfg.i_w,   0, 0, MAX_EXP-1, (FLAG_LEVEL_0 | FLAG_RDONLY  | FLAG_SAVE_NO_VALUE));

   if(!m_diagnoseMngr) {
      m_diagnoseMngr = new TDataManager( LBL_DIAG_STATUS, this);
   }
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
   new TData( bufferExpMngr, L"EnableBuffered",           &m_enableBufferedExp,  0,    0,   1,       NULL,    FLAG_NORMAL2, L"Enable buffered sending of ejection orders");
   new TData( bufferExpMngr, L"PLCbufferSize",            &m_PLCBufferSize,      1000, 100, 100000,   L"0x%x",    FLAG_NORMAL2, L"Ejection buffer size in PLC");
   new TData( bufferExpMngr, L"CheckEjectionPeriode",     &m_ejectionPeriode,    10,   10,  1000,       NULL,    FLAG_NORMAL2, L"( in ms) Periode for checking ejection pending list");

  QueryPerformanceFrequency( &m_freq);

   m_periode   = MS_UPDATE;
   m_stopEvent = NULL;
   m_hthread   = NULL;

   //Arracancamos
   IOOpen();
   if ( !IsRunning())                                                         {
      Sleep(500); // Esperamos un tiempo por si no esta arrancado el Twincat
      IOOpen( );  // Intentamos reconectar
   }

//   if ( !IsRunning())                            {
//      TFormWaitDrvT3 *DlgWaitDriverT3 = new TFormWaitDrvT3( NULL, this);
//      if( DlgWaitDriverT3->ShowModal() == mrOk) {
//         // Tras la espera, si NO se ha pulsado cancelar, intentamos reconectar
//         IOOpen( );
//      }
//
//      delete DlgWaitDriverT3;
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



}


//------------------------------------------------------------------------------
void TEtherCATRT3::IOOpen()                                                    {


   AmsAddr     Addr;
   PAmsAddr    pAddr = &Addr;

   // Declare PLC variable which should notify changes
   char sInfoData[]  ={".eth.Data"};
   char sInfoCfg[]   ={".eth.Cfg"};

   // Open communication port on the ADS router
   nPort = AdsPortOpen();
   nErr  = AdsGetLocalAddress(pAddr);
   if (nErr) {
      LOG_ERROR1(L"ADS_ERROR_OPEN_PORT");
   }
   pAddr->port = ETHERCATRT_PORTNUMBER;

   // Get variable handle
   nErr1 = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0,  sizeof(lHlEthInfoData),  &lHlEthInfoData,  sizeof(sInfoData), sInfoData);
   nErr2 = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0,  sizeof(lHlEthInfoCfg),   &lHlEthInfoCfg,  sizeof(sInfoCfg), sInfoCfg);

   if(nErr1==0 && nErr2==0)                                                                {
      m_IOConnected = SetTimers();
      IOConnected = true;
   }  else                                                                     {
      LOG_ERROR1(L"ERROR_IN_ADS");
   }
}

//------------------------------------------------------------------------------
// Destructor
//
TEtherCATRT3::~TEtherCATRT3()                                           {

   KillTimers();
   ::Sleep( 200);
   m_IOConnected = false;

   for( int j = 0; j < NUM_TRIG; j++) {
      CloseHandle(  m_mutexIR[j]);
   }

}
//------------------------------------------------------------------------------
/**
   PutExp @see TIORealTime::PutExp
*/
int  TEtherCATRT3::PutExp( S_EXP *expT2, int num, bool ejectImmediatly)          {

   if ( num==0) {
      return 0;
   }

   if ( !m_IOConnected)                                                       {
      return 0;      //no despachamos evento porque ya deberia haber un faulty
   }

   bool  late = false,
         noroom = false;

   Update();
   S_EXPT3 *exp = new S_EXPT3[num];
   S_EXPT3 *mypExp = exp;
   S_EXP   *mypExpT2 = expT2;
   for (int i = 0; i < num; i++)                                           {
      mypExp->idEjector = mypExpT2->idEjector;
      mypExp->state     = mypExpT2->state;
      mypExp->delay     = mypExpT2->delay;
      mypExp->duration  = mypExpT2->duration;
      mypExp->syncCount = mypExpT2->syncCount;
      mypExp++;
      mypExpT2++;
    }

   if ( !m_enableBufferedExp || ejectImmediatly)                              {
      //si no está habilitada la buffered ejection, entonces tratar de enviar todos
      //al PLC. NO hay que bloquear el mutex

      int cont = MyPutExp( exp, num, late, noroom);
      MyPostPutExp( cont, late, noroom);

      delete [] exp;
      return cont;
   }
   //primero revisar la lista, antes de intentar enviar los que nos acaban de llegar
   int dispatched =  CheckEjectionList( late, noroom);
   //Si está habilitada la buffered ejection, entonces primero hay que
   //limpiar el array que nos han pasado, borrando del mismo
   //los que se han de guardar en la lista, porque su instante de expulsion está tan lejano
   //en el tiempo que no cabe en el buffer
   S_EXPT3 *pExp = exp,
           *pEnd = exp + num;

//         #ifdef _DEBUG
//         FILE *fitx = fopen( "ejection.txt","a");
//         fprintf( fitx, "PutExp: num=%d,\n", num);
//         #endif

   while ( pExp != pEnd )                                                     {
	  __int64 diff =  pExp->syncCount + pExp->delay  - eth.Data.baseTimeData.counter.QuadPart;
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
   if ( num >0) {
      dispatched +=  MyPutExp( exp, num, late, noroom);
   }
   //si ha habido problemas, notificar
   MyPostPutExp( dispatched, late, noroom);

   delete [] exp;
   return  dispatched;
}


//------------------------------------------------------------------------------
/**
   CheckEjectionList
   Este metodo chequea la lista de expulsiones pendientes y envia al PLC aquellas
   que caben en el bufer temporal del PLC
   @return el numero de expulsiones que se han podido pasar al PLC
*/
int TEtherCATRT3::CheckEjectionList( bool &late, bool &noroom)                 {

   int cont = 0;

   if ( m_listExp.empty()) {
      return 0;
   }

   int num = 0; //numero de expulsiones que se escriben en el array de expulsiones a pasar al PLC

         //tomar puntero para recorrer el array
         S_EXPT3 *pExp = m_ejectionArray;
         //calcular el espacio que nos queda en el buffer de intercambio, para no extraer de la lista más de lo que cabe
         int room;
         if ( eth.Data.putExpData.i_r > eth.Cfg.putExpCfg.i_w) {
            room = eth.Data.putExpData.i_r - eth.Cfg.putExpCfg.i_w;
         }
         else {
            room = eth.Data.putExpData.i_r + MAX_EXP - eth.Cfg.putExpCfg.i_w;
         }

         //recorreremos la lista, de los mas antiguos a los mas recientes
         std::list<S_EXPT3>::iterator it;
         it = m_listExp.begin();
         while ( it != m_listExp.end() )                                      {

            __int64 diff =  ( *it).syncCount + (*it).delay  - eth.Data.baseTimeData.counter.QuadPart;

            if ( diff  >= (__int64) m_PLCBufferSize) {
               break;
            }
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
         if ( cont) {
            num = MyPutExp( m_ejectionArray, cont, late, noroom);
         }
   //si no hemos tenido ningun problema con noroom, entonces actualizar el tiempo
   //si hemos tenido problemas de espacio, entonces NO lo actualizamos, para que
   //se reintente cuanto antes
   if ( noroom==false) {
      m_lastCheckPending = timeGetTime();
   }
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
int TEtherCATRT3::MyPutExp( S_EXPT3 *exp, int num, bool &late,  bool &noroom)    {

   if ( !m_IOConnected) {
      return 0;      //no despachamos evento porque ya deberia haber un faulty
   }

   int contador = 0;

   // Añadimos una expulsión al buffer de putExp
   for( int i = 0; i < num; ++i)                                              {

      if (  eth.Cfg.mode == MODE_AUTO               &&
            eth.Data.baseTimeData.counter.QuadPart  &&
            exp[i].syncCount + exp[i].delay <= eth.Data.baseTimeData.counter.QuadPart){

         late = true;
         continue;
      }
      // Comprobamos si hay sitio
      if (  (eth.Cfg.putExpCfg.i_w < MAX_EXP-1   && eth.Cfg.putExpCfg.i_w + 1!= eth.Data.putExpData.i_r) ||
            (eth.Cfg.putExpCfg.i_w == MAX_EXP-1  && eth.Data.putExpData.i_r != 0))   {

         if ( !m_enableEjection) {
            exp[i].state = 0;
         }

         memcpy( &eth.Cfg.putExpCfg.buffer[eth.Cfg.putExpCfg.i_w], &exp[i], sizeof( exp[i]));

         if( ++eth.Cfg.putExpCfg.i_w >= MAX_EXP) {
            eth.Cfg.putExpCfg.i_w   = 0;
         }

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
void TEtherCATRT3::MyPostPutExp( int num, bool late, bool noroom)              {
   //si se ha escrito alguna orden de expulsion en el área de intercambio, entonces
   //enviar los datos modificados al PLC
   if( num > 0) {
      SetCfg();
   }

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
bool  TEtherCATRT3::PutTrigger(unsigned char t)                                {

   eth.Cfg.trigCfg[t].putTrigger   = true;
   SetCfg();
   return true;
}

//------------------------------------------------------------------------------
// SetMode
//
bool  TEtherCATRT3::SetMode( unsigned char mode)                               {

   eth.Cfg.mode  = mode;
   SetCfg();
   return true;
}

//------------------------------------------------------------------------------
// MyGo
//
bool  TEtherCATRT3::MyGo()                                                       {
   eth.Cfg.status   = running;
   SetCfg();
   return TIORealTime::MyGo();
}
//------------------------------------------------------------------------------
// EnableSinc
//
bool  TEtherCATRT3::MyInit()                                                     {

   m_lastFaultyTime     = 0;
   m_lastLatePutExp     = 0;
   m_lastNoRoomPutExp   = 0;
   m_counterSyncTooLate = 0;

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
   CheckVersion( cfgVersion, eth.Data.codeVersion);

   if (! m_IOConnected)                                                       {
      SetFaultyFlag( FAULTY_ETHERCAT_NOT_CONNECTED, 2);
      return false;
   }
   else                                                                       {
      ResetFaultyFlag(FAULTY_ETHERCAT_NOT_CONNECTED);
      if ( m_enableEjection == 0) {
         SetFaultyFlag( FAULTY_EJECTION_DISABLED, 1);
      }
      else {
         ResetFaultyFlag(FAULTY_EJECTION_DISABLED);
      }
   }

   ::Sleep( 10);

   eth.Cfg.status   = ready;

   SetCfg();
   Update();
   m_dispatchControlEnabled = true;
   //limpiar la lista de expulsiones pendientes
   m_listExp.clear();
   return TIORealTime::MyInit();
}

//------------------------------------------------------------------------------
// MyStop
bool  TEtherCATRT3::MyStop()                                                   {

   bool  ret = TIORealTime::MyStop();

   if (ret)                                                                   {

      eth.Cfg.status   = ready;

      ResetFaultyFlag( FAULTY_INDEX_TRIGG_ADVANCED);
      ResetFaultyFlag( FAULTY_EXECUTE_TIME);
      ResetFaultyFlag( FAULTY_IREAD_EQUALS_IWRITE);
      ResetFaultyFlag( FAULTY_FAIL_PUTEXP );

      SetCfg();
   }
   else                                                                       {
      LOG_ERROR1(L"EV_FAIL_STOP_ETHERCAT_REAL_TIME");
      DISPATCH_EVENT (ERROR_CODE,L"EV_FAIL_STOP_ETHERCAT_REAL_TIME", GetLangName(),this);
      return false;
   }

   return ret;
}

//------------------------------------------------------------------------------
// MyStop
bool TEtherCATRT3::MyClose()                                          {

   KillTimers();

   return TIORealTime::MyClose();

}

//-----------------------------------------------------------------------------
// Read configuration
//
bool TEtherCATRT3::Read( LT::TStream *stream, TDataInterface *intf, bool infoModif, bool *forceSave ) {

   m_reading = true;
   bool b = TDataManagerControl::Read( stream, intf, infoModif, forceSave );

   Update();

   memcpy( &trigCfg,  &eth.Cfg.trigCfg[  m_trigToConfig-1],  sizeof( S_TRIG_CFG_T3));
   memcpy( &trigData, &eth.Data.trigData[ m_trigToConfig-1], sizeof( S_TRIG_DATA_T3));

   memcpy( &strobCfg, &eth.Cfg.strobCfg[ m_strobToConfig-1], sizeof( S_STROB_CFG_T3));
   memcpy( &strobData,&eth.Data.strobData[m_strobToConfig-1],sizeof( S_STROB_DATA_T3));

   pStrobSequence->SetUsedSize( strobCfg.tamSec);

   m_reading = false;
   return b;
}

//-----------------------------------------------------------------------------
// DataModifiedPost
//
void __fastcall TEtherCATRT3::DataModifiedPost( TData* da)                     {

   // evitar que la función sea reentrante
   if (m_updating || da==NULL)   {
      return;
   }

   TIORealTime::DataModifiedPost( da);

   m_updating = true;

   // TDatas que no cuelgan de ningun datamanager
   if( da == pEnableEjection && m_enableEjection == 1) {
      ResetFaultyFlag( FAULTY_EJECTION_DISABLED);
   }
   if( da == pEnableEjection && m_enableEjection == 0) {
      SetFaultyFlag( FAULTY_EJECTION_DISABLED, 1);
   }

   // Solo entraremos aquí, tras leer del registry
   if( !m_reading)                                                            {

      // si se ha cambiado el trigger a configurar, seleccionar su configuración
      if ( da == pTrigToConfig )                                              {
         memcpy( &trigCfg, &eth.Cfg.trigCfg[m_trigToConfig-1], sizeof( S_TRIG_CFG_T3));
         if( mngrTrigger)                                                     {
            std::list<TData*>::iterator it = mngrTrigger->DataBegin();
            while (it != mngrTrigger->DataEnd())                              {
               if( (*it)) {
                  (*it)->UpdateFromMem();
               }
               it++;
            }
         }

        m_updating = false;
        return;              //no queremos provocar un SetCfg
      }

      // si se ha modificado el strobe a configurar, seleccionar su configuración
      if ( da == pStrobeToConfig)                                             {
         memcpy( &strobCfg,   &eth.Cfg.strobCfg[m_strobToConfig-1],  sizeof( S_STROB_CFG_T3));
         pStrobSequence->SetUsedSize( strobCfg.tamSec);
         if( mngrStrobe)                                                      {
            std::list<TData*>::iterator it = mngrStrobe->DataBegin();
            while (it != mngrStrobe->DataEnd())                               {
               if( (*it)) {
                  (*it)->UpdateFromMem();
               }
               it++;
            }
         }
         m_updating = false;
         return;     //no queremos provocar un SetCfg
      }

      memcpy( &eth.Cfg.trigCfg[  m_trigToConfig-1],   &trigCfg,   sizeof( S_TRIG_CFG_T3));
      memcpy( &eth.Cfg.strobCfg[ m_strobToConfig-1],  &strobCfg,  sizeof( S_STROB_CFG_T3));
      if ( pStrobSequence) {
         pStrobSequence->SetUsedSize( strobCfg.tamSec);
      }

      // Update if we have modified a trigger or strobe value, UpdateFromMem
      if( da)                                                                 {
         if ( da->GetParent() == mngrTrigger && pTrigConfiguration) {
            pTrigConfiguration->UpdateFromMem();
         }
         if ( da->GetParent() == mngrStrobe && pStrobeConfiguration) {
            pStrobeConfiguration->UpdateFromMem();
         }
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
bool TEtherCATRT3::Update( )                                                   {

     LARGE_INTEGER   counter0,
                     counter,
                     freq;
   __int64           ethTime;

   AmsAddr           Addr;
   PAmsAddr          pAddr = &Addr;

   if (m_IOConnected==false ) {
      return false;
   }

   // Open communication port on the ADS router
   AdsPortOpen();

   nErr =AdsGetLocalAddress(pAddr);
   if (nErr) {
      LOG_ERROR1(L"ADS_ERROR_OPEN_PORT");
   }
   pAddr->port = ETHERCATRT_PORTNUMBER;

   // Measure time
   QueryPerformanceFrequency( (LARGE_INTEGER*)&freq);
   QueryPerformanceCounter((LARGE_INTEGER*)&counter0);

   // Read the struct to the Plc
   nErr = AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHlEthInfoData, sizeof(eth.Data),(void*) &eth.Data);
   if (nErr) {
      LOG_ERROR1(L"ADS_ERROR_READ");
   }

   ethTime =  eth.Data.baseTimeData.time100ns.QuadPart;

   //copy information from Ethernet to local variables
   memcpy( &trigData,   &eth.Data.trigData[m_trigToConfig-1],   sizeof( trigData));
   memcpy( &strobData,  &eth.Data.strobData[m_strobToConfig-1], sizeof( strobData));

   QueryPerformanceCounter((LARGE_INTEGER*)&counter);

   // guardar tiempo de ultima actualización
   m_lastUpdate            = timeGetTime();

   // guardar en variable local tiempo de EtherCAT..., para posterior SyncTimers()
   ethTime = eth.Data.baseTimeData.time100ns.QuadPart;

      __int64 systemTime;
   GetSystemTimeFromEthTime(ethTime, systemTime);
   m_lastTimeAndQueryPerformaceCounter.systemTime = systemTime;
   m_lastTimeAndQueryPerformaceCounter.queryPerformanceCounter = counter;
   m_lastTimeAndQueryPerformaceCounter.ethCounter = eth.Data.baseTimeData.counter;
   m_lastTimeAndQueryPerformaceCounter.queryPerformanceFreq = m_freq;

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
      if ( precision < 1000 )                                                  {
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
         if (eth.Data.trigData[i].ctrl_i_w == CIRC(eth.Cfg.trigCfg[i].ctrl_i_r-1,NTRIG)) {
            // avanzar el índice de lectura
            if (WaitForSingleObject(m_mutexIR[i], 0) == WAIT_OBJECT_0)        {
               eth.Cfg.trigCfg[i].ctrl_i_r   = CIRC( eth.Cfg.trigCfg[i].ctrl_i_r+1, NTRIG);
               unlock = true;
               SetCfg();                           // pasar índice modificado a EtherCAT
               ReleaseMutex( m_mutexIR[i]);
            }
         }

         // si se ha movido algún índice para desbloquear los triggers, dejar registro
         if ( unlock)                                                         {
            wchar_t  indext[48];
            swprintf( indext, L"%s trigger %d", GetLangName(), i);
            LOG_WARN2( L"EV_INDEX_TRIGGER_ADVANCED", indext);
            DISPATCH_EVENT( WARN_CODE, L"EV_INDEX_TRIGGER_ADVANCED", indext, this);
            unlocked = true;
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
void  TEtherCATRT3::MyPeriodic( __int64 &time)                                 {

   TProcess::MyPeriodic( time);
   m_numRepEjectFail = 0;
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
bool TEtherCATRT3::GetEthTimeFromSystemTime( __int64 systemTime, __int64 &ethTime){

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
bool TEtherCATRT3::GetSystemTimeFromEthTime( __int64 ethTime, __int64 &systemTime){

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
void TEtherCATRT3::SetCfg()                                                    {

   AmsAddr     Addr;
   PAmsAddr    pAddr = &Addr;


   if( m_IOConnected)                                                         {

      // Open communication port on the ADS router
      AdsPortOpen();

      nErr = AdsGetLocalAddress(pAddr);
      if (nErr) {
         LOG_ERROR1(L"ADS_ERROR_OPEN_PORT");
      }
      pAddr->port = ETHERCATRT_PORTNUMBER;

       // Write the struct to the Plc
      nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHlEthInfoCfg, sizeof(eth.Cfg), (void*) &eth.Cfg);
      if (nErr) {
           LOG_ERROR1(L"ADS_ERROR_WRITE");
           IOOpen();
      }

      // Quitamos flag de puttriger si se ha activado alguno
      for( int i = 0; i < NUM_TRIG; ++i)                                      {
         eth.Cfg.trigCfg[i].putTrigger = 0;
      }
   }
}

//-----------------------------------------------------------------------------
//
bool TEtherCATRT3::SetTimers()                                                 {

//   if( idTimerUpdate) {
//      KillTimers();
//   }
//   idTimerUpdate  = timeSetEvent( MS_UPDATE, 0, TimerProc, 0, TIME_PERIODIC|TIME_CALLBACK_FUNCTION);
//   return idTimerUpdate;

   DWORD id;

   if (m_hthread)
      return true;

   m_stopEvent    = CreateEvent( NULL, false, false, L"StopTEtherCATRT3Event");

   ResetEvent( m_stopEvent);

   m_hthread   = CreateThread(
               NULL,                // Address of thread security attributes
               (DWORD)0x10000,      // Initial thread stack  size, in bytes
               TEtherCATRT3Thread,      // Address of thread function
               (LPVOID)this,        // Argument for new thread
               0x0,                 // Creation flags
               &id);                // Address of returned thread identifier
   SetThreadName ( id,m_name);
   SetThreadPriority(m_thread, THREAD_PRIORITY_TIME_CRITICAL);

   if( m_hthread )   {  // parche para meter algo en m_hthread, si finalmente se deja el thread habrá que quitarlo
      idTimerUpdate = 1;
   }
   // Return true if thread is created
   return m_hthread ? true: false;

}

//-----------------------------------------------------------------------------
//
void TEtherCATRT3::KillTimers()                                                {
//   if( idTimerUpdate) {
//      timeKillEvent( idTimerUpdate);
//   }
//   idTimerUpdate = NULL;

     // If thread is running, stop and wait until it's stopped
   if (m_hthread)                                                             {
      SetEvent(m_stopEvent);
      if (WaitForSingleObject(m_hthread,3000)==WAIT_TIMEOUT)                  {
         TerminateThread(m_hthread,0);
         CloseHandle(m_hthread);
      }
      else
         CloseHandle(m_hthread);
   }
   if (m_stopEvent)                                                         {
      ResetEvent( m_stopEvent);
      CloseHandle( m_stopEvent);
   }

   idTimerUpdate = 0;

   m_hthread      = NULL;
   m_stopEvent    = NULL;
}

//------------------------------------------------------------------------------

/**
   @param numTrigger is the number of the trigger where to get a information from (starting at 1)
   @param tmax is the desired maximum time for the trigger time. It is measured in microseconds from 1970
   @param capRTData returns Last Trigger, Last Strobe, and Last Synchronysm from the buffer
   @return  true when got a time from buffer of LastTimes lower than tmax
            in that case, the read index for the buffer is incremented
*/
bool TEtherCATRT3::GetLastTrig( int numTrigger, __int64 tmax, s_captureRealTimeData &capRTData){

   // precondition
   if( numTrigger < 1 || numTrigger > NUM_TRIG) {
      return false;
   }

   // update info from EtherCAT to make sure we are working on the latest data
   Update();

   // take control of mutex
   if (WaitForSingleObject(m_mutexIR[numTrigger-1], 5) != WAIT_OBJECT_0)      {
      LOG_WARN2( m_name, L"Failed to obtain mutex for IR." );
      return false;
   }

   try                                                                        {
      unsigned char &i_r            = eth.Cfg.trigCfg[ numTrigger-1].ctrl_i_r;
      S_TRIG_DATA_T3   &ctrlBinTrigger = eth.Data.trigData[numTrigger-1];
      int            i_w            = ctrlBinTrigger.ctrl_i_w;

     // --- comprobar que el control de trigger está habilitado en EtherCAT
      if (eth.Cfg.trigCfg[numTrigger-1].ctrlEnabled==0 && m_dispatchControlEnabled){
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

         GetSystemTimeFromEthTime(ctrlBinTrigger.lastTime[i_r].QuadPart, capRTData.time);
         capRTData.sinc       = ctrlBinTrigger.lastSinc[i_r].QuadPart;
         capRTData.strobe     = ctrlBinTrigger.lastStrobe[i_r];
         capRTData.frameCount = ctrlBinTrigger.frameCount[i_r];
         capRTData.lastResync = eth.Data.baseTimeData.lastReSync.QuadPart;

         if (m_traceFile)                                                     {
            FILE *fitx=fopen("getTrigger.txt","a");
            fprintf( fitx, "num = %d. lastTime[i_r].QuadPart = %Ld, capRTData.time=%Ld, capRTData.sinc=%Ld Lastcycle = %d\n" ,
            numTrigger,  ctrlBinTrigger.lastTime[i_r].QuadPart, capRTData.time, capRTData.sinc, eth.Data.baseTimeData.lastReSync);
            fclose(fitx);
         }

         // select next trigger & update to EtherCAT
         if( ++i_r >= NTRIG) {
            i_r = 0;
         }
         SetCfg();

         // update visible i_r at the interface
         if( m_trigToConfig == numTrigger) {
            trigCfg.ctrl_i_r = i_r;
         }

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
      //  release trigger
      ReleaseMutex( m_mutexIR[numTrigger-1]);
   }

   return false;
}
//-------------------------------------------------------------------------------


void TEtherCATRT3::AddErrorText( std::wstring &errorText)                      {

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
/**  MyCheckDiagnose
*/
bool TEtherCATRT3::MyCheckDiagnose(e_diagnose_type type_diagnose, TBasicDiagnose *diag)    {

   wchar_t txt[256];
   swprintf(txt, L"\r\nETHERCAT\r\n");
   diag->Add( txt, wcslen( txt) * sizeof(wchar_t));

   //Check if driver is installed
   if (eth.Data.masterOk == false)                                                 {
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
      if (pCodeVersion->AsInt() != eth.Data.codeVersion)                           {
         swprintf(str, L"\t\t\tNOT OK\r\n");
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      else {
         swprintf( str, L"\t\t\tOK\r\n");
      }
      diag->Add( str, wcslen( str) * sizeof(wchar_t));
      }

   //Check Pulses origin
   if ((eth.Cfg.baseTimeCfg.Sync == 2) || (eth.Cfg.baseTimeCfg.Sync == 0))          {
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
   else if (eth.Cfg.baseTimeCfg.Sync == 1)                                       {
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
   if( eth.Cfg.mode == MODE_AUTO)                                                 {
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
      if( eth.Cfg.trigCfg[i].enabled)                                            {
         wchar_t txt1[256];
         swprintf(txt1,  L"\tTrigger:%d : TriggerTime:%d, CicleTime:%d", i, eth.Cfg.trigCfg[i].triggerTime, eth.Cfg.trigCfg[i].cycleTime);
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

         if ( eth.Cfg.trigCfg[i].cycleTime != eth.Cfg.trigCfg[i].cycleTime)         {
            swprintf(txt1, L"\tNOT OK\r\n");
            diag->SetDiagStatus( FAIL_REAL_TIME);
         }
         else {
            swprintf(txt1, L"\tOK\r\n");
         }
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
      if( eth.Cfg.strobCfg[i].enabled)                                           {
         wchar_t txt1[256];
         swprintf(txt1,  L"\tStrobe:%d : StrobeTime:%d\r\n", i,  eth.Cfg.strobCfg[i].strobeTime);
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

      s_test_driver_T3 res;
      TestDriver( res, 10000);

      //--- Funcionamiento de los terminales
      // Número de terminales configurados
      swprintf( txt, L"\t%s\t: %d ", LoadText( L"MSG_ETHERCAT_DEVICES"), res.numTerminales);
      diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
      if( m_numTerminales != res.numTerminales)                               {
         swprintf(txt, L"\t\t\tNOT OK\r\n");
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      else {
         swprintf(txt, L"\t\t\tOK\r\n");
      }
      diag->Add( txt, wcslen( txt) * sizeof(wchar_t));

      // Estado de terminales
      if( res.falloTerminales)                                                {
         wchar_t txt1[128];
         swprintf( txt1, L"\t%s\t: %s %d\r\n", LoadText( L"MSG_DEVICES"), LoadText(L"MSG_FAILURE_DEVICE") ,res.falloTerminales);
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      //-- Comprobamos pulsos de encoder
      if( (eth.Cfg.baseTimeCfg.Sync == 2) || (eth.Cfg.baseTimeCfg.Sync == 0))       {
         wchar_t txt1[128];
         swprintf( txt1, L"\t%s\t: %.1f\tpulses/s", LoadText( L"MSG_PULSE_FREQ"),  res.pulsEncoder);
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

         if(   res.pulsEncoder   < m_minPulsEnc || res.pulsEncoder   > m_maxPulsEnc) {
            swprintf(txt1, L"\t\tNOT OK\r\n");
            diag->SetDiagStatus( FAIL_REAL_TIME);
         }
         else {
            swprintf(txt1, L"\t\tOK\r\n");
         }
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

         swprintf( txt1, L"\t%s\t: %.f pulses", LoadText(L"MSG_PULSE_TURN"), res.pulsEncoderVC);
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

         // Comprobamos que entre vueltas completas tenemos los pulsos que tocan( máximo 4 pulsos de error)
         #define PULS_ERROR 4
         if( m_vueltaCompleta)                                                {
            int dif = ( (unsigned int)res.pulsEncoderVC - m_pulsosPorVuelta);
            dif = abs(dif);
            while (dif > m_pulsosPorVuelta - PULS_ERROR && dif < m_pulsosPorVuelta + PULS_ERROR) {
               dif = abs(dif - m_pulsosPorVuelta);
            }
            wchar_t txt2[128];

            if( res.pulsEncoderVC == 0 || dif >= PULS_ERROR)                  {
               swprintf(txt2, L"\t\t\tNOT OK\r\n");
               diag->SetDiagStatus( FAIL_REAL_TIME);
            }
            else {
               swprintf( txt2, L"\t\t\tOK\r\n");
            }
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
//         diag->SignalAsErrored(this);
      }
      else {
         swprintf(str7, L"\t\tOK\r\n");
      }
      diag->Add( str7, wcslen( str7) * sizeof(wchar_t));
   }

   return true;

}
//------------------------------------------------------------------------------
/** TestDriver
*/
void TEtherCATRT3::TestDriver( s_test_driver_T3 &res, int time)                   {

   double            ctime;
   LARGE_INTEGER     freq,
                     counter[2];
  S_INFO_DATA         info[2];

   QueryPerformanceFrequency( &freq);
   memset( &res, 0x00, sizeof(res));

   // Cogemos parámetros temporales
   if( Update())  {                                 // Contadores de la máquina
      memcpy( &info[0], &eth.Data, sizeof( eth.Data));
   }
   else                                                     {
      return;
   }

   QueryPerformanceCounter( &counter[0]);    // Tiempo actual de la máquina

   while(1)                                                                   {
      // coger valor de los contadores
      if( Update( ))                                                          {  // Contadores de la máquina
         QueryPerformanceCounter( &counter[1]);                // Tiempo actual de la máquina
         memcpy( &info[1], &eth.Data, sizeof( eth.Data));
      }

      ctime = (counter[1].QuadPart-counter[0].QuadPart)*1000./freq.QuadPart;

      if ( info[1].baseTimeData.lastReSync.QuadPart != info[0].baseTimeData.lastReSync.QuadPart) {
         res.pulsEncoderVC = info[1].baseTimeData.lastReSync.QuadPart - info[0].baseTimeData.lastReSync.QuadPart;
         info[0].baseTimeData.lastReSync.QuadPart = info[1].baseTimeData.lastReSync.QuadPart;
      }

      if( ctime > time) {
         break;
      }
      Sleep(100);
   }

   res.pulsTimer   = (info[1].baseTimeData.timer  - info[0].baseTimeData.timer)    *1000./ctime;
   res.pulsEncoder = (info[1].baseTimeData.counter.QuadPart  - info[0].baseTimeData.counter.QuadPart)  *1000./ctime;

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
void TEtherCATRT3::SyncTimers( __int64 perfCounter,__int64 ethTime100ns)       {

   static __int64 lastSystemTime    = 0,
                  lastEthTime       = 0;
   static int     validCount        = 0;

   __int64        systemTime        =  LT::Time::GetTime( perfCounter),
                  systemTime100ns   =  systemTime * 10,
                  st;

   //--- comprobar error para generar traza si supera los 2 ms
   if( m_SyncLastTime)                                                        {
      GetSystemTimeFromEthTime( ethTime100ns, st);
      int   diff        = systemTime - st;

      if( abs( diff) > 2 * 1000)                                              {
         wchar_t message[256];
         swprintf(message, L"Time Sync Error (EtherCAT <-> System): dif = %.1f ms. Synced : %.1f Count: %d", diff/1000., (systemTime-m_SyncLastTime)/1000., validCount);
         LOG_INFO1(message);
         if( --validCount < 0) {
            validCount  = 0;
         }
      }
      else if( ++validCount > 100) {
         validCount     = 100;
      }
   }

   // sólo ajustamos si la sincronización es 100% válida, o ha fallado mucho
   if( validCount < 95 || validCount == 100)                                  {
      if ( WaitForSingleObject( m_mutex2, 0) == WAIT_OBJECT_0)                {
         try                                                                  {

            //--- calculo deriva

            // si es la primera vez, guardar pareja de tiempos
            if( !lastSystemTime)                                              {
               lastEthTime    = ethTime100ns;
               lastSystemTime = systemTime100ns;
            }

            // calculamos deriva sólo si han pasado más de 10s
            else if( systemTime100ns - lastSystemTime > 10 * 1000 * 10)       {

               // diferencia de tiempo pasado en EtherCAT y System
               m_SyncDeriva   = 1.0 - (double)(systemTime100ns - lastSystemTime) / (double)(ethTime100ns - lastEthTime);

               lastEthTime    = ethTime100ns;
               lastSystemTime = systemTime100ns;
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
TProcessData *TEtherCATRT3::MyProcess( TProcessData *processData)              {

   TProcessData *ret =  TIORealTime::MyProcess( processData);
   //si no esta habilitado
   if ( (!m_enableBufferedExp) ||  ( GetMode() == MODE_MANUAL)) {
      return ret;
   }

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

