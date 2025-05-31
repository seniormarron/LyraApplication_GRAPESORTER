//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TMachine.cpp
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

#include "TMachine.h"
#include "TAux.h"
#include "TLabels.h"
#include "TEventDispatcher.h"
#include "THardlockChecker.h"
#include <algorithm>    // Using For remove()

using namespace std;

#define LBL_GO    L"Go"
#define LBL_STOP  L"Stop"
#define LBL_CLOSE L"Close"


//-----------------------------------------------------------------------------
/**
   Check if the name is in the list

   @param   list  List
   @param   name  Name
   @return  True if name is in the list
*/
bool isSomethingHere(std::list<wchar_t*> list, const wchar_t* name)                        {

   std::list<wchar_t *>::iterator  i;
   // Find in the list if name is in the list
   for( i =  list.begin(); i != list.end(); i++)    {
      if (wcscmp(name,(*i)) == 0)            {  // yes
      return true;
      }
   }
   // The name is not in the list
   return false;
}////------------------------------------------------------------------------------///**//   Extract tokens from a string and put them into tokens list////   @param   str      List of modules separated with ; symbol//   @param   tokens   List of destination modules//*///void ExtractTokens( wchar_t *str, std::list<std::wstring> &tokens )                {//
//   wchar_t *p      =  _wcsdup( str );
//   wchar_t *q      =  p;
//   wchar_t *ptoken ;
//
//   // See all the list, and we take the string value before the ";" symbol,
//   // until they have more ";"
//   while ( ( ptoken = wcstok( q, L";") ) != NULL )     {
//      // Copy the string value in tokens
//      std::wstring s_ptoken = std::wstring( ptoken);
//      s_ptoken.erase(remove(s_ptoken.begin(), s_ptoken.end(), ' '), s_ptoken.end());
//      tokens.push_back(s_ptoken);
//
//      q = q + wcslen( ptoken) + 1;
//      if ( q >= p + wcslen( str) )
//         break;
//   }
//   free( p);
//}
//------------------------------------------------------------------------------
/**
   Contructor

   @param   name     Name of machine
   @param   parent   Parent of machine
*/
TMachine::TMachine(const wchar_t *name,  TDataManagerControl *dparent  )
        : TDataManagerControl(name ? name : L"NAME_MACHINE", dparent)           {

   new TData( this, LBL_ENABLED, (int*)NULL, 0, 0, 1, NULL,  FLAG_NORMAL & ~FLAG_SAVE );
   new TData( this, LBL_GO,    (int*)NULL, 0, 0, 1, NULL, FLAG_NORMAL & ~FLAG_SAVE );
   new TData( this, LBL_STOP,  (int*)NULL, 0, 0, 1, NULL, FLAG_NORMAL & ~FLAG_SAVE );
   new TData( this, LBL_CLOSE, (int*)NULL, 0, 0, 1, NULL, FLAG_NORMAL & ~FLAG_SAVE );

   m_flag_start_up      = true;
   m_updating           = false;
   m_closing            = false;
   DataModEventPost     = DataModifiedPost;
}


//----------------------------------------------------------------------------
/**
   Destroy TMachine
*/
TMachine::~TMachine()                                                          {

   // It traverses the list of modules and are eliminated
   list<wchar_t *>::iterator i = m_registeredModulesList.begin();
   while( i != m_registeredModulesList.end())        {
      delete *i;
      i = m_registeredModulesList.erase( i);
   }

   m_configuredModulesList.clear();
}

//------------------------------------------------------------------------------
/**
   Reset machine properties
*/
void TMachine::Reset()                                                         {

   list<wchar_t *>::iterator i = m_registeredModulesList.begin();

    // It traverses the list of modules and are eliminated
   while( i != m_registeredModulesList.end())    {
      delete *i;
      i = m_registeredModulesList.erase( i);
   }

   m_configuredModulesList.clear();

   m_flag_start_up      = true;
   m_updating           = false;
}

