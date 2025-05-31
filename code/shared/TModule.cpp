//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TModule.cpp
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

#include <list>
#include <exception>
#include "TAux.h"

#include "TModule.h"
#include "TMachine.h"
#include "TShowMsgForm.h"

#pragma package(smart_init)

// Data for the callback
TModule* moduleTest = NULL;
extern UINT msg_shutdown;

// ----------------------------------------------------------------------------
// ToDo: en el MyInit y MyGo provocar en el formulario principal la deshabilitacion de los temporizadores. Volver a habilitar los que estuvieran habilitados al final del MyInit y del MyGo
/**
 Constructor
 */
//TModule::TModule(wchar_t *moduleName, wchar_t *paramGM, wchar_t *entryPoint, wchar_t *eventsFilePath,
//    TModuleMngr *modMngr, TPersistence *persistence, TEventPersistence * eventPersistence)
TModule::TModule(wchar_t *moduleName, wchar_t *entryPoint, wchar_t *eventsFilePath,
    TModuleMngr *modMngr, TPersistence *persistence, TEventPersistence * eventPersistence)
    : TBaseModule(moduleName, entryPoint) {

   LOGGER(moduleName);
   m_remotePersistence = false;

   // Remote persistence
//   if ((paramGM) && ( wcslen(paramGM) > 0 )) {
//      m_remotePersistence = true;
//      m_persistence        = NULL;
//      m_eventPersistence   = NULL;
//      m_commMngr = NULL;
//      if ((moduleName == NULL) || (wcslen(moduleName) == 0) ||
//          (entryPoint == NULL) || (wcslen(entryPoint) == 0))                  {
//         LOG_ERROR1(L"Bad Parameters");
//      }
//      else
//         m_commMngr = new TCommMngr(moduleName, paramGM, entryPoint);
//   }
//   else { //Local persistence
      // Create persistence objects
      m_persistence = persistence;
      if(  eventPersistence == NULL){    //standard case
         m_eventPersistence = new TTXTEvent(eventsFilePath);
         m_eventPersistence->Init();
      }else{
         m_eventPersistence = eventPersistence;
         m_eventPersistence->Init();
      }
      if (persistence == NULL)                                                {
         LOG_ERROR1(L"Local persistence does not exist, nor GM parameters.");
      }
      m_commMngr = NULL;
//   }

   m_dataMngr = modMngr;
   if(m_dataMngr) {
      m_dataMngr->SetModule(this);
   }

   if(wcscmp(entryPoint, L"") != 0)                                  {
//      m_commMngr = new TCommMngr(moduleName, paramGM, entryPoint);
      m_commMngr = new TCommMngr(moduleName, entryPoint);
      if (m_commMngr) {
         m_commIfc = new TCommInterface(m_dataMngr, m_commMngr);
         m_commIfc->SetDeleteMe(true);
         //we add the TModule in order to also receive message from communications
         m_commMngr->AddMessageProcessor(this);
      }
      else
         m_commIfc = NULL;
   }
   m_NextModulesOk = NULL;
   m_numNextmodules = 0;
   m_tokensNextModules.clear();

   moduleTest = this;

   m_correctlyLoaded = true;

   m_mutex           = OpenMutex( SYNCHRONIZE , false, L"DeleteMutex");
   if ( m_mutex==NULL)
      m_mutex           = CreateMutex( NULL , false, L"DeleteMutex");

}

void CALLBACK TModule::TimerProcCheckTest(PVOID param, BOOLEAN timerOrWaitFired)
{
   moduleTest->EXECUTE_TESTS();
}

// ------------------------------------------------------------------------------
/**
 Default Destructor
 */
TModule::~TModule() {
   LOG_INFO1( L"Begin Module destructor");
   Close();
   ::Sleep(30);

   if (m_dataMngr)
      m_dataMngr->EraseModulePointer();

   if (m_commMngr)
      delete m_commMngr;

   if (m_opcMngr)
      delete m_opcMngr;

   // if( m_dataMngr)
   // delete m_dataMngr;
   if (m_NextModulesOk)
      delete[]m_NextModulesOk;
   if (m_eventPersistence) {
      delete m_eventPersistence;
      m_eventPersistence = NULL;
   }


   ReleaseMutex( m_mutex);
   CloseHandle( m_mutex);
   LOG_INFO1( L"End Module destructor");

}

