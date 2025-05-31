//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TMachineMhgr.cpp
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

#include "TMachineMngr.h"
#include "TLabels.h"
#include "TAux.h"
#include "TGridInterface.h"
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// ********************************  TMachineMngr Class  **********************
// ----------------------------------------------------------------------------

int j= 0;
/**
 Constructor
 */
TMachineMngr::TMachineMngr(const wchar_t *name, TDataManagerControl *_parent)
    : TModuleMngr(name ? name : tagGM, NULL) {


   m_daInfoMachine = new TData(this, L"Communications Info", NULL, 0, 0, 1, NULL, FLAG_NORMAL&~FLAG_SAVE);

   m_td1    = new TData( this, L"TD1", &td1,   0, 0, 200, NULL, FLAG_NORMAL3 , L"Example Subscriptions");
   // Todo: se sigue la misma filosofia que el modulemngr para los destructores
   DataModEventPost = DataModifiedPost;

//   DataModEventPost(NULL);
};

// ----------------------------------------------------------------------------
/**
 Destructor
 */
TMachineMngr::~TMachineMngr() {

   if (m_daInfoMachine)
      delete m_daInfoMachine;
}

// ----------------------------------------------------------------------------
/**
 Data Modified Post
 */
void __fastcall TMachineMngr::DataModifiedPost(TData* da) {

   if (da == m_pState)
      return;

   static bool execInfoCom = false;
   if (da == NULL) {
      // The manager itself has been modified
      char dllNames[2048];
      unsigned int size = sizeof(dllNames);
      if (GetAttribValue(cAttribNames[ATTR_DLL_NAME], dllNames, size))        {
          std::wstring loadedDlls;
          m_dllmngr.SetDlls(dllNames, loadedDlls);
          m_daLoadedDlls->SetAsText( loadedDlls.c_str());
      }
   }
   else {
      if (da && da->AreYou(LBL_STATE) ) {

         j++;
         m_td1->SetAsInt(j);
         std::list<TDataManager*>list = this->GetDataMnList();
         std::list<TDataManager*>::iterator i;
         int max = closed;
         for (i = list.begin(); i != list.end(); i++) {
            TMachine *m = dynamic_cast<TMachine*>(*i);
            if (m)                                    {
               m->CheckStateModule(da);
               SetStatus((e_state)da->AsInt());
//               m_state.state = da->AsInt();
//               m_pState->NotifyModified();
            }
         }
//         SetStatus(max);
      }
   }
   // Communications Info Message
   if (da == m_daInfoMachine && !execInfoCom) {
      execInfoCom = true;
      if (m_commMngr) {
         wchar_t text[2048];
         text[SIZEOF_CHAR(text)-1] = 0;
         m_commMngr->GetInfoComm(text, SIZEOF_CHAR(text)-1);
         MessageBox(NULL, text, L"Communications Info", MB_OK);
      }
      m_daInfoMachine->SetAsInt(0);
      execInfoCom = false;
   }
   else if (da == m_dataInit) {
      if (m_dataInit->AsInt()) {
         MyInit();
         m_dataInit->SetAsInt(0);
      }
   }
   else if (da == m_dataGo) {
      if (m_dataGo->AsInt()) {
         MyGo();
         m_dataGo->SetAsInt(0);
      }
   }
   else if (da == m_dataStop) {
      if (m_dataStop->AsInt()) {
         MyStop();
         m_dataStop->SetAsInt(0);
      }
   }
   else if (da == m_dataClose) {
      if (m_dataClose->AsInt()) {
         MyClose();
         m_dataClose->SetAsInt(0);
      }
   }
   else if (da == m_dataRequestCfg) {
      if (m_dataRequestCfg->AsInt()) {
         // MyRequestCfg();
         m_dataRequestCfg->SetAsInt(0);
      }
   }
   else if (da == m_dataSaveCfg) {
      if (m_dataSaveCfg->AsInt()) { {
            wchar_t txt[128];
            if (SaveCfg()) {
               swprintf(txt, L"Configuration machine has been saved. ", L"");
               LOG_INFO1(txt);
            }
            else {
               swprintf(txt,
               L"Configuration machine could not be saved. ", L"");
               LOG_WARN1(txt);
            }
            m_dataSaveCfg->SetAsInt(0);
         }
      }
   }
   // else if (da == m_daConfigType)                                             {
   // int index_value;
   // m_daConfigType->AsInt(index_value);
   // wchar_t txt[1024];
   // unsigned int size = sizeof( txt);
   // if ( m_daConfigType->GetAttribValue( cAttribNames[ATTR_LIST], txt, size ) ) {
   // std::list<std::string>  configTokens;
   // TAux::ExtractTokens( txt, configTokens);
   // int sizeTokens = configTokens.size();
   // if (sizeTokens > 0) {
   // std::list<std::string >::iterator i;
   // int index = 0;
   // for( i = configTokens.begin(); i != configTokens.end(); i++)       {
   // if (index == index_value) {
   // std::string value = (*i);
   // size_t found=value.find_first_of("=");
   // if ( ((signed)found != -1) && ( value.size() > 0) ) {
   // value[found]='\0';
   // SetActiveConfig((wchar_t*)value.c_str());
   // break;
   // }
   // }
   // index ++;
   // }
   // }
   // }
   // }
}

