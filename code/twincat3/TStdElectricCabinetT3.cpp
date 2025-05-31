//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TStdElectricCabinetT3.cpp
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

#include "common.h"
#include <math.h>
#include "TStdElectricCabinetT3.h"

//#include "DlgWaitDriverT3.h"


//---------------------------------------------------------------------------

#pragma package(smart_init)

//-----------------------------------------------------------------------------
// Datos para la callback
TStdElectricCabinetT3      *stdBoardT3              = NULL;

HANDLE mutexT3 = CreateMutex( NULL, false, NULL);
//------------------------------------------------------------------------------

// Callback con la ejecución de la tarea de IO de control del cuadro eléctrico
// La llamamos cada 40 tick ( 10 ms)
void CALLBACK TStdElectricCabinetT3::TimerProc_Cuadro( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
   if (WaitForSingleObject( mutexT3, INFINITE)==WAIT_OBJECT_0) {
      try {

         if( stdBoardT3 && stdBoardT3->inited)                                    {

            stdBoardT3->Watchdog();
            stdBoardT3->SetCfg();
            stdBoardT3->Update();
         }
      }
      __finally                                                               {
         ReleaseMutex( mutexT3);
      }
   }
}
//------------------------------------------------------------------------------

// Constructor
//
TStdElectricCabinetT3::TStdElectricCabinetT3( wchar_t *_name, TDataManagerControl *parent)
   :  TElectricCabinet( _name, parent),
      TEtherCAT3( this)                                                        {


   stdBoardT3     = this;     // Puntero a la dirección de EtherCAT ( para la callback)
   inited         = false;    // Aun no estamos inicializados
   m_IOConnected  = false;     // Configuración de la tarea de IO
   idTimer_Cuadro = NULL;

   //--- Control del cuadro eléctrico
   //
//   new TData( this, L"Marcha soft",         (int *)&(inCuadro.MARCHA_SOFT),   0,   0,          1,          NULL, FLAG_NORMAL & ~FLAG_SAVE);
//   new TData( this, L"Paro soft",           (int *)&(inCuadro.PARO_SOFT),     0,   0,          1,          NULL, FLAG_NORMAL & ~FLAG_SAVE);
                      new TData( this, L"Estado soft", (int *)&(inCuadro.ESTADO_SOFT), 0,  0x80000000, 0x7FFFFFFF, NULL, FLAG_NORMAL & ~FLAG_SAVE);
   daEstadoEtherCAT = new TData( this, L"Estado ethercat", (int *)&(outCuadro.ESTADO), 0,  0x80000000, 0x7FFFFFFF, NULL, (FLAG_NORMAL | FLAG_RDONLY) & ~FLAG_SAVE);
//   pCodeVersion = new TData( this, L"Code version",  NULL, 0,   0,          0x7FFFFFFF, NULL, (FLAG_NORMAL | FLAG_RDONLY));

   //TDatas con el significado de cada bit de estado ( hasta 1-31, el 0 es bit de marcha)
   m_bitStateMngr = new TBitStateMngr( L"State bit manager", this);

   IOOpen();    //Arracancamos
   if ( !IsRunning())                                                         {
      Sleep(500); // Esperamos un tiempo por si no esta arrancado el Twincat
      IOOpen( );  // Intentamos reconectar
   }


//   if ( !IsRunning())                            {
//      TFormWaitDrvT3 *DlgWaitDriverT3 = new TFormWaitDrvT3( NULL, this);
//      if( DlgWaitDriverT3->ShowModal() == mrOk)   {
//         // Tras la espera, si NO se ha pulsado cancelar, intentamos reconectar
//         IOOpen( );
//      }
//      delete DlgWaitDriverT3;
//   }

  inited   = true;

}
//------------------------------------------------------------------------------

// Destructor
//
TStdElectricCabinetT3::~TStdElectricCabinetT3()                                    {

   KillTimers();
   ::Sleep( 200);

   if (WaitForSingleObject( mutexT3, INFINITE)==WAIT_OBJECT_0) {
      stdBoardT3   = NULL;
   }

   ReleaseMutex( mutexT3);
   m_IOConnected = false;

}
//-----------------------------------------------------------------------------

void TStdElectricCabinetT3::IOOpen()                                             {

   AmsAddr     Addr;
   PAmsAddr    pAddr = &Addr;

   // Declare PLC variable which should notify changes
   char sOutputSoft[]={"N003092.OUTPUTS_SOFT"};
   char sInputSoft []={"N003092.INPUTS_SOFT"};

   // Open communication port on the ADS router
   nPort = AdsPortOpen();
   nErr  = AdsGetLocalAddress(pAddr);
   if (nErr) {
      LOG_ERROR1(L"ADS_ERROR_OPEN_PORT");
   }
   pAddr->port = CUADRO_ELECTRICO_PORTNUMBER;

   // Get variable handle
   nErr1 = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0,  sizeof(lHdlInput), &lHdlInput,  sizeof(sInputSoft),  sInputSoft);
   nErr2 = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0,  sizeof(lHlOutput), &lHlOutput,  sizeof(sOutputSoft), sOutputSoft);

   if(nErr1==0 && nErr2==0)                                                                {
      m_IOConnected = SetTimers();
      IOConnected = true;
   } else                                                                     {
      LOG_ERROR1(L"ERROR_IN_ADS");
   }

}

//------------------------------------------------------------------------------

bool TStdElectricCabinetT3::MyInit()                                            {

   // Avisamos de cambios de version
   DWORD cfgVersion = pCodeVersion->AsInt();
   CheckVersion( cfgVersion, outCuadro.CODE_VERSION);

   m_counterWatchdog = 0;
   m_watchdog        = false;

   if (! m_IOConnected)                                                        {
      SetFaultyFlag( FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD, 2);
      return false;
   }
   else                                                                       {
      ResetFaultyFlag(FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD);
   }
   return true;
}
//------------------------------------------------------------------------------

