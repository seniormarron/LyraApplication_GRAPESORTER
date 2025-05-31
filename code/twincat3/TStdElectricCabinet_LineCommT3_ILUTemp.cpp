//---------------------------------------------------------------------------
// Modulo:        TStdBoardEtherCAT.cpp
// Comentario:
//
//---------------------------------------------------------------------------

#pragma hdrstop

#include <math.h>

#include "common.h"
#include "TDataExt.h"

#include "TStdElectricCabinet_LineCommT3_ILUTemp.h"

#ifndef _MSC_VER 
#include "DlgWaitDriverT3.h"
#else
#include "DlgWaitDriverT3Qt.h"
#include "QtAux.h"
#include "TModuleMngr.h"
#endif

//---------------------------------------------------------------------------

#ifdef __BORLANDC__
   #pragma package(smart_init)
#endif

//-----------------------------------------------------------------------------
// Datos para la callback
TStdElectricCabinet_LineCommT3_ILUTemp      *stdBoard_lineComT3              = NULL;

HANDLE mutex_l_T3_ILUTEMP  = CreateMutex( NULL, false, NULL);
HANDLE mutex2_l_T3_ILUTEMP = CreateMutex( NULL, false, NULL);

//------------------------------------------------------------------------------

// Callback con la ejecución de la tarea de IO de control del cuadro eléctrico
// La llamamos cada 40 tick ( 10 ms)
void CALLBACK TStdElectricCabinet_LineCommT3_ILUTemp::TimerProc_Cuadro( UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
   if (WaitForSingleObject( mutex_l_T3_ILUTEMP, INFINITE)==WAIT_OBJECT_0) {
#ifdef _MSC_VER
      __try {
#else
      try {
#endif

         if( stdBoard_lineComT3 && stdBoard_lineComT3->inited)                    {

            stdBoard_lineComT3->Watchdog();
            stdBoard_lineComT3->SetCfg();
            stdBoard_lineComT3->Update();
         }
      }
      __finally                                                               {
         ReleaseMutex( mutex_l_T3_ILUTEMP);
      }
   }
}

//------------------------------------------------------------------------------
/**
*/
void CALLBACK TStdElectricCabinet_LineCommT3_ILUTemp::TimerProc_CheckStates( UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
   if (WaitForSingleObject( mutex2_l_T3_ILUTEMP, INFINITE)==WAIT_OBJECT_0) {
#ifdef _MSC_VER
      __try {
#else
      try {
#endif

         if( stdBoard_lineComT3 && stdBoard_lineComT3->inited)                    {

            //Aquí se comprobarán los estados de la máquina tales como:
            // Carga, expulsión
            stdBoard_lineComT3->CheckState();
            stdBoard_lineComT3->ConvertStateToDigitalAnalog();

         }
      }
      __finally                                                               {
         ReleaseMutex( mutex2_l_T3_ILUTEMP);
      }
   }
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//*************** Class TStdElectricCabinet_LineCommT3_ILUTemp --- **********************
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
   Constructor
*/
TStdElectricCabinet_LineCommT3_ILUTemp::TStdElectricCabinet_LineCommT3_ILUTemp( wchar_t *_name, TDataManagerControl *parent)
   :  TStdElectricCabinet_LineComm_Base( _name, parent),
      TEtherCAT3( this)                                                        {

   // Puntero a la dirección de EtherCAT ( para la callback)
   stdBoard_lineComT3   = this;

   // Aun no estamos inicializados
   inited               = false;

   // Configuración de la tarea de IO
   m_IOConnected        = false;

	event_temp           = false;
   event_hopper         = false;
   event_min_pressure   = false;

   //--- Control del cuadro eléctrico
   new TData(     this, L"Estado soft",         (int *)&(inCuadro.ESTADO_SOFT),       0,   0x80000000, 0x7FFFFFFF, NULL, FLAG_NORMAL & ~FLAG_SAVE);
   new TDataByte( this, L"DigitalOutput_1",     (bool *)&(inCuadro.DIGITAL_OUTPUT_1), false, (FLAG_NORMAL | FLAG_RDONLY) & ~FLAG_SAVE);
   new TDataByte( this, L"DigitalOutput_2",     (bool *)&(inCuadro.DIGITAL_OUTPUT_2), false, (FLAG_NORMAL | FLAG_RDONLY) & ~FLAG_SAVE);
   new TDataByte( this, L"DigitalOutput_3",     (bool *)&(inCuadro.DIGITAL_OUTPUT_3), false, (FLAG_NORMAL | FLAG_RDONLY) & ~FLAG_SAVE);
   new TData(  0, this, L"AnalogOutput",        (int *)&(inCuadro.ANALOG_OUTPUT),   (FLAG_NORMAL | FLAG_RDONLY) & ~FLAG_SAVE);
   // temperature control
   m_daTempMngr = new TDataManager(L"Temperature_Control", this);
   new TData (m_daTempMngr, L"CtrlTempIlu",     (int *)&(inCuadro.CTRL_TEMP_ILU),     2,  0, 2, NULL, FLAG_NORMAL, L"Number of temperature controls"  );
   new TData (m_daTempMngr, L"TemperaturaIlu1", (float *)&(outCuadro.TEMPERATURA_ILU_1),     0.00,   0.00, 125.00, L"%.2f", (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"Temperature 1 measured in ºC");
   new TData (m_daTempMngr, L"TemperaturaIlu2", (float *)&(outCuadro.TEMPERATURA_ILU_2),     0.00,   0.00, 125.00, L"%.2f", (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"Temperature 2 measured in ºC");
   new TData (m_daTempMngr, L"MaxWorkingTemp", &m_maxTemp, 20.00, 0.00, 125.00, L"%.2f", (FLAG_NORMAL), L"Maximum working temperature permited ºC");
   new TData (m_daTempMngr, L"MinWorkingTemp", &m_minTemp, 10.00, 0.00, 125.00, L"%.2f", (FLAG_NORMAL), L"Minimum working temperature permited ºC");
   // hopper level control
   m_daHopperMngr = new TDataManager(L"Hopper_Control", this);
   new TDataWord(m_daHopperMngr, L"MaximumLevelExceed", &outCuadro.SIGNAL_SWICH_HOPPER_LEVEL, 0, 0, 1, (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"Level maximum value exceed. Indicates there is no external feeding permission");
   new TDataWord(m_daHopperMngr, L"HopperPhotocelVal",  &outCuadro.SIGNAL_ANALOG_HOPPER_LEVEL,  0, 0, 32767, (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"Analog value readed from hooper photocel");
   new TData (m_daHopperMngr, L"CalibrateMaxLevel",   &m_maxValLevel,      32767,   0,    32767,   NULL, FLAG_NORMAL, L"Analog value for define maximum level");
   new TData (m_daHopperMngr, L"CalibrateMinLevel",   &m_minValLevel,      550,     0,    32767,   NULL, FLAG_NORMAL, L"Analog value for define minimum level");
   new TData (m_daHopperMngr, L"PercentajeMaxLevel",  &m_percMaxLevel,     100,     0,    100,     NULL, FLAG_NORMAL, L"Percentage asociated to the maximum level analog value");
   new TData (m_daHopperMngr, L"PercentajeMinLevel",  &m_percMinLevel,     0,       0,    100,     NULL, FLAG_NORMAL, L"Percentage asociated to the minimum level analog value");
   new TData (m_daHopperMngr, L"NormalizedLevel",     &m_levelNormalized,  0.0,     0.0,  1.0,     L"%.1f", (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"Hopper ocupation between defined range CalibrateMaxLevel-CalibrateMinLevel. Normalized [0-1]");
   new TData (m_daHopperMngr, L"PercentajeLevel",     &m_levelPercentage,  0,       0,    100,     NULL, (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"Hopper ocupation percentage. % unit.");
   // pressure control 
   m_daPressureMngr = new TDataManager(L"Pressure_Control", this);
   new TDataWord(m_daPressureMngr, L"TerminalPressureVal",   &outCuadro.SIGNAL_ANALOG_AIR_PRESSURE, 0, 0, 32767, (FLAG_LEVEL_0  | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"Analog value readed from mesuring presure terminal");
   new TData (m_daPressureMngr, L"InputPressure",          &m_inputPressure,       0.00, 0.00, 10.00,   L"%.2f", (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"Input supply pressure. BAR unit.");
   new TData( m_daPressureMngr, L"MinimumSupplyPressure",    &m_minPressure,         0.00, 0.00, 10.00, L"%.2f", (FLAG_LEVEL_0 | FLAG_SAVE), L"Minimum supply pressure for alarm fault.");
   new TData (m_daPressureMngr, L"MaxValueWhenOpenValve",  &m_maxValPressTest,     0, 0, 32767,         NULL, FLAG_NORMAL , L"Maximum value permited when open a valve in a valves block test at Init().");

   daEstadoEtherCAT = new TData( this, L"Estado ethercat",     (int *)&(outCuadro.ESTADO),       0,   0x80000000, 0x7FFFFFFF, NULL, (FLAG_NORMAL | FLAG_RDONLY) & ~FLAG_SAVE);

   //TDatas con el significado de cada bit de estado ( hasta 1-31, el 0 es bit de marcha)
   m_bitStateMngr = new TBitStateMngr( L"State bit manager", this);
   //Arracancamos
   IOOpen();

   if ( !IsRunning())                                                         {
#if !(defined(_DEBUG) && defined(DEBUG_NOETHERCATWAITDIALOG))
#ifndef _MSC_VER 
      TFormWaitDrvT3 *DlgWaitDriverT3 = new TFormWaitDrvT3( NULL, this);
      if( DlgWaitDriverT3->ShowModal() == mrOk)
         // Tras la espera, si NO se ha pulsado cancelar, intentamos reconectar
         IOOpen( );

      delete DlgWaitDriverT3;
#else
      TFormWaitDrvT3 *DlgWaitDriverT3 = new TFormWaitDrvT3(
          QtAux::GetMainWindow(), this,
          dynamic_cast<TModuleMngr *>(this->GetRootMngr()));

      if (DlgWaitDriverT3->exec() == QDialog::Accepted) {
         // Tras la espera, si NO se ha pulsado cancelar, intentamos reconectar
         IOOpen();
      }

      delete DlgWaitDriverT3;
#endif
#endif
   }

   m_idTimer_checkStates = NULL;
   inited   = true;

   m_form         = NULL;


   m_dataModPostManager = this;
   m_manExternalComm->m_dataModPostManager = this;


}
//------------------------------------------------------------------------------

// Destructor
//
TStdElectricCabinet_LineCommT3_ILUTemp::~TStdElectricCabinet_LineCommT3_ILUTemp()   {

   if (WaitForSingleObject( mutex_l_T3_ILUTEMP, INFINITE)==WAIT_OBJECT_0)     {
      stdBoard_lineComT3   = NULL;
   }

   ReleaseMutex( mutex_l_T3_ILUTEMP);


   if (WaitForSingleObject( mutex2_l_T3_ILUTEMP, INFINITE)==WAIT_OBJECT_0)    {
      stdBoard_lineComT3   = NULL;
   }

   ReleaseMutex( mutex2_l_T3_ILUTEMP);


   KillTimers();
   ::Sleep( 200);
   m_IOConnected = false;

}


//-----------------------------------------------------------------------------
/**
*/
void TStdElectricCabinet_LineCommT3_ILUTemp::IOOpen()                         {

   AmsAddr     Addr;
   PAmsAddr    pAddr = &Addr;

   // Declare PLC variable which should notify changes
   // ToDo: esto no puede estar a pelo poner en un tdata si no no es compatible con diferentes cuadros.
   char sOutputSoft[]={"N007001.OUTPUTS_SOFT"};
   char sInputSoft []={"N007001.INPUTS_SOFT"};

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

   if(nErr1==0 && nErr2==0)                                                   {
      m_IOConnected = SetTimers();
      IOConnected = true;
   } else                                                                     {
      LOG_ERROR1(L"ERROR_IN_ADS");
   }

}

//------------------------------------------------------------------------------
/**
*/
bool TStdElectricCabinet_LineCommT3_ILUTemp::MyInit()                         {

   bool ret = TStdElectricCabinet_LineComm_Base::MyInit();

   // Avisamos de cambios de version
   DWORD cfgVersion = pCodeVersion->AsInt();
   CheckVersion( cfgVersion, outCuadro.CODE_VERSION);

   m_counterWatchdog = 0;
   m_watchdog        = false;

   /// Si está habilitada la comunicación externa
   if (m_enabledExternalComm)                                                 {

      if (!m_idTimer_checkStates)
         m_idTimer_checkStates = timeSetEvent( 1000, 0, TimerProc_CheckStates, 0, TIME_PERIODIC|TIME_CALLBACK_FUNCTION );
   }

   if (! m_IOConnected)                                                       {
      SetFaultyFlag( FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD, 2);
      DISPATCH_EVENT( WARN_CODE, L"EV_ETHERCAT_NOT_CONNECTED_STDBOARD" , L"", this);
      LOG_WARN1(LoadText(L"EV_ETHERCAT_NOT_CONNECTED_STDBOARD"));
      return false;
   }
   else                                                                       {
      ResetFaultyFlag(FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD);
   }

   return ret;
}
//------------------------------------------------------------------------------



// SetCfg
// Actualizar estados de la configuración del PLC
void TStdElectricCabinet_LineCommT3_ILUTemp::SetCfg()                         {


   AmsAddr     Addr;
   PAmsAddr    pAddr = &Addr;


   // Open communication port on the ADS router
   AdsPortOpen();
   nErr = AdsGetLocalAddress(pAddr);
   if (nErr) {
      LOG_ERROR1(L"ADS_ERROR_OPEN_PORT");
   }
   pAddr->port = CUADRO_ELECTRICO_PORTNUMBER;

   if( m_IOConnected)
                                                                              {
      // Write the struct to the Plc
      nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlInput, sizeof(inCuadro), (void*) &inCuadro);
      if (nErr) {
           LOG_ERROR1(L"ADS_ERROR_WRITE");
           IOOpen();
      }
   }
}

//------------------------------------------------------------------------------
/**
*/
void TStdElectricCabinet_LineCommT3_ILUTemp::Watchdog()                       {

   //Cambiamos cada 500 ms
   m_counterWatchdog++;
   if (m_counterWatchdog > 50)                                                {
      //se cambia el estado
      m_watchdog = !m_watchdog;
      m_counterWatchdog = 0;
   }

   if( m_watchdog)
      SetSoftState( inCuadro.ESTADO_SOFT | WATCHDOG);
   else
      SetSoftState( inCuadro.ESTADO_SOFT & ~WATCHDOG);
}

//------------------------------------------------------------------------------
/**
   ConvertStateToDigitalAnalog
*/
void TStdElectricCabinet_LineCommT3_ILUTemp::ConvertStateToDigitalAnalog()    {

   //Hay que convertir el currentState, en el valor analógico y digital
   //Analógico
   if (m_manAnalog)                                                           {
      TData *da = m_manAnalog->GetDataObject( m_commNames[m_currentState]);
      if (da)
         inCuadro.ANALOG_OUTPUT = da->AsInt();
   }

   if (m_manDigital)                                                          {
      TData *da = m_manDigital->GetDataObject( m_commNames[m_currentState]);
      if (da)                                                                 {
         inCuadro.DIGITAL_OUTPUT_1 = da->AsInt() & 0x1;
         inCuadro.DIGITAL_OUTPUT_2 = da->AsInt() & 0x2;
         inCuadro.DIGITAL_OUTPUT_3 = da->AsInt() & 0x4;
      }
   }
}
//-----------------------------------------------------------------------------
/**
*/
bool TStdElectricCabinet_LineCommT3_ILUTemp::SetTimers()                       {

   idTimer_Cuadro       = timeSetEvent( 20, 0,TimerProc_Cuadro, 0, TIME_PERIODIC|TIME_CALLBACK_FUNCTION );

   if ( idTimer_Cuadro)
      return true;

   return false;
}

//-----------------------------------------------------------------------------
/**
*/
void TStdElectricCabinet_LineCommT3_ILUTemp::KillTimers()                        {

   // events are no longer needed
   MMRESULT res = timeKillEvent( idTimer_Cuadro);
   if ( res != TIMERR_NOERROR)                                                   {
      if (res == MMSYSERR_INVALPARAM) {
         LOG_ERROR1( L"Error killing electric cabinet timer: timer event doesn't exist");
      }
      else
         LOG_ERROR1( L"Error killing electric cabinet timer:unknow error");
   }

   // events are no longer needed
   res = timeKillEvent( m_idTimer_checkStates);
   if ( res != TIMERR_NOERROR)                                                   {
      if (res == MMSYSERR_INVALPARAM) {
         LOG_ERROR1( L"Error killing electric cabinet checkStates timer: timer event doesn't exist");
      }
      else
         LOG_ERROR1( L"Error killing electric cabinet checkStates timer:unknow error");
   }
}

//------------------------------------------------------------------------------
// Update
// Actualiza las entradas y salidas del contorl del cuadro eléctrico
bool TStdElectricCabinet_LineCommT3_ILUTemp::Update( )                           {
   AmsAddr     Addr;
   PAmsAddr    pAddr = &Addr;


   // Open communication port on the ADS router
   AdsPortOpen();
   nErr = AdsGetLocalAddress(pAddr);
   if (nErr) {
      LOG_ERROR1(L"ADS_ERROR_OPEN_PORT");
   }
   pAddr->port = CUADRO_ELECTRICO_PORTNUMBER;

   if( m_IOConnected)                                                            {

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

//------------------------------------------------------------------------------
/**
*/
void TStdElectricCabinet_LineCommT3_ILUTemp::AddErrorText( std::wstring &errorText) {

   TElectricCabinet::AddErrorText( errorText);

   if ( (m_state.faultyFlags & FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD) || (m_state.faultyFlags & (FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD << 16)))   {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText( L"FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD");
      errorText = errorText + L"\r\n";
   }

   if ((m_state.faultyFlags & FAULTY_ILU_TEMP_IS_TOO_LOW) || (m_state.faultyFlags & (FAULTY_ILU_TEMP_IS_TOO_LOW << 16))) {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText(L"FAULTY_ILU_TEMP_IS_TOO_LOW");
      errorText = errorText + L"\r\n";
   }
   if ((m_state.faultyFlags & FAULTY_ILU_TEMP_IS_TOO_HIGHT) || (m_state.faultyFlags & (FAULTY_ILU_TEMP_IS_TOO_HIGHT << 16))) {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText(L"FAULTY_ILU_TEMP_IS_TOO_HIGHT");
      errorText = errorText + L"\r\n";
   }
   if ((m_state.faultyFlags & FAULTY_HOPPER_LEVEL_EXCEED) || (m_state.faultyFlags & (FAULTY_HOPPER_LEVEL_EXCEED << 16))) {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText(L"FAULTY_HOPPER_LEVEL_EXCEED");
      errorText = errorText + L"\r\n";
   }
   if ((m_state.faultyFlags & FAULTY_LOW_SUPPLY_PRESSURE) || (m_state.faultyFlags & (FAULTY_LOW_SUPPLY_PRESSURE << 16))) {
      errorText = errorText + GetLangName();
      errorText = errorText + L":";
      errorText = errorText + LoadText(L"FAULTY_LOW_SUPPLY_PRESSURE");
      errorText = errorText + L"\r\n";
   }
   
   
}

//------------------------------------------------------------------------------
/**
*/
bool TStdElectricCabinet_LineCommT3_ILUTemp::MyCheckDiagnose(e_diagnose_type type_diagnose, TBasicDiagnose *diag)    {

   wchar_t txt[256];
   swprintf(txt, L"\r\n%s\r\n", LoadText(L"MSG_ELECTRIC_CABINET"));
   diag->Add( txt, wcslen( txt) * sizeof(wchar_t));

   //Check Versions
   if (pCodeVersion)                                                          {
      wchar_t str[128];
      swprintf(str,  L"\t%s:%d", LoadText(L"MSG_ETHERCAT_VERSION"), pCodeVersion->AsInt());
      diag->Add( str, wcslen( str) * sizeof(wchar_t));
      if ((unsigned)pCodeVersion->AsInt() != outCuadro.CODE_VERSION)          {
         swprintf(str, L"\t\t\tNOT OK\r\n");
         diag->SetDiagStatus( FAIL_REAL_TIME);
      }
      else
         swprintf( str, L"\t\t\tOK\r\n");
      diag->Add( str, wcslen( str) * sizeof(wchar_t));
   }

   return true;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/**
*/
void TStdElectricCabinet_LineCommT3_ILUTemp::DataModifiedPost( TData *da)     {

   TElectricCabinet::DataModifiedPost( da);

   if ( da && (da == m_daShowForm) && da->AsInt())                            {
#ifndef _MSC_VER 
      if (m_form == NULL)
         m_form = new TConditionsLineCommForm( Application->MainForm, this);

      if ( m_form )                                                           {

               m_form->FormStyle = fsStayOnTop;
               m_form->CheckMode( m_autoMode);
               m_form->Show();

      }
#else
      if (m_form == NULL)
         m_form = new TConditionsLineCommForm(QtAux::GetMainWindow(), this);

      if (m_form) {
         m_form->CheckMode(m_autoMode);
         m_form->show();

      }
#endif
      da->SetAsInt( 0);
   }

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/**
*/
void TStdElectricCabinet_LineCommT3_ILUTemp::CheckState()                     {

   //Si la máquina no está en marcha, se le envia el estado de STOP
   if ( !(inCuadro.ESTADO_SOFT & MARCHA_APP))                                 {
      m_currentState = STOP;
      return;
   }

   TStdElectricCabinet_LineComm_Base::CheckState();

}

/**
MyPeriodic
Calls to TDataManagerControl::MyPeriodic, and
resets faulty if it has been a long time since last signaled
*/
void  TStdElectricCabinet_LineCommT3_ILUTemp::MyPeriodic(__int64 &time)       {

   // Default actions
   TDataManagerControl::MyPeriodic(time);

   // -- Sensory devices
   IlluminationTemperatureControl();
   HopperLevelControl();
   PressureControl();
}

//------------------------------------------------------------------------------
/**
   IlluminationTemperatureControl
   
*/
void  TStdElectricCabinet_LineCommT3_ILUTemp::IlluminationTemperatureControl() {

   float temp1 = outCuadro.TEMPERATURA_ILU_1;
   float temp2 = outCuadro.TEMPERATURA_ILU_2;

   if (temp1 > m_maxTemp || temp2 > m_maxTemp) {
      // exccesive illumination temperature
      SetFaultyFlag(FAULTY_ILU_TEMP_IS_TOO_HIGHT, 2);
      if (event_temp == false) {
         DISPATCH_EVENT(ERROR_CODE, L"EV_ILU_TEMP_IS_TOO_HIGHT", NULL, this);
         LOG_WARN1(LoadText(L"EV_ILU_TEMP_IS_TOO_HIGHT"));
         event_temp = true;
      }
   }
   else if (temp1 < m_minTemp || temp2 < m_minTemp) {
      // illumination temperature not enought
      SetFaultyFlag(FAULTY_ILU_TEMP_IS_TOO_LOW, 2);
      if (event_temp == false) {
         DISPATCH_EVENT(ERROR_CODE, L"EV_ILU_TEMP_IS_TOO_LOW", NULL, this);
         LOG_WARN1(LoadText(L"EV_ILU_TEMP_IS_TOO_LOW"));
         event_temp = true;
      }
   }
   else {
      ResetFaultyFlag(FAULTY_ILU_TEMP_IS_TOO_HIGHT);
      ResetFaultyFlag(FAULTY_ILU_TEMP_IS_TOO_LOW);
      event_temp = false;
   }

}

//------------------------------------------------------------------------------
/**
   IlluminationTemperatureControl
*/
void  TStdElectricCabinet_LineCommT3_ILUTemp::HopperLevelControl()               {


   if (( m_minValLevel - m_maxValLevel) == 0)                                    {
      DISPATCH_EVENT(WARN_CODE, L"EV_HOPPER_PERCENTAGES_WRONG_CONFIGURED", NULL, this);
      LOG_WARN1(LoadText(L"EV_HOPPER_PERCENTAGES_WRONG_CONFIGURED"));
      return;
   }

   // Calculate normalized level between max and min possible values.
   m_levelNormalized = (outCuadro.SIGNAL_ANALOG_HOPPER_LEVEL - m_minValLevel) * 1.0 / (m_maxValLevel - m_minValLevel);

   // Scaled value between m_percMaxLevel and m_percMinLevel
   m_levelPercentage = (m_levelNormalized ) * (m_percMaxLevel - m_percMinLevel) + m_percMinLevel;
   // Margins verify
   m_levelPercentage = (m_levelPercentage < m_percMinLevel) ? m_percMinLevel : ((m_levelPercentage > m_percMaxLevel) ? m_percMaxLevel : m_levelPercentage);


   // Alarms  
   if (outCuadro.SIGNAL_SWICH_HOPPER_LEVEL == 1)                                 {
      // Hopper level exceed the maximum level
      SetFaultyFlag(FAULTY_HOPPER_LEVEL_EXCEED, 2);
      if (event_hopper == false)                                                 {
         DISPATCH_EVENT(ERROR_CODE, L"EV_HOPPER_LEVEL_EXCEED", NULL, this);
         LOG_ERROR(LoadText(L"EV_HOPPER_LEVEL_EXCEED"));
         event_hopper = true;
      }
   }
   else                                                                          {
      ResetFaultyFlag(FAULTY_HOPPER_LEVEL_EXCEED);
      event_hopper = false;
   }
}

//------------------------------------------------------------------------------
/**
   PressureControl
*/
void  TStdElectricCabinet_LineCommT3_ILUTemp::PressureControl()                  {

   // Calculate pressure in bars from analog terminal readed value
   m_inputPressure = outCuadro.SIGNAL_ANALOG_AIR_PRESSURE * 1.0 / PRESSURE_FACTOR_CALIBRATION;  // miliBares

   // Alarms 
   if (m_inputPressure < m_minPressure)                                          {
      // supply pressure too low
      SetFaultyFlag(FAULTY_LOW_SUPPLY_PRESSURE, 2);
      if (event_min_pressure == false)                                           {
         DISPATCH_EVENT(ERROR_CODE, L"EV_LOW_SUPPLY_PRESSURE", NULL, this);
         LOG_ERROR(LoadText(L"EV_LOW_SUPPLY_PRESSURE"));
         event_min_pressure = true;
      }
   }
   else                                                                          {
      ResetFaultyFlag(FAULTY_LOW_SUPPLY_PRESSURE);
      event_min_pressure = false;
   }
}