//-----------------------------------------------------------------------------
/**
   Start the machine, load configuration machines

   @return True if start the machines
*/
bool  TMachine::Start()                                                        {

   bool ret = true;
   Reset();

   // Return paramater of manager

   TDataManager* modules = GetManagerObject(L"modules");

   if (!modules)
      return false;

//   TData *tdata_n = modules->GetDataObject( L"Number");
//
//   if (tdata_n)
//      tdata_n -> GetAttribValue(cAttribNames[ATTR_VALUE],m_number_modules);

   // Load order
   wchar_t inList[1024];
   unsigned int count = SIZEOF_CHAR( inList);

   TData* orderDa = modules->GetDataObject(L"Order");

   if (!orderDa)        {
      // Search module in the configuration
      std::list<TDataManager *>::iterator it = modules->DataManagerBegin();
      std::list<TDataManager *>::iterator itEnd = modules->DataManagerEnd();
      wchar_t dest[1024] = L"";
      while ( it != itEnd)                                                    {

         TDataManager *dataManager = (*it);
         if (wcslen(dest) > 0) {
            wcscat(dest,L";");
         }
         wcscat(dest,dataManager->GetName());
        it++;
      }
      new TData( modules,  L"Order", NULL, dest, 1024,FLAG_NORMAL);
   }

   // Load Configuration Machines
   TData *tdata     = modules->GetDataObject( L"Order");
   if (tdata)                                                               {
      if ( tdata->GetAttribValue(cAttribNames[ATTR_VALUE], inList, count ) ) {
            TAux::ExtractTokens( inList, m_configuredModulesList );
      }
   }


   if (m_configuredModulesList.size()  == 0)                 {
      LOG_WARN1( L"There is no Order Configured for Modules");
      ret = false;
   }

   return ret;
}

//----------------------------------------------------------------------------
/**
   Check if the module is in registered modules list

   @param   name  Module name
   @return  True if the module are registered in modules list
*/
bool TMachine::isRegistered(const wchar_t* name)                                        {

   std::list<wchar_t *>::iterator  i;
   // See if the name of the string is in registered modules list
   for( i = m_registeredModulesList.begin(); i != m_registeredModulesList.end(); i++)  {
      wchar_t val[32];
      swprintf(val,L"%s",(*i));
      if (wcscmp(name,val) == 0)                 {
      return true;
      }
   }
   //The name could not registered
   return false;
}

//----------------------------------------------------------------------------
/**
   Get the next module in the order to be executed

   @param name_module   Name of actual module
   @param next_module   Name of the next module, null otherwise
*/
bool TMachine::GetNextModule( const wchar_t* name_module, wchar_t* next_module)     {

   std::list<std::wstring>::iterator  i;
   bool found = false;

   //configured Modules list: List of the current modules in the configuration
   for( i = m_configuredModulesList.begin(); i != m_configuredModulesList.end(); i++)    {
      swprintf(next_module, L"%s", (*i).c_str());
      if (found)
         break;
      if (wcscmp(next_module,name_module) == 0)
         found = true;
   }

   if ((wcscmp(next_module,name_module) == 0) && (m_configuredModulesList.size() == 1))
      found = false;

   return found;
}

//----------------------------------------------------------------------------
/**
   Get the previus module in the order to be executed

   @param name_module   Name of actual module
   @param prev_module   Name of the previus module, null otherwise
*/
void  TMachine::GetPreviousModule( const wchar_t*  name_module, wchar_t* prev_module)       {

   std::list<std::wstring>::iterator  i;
   bool found           = false;
   wchar_t prev_name[32]   = {0};
   swprintf(prev_name, L"%s",m_configuredModulesList.front().c_str());

   for( i =  m_configuredModulesList.begin(); i != m_configuredModulesList.end(); i++)    {
      wchar_t name[32]= {0};
      swprintf(name, L"%s", (*i).c_str());
      if (wcscmp(name,name_module) == 0)             {
         found = true;
      }
      if ( found )                                   {
         swprintf(prev_module, L"%s", prev_name);
         return;
      }
      else                                           {
         swprintf(prev_name, L"%s", (*i).c_str());
      }
   }
}
//----------------------------------------------------------------------------
/**
   Get state of the machine

   @param   name  Module name
*/
int TMachine::GetState( const wchar_t* name)                                             {

   if (name == NULL)                                                       {
      LOG_ERROR1( L"Could not find the module name");
      return -1;
   }
   TDataManager* modules_mngr     = GetManagerObject(L"modules");
   if (modules_mngr)                                                      {
      TDataManager* module_mngr = modules_mngr->GetManagerObject(name);
      if (module_mngr)                                                    {
         TData* tdata = module_mngr->GetDataObject(LBL_STATE);
         int state_module = 0;
         if (tdata)                                                       {
            tdata -> GetAttribValue(cAttribNames[ATTR_VALUE],state_module);
            return state_module;
         }
      }
   }
   LOG_ERROR1( L"Could not find the module manager");
   return -1;
}
//----------------------------------------------------------------------------
/**
   Check if the module is in configured module list

   @param   name  Module name
*/
bool TMachine::isConfiguredModule(const wchar_t* name)                        {

   std::list<std::wstring>::iterator  i;
   for( i =  m_configuredModulesList.begin(); i != m_configuredModulesList.end(); i++)  {
      wchar_t val[64];
      swprintf(val, L"%s",(*i).c_str());
      if (wcscmp(name,val) == 0)                                          {
         return true;
      }
   }
   // The module is not in configure list
   return false;
}