// ------------------------------------------------------------------------------
/**
 Start-up module. checks that tabseModule starts and exits a pointer to moduleMngr
 */
bool TModule::Start()                                                         {
   //ToDo: revisar para el caso de multimodulo
   return TBaseModule::Start() && m_dataMngr;

}

// ------------------------------------------------------------------------------
/**
 Init signal for Module
 */

bool TModule::Init() {

   m_resultInit = m_dataMngr ? m_dataMngr->Init() : true;
   TData *tdata = m_dataMngr->GetDataObject(LBL_STATE);
   if (tdata) {
      if (m_resultInit)
         if (m_NextModulesOk || m_numNextmodules == 0) {
            for (int i = 0; i < m_numNextmodules; ++i)
               if (!m_NextModulesOk[i])
                  return m_resultInit;
            // // If all next modules are ok, go to ready state
            tdata->SetAsInt(m_dataMngr->Status());
         }
         else {
            tdata->SetAsInt(m_dataMngr->Status());
            LOG_ERROR1(L" Signal INIT failed");
         }
   }


//    InitServer();

   //set the commMngr to connected so it can start receiving messages
   //ToDo do this only if the commMngr is the server
//   if(  m_commMngr->m_serverMode)
   if( m_commMngr )
      m_commMngr->SetConnectionStatus(e_commMngrStatus::CONNECTED);

   return m_resultInit;
}

// -----------------------------------------------------------------------------
/**
 Go signal for Module
 @return bool flag to indicate go signal result
 */
bool TModule::Go() {
   return m_dataMngr ? m_dataMngr->Go() : false;
};

// -----------------------------------------------------------------------------
/**
 Stop signal for Module
 @return bool flag to indicate stop signal result
 */
bool TModule::Stop() {
   bool ret = (m_dataMngr ? m_dataMngr->Stop() : false);
   SendPartialCfg();
   return ret;
}

// -----------------------------------------------------------------------------
/**
 Close signal for Module
 @return bool flag to indicate close signal result
 */
bool TModule::Close() {

   if (m_dataMngr && m_dataMngr->Status()  == closed )
      return true;

   SendPartialCfg();

   if (m_dataMngr)
      m_dataMngr->DisableInterfaces(); // ToDO: prueba para evitar fallos en

   return (m_dataMngr ? m_dataMngr->Close() : true);
}

// ----------------------------------------------------------------------------
/**
 Load configuration from a stream into the manager

 @param   ms Pointer to stream where to get configuration
*/

bool TModule::LoadCfg(LT::TStream *ms /*, bool saveBeforeClearToSave*/ )          {
   //read the name of the type sent in this configuration
   char typeName[TYPE_NAME_SIZE];
   memset( typeName, 0x00, sizeof( typeName));
   ms->Read( typeName, sizeof(typeName));
   wchar_t *wTypeName = TAux::STR_TO_WSTR(typeName);
   //set active type
   if( ! m_dataMngr->SetActiveType( wTypeName)) {
      LOG_WARN2(L"Fail setting active type: ",wTypeName);
   }
   delete [] wTypeName;

   m_correctlyLoaded = m_dataMngr->LoadCfg( ms /*, saveBeforeClearToSave*/);

   if (m_dataMngr->Status() < configured)                                     {
      DISPATCH_EVENT(WARN_CODE, L"EV_CORE_CFG", L"Module CLOSED.", m_dataMngr);
      LOG_WARN2( LoadText(L"EV_CORE_CFG"), L"Module CLOSED.");
   }
   if (m_correctlyLoaded)                                                     {
      DISPATCH_EVENT(INFO_CODE, L"EV_CORE_CFG", L"Configuration Loaded.", m_dataMngr);
      LOG_INFO2( LoadText(L"EV_CORE_CFG"), L"Configuration Loaded.");
   }
   else                                                                       {
      DISPATCH_EVENT(INFO_CODE, L"EV_CORE_CFG", L"Configuration Not correctly Loaded.", m_dataMngr);
      LOG_INFO2( LoadText(L"EV_CORE_CFG"), L"Configuration Not correctly Loaded.");
   }
   return m_correctlyLoaded;
}

// ----------------------------------------------------------------------------
/**
 Request Configuration

 */
