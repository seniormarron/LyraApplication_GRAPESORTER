//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TStdElectricCabinet.h
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

#include "common.h"
#include <math.h>
#include "TStdElectricCabinet.h"

#pragma package(smart_init)

//-----------------------------------------------------------------------------
// Datos para la callback
TStdElectricCabinet      *stdBoard              = NULL;
HANDLE mutex = CreateMutex( NULL, false, NULL);

//------------------------------------------------------------------------------

// Callback con la ejecución de la tarea de IO de control del cuadro eléctrico
// La llamamos cada 40 tick ( 10 ms)
void CALLBACK TStdElectricCabinet::TimerProc_Cuadro( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
   if (WaitForSingleObject( mutex, INFINITE)==WAIT_OBJECT_0)                  {
      try                                                                     {
         if( stdBoard && stdBoard->inited)                                    {
            stdBoard->SetCfg();
            stdBoard->Update();
         }
      }
      __finally                                                               {
         ReleaseMutex( mutex);
      }
   }
}

//---------------------------------------------------------------------------
/*
   TStdElectricCabinet
 */
TStdElectricCabinet::TStdElectricCabinet( wchar_t *_name, TDataManagerControl *parent)
   :  TElectricCabinet( _name, parent),
      TEtherCAT( this)                                                        {

   // Puntero a la dirección de EtherCAT ( para la callback)
   stdBoard      = this;

   // Configuración de la tarea de IO
   m_IOConnected  = false;
   idTimer_Cuadro = NULL;

   inited         = false;
   m_stateMarcha  = false;

   inCuadro.Arranca_alimentacion = 0;
   inCuadro.Arranca_cinta = 0;
   inCuadro.Para_cinta = 0;

   //--- Control del cuadro eléctrico
   new TData( this, L"Estado soft",         (int *)&(inCuadro.ESTADO_SOFT),   0,   0x80000000, 0x7FFFFFFF, NULL, FLAG_NORMAL & ~FLAG_SAVE);
   daEstadoEtherCAT = new TData( this, L"Estado ethercat",     (int *)&(outCuadro.ESTADO),       0,   0x80000000, 0x7FFFFFFF, NULL, (FLAG_NORMAL | FLAG_RDONLY) & ~FLAG_SAVE);

   m_pArrancaCinta   = new TDataByte( this, L"ARRANCA_CINTA",          (bool *)&(inCuadro.Arranca_cinta),         false, FLAG_NORMAL  & ~FLAG_SAVE, L"Pulsador para arrancar cinta");
   m_pParaCinta      = new TDataByte( this, L"PARA_CINTA",             (bool *)&(inCuadro.Para_cinta),            false, FLAG_NORMAL  & ~FLAG_SAVE, L"Pulsador para parar cinta");
   m_pArrancaAliment = new TDataByte( this, L"ARRANCA_ALIMENTACION",   (bool *)&(inCuadro.Arranca_alimentacion),  false, FLAG_NORMAL  & ~FLAG_SAVE, L"Interruptor para arrancar alimentación");
                       new TDataByte( this, L"PERMISO_LLAVE",          (bool *)&(outCuadro.Llave_permiso),        false, (FLAG_NORMAL | FLAG_RDONLY) & ~FLAG_SAVE, L"Lectura estado permiso llave visión");

   //TDatas con el significado de cada bit de estado ( hasta 1-31, el 0 es bit de marcha)
   m_bitStateMngr = new TBitStateMngr( L"State bit manager", this);

   //Arracancamos
   IOOpen();
   if ( !IsRunning())                                                         {
      Sleep(500); // Esperamos un tiempo por si no esta arrancado el Twincat
      IOOpen( );  // Intentamos reconectar
   }

  inited   = true;

   DataModEventPost = DataModifiedPost;
}

//---------------------------------------------------------------------------
/*
   ~TStdElectricCabinet
 */