//----------------------------------------------------------------------------

/**
   Update state machine
*/
void TMachine::UpdateStateMachine()                                            {

   TDataManager* modules_mngr     = GetManagerObject(L"modules");

   bool s_configured  = true;
   bool s_ready       = true;
   bool s_running     = true;
   bool s_closed      = true;

   std::list<TDataManager *>::iterator it    = modules_mngr->DataManagerBegin();
   std::list<TDataManager *>::iterator itEnd = modules_mngr->DataManagerEnd();

   while ( (it) != itEnd)        {
      TDataManager *dataManager = (*it);
      if (isConfiguredModule( dataManager->GetName()))                         {

         TData *tdata_state     = dataManager->GetDataObject(LBL_STATE);
         int state_module = 0;
         if (tdata_state)                                                {
            tdata_state -> GetAttribValue(cAttribNames[ATTR_VALUE],state_module);

            s_configured  = s_configured  &&  (state_module == configured);
            s_ready       = s_ready       &&  (state_module == ready);
            s_running     = s_running     &&  (state_module == running);
            s_closed      = s_closed      &&  (state_module == closed);
         }
      }
      it++;
   }
      // update state in machine.
      bool isModified = false;

      switch ( m_state.state )                                 {
         case closed:                                          {
            if (s_configured )                                 {
               m_state.state  = configured;
               isModified     = true;
            }
         }
         break;
         case configured:                                      {
///            LOG_INFO1(L"MAQUINA CONFIGURADA");
            if (s_ready )                                      {
               LOG_INFO1(L"MAQUINA EN READY");
               m_state.state = ready;
               isModified = true;
            }
            else if  (s_closed )                               {
               m_state.state = closed;
               isModified = true;
            }
         }
         break;
         case ready:                                           {
            if (s_running )                                    {
               m_state.state = running ;
               isModified = true;
            }
            else if (s_configured)                             {
               m_state.state = configured;
               isModified = true;
            }
            else if  (s_closed )                               {
               m_state.state = closed;
               isModified = true;
            }

            if ( m_flag_start_up )
               m_flag_start_up = false;

         }  break;
         case running:                                         {
            if (s_configured || s_ready)                       {
               m_state.state = configured;
               isModified = true;
               if ( m_flag_start_up )
                  m_flag_start_up = false;
            }
         }
         break;
         default: break;
      }

   // Refresh TData state
   if (isModified )
      GetDataObject(LBL_STATE)->NotifyModified();
}
//----------------------------------------------------------------------------
/**
   Check and Set state
*/
void TMachine::CheckStateModule(TData* state)                                 {

   // Get state MACHINE
   int state_module;
   if ( state )                                                               {

      state -> GetAttribValue(cAttribNames[ATTR_VALUE],state_module);
      if (isConfiguredModule(state->GetParent()->GetName( )) || wcscmp( state->GetParent()->GetName( ), m_name ) == 0)    {
         switch (state_module)                                       {

            case configured:                                         {
               bool ret = (m_state.state == configured);
               if (ret && m_flag_start_up) // To start initialization, all modules (m_state ) must be configured.
                  InitModule();                // INIT all modules in an order.
               else                                                  {
                  ret = (m_state.state == running);
                  if (ret)                                           {
                     wchar_t prev_module[32]  = {0};
                     GetPreviousModule(state->GetParent()->GetName( ),prev_module);  // 1.- Get the name of previous module
                     if (wcscmp(state->GetParent()->GetName( ), prev_module)  != 0)  {
                        int state_prev_module = GetState(prev_module);
                        if (state_prev_module ==  running)
                           StopModule(prev_module);
                     }
                  }
               }
            }
            break;
            case ready:                                                       {
               if (m_closing)
                  break;

               bool ret = (m_state.state == configured);
               if (ret)                                              { //Init the next module
                     wchar_t next_module[32]  = {0};
                     if (! GetNextModule(state->GetParent()->GetName( ),next_module) ) // 1.- Get the name of next module
                        break;
                     if (wcscmp(state->GetParent()->GetName( ), next_module)  != 0)    {
                        int state_next_module = GetState(next_module);
                        if (state_next_module ==  configured)
                           InitModule(next_module);
                     }
               }
               else                                                  {
                     ret = (m_state.state == ready ); //&& !m_flag_first_ongoing );
                     if (ret) // && m_flag_start_up)
                         MyGo();   // GO in an order.
               }
            }
            break;
            case running:                                      {
               bool ret = (m_state.state == ready);
               // Go in order.
               if (ret)                                     {
                  //Init the next module
                  wchar_t next_module[32]  = {0};
                  GetNextModule(state->GetParent()->GetName( ),next_module);  // 1.- Get the name of previous module
                  if (wcscmp(state->GetParent()->GetName( ),next_module)  != 0)     {
                     int state_next_module = GetState(next_module);
                     if (state_next_module ==  ready)
                        GoModule(next_module);
                  }
               }
            }
            break;
            case closed:                                       {
               if ( m_closing)                                 {
                  wchar_t prev_module[32]  = {0};
                  GetPreviousModule(state->GetParent()->GetName( ),prev_module);  // 1.- Get the name of next module
                  if (wcscmp(state->GetParent()->GetName( ),prev_module)  != 0)
                     CloseModule(prev_module);
               }
            }  break;


            default: break;
         }
      UpdateStateMachine();
      }
   }
}