// ----------------------------------------------------------------------------
/**
 Set Active Configuration
 @param name name of the new active configuration
 */
void TMachineMngr::SetActiveConfig(wchar_t* name) {

   // LT::TMessage *sendActiveConfig = new LT::TMessage( MSG_ACTIVE_TYPE, m_name,
   // tagGM, PRIOR_NORMAL, name, strlen( name));
   // if (sendActiveConfig)
   // SendMsg( sendActiveConfig);
}

// ----------------------------------------------------------------------------
/**
 Provides creation of managers in inherited classes and dynamic libraries.
 @param   list
 @param   name
 @param   manager
 */
void TMachineMngr::WhatCanYouCreate(std::list<s_wcyc_container_class> &list,
    char *name, bool manager) {

   m_dllmngr.WhatCanDllsCreate(list, name, manager);
}

// ----------------------------------------------------------------------------
/**
 Creates managers from code type.
 @param   name
 @param   code
 @param   dparent
 */
TDataManager *TMachineMngr::CreateDataMngr(wchar_t *name, char* code,
    TDataManager *dparent) {

   TDataManager *mn = NULL;

   if (stricmp(code, typeid(TMachine).name()) == 0)
      mn = new TMachine(name, (TDataManagerControl*)dparent);
   else {

      // Try with dll's
      TDataManager *mn = m_dllmngr.CreateDataMngr(name, code, dparent);

      if (mn != NULL)
         return mn;
      else {
         wchar_t text[256];
         swprintf(text,
             L"Machine with name:  %s code:  %d not found for creation.",
             name, code);
         LOG_WARN1(text);
         // Manager not found, a virtual manager with name and code is created.
         // but state configured is never arrived.
         mn = new TDataManager(name, code, dparent);
         if (mn)                                                              {
            DISPATCH_EVENT(WARN_CODE, L"ST_FAULTY_MNGR_NOT_FOUND", text, this);
            LOG_WARN2( LoadText(L"ST_FAULTY_MNGR_NOT_FOUND"), text);
         }
      }
   }
   return mn;
}

// ----------------------------------------------------------------------------
/**
 Creates a TData parameter with name, code and parent values.
 @param   name
 @param   code
 @param   dparent
 */
TData *TMachineMngr::CreateData(wchar_t *name, char* code,
    TDataManager *dparent) {

   // Try with dll's
   TData *data = m_dllmngr.CreateData(name, code, dparent);
   if (data != NULL)
      return data;
   else {
      wchar_t text[256];
      swprintf(text, L"Data with name:  %s code:  %d not found for creation.",
          name, code);
      LOG_WARN1(text);
   }
   return NULL;
}

// ----------------------------------------------------------------------------
/**
 Process Message
 @param   msg
 */
bool TMachineMngr::ProcMsg(LT::TMessage *msg) {

   std::list<TDataManager*>list = this->GetDataMnList();
   std::list<TDataManager*>::iterator i;

   for (i = list.begin(); i != list.end(); i++) {
      TMachine *m = dynamic_cast<TMachine*>(*i);
      if (m)
         m->ProcMsg(msg);
   }
   return true;
}

// ----------------------------------------------------------------------------
/**
 @param   commMngr
 @param   CommIfc
 @param   persistence
 */
