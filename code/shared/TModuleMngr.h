//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TModuleMngr.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TModuleMngrH
#define TModuleMngrH

//---------------------------------------------------------------------------
#include "TProcess.h"
#include "TDllManager.h"
#include "TCommInterface.h"
#include "TMessageProcessor.h"


// Forward Declarations
class TCommMngr;
class TModule;
class TIORealTime; //forward declaration

#define MACHINE_FAULTY_TEXT   L"MachineFaultyText"

#define MACHINE_FAULTY_STATE  L"MachineFaultyState"

#define LBL_DELAY_TIME        L"DelayTime"

#define LBL_ORIGIN_MODULE L"OrgMod"

#include "TUser.h"

 /// Faulty flag codes
#define FAULTY_TYPE_NOT_FOUND  		0x1  					///< bit 0 of flagsArray
  #define LBL_ACTIVE_TYPE_NAME     L"ActiveTypeName"

#define LBL_ORIGIN_MODULE L"OrgMod"

class SubtypesManager;



//----------------------------------------------------------------------------
//*****************************  TModuleMngr Class  **************************
//----------------------------------------------------------------------------
/**
   ModuleMngr Class is intended to be root class for manager modules. To do this work ModuleMngr
   contains the following main components:
      - TDllManager: to load processes from dynamic libraries
      - TModule: a reference to Module component joined to ModuleMngr. ModuleMngr will use from TModule:
            - persistence methods and,
            - communicatinon methods (Send & Subscribe)
*/
/// Class to define root process

class TModuleMngr : public TProcess , public TMessageProcessor                {

  protected:
   TData          *m_daActiveType;           ///< TData que almacena el nombre del tipo activo
   SubtypesManager *m_subtypesMngr;

   wchar_t        m_autoSaveResetName[50];   ///< to store the name of tdata that provoqued the last autosave reset
   LT::LTTimeManager  m_timeManager;
   HANDLE         m_mutexAutoSave;
//   MMRESULT       m_periodicTimer; //Call to Periodic() method, that propagates to childs
   int            m_periode;
   std::list<TData *>   m_faultyList;       ///< list with TDatas in faulty state

   TData          *m_faultyMachineState,    ///< TData with faulty state for all this moduleMngr
                  *m_faultyMachineText;     ///< TData with faulty text for all this moduleMngr

   TData          *m_dataInit,               ///< Signal Init control
                  *m_dataGo,                 ///< Signal Go control
                  *m_dataStop,               ///< Signal Stop control
                  *m_dataClose,              ///< Signal Close control
                  *m_dataRequestCfg,         ///< Signal Load Configuration control
                  *m_dataSaveCfg,            ///< Signal Save Configuration control
                  *m_daCfgDir,               ///< Local folder/drive reference for configuration files
                  *m_daExternalCfgDir,       ///< Esternal drive reference for configuration files
                  *m_events,                 ///< Containing last 10 events.
                  *m_daLang,
                  *m_daLoadedDlls,
//                *m_testSaveCfg,
                  *m_daAssist,
                  *m_daConfigVersion,        ///< Configuration version date and number
                  *m_daAssistDir,
                  *m_daAssistExe,
                  *m_daExplorer,
                  *m_daExplorerExe,
                  *m_daMsDos,
                  *m_daMsDosExe,
                  *m_daTouch,
                  *m_daTouchDir,
                  *m_daTouchExe,
                  *m_daExtract,
                  *m_daExtractExe,
                  *m_daExtractParam,
                  *m_daKey,
                  *m_daKeyExe,
                  *m_daRegedit,
                  *m_daRegeditExe,
                  *m_daMaintenance,
                  *m_daMaintenanceExe,
                  *m_daLogChangesNameAndDir,
                  *m_daLogMemoryEveryXMyPeriodics,
                  *m_daMaxMemoryToNotify,
#ifdef _DEBUG
                  *m_daLogMemoryOnSet,
#endif
                  *m_checkNotAllowedTDatas;

   TDllManager    m_dllmngr;                  ///< Reference to Dll Manager
   TModule*       m_module;                   ///< Reference to Module Where ManagerModule will be loaded.
   TData          *m_daPeriode,
                  *m_daPeriodeTime,
                  *m_daResyncPeriode;
   TData*         m_daInfoComm;               ///< Signal for Communications Information
   TData*         m_daClearCommStat;          ///< Signal for Clear the Communications Statistics
   TData*         m_daInfoSubs;               ///< Subscription Information
   TData*         m_dataAutoSaveResets;       ///< TData to show autoSave resets
   TData*         m_dataAutoSaveResetN;       ///< TData to show name of tdata that provoqued last autosave reset