//----------------------------------------------------------------------------
/**
*  Register a module in TMACHINE
   @param   name  Module name
*/
void TMachine::RegisterModule(const wchar_t* name)                            {

   if ( isRegistered (name))       //The module is already registered
      return;

   // Search module in the configuration
   TDataManager* modules_mngr     = GetManagerObject(L"modules");

   std::list<TDataManager *>::iterator it = modules_mngr->DataManagerBegin();
   std::list<TDataManager *>::iterator itEnd = modules_mngr->DataManagerEnd();
   while ( it != itEnd)                                                    {
      TDataManager *dataManager = (*it);
      if (wcscmp( dataManager->GetName( ), name) == 0)                     {
         // Registration of module
         if (!isSomethingHere(m_registeredModulesList,name))               {
            m_registeredModulesList.push_back( _wcsdup(dataManager->GetName( )));

            wchar_t  name[1024], oriFullName[1024];

            // Get TDATA State
            TData *tdata = (dataManager)->GetDataObject(LBL_STATE);
            if ( tdata)                                                    {
               // SUBSCRIPTIONS to the STATE of each module
               // Get its fullname in source module
               name[0]=L'\0';
               oriFullName[0] =L'\0';

               if (!tdata->GetFullName( name, SIZEOF_CHAR(oriFullName)))
                  return;
               wchar_t  *p = name;
               wchar_t  *p_ant = p;

               while (( p = wcschr( p, L'\\')) !=NULL)                     {
                  p++;
                  if ( ( wcschr( p, L'\\')) !=NULL)                        {
                  }
                  else                                                     {
                     swprintf( oriFullName, L"%s",p_ant);
                  }
                  p_ant = p;
               }
               Subscribe(oriFullName,tdata, PRIOR_HIGH);

               // SUBSCRIPTIONS to the STATE of each module
               tdata = (dataManager)->GetDataObject(LBL_STATE_FAULTY);
               if (tdata)     {
                  // Get its fullname in source module
                  name[0]= L'\0';
                  oriFullName[0] =L'\0';

                  if (!tdata->GetFullName( name, SIZEOF_CHAR( oriFullName)))
                     return;
                  wchar_t  *p1 = name;
                  wchar_t  *p_ant1 = p1;

                  while (( p1 = wcschr( p1, L'\\')) !=NULL)             {
                        p1++;
                        if ( ( wcschr( p1, L'\\')) !=NULL)             {
                     }
                     else                                            {
                        swprintf(oriFullName, L"%s",p_ant1);
                     }
                     p_ant1 = p1;
                  }
                  Subscribe(oriFullName,tdata, PRIOR_HIGH);
               }

               // SUBSCRIPTIONS to the ENABLED of each module
               tdata = (dataManager)->GetDataObject(LBL_ENABLED);
               if (tdata)     {
                  // Get its fullname in source module
                  name[0]= L'\0';
                  oriFullName[0] =L'\0';

                  if (!tdata->GetFullName( name, SIZEOF_CHAR( oriFullName)))
                     return;
                  wchar_t  *p1 = name;
                  wchar_t  *p_ant1 = p1;

                  while (( p1 = wcschr( p1, L'\\')) !=NULL)             {
                        p1++;
                        if ( ( wcschr( p1, L'\\')) !=NULL)             {
                     }
                     else                                            {
                        swprintf(oriFullName, L"%s",p_ant1);
                     }
                     p_ant1 = p1;
                  }
                  Subscribe(oriFullName,tdata, PRIOR_HIGH);
               }
            } //
         }
      }
      it++;
   } // End iterator manager list
 }