bool TMachineMngr::StartMachines(TCommMngr* commMngr, TCommInterface* CommIfc,
    TPersistence* persistence) {

   if (commMngr)
      m_commMngr = commMngr;
   if (persistence)
      m_persistence = persistence;
   if (CommIfc)
      m_commIfc = CommIfc;

   // Set configuration - Machine Manager
   // Load configuration

   // m_persistence->GetConfigMachineMngr(this);

   // Load Configuration for Machines
   // std::list<TDataManager*> mList;
   // m_persistence->GetAvailableMachines(mList);
   //
   std::list<TDataManager*>::iterator i;
   // // Load configuration for each machine
   // for( i =  mList.begin(); i != mList.end(); i++)                            {
   //
   // const wchar_t *str_name =  (*i)->GetName();
   // unsigned int code    =  (*i)->GetCode();
   //
   // wchar_t name [1024];
   // swprintf(name,L"%s",str_name);
   //
   // if ( TDataManager* obj = GetManagerObject(name) )
   // DeleteFromList(obj);
   //


   bool ret = true;
   // if ( m_listDataMngr.size() > 0 )                                           {
   for (i = m_listDataMngr.begin(); i != m_listDataMngr.end(); i++) {
      TMachine *m = dynamic_cast<TMachine*>(*i);
      if (m)
         ret = ret && m->Start();
   }
   // }
   // else
   //return false;
   return ret;
}

// ----------------------------------------------------------------------------
/**
 Load the configuration
 @param   ms
 */
bool TMachineMngr::LoadCfg(LT::TStream *ms, bool saveBeforeClearToSave) {

   return StartMachines(m_commMngr, m_commIfc, m_persistence);
}

// ----------------------------------------------------------------------------
/**
 Process Messages.
 @param   msg
 */
bool TMachineMngr::SendMsg(LT::TMessage *msg) {

   if (m_commMngr)   {
      bool ret = m_commMngr->PushOutMsg(msg);
      if( !ret )  {
         LOG_WARN1(L"Failed to PushOutMsg in TMachineMngr::SendMsg");
      }
      return ret;
   }
   else
      return false;
}

// ----------------------------------------------------------------------------
/**
 Subscriptions machines
 */
bool TMachineMngr::Subscribe(const wchar_t *oriFullName, TNode *tLocalNode,
    e_priority priority) {

   if (m_commIfc)
      return m_commIfc->Subscribe(oriFullName, tLocalNode, PRIOR_HIGH);
   else
      return false;
};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/**

 */
bool TMachineMngr::MyInit() {

   bool ret = true;
   std::list<TDataManager*>::iterator i;
   for (i = m_listDataMngr.begin(); i != m_listDataMngr.end(); i++) {
      TMachine *m = dynamic_cast<TMachine*>(*i);
      if (m)
         ret = ret && m->MyInit();
   }
   return ret;
}

bool TMachineMngr::MyGo() {

     bool ret = true;
   std::list<TDataManager*>::iterator i;
   for (i = m_listDataMngr.begin(); i != m_listDataMngr.end(); i++) {
      TMachine *m = dynamic_cast<TMachine*>(*i);
      if (m)
         ret = ret && m->MyGo();
   }
   return ret;
}

bool TMachineMngr::MyStop() {

     bool ret = true;
   std::list<TDataManager*>::iterator i;
   for (i = m_listDataMngr.begin(); i != m_listDataMngr.end(); i++) {
      TMachine *m = dynamic_cast<TMachine*>(*i);
      if (m)
         ret = ret && m->MyStop();
   }
   return ret;
}

bool TMachineMngr::MyClose() {

    bool ret = true;
   std::list<TDataManager*>::iterator i;
   for (i = m_listDataMngr.begin(); i != m_listDataMngr.end(); i++) {
      TMachine *m = dynamic_cast<TMachine*>(*i);
      if (m)
         ret = ret && m->MyClose();
   }
   return ret;
}

bool TMachineMngr::ShutDown( )              {

   bool ret = true;
   std::list<TDataManager*>::iterator i;
   for (i = m_listDataMngr.begin(); i != m_listDataMngr.end(); i++) {
      TMachine *m = dynamic_cast<TMachine*>(*i);
      if (m)
         ret = ret && m->ShutDown();
   }
   return ret;
}


e_state TMachineMngr::GetStateMachines( )                            {

   e_state state = running;

   std::list<TDataManager*>::iterator i;
   for (i = m_listDataMngr.begin(); i != m_listDataMngr.end(); i++) {
      TMachine *m = dynamic_cast<TMachine*>(*i);
      if (m && m->Status() < state)
         state = (e_state)m->Status()  ;
   }

   return state;
}



//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------

