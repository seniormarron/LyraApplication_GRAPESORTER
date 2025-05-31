//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TGm.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#pragma hdrstop
#pragma package(smart_init)

#include "TGridInterface.h"
//#include "TModuleForm.h"

#include "TXmlManager.h"
#include "TTXTEvent.h"
#include "TGm.h"
//----------------------------------------------------------------------------

#ifdef _LYRATECHS_TEST
// Data for the callback
TGm      *gmTest              = NULL;
#endif


//----------------------------------------------------------------------------
//********************************  TGm Class  *******************************
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
   Default Constructor
*/
TGm::TGm( wchar_t *moduleName, wchar_t *param, wchar_t *entryPoint, wchar_t *eventsFilePath,TMachineMngr* mngr)
    :TBaseModule      ( moduleName, entryPoint)        {

   LOGGER( moduleName);

   if ((moduleName == NULL) || (wcslen(moduleName) == 0) || (entryPoint == NULL) || (wcslen(entryPoint) == 0))   {
      LOG_ERROR1( L"Bad Parameters");
   }
   else  // Setting CommManager
//      m_commMngr = new TCommMngr( moduleName,param, entryPoint);
      m_commMngr = new TCommMngr( moduleName, entryPoint);

   if ( !m_commMngr )                                                         {
      LOG_WARN1(L"CommMngr could not be loaded!");
   }

   //Setting Persistence
   m_persistence = new TXmlManager( param);

   if ( m_persistence->ConfigLoaded() ) {
   //   char nameFile[256];
      wchar_t msg[150];
//      (( TXmlManager* ) m_persistence)-> getFileConfigName( nameFile );
       wsprintf(msg, L"%s -- File ",param );
      LOG_INFO2(L"Config Loaded from -- ",msg);

      // -- Set Machine Manager

      m_machineMngr = mngr;
      if (!mngr)
         m_machineMngr  = new TMachineMngr();

      m_commIfc  = new TCommInterface( m_machineMngr , m_commMngr);

   } else   {
       LOG_WARN1(L"Persistence could not be loaded");
       LOG_WARN1(L"Configuration from machines and modules could not be loaded");
       m_persistence = NULL;
   }

   m_eventPersistence      = new TTXTEvent   (eventsFilePath);
//   m_eventPersistence = NULL; //new TTXTEvent( m_persistence);

  // m_intf = NULL;

#ifdef _LYRATECHS_TEST
 // Start "Queue Timer" for testing purposes
   gmTest = this;
   CreateTimerQueueTimer(&m_timerHandle,
                        NULL,
                        TimerProcCheckTest,
                        0,
                        15000,
                        1000,
                        WT_EXECUTEINIOTHREAD);

#endif

   LOG_INFO2(m_name,L" constructed ");

}

#ifdef _LYRATECHS_TEST
void CALLBACK TGm::TimerProcCheckTest(PVOID param, BOOLEAN timerOrWaitFired) {
   gmTest->EXECUTE_TESTS();
}
#endif

//----------------------------------------------------------------------------
/**
   Default Destructor
*/
TGm::~TGm()                                                                   {

   // Send signal stops Modules
   m_machineMngr->Stop();

   if (m_persistence)
      delete m_persistence;
  if (m_eventPersistence)
      delete m_eventPersistence;

   if( m_commMngr)
      delete m_commMngr;

//   if( m_machineMngr)
//      delete m_machineMngr;

  #ifdef _LYRATECHS_TEST
   // Destroy "Queue Timer"
   DeleteTimerQueueTimer(NULL, m_timerHandle, NULL);
   CloseHandle (m_timerHandle);
  #endif
}

//----------------------------------------------------------------------------
/**
   Starts Global Manager Module
*/
bool  TGm::Start()                                                            {

   m_started = false;
   m_started = TBaseModule::Start();

   if ( m_started )                                                           {
      bool ret =  m_machineMngr->StartMachines(m_commMngr,m_commIfc,m_persistence);
      if ( !ret ){
         LOG_WARN1(L"Machines not started correctly. ");
      }
      else
         this->m_machineMngr->RefreshAll();
   }

   return m_started;
}

//----------------------------------------------------------------------------
/**
   Starts Global Manager Module
*/
bool TGm::Init  ()                                                         {
   return m_machineMngr->MyInit();
}
//----------------------------------------------------------------------------
/**
   Starts Global Manager Module
*/
bool   TGm::Go    (){
   return m_machineMngr->MyGo();
}
//----------------------------------------------------------------------------
/**
   Starts Global Manager Module
*/
bool TGm::Stop  (){
   return m_machineMngr->MyStop();
}

//----------------------------------------------------------------------------
/**
   Starts Global Manager Module
*/
bool   TGm::Close    (){
   return m_machineMngr->MyClose();

}

//----------------------------------------------------------------------------
/**
   Process Message for Global Managers
   @param msg to be processed
*/
bool   TGm::ProcMsg( LT::TMessage *msg)                                      {

    // if GM is not dest, or we don't have persistence
    if( ! msg->IsDest( m_name) || !m_persistence )
        return false;

   bool ret = false;
   switch(msg->Type())                                                        {

      case  MSG_COMM_IFC_NOTIFY_SUBSCRIBERS:
//      case  MSG_COMM_IFC_TDATA_READ:
//      case  MSG_COMM_IFC_TDATA_WRITE:
      case  MSG_COMM_IFC_SUBSCRIBE:
      case  MSG_COMM_IFC_UNSUBSCRIBE:
      case  MSG_COMM_IFC_NOTIFY_SUBSCRIBERS_ERASED:                           {

         ret = m_commIfc->ProcMsg(msg);
      }  break;

      default:                                                                {
         LT::TMessage *outputMsg = NULL;
         if ( m_persistence->ProcMsg(msg, outputMsg))
            if ( outputMsg)
               m_commMngr->PushOutMsg( outputMsg);

         ret = m_machineMngr->ProcMsg(msg);
         if  ( m_eventPersistence )                                           {
            LT::TMessage *outputMsg = NULL;
            m_eventPersistence->ProcMsg(msg, outputMsg);
         }
      } break;
   }
   return ret;
}





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------