//------------------------------------------------------------------------------
/**
   Go Module

   @param   nameModule  Name of module
*/
bool   TMachine::GoModule( const wchar_t* nameModule)                                {

   bool ret = true;
   if (nameModule == NULL)                                                  {
      std::wstring name = m_configuredModulesList.front();
      GoModule( name.c_str());
   }
   else                                                                     {
      wchar_t name[32]= {0};
      swprintf(name, L"%s", nameModule);
      LT::TMessage *request	= new LT::TMessage( MSG_MODULE_GO,
                                                tagGM,
                                                name,
                                                PRIOR_NORMAL,
                                                "", //data  config
                                                0 ); //size
      LOG_INFO2( L"Sent MSG_MODULE_GO to", name);
      SendMsg(request);
   }
   return ret;
}

//------------------------------------------------------------------------------

/**
   Close Module
   @param   nameModule  Name of Module
*/
bool   TMachine::CloseModule( const wchar_t* nameModule)                                 {

   m_closing = true;
   bool ret = true;
   if (nameModule == NULL)                                                  {
      std::wstring name = m_configuredModulesList.back();
      CloseModule( name.c_str());
   }
   else                                                                     {
      wchar_t name[32]= {0};
      swprintf(name, L"%s", nameModule);
      LOG_INFO2( L"Close module ", name);
      LT::TMessage *request	= new LT::TMessage( MSG_MODULE_CLOSE,
                                                tagGM,
                                                name,
                                                PRIOR_NORMAL,
                                                "", //data  config
                                                0 ); //size
     SendMsg(request);
   }
   return ret;
}

//----------------------------------------------------------------------------
/**
   Init Module
   @param   nameModule  Name of Module
*/
bool   TMachine::InitModule( const wchar_t* nameModule)                       {

   bool ret = true;
   if ( !m_validated)                                                         {
      DISPATCH_EVENT( ERROR_CODE, L"EV_CORE_HARDLOCK", L"HARDLOCK Couldnt validate machine code" , this);
      LOG_ERROR2( LoadText(L"EV_CORE_HARDLOCK"), L"HARDLOCK Couldnt validate machine code" );
      return false;
   }
   if (nameModule == NULL)                                                    {
            std::wstring name = m_configuredModulesList.front();
            InitModule( name.c_str());
   }
   else                                                                       {
      wchar_t name[32]= {0};
      swprintf(name, L"%s", nameModule);
      LT::TMessage *request	= new LT::TMessage( MSG_MODULE_INIT,
                                             tagGM,
                                             name,
                                             PRIOR_NORMAL,
                                             "", //data  config
                                             0 ); //size
      LOG_INFO2( L"Sent MSG_MODULE_INIT to", name);
      SendMsg(request);
   }
   return ret;
}

