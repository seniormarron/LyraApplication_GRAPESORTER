//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TAdvantechMonitor.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   Advantech
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef TAdvantechMonitorH
#define TAdvantechMonitorH

/// Basic include libraries
#include <vcl.h>

#include <mmsystem.h>

#include "TDataMngControl.h"
#include "TAdvantechMonitorVcl.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

#include "REL_SUSI.h"
#include "ERRDRV.h"
#include "REL_DEBUG.h"
#include "REL_ERRLIB.h"

#include <Susi4.h>
#include <OsDeclarations.h>

#include <windows.h>


//----------------------------------------------------------------------------
 /// Faulty flag codes
#define FAULTY_STOP_CPU_FAN_SPEED               0x10        ///< bit 0 of flagsArray
#define FAULTY_STOP_SYS_FAN_SPEED               0x20        ///< bit 1 of flagsArray
#define FAULTY_CRITICAL_TEMPERATURE_CPU         0x40        ///< bit 2 of flagsArray
#define FAULTY_HIGH_TEMPERATURE_CPU             0x80        ///< bit 3 of flagsArray
#define FAULTY_CRITICAL_TEMPERATURE_SYS         0x100       ///< bit 4 of flagsArray
#define FAULTY_HIGH_TEMPERATURE_SYS             0x200       ///< bit 5 of flagsArray

#define CELCIUS	273


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//*********************** TAdvantechMonitor Class ******************************
//------------------------------------------------------------------------------
/**
*/


class TAdvantechMonitor : public TDataManagerControl, public TAdvantechMonitorVcl{

 protected:

   int   m_maxTempCPU,              //Maxima temperatura de la CPU para aviso
         m_maxTempCPUStop,          //Maxima temperatura de la CPU para stop
         m_maxTempSYS,              //Maxima temperatura del ambiento para aviso
         m_maxTempSYSStop,          //Maxima temperatura del ambiento para stop
         m_oldTempCPU,
         m_oldCTempCPU,
         m_minRPMVent,
         m_TempCPU,
         m_TempSYS,
         m_FANCPU,
         m_FANSYS1,
         m_susi,
         m_FANSYS2;

   bool  m_susiInit,
         m_susiAvaiable;

   TData    *m_daManufacturer,
            *m_daBName;

   wchar_t  m_manufacturer[128],
            m_Bname[128];

   virtual void   MyPeriodic( __int64 &time);   ///< @see TDataManagerControL::MyPeriodic
   virtual bool   MyCheckDiagnose( e_diagnose_type type_diagnose, TBasicDiagnose *diag);

 public:

   TAdvantechMonitor( wchar_t *name, TDataManagerControl *_parent);   ///< Constructor.
   ~TAdvantechMonitor();                                              ///< Destructor
   virtual bool MyInit();

   void AddErrorText( std::wstring &errorText);
   virtual void ShowPcStatusForm( TPanel *panel = NULL, int interfaceType = 0){};             ///< Show the Pc Status Form in the panel




};
#endif






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