bool TModule::RequestCfg( bool embedded)                                      {
   // 15/02/2013 el tipo de mensaje MSG_GET_CFG_EMBEDDED solamente sirve para que el GM nos conteste
   //con un tipo de mensaje diferente al MSG_CFG ( llamado MSG_CFG_EMBEDDED), de manera que tras cargar configuracion
   //embebida podamos hacer un SaveCfg, para actualizar los ficheros en disco y limpiar el GM de los datos embebidos
   e_typeMsg msgType = embedded==false?MSG_GET_CFG:MSG_GET_CFG_EMBEDDED;

   LT::TMessage *firstMsg = new LT::TMessage( msgType, m_name, tagGM,
       PRIOR_NORMAL, (char*)m_name, wcslen(m_name)*sizeof(wchar_t));

   return SendMsg( firstMsg);
}

// ----------------------------------------------------------------------------
/**
 Reset Comunications
 Sends a configuration changed message to all links, then deletes all links and
 finally removes all subscriptions.
 */
void  TModule::ResetComunications(bool loadAd){
   //enable comminterface
   if(m_commIfc)
      m_commIfc->SetEnabled(true);

      //vic:  June 2016 . commented to enable the MGS (i5 multimodule)
//   if(m_commMngr){
//      m_commMngr->SendCFGLoadedToAllLinks();
//      //m_commMngr->DeleteAllLinks();
//   }
//
//   //removing all subscriptions
   if(m_commIfc && loadAd)  {
      m_commIfc->DeleteAllSubscriptions();
   }
      //end vic:

   //OPCServer
   if( m_opcMngr && m_dataMngr)         {
      // get list to publish
      std::list<TData*> opc_tdatas;
      m_dataMngr->GetTDatasWithFlag(opc_tdatas, FLAG_PUBLISH_REMOTE);
      //send list to OPCMngr, this method will also enable the OPCInterface
      m_opcMngr->UpdateOPCConfiguration(opc_tdatas);
   }
}






//-----------------------------------------------------------------------------

bool TModule::SendEmbeddedCfgToSaveTo( wchar_t *name, bool force)             {

   if ( force)
      m_correctlyLoaded = true;

   if (  !m_correctlyLoaded )                                                {
      DISPATCH_EVENT( WARN_CODE, L"EV_CFG_NOT_SEND", L"Configuration was not sent because wasn't correctly loaded previously", m_dataMngr);
      return false;
   }
   //evitamos que se lance desde dos threads a la vez, por ejemplo si hacemos un Stop desde interfaz
   //de usuario y, simultaneamente, vencia el temporizador que provoca un AutoSave
   if ( WaitForSingleObject( m_mutex, 0) == WAIT_OBJECT_0)                    {
      try                                                                     {
         LT::TMessage *msg = new LT::TMessage(MSG_SAVE_CFG_TO, m_name, tagGM,
             PRIOR_NORMAL, "", // data  config
             strlen("")); // size
         msg->GetTStream()->Write( name, wcslen( name));
         msg->GetTStream()->Write( L"\n", 1);   //new line character, so when reading these message we'll begin with a ReadLn
         if ( m_dataMngr->Write(msg->GetTStream(), true, false, false, true,
             FLAG_SAVE | FLAG_SAVE_NO_VALUE, 0xFFFFFFFF, 0x00000000, true ) == false)  {

            LOG_ERROR2( L"EV_SERIALIZATION_ERROR", m_name);
//               DISPATCH_EVENT( ERROR_CODE, L"EV_SERIALIZATION_ERROR", m_name, m_dataMngr);
            delete msg;
            return false;
         }
         msg->WriteSize();

//             responseMsg->GetTStream()->SaveToFile(L"Configuracion.txt");
         return SendMsg( msg);
      }
      __finally                                                               {
         ReleaseMutex( m_mutex);
      }
   }
   return false;


}

// ----------------------------------------------------------------------------
/**
 Send to GM configuration to be saved
 */ // TODO: crear una funcion en tmodulemngr para sendcfg, sendpartialcfg, request. pasarlas a modulemngr.
