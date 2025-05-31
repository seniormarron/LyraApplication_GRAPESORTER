//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TMyMachineModuleMngr.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



#ifndef TMyMachineModuleMngrH
#define TMyMachineModuleMngrH
//---------------------------------------------------------------------------

#include "TModuleMngr.h"
#include "TConveyorControlBase.h"
#include "TViewMMInterface.h"
#include "TEjectorTest.h"
#include "TThreshold.h"
#include "TBatchReportDrawer.h"
#include "TFileManagement.h"
#include "TInfoCyclesForm.h"
#include "TCapturerMngr.h"
#include "TCamera.h"

#define LBL_CHANGE_STATE "ChangeState"

//#define HYPERION   0
//#define TEALABS    1
#define GRAPE_SORTER 0

#include "TAuxGraph.h"
#include "TElectricCabinet.h"
#include <StdCtrls.hpp>

extern UINT     msg_paint_maingraph;

class TZoneClassifier;

#define FAULTY_ENCODER_NO_PULSES          0x10
#define FAULTY_ENCODER_NO_ZERO            0x20
#define FAULTY_ENCODER_WRONG_ZERO         0x40
#define FAULTY_ENCODER_PULSES_NOT_RUNNING 0x80
#define FAULTY_CLEAN_MODE                 0x100

//---------------------------------------------------------------------------
/**
   s_prodTrace: Structure to hold production trace configuration
*/
struct s_prodTrace                                                            {
   int      enabled,
            interval,
            dateInFileName;
   wchar_t  fileName[64],
            path[MAX_PATH],
            path2[MAX_PATH];      ///< Second Path to save the load file
   s_prodTrace &operator=(const s_prodTrace &prodTrace)                       {
      enabled           = prodTrace.enabled;
      interval          = prodTrace.interval;
      dateInFileName    = prodTrace.dateInFileName;
      wcscpy( fileName, prodTrace.fileName);
      wcscpy( path, prodTrace.path);
      wcscpy( path2, prodTrace.path2);

      return (*this);
   }
};

//---------------------------------------------------------------------------
/**
*  @struct s_category
*  stores the ID and name the TCategory
*/
typedef struct                                                                {
   int         idCat;    ///< Id category
   wchar_t     lang_name[256]; ///< Name Category
}s_category;
//---------------------------------------------------------------------------

class TMyMachineModuleMngr : public TModuleMngr                             {

  private:

   // -- Variables
   TData                   *m_dataLockMode,
                           *m_daRejectMode,
                           *m_version,
                           *m_appName,
                           *m_serialNumber,
                           *m_daTimeLock,
                           *m_daToExternalPerm,
                           *m_daExplorer,
                           *m_daShowFormEjectTest,
                           *m_daMachineModel;

   TEjectorMan             *m_em;
   TElectricCabinet        *m_electricCab;
   TIORealTime             *m_IORealTime;
   TEjectorTest            *m_ejectorTest;
   TFileManagement         *m_fileAccess;
   TZoneClassifier         *m_zoneClass;
   TInfoProdDrawer         *m_reportMngr;
//   TInfoCyclesForm         *m_cyclesForm;
   e_accessLevel           m_accessLevel;

   int                     m_timeLock,
                           m_contBanks,
                           m_showEjectionPanel;

 protected:

   // -- Variables
   DWORD                   m_status;
   s_prodTrace             prodTrace;
   std::list<TCamera*>     camList;
   std::list<s_category>   m_categoryList;

   bool                    m_running;
   wchar_t                 m_explorer[100];
   int                     m_lockMode;

   wchar_t                 m_AppName[100];

 public:

      // -- Functions
      TMyMachineModuleMngr( wchar_t *_name, TProcess *_parent, int _flags = FLAG_NORMAL, wchar_t *des = NULL, wchar_t *_im = NULL);
      virtual ~TMyMachineModuleMngr();

      TViewMMInterface        *m_viewIfc;

      virtual  TDataManager*  CreateDataMngr    ( wchar_t *name, char* code, TDataManager *dparent   );             ///<
      virtual  void           WhatCanYouCreate  ( std::list<s_wcyc_container_class > &list , char *name , bool manager);///<
      int      GetLockMode();

