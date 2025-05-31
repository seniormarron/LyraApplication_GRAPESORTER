//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TAdvantechMonitor.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   Advantech
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "TAdvantechMonitor.h"
#include "TAux.h"


#pragma package(smart_init)

//-----------------------------------------------------------------------------
//*************** Class TAdvantechMonitor *************************************
//-----------------------------------------------------------------------------
/**
   Constructor
   @param      name     wchar_t pointer with the name
   @param      parent   Parent node in the tree structure (its type is TDataManager)
   @return     void
*/
TAdvantechMonitor::TAdvantechMonitor ( wchar_t *_name, TDataManagerControl *parent)
 : TDataManagerControl( _name, parent)                                        {


   new TData(this, L"MaxTempCPU",                &m_maxTempCPU,      87, 0, 100,    NULL, FLAG_NORMAL);
   new TData(this, L"MaxTempCPUStop",            &m_maxTempCPUStop,  95, 0, 100,    NULL, FLAG_NORMAL);
   new TData(this, L"MaxTempSYS",                &m_maxTempSYS,      45, 0, 100,    NULL, FLAG_NORMAL);
   new TData(this, L"MaxTempSYSStop",            &m_maxTempSYSStop,  55, 0, 100,    NULL, FLAG_NORMAL);
   new TData(this, L"MinRPMVent",                &m_minRPMVent,     100, 0, 10000, NULL, FLAG_NORMAL);

   TDataManager *mnInfo = new TDataManager( L"Info", this);
   m_daManufacturer  =  new TData( mnInfo, L"Manufacturer",  m_manufacturer, L"",   SIZEOF_CHAR( m_manufacturer),   (FLAG_NORMAL | FLAG_RDONLY) &~FLAG_SAVE,  L"Board manufacturer");
   m_daBName         =  new TData( mnInfo, L"BName",         m_Bname,        L"",   SIZEOF_CHAR( m_Bname),          (FLAG_NORMAL | FLAG_RDONLY) &~FLAG_SAVE,  L"Board name");

   new TData( mnInfo, L"TempCPU",         &m_TempCPU,      0, 0, 100,    NULL, (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"CPU temperature");
   new TData( mnInfo, L"TempSYS",         &m_TempSYS,      0, 0, 100,    NULL, (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"System temperature");
   new TData( mnInfo, L"FANCPU",          &m_FANCPU,       0, 0, 100,    NULL, (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"CPU FAN speed");
   new TData( mnInfo, L"FANSYS1",         &m_FANSYS1,      0, 0, 100000, NULL, (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"System FAN speed");
   new TData( mnInfo, L"FANSYS2",         &m_FANSYS2,      0, 0, 100000, NULL, (FLAG_LEVEL_0 | FLAG_RDONLY | FLAG_SAVE_NO_VALUE), L"System 2 FAN speed");


   m_susiInit = false;

   if ( SusiLibInitialize() == SUSI_STATUS_SUCCESS )                          {
     m_susiInit = true;
     m_susi = 4;
   }
   else                                                                       {
      m_susiAvaiable = SusiHWMAvailable();
      m_susiInit     = SusiDllInit();
      if (m_susiAvaiable && m_susiInit)
         m_susi = 3;
   }

   m_oldTempCPU   = 0;
   m_oldCTempCPU   = 0;


}
//------------------------------------------------------------------------------
/**
   Destructor
*/
TAdvantechMonitor::~TAdvantechMonitor()                                       {

}

//-----------------------------------------------------------------------------
bool TAdvantechMonitor::MyInit()                                              {


   if ( m_susiInit && m_susi == 4 )                                           {

      char manufacturer[32],
           Bname[32];

      unsigned int sizeManufacturer = sizeof( manufacturer);
      unsigned int sizeName         = sizeof( Bname);

      SusiBoardGetStringA( SUSI_ID_BOARD_MANUFACTURER_STR, manufacturer, &sizeManufacturer);
      wchar_t *wManufacturer = TAux::STR_TO_WSTR(manufacturer);
      m_daManufacturer->SetAsText( wManufacturer);
      delete[]wManufacturer;

	  //TODO: NO FUNCIONA CORRECTAMENTE EL  SUSI_ID_BOARD_NAME_STR
//	  SusiBoardGetStringA( SUSI_ID_BOARD_NAME_STR, Bname, &sizeName);
//	  wchar_t *wBoardName = TAux::STR_TO_WSTR(Bname);
//	  m_daBName->SetAsText( Bname);
//	  delete[]wBoardName;
   }
   return TDataManagerControl::MyInit();

}

//------------------------------------------------------------------------------
/**
   MyPeriodic for check monitoring values

   Test Rápido
*/
void TAdvantechMonitor::MyPeriodic( __int64 &time)                            {

   TDataManagerControl::MyPeriodic( time);
   if ( m_susiInit )                                                          {

      unsigned int  fanC;
      unsigned int  fanS;
      unsigned int  fanS1;

      if ( m_susi == 4) {

         unsigned int TempCPU;
		 unsigned int TempSYS;


		 SusiBoardGetValue( SUSI_ID_HWM_TEMP_CPU,     &TempCPU);
		 //Comprobar números negativos
		 if (TempCPU > CELCIUS) 												  {
			TempCPU = SUSI_DECODE_CELCIUS( TempCPU);
		 }
		 else																  {
			 TempSYS = 0;
		 }

		 SusiBoardGetValue( SUSI_ID_HWM_TEMP_SYSTEM,  &TempSYS);

		 //Comprobar números negativos
		 if (TempSYS > CELCIUS) 												  {
			TempSYS = SUSI_DECODE_CELCIUS( TempSYS);
		 }
		 else																  {
			 TempSYS = 0;
		 }

		 SusiBoardGetValue( SUSI_ID_HWM_FAN_CPU,    &fanC);
		 SusiBoardGetValue( SUSI_ID_HWM_FAN_SYSTEM, &fanS);
		 SusiBoardGetValue( SUSI_ID_HWM_FAN_OEM0,   &fanS1);

		 m_TempCPU = TempCPU;
         m_TempSYS = TempSYS;
         m_FANCPU  = fanC;
         m_FANSYS1 = fanS;
		 m_FANSYS2 = fanS1;
	  }
	  else if (m_susi == 3)                                                   {
         float TempCPU;
         float TempSYS;
		 WORD  fanCPU;
         WORD  fanSYS;
         WORD  fanSYS2;

         SusiHWMGetTemperature(TCPU, &TempCPU);       //Get CPU Temperature
         SusiHWMGetTemperature(TSYS, &TempSYS);       //Get SYS Temperature
         SusiHWMGetFanSpeed   (FCPU, &fanCPU);        //Get CPU Fan Speed
         SusiHWMGetFanSpeed   (FSYS, &fanSYS);        //Get SYS Fan Speed
         SusiHWMGetFanSpeed   (F2ND, &fanSYS2);       //Get SYS2 Fan Speed

         fanC  = fanCPU;
         fanS  = fanSYS;
         fanS1 = fanSYS2;

         m_TempCPU = TempCPU;
         m_TempSYS = TempSYS;
         m_FANCPU  = fanC;
         m_FANSYS1 = fanS;
         m_FANSYS2 = fanS1;
      }

      //Max temperature CPU
	  if  (m_TempCPU > m_maxTempCPU)                                            {
         wchar_t cadena[256];
         swprintf(cadena, L"%d", (int)m_TempCPU);
         if (m_oldTempCPU < (int)m_TempCPU)                                     {
            DISPATCH_EVENT( WARN_CODE, L"EV_HIGH_TEMPERATURE_CPU", cadena, this);
            LOG_WARN2(LoadText(L"EV_HIGH_TEMPERATURE_CPU"), cadena);
            m_oldTempCPU = m_TempCPU;
         }
         else
            m_oldTempCPU = m_TempCPU;
         SetFaultyFlag(FAULTY_HIGH_TEMPERATURE_CPU, 1);
	  }
      else if (m_oldTempCPU !=0)                                              {
         wchar_t cadena[256];
         swprintf(cadena, L"%d", (int)m_TempCPU);
         DISPATCH_EVENT( INFO_CODE, L"EV_HIGH_TEMPERATURE_CPU", cadena, this);
         LOG_INFO2(LoadText(L"EV_HIGH_TEMPERATURE_CPU"), cadena);
         m_oldTempCPU = 0;
      }
      else                                                                    {
         ResetFaultyFlag(FAULTY_HIGH_TEMPERATURE_CPU);
      }
      //Critical temperature for CPU
      if (m_TempCPU > m_maxTempCPUStop)                                         {
         wchar_t cadena[256];
         wsprintf(cadena, L"%d", m_TempCPU);
         if (m_oldCTempCPU < (int)m_TempCPU)                                     {
            DISPATCH_EVENT( WARN_CODE, L"EV_CRITICAL_TEMPERATURE_CPU", cadena, this);
            LOG_WARN2(LoadText(L"EV_CRITICAL_TEMPERATURE_CPU"), cadena);
            m_oldCTempCPU = m_TempCPU;
         }
         else
			m_oldCTempCPU = m_TempCPU;

         SetFaultyFlag(FAULTY_CRITICAL_TEMPERATURE_CPU, 2);
      }
      else if (m_oldCTempCPU !=0)                                              {
         wchar_t cadena[256];
         swprintf(cadena, L"%d", (int)m_TempCPU);
         DISPATCH_EVENT( INFO_CODE, L"EV_CRITICAL_TEMPERATURE_CPU", cadena, this);
         LOG_INFO2(LoadText(L"EV_CRITICAL_TEMPERATURE_CPU"), cadena);
         m_oldCTempCPU = 0;
      }
      else                                                                    {
         ResetFaultyFlag(FAULTY_CRITICAL_TEMPERATURE_CPU);
      }
      //Max system temperature
	  if (m_TempSYS > m_maxTempSYS)                                             {
		 wchar_t cadena[256];
		 wsprintf(cadena, L"%d", m_TempSYS);
         DISPATCH_EVENT( WARN_CODE, L"EV_HIGH_TEMPERATURE_SYS", cadena, this);
         LOG_WARN2(LoadText(L"EV_HIGH_TEMPERATURE_SYS"), cadena);
         SetFaultyFlag(FAULTY_HIGH_TEMPERATURE_SYS, 1);
      }
      else                                                                    {
         ResetFaultyFlag(FAULTY_HIGH_TEMPERATURE_SYS);
      }
      //Critical system temperature
      if (m_TempSYS > m_maxTempSYSStop)                                         {
         wchar_t cadena[256];
         wsprintf(cadena, L"%d", m_TempSYS);
         DISPATCH_EVENT( WARN_CODE, L"EV_CRITICAL_TEMPERATURE_SYS", cadena, this);
         LOG_WARN2(LoadText(L"EV_CRITICAL_TEMPERATURE_SYS"), cadena);
         SetFaultyFlag(FAULTY_CRITICAL_TEMPERATURE_SYS, 2);
      }
      else                                                                    {
         ResetFaultyFlag(FAULTY_CRITICAL_TEMPERATURE_SYS);
      }


      //CPU fan Speed is stopped
      if ( m_FANCPU < m_minRPMVent)                                               {
         DISPATCH_EVENT( WARN_CODE, L"EV_STOP_CPU_FAN_SPEED", NULL, this);
         LOG_WARN1(LoadText(L"EV_STOP_CPU_FAN_SPEED"));
         SetFaultyFlag(FAULTY_STOP_CPU_FAN_SPEED, 2);
      }
      else                                                                    {
         ResetFaultyFlag(FAULTY_STOP_CPU_FAN_SPEED);
      }
      //SYSTEM fan Speed is stopped
      if ( m_FANSYS1 < m_minRPMVent)                                               {
         DISPATCH_EVENT( WARN_CODE, L"EV_STOP_SYS_FAN_SPEED", NULL, this);
         LOG_WARN1(LoadText(L"EV_STOP_SYS_FAN_SPEED"));
         SetFaultyFlag(FAULTY_STOP_SYS_FAN_SPEED, 2);
      }
      else                                                                    {
         ResetFaultyFlag(FAULTY_STOP_SYS_FAN_SPEED);
      }
      //SYSTEM2 fan speed is stopped
      if ( m_FANSYS2 < m_minRPMVent)                                              {
         DISPATCH_EVENT( WARN_CODE, L"EV_STOP_SYS_FAN_SPEED", NULL, this);
         LOG_WARN1(LoadText(L"EV_STOP_SYS_FAN_SPEED"));
         SetFaultyFlag(FAULTY_STOP_SYS_FAN_SPEED, 2);
      }
      else                                                                    {
         ResetFaultyFlag(FAULTY_STOP_SYS_FAN_SPEED);
      }
   }
}
//-----------------------------------------------------------------------------
/**
*/
bool TAdvantechMonitor::MyCheckDiagnose(e_diagnose_type type_diagnose,
      TBasicDiagnose *diag)                                                   {

      wchar_t txt[256];
      swprintf( txt,  L"\r\n%s\r\n", LoadText( L"MSG_SYSTEM_MONITOR"));
      diag->Add( txt, wcslen( txt) * sizeof(wchar_t));

      if ( m_susiInit)                                                        {

         //Si que tenemos comunicación con el sistema

         snwprintf(txt, SIZEOF_CHAR(txt)-1, L"\t%s \t\t\tOK\r\n", LoadText(L"MSG_COMMUNIC_SYS"));
         diag->Add( txt, wcslen( txt) * sizeof(wchar_t));

         //Comprobamos si la temperatura de la CPU es correcta
         snwprintf(txt, SIZEOF_CHAR(txt)-1, L"\t%s: %d", LoadText(L"MSG_TEMP_CPU"), m_TempCPU );
         diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
         wchar_t txt1[256];
         if (m_TempCPU < m_maxTempCPU)                                        {
            snwprintf(txt1, SIZEOF_CHAR(txt1)-1, L"\t\t\tOK\r\n");
         }
         else                                                                 {
            snwprintf(txt1, SIZEOF_CHAR(txt1)-1, L"\t\t\tNOT OK\r\n");
            diag->SetDiagStatus( FAIL_SYSTEM);
         }
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

         //Comprobamos si la temperatura del system es correcta
         snwprintf(txt, SIZEOF_CHAR(txt)-1, L"\t%s: %d", LoadText(L"MSG_TEMP_SYS"), m_TempSYS );
         diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
         if (m_TempSYS < m_maxTempSYS)                                        {
            snwprintf(txt1, SIZEOF_CHAR(txt1)-1, L"\t\t\tOK\r\n");
         }
         else                                                                 {
            snwprintf(txt1, SIZEOF_CHAR(txt1)-1, L"\t\t\tNOT OK\r\n");
            diag->SetDiagStatus( FAIL_SYSTEM);
         }
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

         //Comprobamos la velocidad del ventilador de la CPU
         snwprintf(txt, SIZEOF_CHAR(txt)-1, L"\t%s: %d", LoadText(L"MSG_FAN_SPEED_CPU"), m_FANCPU );
         diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
         if (m_FANCPU > m_minRPMVent)                                         {
            snwprintf(txt1, SIZEOF_CHAR(txt1)-1, L"\t\t\tOK\r\n");
         }
         else                                                                 {
            snwprintf(txt1, SIZEOF_CHAR(txt1)-1, L"\t\t\tNOT OK\r\n");
            diag->SetDiagStatus( FAIL_SYSTEM);
         }
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

         //Comprobamos la velocidad del ventilador 1 del sistema
         snwprintf(txt, SIZEOF_CHAR(txt)-1, L"\t%s: %d", LoadText(L"MSG_FAN_SPEED_SYS"), m_FANSYS1 );
         diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
         if (m_FANSYS1 > m_minRPMVent)                                        {
            snwprintf(txt1, SIZEOF_CHAR(txt1)-1, L"\t\t\tOK\r\n");
         }
         else                                                                 {
            snwprintf(txt1, SIZEOF_CHAR(txt1)-1, L"\t\t\tNOT OK\r\n");
            diag->SetDiagStatus( FAIL_SYSTEM);
         }
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

         //Comprobamos la velocidad del ventilador 1 del sistema
         snwprintf(txt, SIZEOF_CHAR(txt)-1, L"\t%s: %d", LoadText(L"MSG_FAN_SPEED_SYS"), m_FANSYS2 );
         diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
         if (m_FANSYS2 > m_minRPMVent)                                        {
            snwprintf(txt1, SIZEOF_CHAR(txt1)-1, L"\t\t\tOK\r\n");
         }
         else                                                                 {
            snwprintf(txt1, SIZEOF_CHAR(txt1)-1, L"\t\t\tNOT OK\r\n");
            diag->SetDiagStatus( FAIL_SYSTEM);
         }
         diag->Add( txt1, wcslen( txt1) * sizeof(wchar_t));

      }
      else                                                                    {
         //Comprobamos la velocidad del ventilador 1 del sistema
         snwprintf(txt, SIZEOF_CHAR(txt)-1, L"\t%s \t\t\tNOT OK\r\n", LoadText(L"MSG_COMMUNIC_SYS"));
         diag->SetDiagStatus( FAIL_SYSTEM);
         diag->Add( txt, wcslen( txt) * sizeof(wchar_t));
      }

      return true;
}

//------------------------------------------------------------------------------

void TAdvantechMonitor::AddErrorText( std::wstring &errorText)                {

   TDataManagerControl::AddErrorText( errorText);
   if ( (m_state.faultyFlags & FAULTY_STOP_CPU_FAN_SPEED) || (m_state.faultyFlags & (FAULTY_STOP_CPU_FAN_SPEED << 16)))   {
      errorText = errorText + LoadText( L"FAULTY_STOP_CPU_FAN_SPEED");
      errorText = errorText + L"\r\n";
   }
   if ( (m_state.faultyFlags & FAULTY_STOP_SYS_FAN_SPEED) || (m_state.faultyFlags & (FAULTY_STOP_SYS_FAN_SPEED << 16)))   {
      errorText = errorText + LoadText( L"FAULTY_STOP_SYS_FAN_SPEED");
      errorText = errorText + L"\r\n";
   }
   if ( (m_state.faultyFlags & FAULTY_CRITICAL_TEMPERATURE_CPU) || (m_state.faultyFlags & (FAULTY_CRITICAL_TEMPERATURE_CPU << 16)))   {
      errorText = errorText + LoadText( L"FAULTY_CRITICAL_TEMPERATURE_CPU");
      errorText = errorText + L"\r\n";
   }
   if ( (m_state.faultyFlags & FAULTY_CRITICAL_TEMPERATURE_SYS) || (m_state.faultyFlags & (FAULTY_CRITICAL_TEMPERATURE_SYS << 16)))   {
      errorText = errorText + LoadText( L"FAULTY_CRITICAL_TEMPERATURE_SYS");
      errorText = errorText + L"\r\n";
   }
   if ( (m_state.faultyFlags & FAULTY_HIGH_TEMPERATURE_SYS) || (m_state.faultyFlags & (FAULTY_HIGH_TEMPERATURE_SYS << 16)))   {
      errorText = errorText + LoadText( L"FAULTY_HIGH_TEMPERATURE_SYS");
      errorText = errorText + L"\r\n";
   }
   if ( (m_state.faultyFlags & FAULTY_HIGH_TEMPERATURE_CPU) || (m_state.faultyFlags & (FAULTY_HIGH_TEMPERATURE_CPU << 16)))   {
      errorText = errorText + LoadText( L"FAULTY_HIGH_TEMPERATURE_CPU");
      errorText = errorText + L"\r\n";
   }


}






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