bool TModule::SendCfg()                                                       {


   if (  !m_correctlyLoaded )                                                 {
       if ( m_dataMngr->GetAccessLevel() >= TechnicalService)                 {
        //PREGUNTAR
        if (ShowMsg (LoadText(L"MSG_CONF_LOAD_ERROR_SAVE_ANYWAY"), L"INFO", MY_MB_OKCANCEL) == mrOk)
                                                                              {
            m_correctlyLoaded = true;
        }
        else                                                                  {
         DISPATCH_EVENT( WARN_CODE, L"EV_CFG_NOT_SEND", L"Configuration was not sent because wasn't correctly loaded previously", m_dataMngr);
         LOG_ERROR1( L"Configuration was not sent because wasn't correctly loaded previously");
         return false;
        }
      }
      else                                                                    {
         LOG_ERROR1( L"Configuration was not sent because wasn't correctly loaded previously");
         DISPATCH_EVENT( WARN_CODE, L"EV_CFG_NOT_SEND", L"Configuration was not sent because wasn't correctly loaded previously", m_dataMngr);
         return false;
      }
   }

   //evitamos que se lance desde dos threads a la vez, por ejemplo si hacemos un Stop desde interfaz
   //de usuario y, simultaneamente, vencia el temporizador que provoca un AutoSave
   if ( WaitForSingleObject( m_mutex, 0) == WAIT_OBJECT_0)                    {
      try                                                                     {
         LT::TMessage *msg = new LT::TMessage(MSG_SAVE_CFG, m_name, tagGM,
             PRIOR_NORMAL, "", // data  config
             strlen("")); // size

         if ( m_dataMngr->Write(msg->GetTStream(), true, false, false, true,
             FLAG_SAVE | FLAG_SAVE_NO_VALUE, 0xFFFFFFFF, 0x00000000, false ) ==false)  {

            LOG_ERROR2( L"SERIALIZATION_ERROR", m_name);
//               DISPATCH_EVENT( ERROR_CODE, L"EV_SERIALIZATION_ERROR", m_name, m_dataMngr);
            delete msg;
            return false;

         }
         msg->WriteSize();

//             responseMsg->GetTStream()->SaveToFile(L"Configuracion.txt");
         return SendMsg(msg);
      }
      __finally                                                               {
         ReleaseMutex( m_mutex);
      }
   }
   return false;

}

// ----------------------------------------------------------------------------

/**
 Send to GM partial configuration  modified in run time to be saved.
 @return true if partial configuration has been sent correctly.
 */
bool TModule::SendPartialCfg()                                                {



   //evitamos que se lance desde dos threads a la vez, por ejemplo si hacemos un Stop desde interfaz
   //de usuario y, simultaneamente, vencia el temporizador que provoca un AutoSave
   if ( WaitForSingleObject( m_mutex, 0) == WAIT_OBJECT_0)                 {
#ifndef _DEBUG
   try                                                                        {
#endif

      try                                                                  {

         LT::TMessage *responseMsg = new LT::TMessage( MSG_SAVE_PARTIAL_CFG,
                                                m_name,
                                                tagGM,
                                                PRIOR_NORMAL,
                                                "", //data  config
                                                strlen("") ); //size

         // TODO: SITUACIONES EN LAS QUE NO FUNCIONA
         // 1.- No se puede modificar el valor de un tdata que no se ha guardado previamente
         // porque no se dispara el NotifyModify, el ToSave está a false, por lo que no se enviará.
         if (responseMsg) {

            int oldsize = responseMsg->GetTStream()->Size();
            bool ret = m_dataMngr->Write(responseMsg->GetTStream(), true, false, true,
                true, FLAG_SAVE | FLAG_SAVE_NO_VALUE);
            int newsize = responseMsg->GetTStream()->Size();

            if (ret && (newsize != oldsize) && responseMsg->WriteSize())
               return SendMsg( responseMsg);

            delete responseMsg;
            //LOG_INFO1(L"Empty configuration. Nothing to Save.");
            return false;
         }

         LOG_ERROR1(L"Configuration could not be sent to GM");
         return false;


      }
      __finally                                                            {
         ReleaseMutex( m_mutex);
      }
#ifndef _DEBUG
   }
   __except(EXCEPTION_EXECUTE_HANDLER)                                        {
      //Hardwaare Exceptions
      unsigned long code = GetExceptionCode();
      wchar_t msg [1024];
      swprintf(msg, L"TModule::SendPartialCfg: %x",code);
      LOG_ERROR(msg);
   }
#endif
   }
   return false;
}