TStdElectricCabinet::~TStdElectricCabinet()                                    {

   KillTimers();
   ::Sleep( 200);

   if (WaitForSingleObject( mutex, INFINITE)==WAIT_OBJECT_0) {
      stdBoard   = NULL;
   }

   ReleaseMutex( mutex);

   m_IOConnected = false;
}

//---------------------------------------------------------------------------
/*
   IOOpen
 */
void TStdElectricCabinet::IOOpen()                                             {

   IOOpenParam( CUADRO_ELECTRICO_PORTNUMBER, (void**)&pInputs_Cuadro,  sizeof(Cuadro_Electrico_Outputs), (void**)&pOutputs_Cuadro,   sizeof(Cuadro_Electrico_Inputs));
}

//---------------------------------------------------------------------------
/*
   MyInit
 */
bool TStdElectricCabinet::MyInit()                                            {

   // Avisamos de cambios de version
   DWORD cfgVersion = pCodeVersion->AsInt();
   CheckVersion( cfgVersion, outCuadro.CODE_VERSION);

   // Me aseguro que la cinta y la alimentación estan paradas.
//   m_pArrancaCinta->SetAsInt(0);
//   m_pArrancaAliment->SetAsInt(0);

   if (! m_IOConnected)                                                        {
      SetFaultyFlag( FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD, 2);
      DISPATCH_EVENT( WARN_CODE, L"EV_ETHERCAT_NOT_CONNECTED_STDBOARD" , L"", this);
      LOG_WARN1(LoadText(L"EV_ETHERCAT_NOT_CONNECTED_STDBOARD"));
      return false;
   }
   else                                                                       {
      ResetFaultyFlag(FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD);
   }
   return true;
}

//---------------------------------------------------------------------------
/*
   Read
 */
bool TStdElectricCabinet::Read( LT::TStream *stream, TDataInterface *intf, bool infoModif, bool *forceSave  )    {

  inited   = false;

  bool b = TDataManagerControl::Read( stream, intf, infoModif, forceSave );

  inited   = true;

   return b;
}

//---------------------------------------------------------------------------
/*
   SetCfg
 */
void TStdElectricCabinet::SetCfg()                                            {

   if( m_IOConnected)                                                         {
      // Actualizamos las outputs (Estados y cfg enviada desde el PC al nivel de EtherCAT)
      if( memcmp( pInputs_Cuadro,      &inCuadro,   sizeof(inCuadro)))
         memcpy(  pInputs_Cuadro,      &inCuadro,   sizeof(inCuadro));

      // start the I/O update and field bus cycle
      TCatIoOutputUpdate( CUADRO_ELECTRICO_PORTNUMBER );
   }
}


//---------------------------------------------------------------------------
/*
   SetTimers
 */
bool TStdElectricCabinet::SetTimers()                                         {

   if ( idTimer_Cuadro)
      KillTimers();

   idTimer_Cuadro       = timeSetEvent( 20, 0,TimerProc_Cuadro, 0, TIME_PERIODIC|TIME_CALLBACK_FUNCTION );
   if ( idTimer_Cuadro)
      return true;
   return false;
}

//---------------------------------------------------------------------------
/*
   KillTimers
 */
void TStdElectricCabinet::KillTimers()                                        {

   // events are no longer needed
   MMRESULT res = timeKillEvent( idTimer_Cuadro);
   if ( res != TIMERR_NOERROR)                                                {
      if (res == MMSYSERR_INVALPARAM) {
         LOG_ERROR1( L"Error killing electric cabinet timer: timer event doesn't exist");
      }
      else
         LOG_ERROR1( L"Error killing electric cabinet timer:unknow error");
   }
   else
      idTimer_Cuadro = NULL;

}

//---------------------------------------------------------------------------
/*
   Update
 */
bool TStdElectricCabinet::Update( )                                           {
   // Actualizamos las entradas (Estados enviados desde EtherCAT a nivel de PC)
   if( m_IOConnected)                                                         {

      if( TCatIoInputUpdate( CUADRO_ELECTRICO_PORTNUMBER ) == 0)              {
         memcpy( &outCuadro,    pOutputs_Cuadro,    sizeof( outCuadro));
      }

      // WE've to be careful, some of the status bits are processed here, and also can be accesed
      if( m_bitStateMngr)
         m_bitStateMngr->ProcessStatus( outCuadro.ESTADO);

	  daEstadoEtherCAT->SetAsInt( outCuadro.ESTADO);
   }
   return true;
}