// RdRegistry
//
bool TStdElectricCabinetT3::Read( LT::TStream *stream, TDataInterface *intf, bool infoModif, bool *forceSave  )    {
  inited   = false;

   bool b = TDataManagerControl::Read( stream, intf, infoModif, forceSave );

//   Update ();

   inited   = true;

   return b;
}
//-----------------------------------------------------------------------------
// SetCfg
// Actualizar estados de la configuración del PLC
void TStdElectricCabinetT3::SetCfg()                                            {

   AmsAddr     Addr;
   PAmsAddr    pAddr = &Addr;

   // Open communication port on the ADS router
   AdsPortOpen();
   nErr = AdsGetLocalAddress(pAddr);
   if (nErr) {
      LOG_ERROR1(L"ADS_ERROR_OPEN_PORT");
      IOOpen();
   }
   pAddr->port = CUADRO_ELECTRICO_PORTNUMBER;

   if( m_IOConnected)
                                                        {
      // Write the struct to the Plc
      nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlInput, sizeof(inCuadro), (void*) &inCuadro);
      if (nErr) {
           LOG_ERROR1(L"ADS_ERROR_WRITE");
      }
   }
}
//-----------------------------------------------------------------------------
void TStdElectricCabinetT3::Watchdog()                                          {

   //Cambiamos cada 500 ms
   m_counterWatchdog++;
   if (m_counterWatchdog > 50)                                                {
      //se cambia el estado
      m_watchdog = !m_watchdog;
      m_counterWatchdog = 0;
   }

   if( m_watchdog) {
      SetSoftState( inCuadro.ESTADO_SOFT | WATCHDOG);
   }
   else {
      SetSoftState( inCuadro.ESTADO_SOFT & ~WATCHDOG);
   }
}

//-----------------------------------------------------------------------------

bool TStdElectricCabinetT3::SetTimers()                                         {

   if ( idTimer_Cuadro) {
      KillTimers();
   }

   idTimer_Cuadro = timeSetEvent( 20, 0,TimerProc_Cuadro, 0, TIME_PERIODIC|TIME_CALLBACK_FUNCTION );
   if ( idTimer_Cuadro) {
      return true;
   }
   return false;
}

//-----------------------------------------------------------------------------


void TStdElectricCabinetT3::KillTimers()                                        {

   // events are no longer needed
   MMRESULT res = timeKillEvent( idTimer_Cuadro);
   if ( res != TIMERR_NOERROR)                                                {
      if (res == MMSYSERR_INVALPARAM) {
         LOG_ERROR1( L"Error killing electric cabinet timer: timer event doesn't exist");
      }
      else {
         LOG_ERROR1( L"Error killing electric cabinet timer:unknow error");
      }
   }
   else {
      idTimer_Cuadro = NULL;
   }

}
//-----------------------------------------------------------------------------
// Update
// Actualiza las entradas y salidas del contorl del cuadro eléctrico
bool TStdElectricCabinetT3::Update( )                                            {

   AmsAddr     Addr;
   PAmsAddr    pAddr = &Addr;

   // Open communication port on the ADS router
   AdsPortOpen();
   nErr = AdsGetLocalAddress(pAddr);
   if (nErr) {
      LOG_ERROR1(L"ADS_ERROR_OPEN_PORT");
   }
   pAddr->port = CUADRO_ELECTRICO_PORTNUMBER;

   if( m_IOConnected)                                                         {

      // Read the struct to the Plc
      nErr = AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHlOutput, sizeof(outCuadro), (void*) &outCuadro);
      if (nErr) {
           LOG_ERROR1(L"ADS_ERROR_READ");
      }
      daEstadoEtherCAT->SetAsInt( outCuadro.ESTADO);

      // WE've to be careful, some of the status bits are processed here, and also can be accesed
      if( m_bitStateMngr) {
         m_bitStateMngr->ProcessStatus( outCuadro.ESTADO);
      }
   }
   return true;
}
//-----------------------------------------------------------------------------
/**
*/
void TStdElectricCabinetT3::AddErrorText( std::wstring &errorText)              {

   TElectricCabinet::AddErrorText( errorText);

   if ( (m_state.faultyFlags & FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD) || (m_state.faultyFlags & (FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD << 16)))   {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText( L"FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD");
      errorText = errorText + L"\r\n";
   }

}
//------------------------------------------------------------------------------
/**
*/
bool TStdElectricCabinetT3::MyCheckDiagnose(e_diagnose_type type_diagnose, TBasicDiagnose *diag)    {

   wchar_t txt[256];
   swprintf(txt, L"\r\n%s\r\n", LoadText(L"MSG_ELECTRIC_CABINET"));
   diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
//   diag->RegisterNode(this);
   //Check Versions
   if (pCodeVersion)                                                          {
      wchar_t str[128];
      swprintf(str,  L"\t%s:%d", LoadText(L"MSG_ETHERCAT_VERSION"), pCodeVersion->AsInt());
      diag->Add( str, wcslen( str) * sizeof(wchar_t));
      if ((unsigned)pCodeVersion->AsInt() != outCuadro.CODE_VERSION)           {
         swprintf(str, L"\t\t\tNOT OK\r\n");
         diag->SetDiagStatus( FAIL_REAL_TIME);
//         diag->SignalAsErrored(this);
      }
      else {
         swprintf( str, L"\t\t\tOK\r\n");
      }
      diag->Add( str, wcslen( str) * sizeof(wchar_t));
   }

   return true;
}



//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