// ----------------------------------------------------------------------------
///**
// Send configuration and kill timer in TModuleMngr for AutoSave
// */
//void TModule::AutoSave() {
//      //evitamos que se lance desde dos threads a la vez, por ejemplo si hacemos un Stop desde interfaz
//      //de usuario y, simultaneamente, vencia el temporizador que provoca un AutoSave
//      if ( WaitForSingleObject( m_mutex, 0) == WAIT_OBJECT_0)                 {
//         try                                                                  {
//            SendPartialCfg();
//         }
//         __finally                                                            {
//            ReleaseMutex( m_mutex);
//         }
//      }
//}

// ----------------------------------------------------------------------------
/**
 Process messages from module
 @param message to be processed
 */
bool TModule::ProcMsg(LT::TMessage *mymsg) {

   bool proc = false;
   wchar_t org[addressSize];
   mymsg->GetOrg(org, sizeof(org));

   switch (mymsg->Type()) {

      //
   case MSG_PROCESS_DATA:
      if (m_dataMngr)
         proc = m_dataMngr->ProcDataMsg(mymsg);
      break;

      // Signal Init
   case MSG_MODULE_INIT: {
         // Checking availables destinations.
         GetRequiredDestinations();
         Init();
         proc = true;
      } break;
      // Signal Module
   case MSG_MODULE_GO: {
         Go();
         proc = true;
      } break;
      // Signal Module
   case MSG_MODULE_STOP: {
         Stop();
         proc = true;
      } break;
      // Signal Module
   case MSG_MODULE_CLOSE: {
         Close();
         proc = true;
      } break;
   case MSG_GET_CFG: {
//      LOG_INFO1(L"Configuration request Received");
         if( m_commMngr)  {   //we shouldnt get any MSG_GET_MSG without commMngr
            LT::TMessage *responseMsg =  new LT::TMessage( MSG_CFG, m_name, org, PRIOR_NORMAL, "", 0);

            wchar_t mName[addressSize];
            //maybe we should filter by different flags according to the client accessLevel or similar
            unsigned int   filterFlags = FLAG_PUBLISH_REMOTE;

            m_dataMngr->Write( responseMsg->GetTStream(), true, false, false, true , filterFlags, 0xFFFFFFFF, 0x00000000, true);
            responseMsg->WriteSize();  //ToDo check if needed


            bool ret = m_commMngr->PushOutMsg(responseMsg);
            if( !ret )  {
               LOG_WARN1(L"Failed to PushOutMsg in TModule::ProcMsg type MSG_GET_CFG");
            }

         }
      proc = true;
   } break;

   case MSG_CFG_EMBEDDED:
   case MSG_CFG: {
         LOG_INFO1(L"Configuration Received");
         bool ret = LoadCfg(mymsg->GetTStream() /*, mymsg->Type()==MSG_CFG_EMBEDDED */);
         if (ret)
             // automatic start-up
            if (m_persistence && !m_remotePersistence)
               Init();
         ResetComunications();
         proc = true;
      } break;
      // No configuration from module has been loaded
   case MSG_NO_CFG: {
         LOG_WARN1(
             L"Configuration for Module not available. Default configuration will be loaded");
         bool configured = LoadCfg(mymsg->GetTStream());
         if (configured)
            proc = true;
      } break;
//      // Link request for a Module failed.
//   case MSG_COMM_NO_ADDRESS_BY_NAME: {
//         TData *tdata = m_dataMngr->GetDataObject(LBL_STATE_FAULTY);
//         if (tdata) {
//            // Module is set to readyFaulty.
//            tdata->SetAsInt(1);
//
//            LOG_ERROR3(L"Module ", m_name, L" not available");
//         }
//      } break;
   case MSG_HELLO: {
         // A module want to connect with me, answer a hello2
         LT::TMessage *answer = new LT::TMessage(MSG_HELLO_2, m_name, org,
             // Origin of the Hello is detination on hello2
             PRIOR_HIGH, "", 0);

         bool ret = m_commMngr->PushOutMsg(answer);
         if( !ret )  {
            LOG_WARN1(L"Failed to PushOutMsg in TModule::ProcMsg type MSG_HELLO_2");
         }

      } break;
   case MSG_HELLO_2: {
         // Chek if all my destination modules have answered and update init module state
         CheckNextModules(org);
      } break;
   case MSG_SHUTDOWN:                                                         {
         if (m_commMngr)                                                      {
            PostMessage(m_commMngr->GetHWnd(), msg_shutdown, WPARAM(0), LPARAM(0));
            LOG_INFO2(L"---------------- SHUT DOWN -------------", m_name);
         }
         else
            LOG_WARN2(L" Shut Down of module could NOT be done. ", m_name);
      } break;
   case MSG_COMM_IFC_NOTIFY_SUBSCRIBERS:
   case MSG_COMM_IFC_SUBSCRIBE:
   case MSG_COMM_IFC_UNSUBSCRIBE:
   case MSG_COMM_IFC_NOTIFY_SUBSCRIBERS_ERASED:
   //we remove this call because these messages will be also processed by the TCommInterface (already set as TMessageProcessor)
//      if (m_commIfc)
//         proc = m_commIfc->ProcMsg(mymsg);
      break;
   default:
      proc = false;
   }

   return proc;
}