//----------------------------------------------------------------------------
/**
   Stop Module
   @param   nameModule  Name of Module
*/
bool   TMachine::StopModule( const wchar_t* nameModule)                                  {

   bool ret = true;
   if (nameModule == NULL)                                                  {
      // signals are sended in reverse order
      std::wstring name = m_configuredModulesList.back();
      StopModule( name.c_str());
   }
   else                                                                     {
      wchar_t name[32]= {0};
      swprintf(name,L"%s", nameModule);
      LT::TMessage *request	= new LT::TMessage( MSG_MODULE_STOP,
                                       tagGM,
                                       name,
                                       PRIOR_NORMAL,
                                       "", //data  config
                                       0 ); //size

      LOG_INFO2( L"Sent MSG_MODULE_STOP to", name);
      SendMsg(request);
   }

 return ret;
}
//----------------------------------------------------------------------------
/**
   Processing messages

   @param   msg
*/
bool   TMachine::ProcMsg( LT::TMessage *msg)                                  {

   bool proc = true;
   switch (msg->Type())                                                     {
      case MSG_GET_CFG:                                                  {
         //ONLY NEEDED FOR REGISTRATION PURPOSES
         wchar_t sourceModuleName [ addressSize ];
         msg -> GetOrg( sourceModuleName, sizeof( sourceModuleName));
         RegisterModule( sourceModuleName );
      }  break;

      case MSG_LT_EVENT:                                                {
         // m_eventEngine->ProcMsg(msg);
      }  break;

      default:
         proc = false;
   }
   return proc;
}


//------------------------------------------------------------------------------
/**
*/
void __fastcall TMachine::DataModifiedPost(TData* da)                         {

   if ( da==NULL)
      return;
   if ( m_updating /*|| ( m_inited == false) */)
      return;

   if ( da->AreYou(LBL_ENABLED))                                              {
     TData *data = GetDataObject(LBL_ENABLED);
      if (data)  {
         bool flag = (bool) (data->AsInt() + da->AsInt());
         data->SetAsInt(int(flag));
      }
      return;
   }

   if ( da->AreYou(LBL_GO))                                                   {
      int val;
      if ( da->AsInt(val))    {
         if ( val  )  {
            if ( !m_flag_start_up )
                  m_flag_start_up = true;
            GetDataObject(LBL_STATE)->NotifyModified();
            MyInit();
            da->SetAsInt(0);

         }
        }
   }
   else
   if ( da->AreYou(LBL_STOP))                                           {
      int val;
      if ( da->AsInt(val))                                                 {
         if ( val  )   {
            MyStop();
            da->SetAsInt(0);
         }
      }
   }
   else if ( da->AreYou(LBL_CLOSE))                                          {
      int val;
      if ( da->AsInt(val))                                                 {
         if ( val  )                                                       {
            LOG_INFO1( L"Close");
            MyClose();
            da->SetAsInt(0);
         }

      }
   }
//   else if ( da->AreYou(L"Init"))                                         {
//      int val;
//      if ( da->AsInt(val))                                                 {
//         if ( val  )                                                       {
//            LOG_INFO1( L"Init machine");
//            MyInit();
//            da->SetAsInt(0);
//         }
//
//      }
//   }
//   else if ( da->AreYou(LBL_STATE))                                           {
//      int val;
//      if ( da->AsInt(val))                                               {
//         switch ( val)                                                {
//            case closed:
//            case running:                                         {
//               TData *dataGo = GetDataObject(LBL_GO);
//               if ( dataGo )
//                  dataGo->SetAsInt( 0);
//
//            }  break;
//            case configured:  {
//                  MyInit();
//                  da->SetAsInt(0);
//            }
//            break;
//         }
//      }
//   }
   m_updating = false;
}

bool TMachine::MyGo     (){

   return  GoModule();

}
bool TMachine::MyStop     (){

      return StopModule();

}

bool TMachine::MyClose     ()                                                 {
   if (m_state.state == configured)
      return CloseModule();
   return false;
}
bool TMachine::MyInit     ()                                               {
   return InitModule();
}

bool TMachine::ShutDown     ()                                               {


   std::list<wchar_t *>::iterator  i;
   // See if the name of the string is in registered modules list
   for( i = m_registeredModulesList.begin(); i != m_registeredModulesList.end(); i++)  {
      if (*i) {
         wchar_t nameModule[32];
         wcscpy(nameModule,(*i) );
         SendShutDownMsg(nameModule);
      }
   }
   return true;
}

bool TMachine::SendShutDownMsg     (wchar_t* name)                                     {

   LT::TMessage *request	= new LT::TMessage( MSG_SHUTDOWN,
                                    tagGM,
                                    name,
                                    PRIOR_NORMAL,
                                    "", //data  config
                                    0 ); //size

   LOG_INFO2( L"Sent MSG_SHUTDOWN to", name);
   SendMsg(request);
   return true;
}





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