//---------------------------------------------------------------------------
/*
   CheckHayFallo
 */
int TStdElectricCabinet::CheckHayFalloNoArranque()                            {

	// Leer los bits de menor peso
   DWORD estado = outCuadro.ESTADO & FALLO_MASK;

   //  No leer el bit de fallo llave ya que este si que permite arrancar
   estado = estado & ~FALLO_NO_HAY_LLAVE;

   // Mascara para ver el bit de estado
   switch( estado)                                     						  {
      case FALLO_EMERGENCIA:
         return 2;
	  case FALLO_PERM_EXTERNO:
		 return 4;
      default:
		 return 0;
   }

   return 0;
}



//---------------------------------------------------------------------------
/*
   AddErrorText
 */
void TStdElectricCabinet::AddErrorText( std::wstring &errorText)              {

   TElectricCabinet::AddErrorText( errorText);

   if ( (m_state.faultyFlags & FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD) || (m_state.faultyFlags & (FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD << 16)))   {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText( L"FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD");
      errorText = errorText + L"\r\n";
   }

}

//---------------------------------------------------------------------------
/*
   MyCheckDiagnose
 */
bool TStdElectricCabinet::MyCheckDiagnose(e_diagnose_type type_diagnose, TBasicDiagnose *diag) {

   wchar_t txt[256];
   swprintf(txt, L"\r\n%s\r\n", LoadText(L"MSG_ELECTRIC_CABINET"));
   diag->Add( txt, wcslen( txt) * sizeof(wchar_t));

   //Check Versions
   if (pCodeVersion)                                                          {
      wchar_t str[128];
      swprintf(str,  L"\t%s:%d", LoadText(L"MSG_ETHERCAT_VERSION"), pCodeVersion->AsInt());
      diag->Add( str, wcslen( str) * sizeof(wchar_t));
      if ((unsigned)pCodeVersion->AsInt() != outCuadro.CODE_VERSION)           {
         swprintf(str, L"\t\t\tNOT OK\r\n");
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      else
         swprintf( str, L"\t\t\tOK\r\n");
      diag->Add( str, wcslen( str) * sizeof(wchar_t));
   }

   return true;
}


void __fastcall TStdElectricCabinet::DataModifiedPost( TData* da)             {

   // Accion pulsador devolver el Tdata a cero
   if( da == m_pArrancaCinta && da->AsInt() )                                 {
      for (__int64 i = 0; i < 100000000; i++)                                {
         // waiting a little bit time before change de value
         int waiting = 1;
      }
      inCuadro.Arranca_cinta = 0;
   }

   // Accion pulsador devolver el Tdata a cero
   if( da == m_pParaCinta && da->AsInt() )                                    {
      for (__int64 i = 0; i < 100000000; i++)                                 {
         // waiting a little bit time before change de value
         int waiting = 1;
      }
      inCuadro.Para_cinta = 0;
   }

   // Accion guardar el cambio de estado de marcha
   if( da == daEstadoEtherCAT)                                                {

	  // Leer los bits de menor peso
	  DWORD estado = outCuadro.ESTADO & FALLO_MASK;

	  //  No leer el bit de fallo llave ya que este si que permite la marcha
	  estado = estado & ~FALLO_NO_HAY_LLAVE;

      // Mascara para ver el bit de estado
      switch( estado)                                                         {
         case BIT_MARCHA:
            m_stateMarcha = true;
            break;
         case FALLO_EMERGENCIA:
            m_stateMarcha = false;
            break;
         case FALLO_PERM_EXTERNO:
            m_stateMarcha = false;
            break;
         default:
            m_stateMarcha = false;
            break;
      }
   }


}


















//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