// ----------------------------------------------------------------------------
/**
 Look for modules neighbour modules. That is, TModules where I have to send data
 or TModules where I expect to receive data. Keeping this in mind, it is necessary
 to create a TLink with each of these modules.
 Please note that it is important not to replicate TLinks. One link per connection
 is enough.
 @ return False if no module has been found, True in any other case.
 */
bool TModule::GetRequiredDestinations() {

   // Get Next Modules from the capturer
   wchar_t destModName[128];

   if (!m_dataMngr->GetRequiredDestinations(destModName, sizeof(destModName)) ||
       wcslen(destModName) == 0)
      return false;

   // Save all destination modules on tokensNextModules, and create an array of bool
   // to check module by module in CheckNextModules method
   m_tokensNextModules.clear();
   // LOG_INFO4(" ---------------- ORIGEN",m_name,"DESTINOS: ", destModName);
   TAux::ExtractTokensNotRepeated(destModName, m_tokensNextModules);
   m_numNextmodules = m_tokensNextModules.size();

   if (m_NextModulesOk)
      delete[]m_NextModulesOk;

   if (m_numNextmodules == 0)       {
      LOG_INFO1(L"No Modules for connecting to") ;
      return false;
   }

   m_NextModulesOk = new bool[m_numNextmodules];
   memset(m_NextModulesOk, 0x00, sizeof(bool)* m_numNextmodules);

   // Send Hello messages to all modules
   wchar_t modName[128];
   if (!m_commMngr->GetModuleName(modName, SIZEOF_CHAR(modName)))
      return false;

   std::list<std::wstring>::iterator i;
   for (i = m_tokensNextModules.begin(); i != m_tokensNextModules.end(); i++) {
      wchar_t dest[128];
      wcscpy(dest,(*i).c_str());
//         swprintf(dest, L"%s", (*i).c_str());
         LT::TMessage *request = new LT::TMessage(MSG_HELLO, modName, dest, PRIOR_HIGH, "", 0);

         bool ret = m_commMngr->PushOutMsg(request);
         if( !ret )  {
            LOG_WARN1(L"Failed to PushOutMsg in TModule::GetRequiredDestinations type MSG_HELLO");
         }

   }
   return true;
}

// ----------------------------------------------------------------------------
/**
 Check if all my destination modules have answered

 @param   org   Module origin
 */
bool TModule::CheckNextModules(wchar_t *org) {

   if (!m_NextModulesOk)
      return false;

   // Find the module on the tokens list, and uopdate its state on the array
   std::list<std::wstring>::iterator i;
   int k = 0;

   LOG_INFO2(L" MODUL0: ", m_name);
   for (i = m_tokensNextModules.begin(); i != m_tokensNextModules.end();
   i++, k++) {

      LOG_INFO2(L" LINK A", (*i).c_str());
      if (!wcscmp(org, (*i).c_str())) {
         m_NextModulesOk[k] = true;
         break;
      }
   }

   if (m_resultInit) {
      // if all modules have answered, adjust state to ready
      for (int j = 0; j < m_numNextmodules; ++j)
         if (!m_NextModulesOk[j])
            return true;

      TData *tdata = m_dataMngr->GetDataObject(LBL_STATE);
      tdata->SetAsInt(ready);
   }
   return true;
}