   MMRESULT       m_idTimerAutoSave;          ///< Signal for Auto Save Timer
   bool           m_blockAutoSave;            ///< To avoid setting autosave timer each time a parameter changes during load of configuration
   // User Management
   TAccessLevel*  m_accessManager;            ///< For indicate the Access level of the user
   wchar_t        m_diagnose[8192],           ///< Diagnose info
                  m_exeAssist[256],           ///< .exe for Technical Assistance
                  m_exeExplorer[256],         ///< .exe for windows explorer
                  m_exeRegedit[256],          ///< .exe for regedit
                  m_exeMsDos[256],            ///< .exe for MsDos
                  m_exeExtract[256],
                  m_configVersion[256],       ///< configured version
                  m_exeKey[256],
                  m_parExtract[256],
                  m_exeTouch[256],
                  m_dirTouch[256],
                  m_exeMaintenance[256],      ///< .exe for manteinance APP
                  m_directory[1028],          ///< Directory for Technical Assistance
                  m_logChangesNameAndDir[1024];
   int            m_logChangesLinesToWrite,
                  m_logMemoryEveryXMyPeriodics;
#ifdef _DEBUG
   int            m_logMemoryOnSet;
#endif

   int            m_enableDiag;               ///< Enable Diagnose
   TData          *m_daDiagnose,              ///< General diagnose of 1394Bus.
                  *m_daEnaDiagnose;           ///< Enables diagnose.
   TData          *m_delayTimeData;
   float          m_delayTime;                ///< Delay of capture ( last)
   float          m_maxDelayTime;             ///< Delay of capture ( last)
   wchar_t        m_periodicMilisec[60];      ///< duration of last Periodic, in ms
   std::list<TProcess*> m_listProc;           ///< list of first level processes
   HANDLE         m_mutex;

   static void CALLBACK TimerPeriodic( PVOID parameter, BOOLEAN timerOrWaitFired);
   HANDLE         m_timerHandle;              ///< timer to check hardlock
   HANDLE         m_hTimerQueue;
//   HANDLE         m_threadPeriodic;
//   bool           m_stopPeriodic;

 //Log changes to file
   HANDLE         m_mutexChanges;            /// mutex to control acces to log changes
   wstring        m_lastSavedName;           /// last written name, to check if we have to write
   wstring        m_changedLog;              /// value to write if changes in another tdata arrive (or timer, periodic, or close app). Empty if everything is written

   int              m_maxProcessMemoryToNotify;
   unsigned __int64 m_maxProcessMemoryToNotify64;

  public:
   TModuleMngr( const wchar_t *name, TProcess *_parent);        ///< Constructor
   virtual ~TModuleMngr();                                      ///< Destructor


   virtual void Refresh( TNode *node, TDataInterface *intf , bool forceUpdate);                   ///< notifys its interfaces that data of a TData has been modified
   virtual bool Init ();                                                               ///<
   TIORealTime *FindRT( TDataManager* mn );                       ///< find iorealtime
//   virtual bool ReadCfg ( LT::TStream *stream);

   //-- Users management
   int   LoginTechnicalUser  ( wchar_t *password, int randomValue);
   int   LoginMaster         ( wchar_t *password, int randomValue);
   int   LoginUser           ( wchar_t *name, wchar_t *password);
   void  LogoutUser           ();
   bool  MyAutoSaveCfg();   ///<  method that is called when autosave timer arrives to timeout

   void GetUserNames( std::list<const wchar_t*> &l);
   void GetUsers(std::list<TUser*> &l);
   bool AddUser      ( wchar_t *name, wchar_t *password, int level = User, int subType=0);
   bool DeleteUser   ( wchar_t *name);
   bool ChangePasswordTo( wchar_t* name, wchar_t *newpwd);
   virtual bool  GetSubtypeByUser(const wchar_t* userName, int &subType, int &type);
   virtual e_accessLevel  GetAccessLevelByUser(const wchar_t* userName);
   TAccessLevel* GetAccessLevelManager() { return m_accessManager; };
   virtual bool GetCompleteAccessLevel( s_completeAccessLevel &cAL);
   const wchar_t* GetLoggedUserName() { return m_accessManager->GetLoggedUserName();}
//   static unsigned WINAPI TModuleMngr::ThreadPeriodicFunc( LPVOID o);
   bool SendCfg();                                                ///< instantaneous SaveCfg
   bool SendPartialCfg();                                         ///< instantaneous PartialCfg
   int   m_autoSaveResetsCons;      ///< counts the consecutives resets of the AutoSave ( provoqued by a Refresh of a TNode with Save flag)
   int   m_autoSavePending;         ///<  to aovid killing timer for AutoSave when not requiered
  protected:
   virtual  bool  MyInit  ();                                     ///<  @see TProcess::MyInit()
   virtual  bool  MyGo  ();                                     ///<  @see TProcess::MyInit()
   virtual  bool  MyClose ();                                     ///<  @see TProcess::MyClose()
   virtual  bool  MyStop ();                                      ///<  @see TProcess::MyStop()
   virtual bool   LaunchThread();            ///< Launch Thread Function
   virtual void   SetLang();
   virtual void MyPeriodic( __int64 &time);
   virtual bool Read( LT::TStream *stream, TDataInterface *intf, bool infoModif, bool *forceSave  );

