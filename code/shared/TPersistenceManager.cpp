//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TPersistenceManager.cpp
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

#include "TModuleMngr.h"
#include "TPersistenceManager.h"

#include "TModule.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
#pragma hdrstop

#include "TAux.h"
#include "TLabels.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//----------------------------------------------------------------------------
//********************************  TMachineMngr Class  **********************
//----------------------------------------------------------------------------

/**
   Constructor
*/
TPersistenceManager::TPersistenceManager( const wchar_t *name,      TDataManager *_parent)
         : TDataManager( name ? name : L"PersisteceManager", _parent)                 {

   m_updating = true;
   m_persistence        =  NULL;
   m_eventPersistence   =  NULL;
   m_daSelectType       =  NULL;
   m_daDeleteType       =  NULL;
   m_daNewType          =  NULL;

   SetFlags(GetFlags() | FLAG_SAVE_NO_VALUE);

   TModuleMngr* root = (TModuleMngr*) GetRootMngr();
   if ( root ) {
      m_persistence        = root->GetModule()->GetPersistence();
      m_eventPersistence   = root->GetModule()->GetEventPersistence() ;
   }

   if (m_persistence  == NULL )
      LOG_ERROR1(L"Local Persistence could not be loaded.");

   if (m_eventPersistence  == NULL )
      LOG_ERROR1(L"Local Persistence for Events could not be loaded.");


   if ( m_persistence)  {
      m_daSelectType = new TData( this, L"Types"   , &m_selectType, 0, L"NONE=0", (FLAG_NORMAL3 ) & ~FLAG_SAVE, L"Set a configuration type as Active. ");
//      m_daDeleteType = new TData( this, L"ToDelete", &m_deleteType, 0, L"NONE=0", (FLAG_NORMAL ) & ~FLAG_SAVE, L"Delete a configuration type from persistence.");
      m_daDeleteType = new TData( this, L"ToDelete", NULL, L"", 128,  (FLAG_NORMAL3 ) & ~FLAG_SAVE, L"Delete a configuration type from persistence. Enter Type Name to be deleted and click OK.");
      m_daNewType    = new TData( this, L"New Type", NULL, L"", 128,  (FLAG_NORMAL3 ) & ~FLAG_SAVE, L"Create a new configuration type. Enter Type Name to be created and click OK.");
      UpdateData();
   }

   DataModEventPost  = DataModifiedPost;
   DataModEventPost(NULL);
   m_updating = false;

}

//----------------------------------------------------------------------------
/**
   Destructor
*/
TPersistenceManager::~TPersistenceManager()                                   {
}

void TPersistenceManager::UpdateData()                                        {

   m_updating = true;

   // update current list types.
   std::list<wchar_t*> types;
   m_persistence->GetAvailableTypesNames(types);

   wchar_t  list[256]=L"";
   if (types.size() > 0) {
      int n = 0;

   std::list<wchar_t*>::iterator i  = types.begin();
   while ( i != types.end() )            {
         wchar_t newItem[1024];
         swprintf( newItem, L"%s=%d;", (wchar_t*)(*i), n++);
         wcscat( list, newItem);
         wchar_t* todelete = (*i);
         i  = types.erase (i);
         free (todelete);
   }
}

   m_daSelectType->SetList(list);
//   m_daDeleteType->SetList(list);
   m_daDeleteType->SetAsText(L"");
   m_daNewType->SetAsText(L"");

   m_updating = false;
}

//----------------------------------------------------------------------------
/**
   Data Modified Post
*/
void __fastcall TPersistenceManager::DataModifiedPost( TData* da)              {

   TDataManagerControl* root =  (TDataManagerControl*)GetRootMngr();
   if (!root)
      return;

   if (! m_updating )                                                         {
      if ( root->Status() == configured)                                      {

      if (da == m_daSelectType)                                               {
         int index = m_daSelectType->AsInt();
         wchar_t list[256];
         m_daSelectType-> GetList(list, sizeof(list));
         std::list<std::wstring> types;
         TAux::ExtractTokens( list, types);
         int i = 0;
         for ( std::list<std::wstring>::iterator j = types.begin(); j != types.end(); j++ )   {
            if (index == i) {
               wchar_t* tmp = wcschr( (*j).c_str(), L'=');
               if (tmp)                                                       {
               int size =  tmp - (*j).c_str() ;
               wchar_t value [32];
               swprintf(value, L"%s", (*j).c_str() );
               value [size ] = L'\0';
               if ( m_persistence->SetActiveTypeConfig(value) )               {
                  wchar_t text[1024];
                  swprintf(text,L"Type Configuration with name - %s- Created. ", value);
                  DISPATCH_EVENT(INFO_CODE, L"EV_CORE_CFG",text, this);
                  LOG_INFO2(LoadText(L"EV_CORE_CFG"), text);
               }
               }
               break;
            }
            i++;
         }
      } else if ( da == m_daDeleteType )                                      {
            wchar_t value[32];
            unsigned int size = sizeof(value);
            m_daDeleteType->AsText(value, size);
            if ( m_persistence->DeleteTypeConfig(value) ) {
                  wchar_t text[1024];
                  swprintf(text,L"Type Configuration with name - %s- Deleted. ", value);
                  DISPATCH_EVENT(INFO_CODE, L"EV_CORE_CFG",text, this);
                  LOG_INFO2(LoadText(L"EV_CORE_CFG"), text);
               }
            UpdateData();

      } else if (da == m_daNewType)                                           {
            wchar_t value[32];
            unsigned int size = sizeof(value);
            m_daNewType->AsText(value, size);
            const  wchar_t* moduleName =  root->GetName();
            //if (m_persistence->CreateTypeConfig(value, (wchar_t*)moduleName)) {
            if (m_persistence->CreateTypeConfig(value)) {      //version distribuida
                  wchar_t text[1024];
                  swprintf(text,L"TYPE ACTIVE: %s ", value);
                  DISPATCH_EVENT(INFO_CODE, L"EV_CORE_CFG",text, this);
                  LOG_INFO2(LoadText(L"EV_CORE_CFG"), text);
               }
            UpdateData();
      }
      }
   else {

      UpdateData();
   }
   }

}





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------