// ----------------------------------------------------------------------------
/**
 Send a Message

 @param msg Message to be sent
 @return true if message was sent sucessfully, false otherwise
 */
bool TModule::SendMsg(LT::TMessage *msg)                                     {

   bool ret = false;
   switch (msg->Type())                                                       {
   /// Messages types for local persistence
   case  MSG_GET_CFG:
   case  MSG_GET_CFG_EMBEDDED:
   case  MSG_GET_TYPE_CONFIG_ACTIVE:
   case  MSG_TYPE_CONFIG_ACTIVE:
   case  MSG_TYPE_CONFIG_GET_AVAILABLES:                 ///< Message to return a list with the avaibles names of types.
   case  MSG_TYPE_CONFIG_AVAILABLES:
//   case  MSG_TYPE_CONFIG_GET_TRANSLATIONS_AVAILABLES:    ///< Message to return a list with the availabel translatons of types.
   case  MSG_TYPE_CONFIG_TRANSLATIONS_AVAILABLES:
      {
      if (m_persistence && !m_remotePersistence) {
         LT::TMessage *resp = NULL;
         if (m_persistence->ProcMsg(msg, resp)) {
            if (resp)
               ret = ProcMsg(resp);
            else
               ret = true;
         }
         else
            LOG_ERROR1( L"TModule::SendMsg Not processed Corryectly. See log messages.");

         if (msg)
            delete msg;
         if (resp)
            delete resp;
         return ret;
      }
      else
         if (m_commMngr)   {
            bool ret = m_commMngr->PushOutMsg(msg);
            if( !ret )  {
               LOG_WARN1(L"Failed to PushOutMsg in TModule::SendMsg  types configs and type_configs");
            }
            return ret;
         }
      }
      break;

   case  MSG_SAVE_CFG:
   case  MSG_SAVE_PARTIAL_CFG:
   case  MSG_SAVE_CFG_TO:
   case  MSG_SET_TYPE_CONFIG_ACTIVE:
   case  MSG_TYPE_CONFIG_NEW:
   case  MSG_TYPE_CONFIG_DELETE:
   case  MSG_TYPE_CONFIG_RENAME:
   case  MSG_TYPE_CONFIG_SAVE_AS:
      {
      if (m_persistence && !m_remotePersistence)                              {
         LT::TMessage *resp = NULL;
         if (m_persistence->ProcMsg(msg, resp))                               {
            if (resp)
               ret = ProcMsg(resp);
            else
               ret = true;
         }
         else                                                                 {
            LOG_ERROR1( L"TModule::SendMsg Not processed Correctly. See log messages.");
//            DISPATCH_EVENT( ERROR_CODE, L"EV_MSG_INCORRECT_PROCESSED", L"See Log messages", m_dataMngr);
         }

         // Notice: No delete msg!!! only resp if exists.
         if (resp)
            delete resp;
         return ret;
      }
      else
         if (m_commMngr)   {
            bool ret = m_commMngr->PushOutMsg(msg);
            if( !ret )  {
               LOG_WARN1(L"Failed to PushOutMsg in TModule::SendMsg  types saves and type_configs_*");
            }
            return ret;
         }
      }
      break;
   case MSG_LT_TXT_TO_FILE:
   case MSG_LT_EVENT:
      {
      if (m_eventPersistence)                                                 {
         LT::TMessage *resp = NULL;
         if (m_eventPersistence->ProcMsg(msg, resp))
            ret = true;
         else {
            LOG_ERROR1(
                L"TModule::SendMsg without commMngr and without persistence");
            ret = false;
         }
         if (msg)
            delete msg;
         if (resp)
            delete resp;
         return ret;
      }
      else
         if (m_commMngr)   {
            bool ret = m_commMngr->PushOutMsg(msg);
            if( !ret )  {
               LOG_WARN1(L"Failed to PushOutMsg in TModule::SendMsg  types MSG_LT_TXT_TO_FILE or MSG_LYRATECHS_EVENT");
            }
            return ret;
         }
      }
      break;
   default: {
         if (m_commMngr)   {
            bool ret = m_commMngr->PushOutMsg(msg);
            if( !ret )  {
               LOG_WARN1(L"Failed to PushOutMsg in TModule::SendMsg  types default");
            }
            return ret;
         }
         break;
      }
   }
   return ret;
}


//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