      bool     GetApplicationName( wchar_t *buffer, unsigned int &count)      {
         return m_appName->AsText( buffer, count);
      }
      bool     SetApplicationName( wchar_t *buffer)                           {
         return m_appName->SetAsText( buffer);
      }

      virtual bool SendMsg( LT::TMessage *msg) ;
      virtual eFaultyLevel FaultyModified( TData *data);
      virtual bool MyStop();
      virtual bool MyClose ();                                     ///<  @see TProcess::MyClose()
      virtual bool Read( LT::TStream *stream, TDataInterface *intf, bool infoModif, bool *forceSave   );

      virtual bool DeleteFromList ( TDataManager   *mngr);    ///< see TDataManager>>DeleteFromList
      virtual bool DeleteFromList ( TData          *data);    ///< see TDataManager>>DeleteFromList



      void UpdateGraphs();
      void CheckButtonStartState();
      void EjectionPanel();
      void SetEjectionConfiguration();
      void EjectionAll(int bank);
      void AddErrorText( std::wstring &errorText);
      void SetAccessLevel(e_accessLevel level);
      void UpdateAccessLevel( bool lock);
      bool LaunchAndShowDiagnose(TStrings *lines);

      TCamera*       GetCameraByID (int id);
      e_accessLevel  GetAccessLevel();


      // -- Inline functions
      inline TElectricCabinet* GetElectricCabinet()                           {
         return m_electricCab;
      };
      inline TZoneClassifier* GetZoneClas()                                   {
         return m_zoneClass;
      };
      inline bool ShowEjectionPanel()                                         {
         return m_showEjectionPanel;
      };
            inline void TMyMachineModuleMngr::PaintValuesProd (TRichEdit *richEdit, TDBChart *chart) {
         if ( m_reportMngr)
            m_reportMngr->PaintValues( richEdit, chart);
      }
      inline void TMyMachineModuleMngr::UpdateTypeInReport( wchar_t *type)    {
//L         if ( m_reportMngr)
//L            m_reportMngr->UpdateTypeAndTime( type, m_haspLog ? m_haspLog->GetHaspTime() : L"CAN'T READ");
      }
      inline void TMyMachineModuleMngr::ReportIntervalAndInit( bool changeType) {
         if( m_reportMngr)                                                    {
            std::wstring str;
            m_reportMngr->ReportIntervalAndInit( str, true, 0, changeType);
         }
      };
      inline void TMyMachineModuleMngr::ShowFileAccess()                      {
         if( m_fileAccess)
            m_fileAccess->ShowForm();
      };
      inline void TMyMachineModuleMngr::ShowEjectorForm()                     {
         if( m_ejectorTest)
            m_ejectorTest->ShowForm();
      };
      inline int TMyMachineModuleMngr::GetLockTime()                          {
         return m_timeLock;
      };
      inline bool SaveCarga()                                                 {
         return prodTrace.enabled;
      }
      inline void GetProdTrace( s_prodTrace &pT)                              {
         pT = prodTrace;
      }
      inline int TMyMachineModuleMngr::GetBanks()                             {
         return m_contBanks;
      }
      inline int GetMachineModel()                                            {
         return m_daMachineModel->AsInt();
      }
      inline int GetRejectMode()                                              {
         return m_daRejectMode->AsInt();
      }




 protected:

   // -- Functions
   void __fastcall   DataModifiedPost( TData* da);
   virtual bool      MyInit();
   virtual void      ProcessLTEventMsg( LT::TMessage *msg);
   virtual void      SetLang();

   bool PaintGauge ( TThresholdBase *th, TProgressBar *pb);
   void LookForReportMngr     (TDataManager *mn);
   void LookForHistZone       (TDataManager *mn);
   void LookForCategory       (TDataManager *mn);
   void ContEjectorBanks      (TEjectorMan *mn) ;
   TEjectorMan *LookForEjectorMan ( TDataManager *mn);
};

#endif












//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