   void LogChangesInTData(TData * da);
   void WriteToFileLog(wstring &entryLog);   ///< writes specified string to log files
   void WriteToFileLogProt();                ///< writes usual string to log files, with mutex protection

   void SetMaxNumThreadAllProcesses( TProcess *process);          ///< recursively sets the numbr of threads in each process to its maximum allowed number
   TProcessData*  LoadProcessData ( LT::TStream* stream) ;       ///< Load ProcessData from stream
   void  RecursiveUnsubscribe(TNode *node);                       ///<
//   void TestSaveCfg();
   void __fastcall   DataModifiedPost( TData* da);                ///< Event Handler method for Control of modified data
   static void CALLBACK AutoSaveCfg ( UINT uTimerID, UINT uMsg,
                              DWORD_PTR dwUser,
                              DWORD_PTR dw1,
                              DWORD_PTR dw2 );                    ///< Auto Save Configuration
   virtual eFaultyLevel FaultyModified( TData *data);
   static void CALLBACK AutoPeriodic( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
   void EraseModulePointer()                                                {
      m_module = NULL;
   }

  public:
   virtual void AddProcInformation( std::wstring & str);
   virtual  TDataManager*  CreateDataMngr    ( wchar_t *name, char* code, TDataManager *dparent   );             ///< Create managers of TModule type
   virtual  TData*         CreateData        ( wchar_t *name, char* code, TDataManager *dparent   );             ///< Create a parameter with name, code and parent values
   virtual  void           WhatCanYouCreate  ( std::list<s_wcyc_container_class > &list , char*name , bool manager);///< Provides creation of managers in inherited classes and dynamic libraries

   virtual  void  SetModule    ( TModule *module);                                ///< Set Module
   virtual  bool  LoadCfg  ( LT::TStream *ms /*, bool saveBeforeClearToSave= false*/);                               ///< Load Configuration
   virtual  bool  ProcDataMsg( LT::TMessage *msg);                               ///< Process Message
   virtual  bool  SendMsg( LT::TMessage *msg) ;                              ///< Send a message through commMngr
   virtual  bool  Subscribe( const wchar_t *oriFullName,
                  TNode *tLocalNode,  e_priority priority = PRIOR_NORMAL) ;   ///< Subscription to a parameter
   virtual  bool  SubscribeWithoutNotification( const wchar_t *oriFullName,
                  TNode *tLocalNode,  e_priority priority = PRIOR_NORMAL) ;   ///< Subscription to a parameter



   bool  UpdateSubscriptors( TDataManager *mn ,    bool recursive = true);    ///<
   eFaultyLevel AllFaultiesLevel();                                           ///< iterates thorugh faulties in list, and returns the maximum level

   virtual bool  SetActiveType( wchar_t *activeType);                            ///< sets the name of the active type
   virtual bool  GetActiveType( wchar_t *activeType, unsigned int &size);             ///< gets the name of the active type
   virtual e_accessLevel  GetAccessLevel();                                           ///< Returns current access level
   //- Signals
//   TCommMngr*        GetCommMngr();                                           ///< Get communication manager
//   TCommInterface*   GetCommIfc();                                            ///< Get communication interface


   virtual TProcessData *MyPostProcess(   TProcessData  *obj);

  public:

   void LaunchKeyboard();

   //-------------------------------------------------------------------------
   /**
   */
   bool    KillTimerAutoSave()                                                {
         if ( m_idTimerAutoSave )   {
            timeKillEvent( m_idTimerAutoSave);
            m_idTimerAutoSave = NULL;
            return true;
         }
         return false;
   }

   //-------------------------------------------------------------------------
   /**
      @return
   */
   TModule* GetModule()                                                       {
      return m_module;
   };

   //---------------------------------------------------------------------------

   float GetDelayTime()                                                       {
      return m_delayTime;
   }

   //---------------------------------------------------------------------------

   float GetMaxDelayTime( bool reset)                                         {
      float ret = m_maxDelayTime;
      if ( reset)
         m_maxDelayTime = 0;
      return ret;
   }


   //---------------------------------------------------------------------------
   wchar_t* GetConfigVersion()                                                 {
      return m_configVersion;
   }

   //---------------------------------------------------------------------------

   friend class TModule;

};

//-------------------------------------------------------------------------------

/**
   Class for managing subtypes
*/
class SubtypesManager : public TDataManagerControl  {

   protected:
    std::list<int> m_subtypesList;
    TData *m_daSubtypes;

   public:
    SubtypesManager( wchar_t *name, TDataManager *parent);
    virtual ~SubtypesManager();
    void __fastcall   DataModifiedPost( TData* da);                ///< Event Handler method for Control of modified data
    virtual bool MyInit();
    virtual bool MyStop();
    virtual bool MyClose();
    void SetSubtypes();

   protected:
    void ExtractStdListFromList( std::list<int> &list);
    void FromValueToActiveSubtype( TDataManager *mn);
    void SetSubtypes( TDataManager *mn, std::list<int> &subtypeList, int activeSubtype);

};

#endif




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
