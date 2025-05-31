//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TDllManager.cpp
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

#include "TDllManager.h"
#include "TAux.h"
#pragma package(smart_init)
#include "mastermind_num_version.h"
#include "FileVersionInfo.h"
#include "definesFlags.h"
#include "definesLang.h"

//----------------------------------------------------------------------------
/**
   Default Constructor
*/
TDllManager::TDllManager( TDataManager *parent)                                                   {
   m_parent = parent;
   wcscpy(m_dllNamesAct,L"");

   int numbers[4];
   int cont = TAux::ExtractInts( MASTERMIND_VERSION, numbers, sizeof(numbers)/sizeof(numbers[0]) , L',');

   if (cont >= 3)                                                             {
      m_kernel_Major_Number = numbers[0];
      m_kernel_Minor_Number = numbers[1];
      m_kernel_Release      = numbers[2];
   }
   else                                                                       {
      m_kernel_Major_Number = -1;
      m_kernel_Minor_Number = -1;
      m_kernel_Release      = -1;
   }
}

//----------------------------------------------------------------------------
/**
   Default Constructor
*/
TDllManager::~TDllManager( )                                                  {

   if ( !UnLoadDlls ())
      LOG_WARN1(L"Dynamic Libraries Not unloaded.");
}

//----------------------------------------------------------------------------
/**
   Extracts names form dllNames ( separated by ; )
   Loads dlls and inserts handles to important functions in a list
   It doesn't reset its content

   @param dllNames names of dynamic libraries to be loaded
   @param loadedDlls return the loaded Dlls
   @return true
*/
bool TDllManager::SetDlls( char *dllNames, std::wstring &loadedDlls)          {

   try                                                                        {
      wchar_t *wdllNames = TAux::STR_TO_WSTR( dllNames);
      LOG_INFO2( L"SetDlls", wdllNames);
      if ( _wcsicmp( wdllNames, m_dllNamesAct) ==0)                           {
         //if string with dll names hasn't changed, do nothing
         delete [] wdllNames;
         return true;
      }

      //store new string with dll names
      if( wcslen(wdllNames) < sizeof( m_dllNamesAct) )
         wcscpy( m_dllNamesAct , wdllNames );
      else                                                                    {
         delete [] wdllNames;
         return false;
      }

      wchar_t *p      =  _wcsdup( wdllNames );
      wchar_t *q      =  p;
      wchar_t *ptoken ;

      while ( ( ptoken = wcstok( q, L";") ) != NULL )                         {
         //get dllName
         wchar_t *dllName = new wchar_t[ wcslen(ptoken)+1];
         wcscpy( dllName, ptoken);

         //Load Library
         LoadDll( dllName);

         q = q + wcslen( ptoken) + 1;
         if ( q >= p + wcslen( wdllNames ))                                   {
            delete [] dllName;
            break;
         }
         delete [] dllName;
      }
      free( p);
      delete [] wdllNames;
      return true;
   }

   __finally                                                                  {
      GetLoadedDlls( loadedDlls);

   }
   return true;
}

//----------------------------------------------------------------------------
/**
   Checks major number and minor number of the version

   @param   dllName  Name of library
*/
bool TDllManager::CheckVersion( wchar_t *dllName)                                {

//TODO: hacer un GetLibVersion CORE&IMG
   TFileVersionInfo  fvi;

   // Retrieve version information for this module
   if (fvi.Create(dllName))                                                   {

//      int versionNumbers[4];
//      TAux::ExtractInts( MASTERMIND_VERSION, versionNumbers, sizeof(versionNumbers)/sizeof(versionNumbers[0]) , ',');

      int prodVersion[4];
      fvi.GetProductVersion( prodVersion);
     // int majorNumber = versionNumbers[0];
//      if (prodVersion[0]!= m_kernel_Major_Number || prodVersion[1] > m_kernel_Minor_Number /*|| prodVersion[2]!= m_kernel_Release*/)                             {
      if (prodVersion[0]!= m_kernel_Major_Number || prodVersion[1] != m_kernel_Minor_Number || prodVersion[2]!= m_kernel_Release)                                 {
         // Diferent kernel version
         if((prodVersion[0] < m_kernel_Major_Number) ||
            (prodVersion[0] == m_kernel_Major_Number && prodVersion[1] < m_kernel_Minor_Number) ||
            (prodVersion[0] == m_kernel_Major_Number && prodVersion[1] == m_kernel_Minor_Number && prodVersion[2] < m_kernel_Release))                             {

            // If using a newer kernel version just warn
            wchar_t msg [512];
            swprintf(msg, L"WARNING: %s: Kernel version loaded (%d.%d.%d) is newer than expected by DLL (%d.%d.%d). DLL loaded",
               dllName, m_kernel_Major_Number, m_kernel_Minor_Number, m_kernel_Release, prodVersion[0], prodVersion[1], prodVersion[2] );
            MessageBox( NULL, msg, L"DLL and kernel version mismatch", MB_OK);
            LOG_WARN1(msg);
         }
         else                                                                 {
            // If using an older kernel version DLL don't load
            wchar_t msg [512];
            swprintf(msg, L"WARNING: %s: Kernel version loaded (%d.%d.%d) is older than expected by DLL (%d.%d.%d). DLL NOT loaded",
               dllName, m_kernel_Major_Number, m_kernel_Minor_Number, m_kernel_Release, prodVersion[0], prodVersion[1], prodVersion[2] );
            MessageBox( NULL, msg, L"DLL and kernel version mismatch", MB_OK);
            LOG_WARN1(msg);
            return false;
         }
      }

   }
   else                                                                      {
      wchar_t msg [128];
      swprintf(msg, L"Couldn't check dll version %s", dllName);
      LOG_WARN1(msg);

      return false;
   }
   return true;
}
//----------------------------------------------------------------------------

HINSTANCE TDllManager::LoadAndGetInstanceDll( wchar_t *dllName)             {

   if (!CheckVersion ( dllName))                                             {
//      wchar_t msg [128];
//      swprintf(msg, L"Incorrect version %s", dllName);
//      LOG_WARN1(msg);
//      DISPATCH_EVENT(WARN_CODE, EV_KERNEL_INCORRECT_VERSION,msg,this);
      return false;
   }

  return _LoadAndGetInstanceDll( dllName);
}

//----------------------------------------------------------------------------

HINSTANCE TDllManager::_LoadAndGetInstanceDll( wchar_t *dllName)             {

   HINSTANCE hlib  = LoadLibrary( dllName);
   if ( hlib == NULL )                                                        {

      LPVOID lpMsgBuf;
      LPVOID lpDisplayBuf;
      DWORD dw = GetLastError();

      FormatMessage                                                           (
              FORMAT_MESSAGE_ALLOCATE_BUFFER |
              FORMAT_MESSAGE_FROM_SYSTEM,
              NULL,
              dw,
              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
              (LPTSTR) &lpMsgBuf,
              0, NULL );

      lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,(lstrlen((LPCTSTR)lpMsgBuf)+ 40)*sizeof(TCHAR));
      swprintf((LPTSTR)lpDisplayBuf, TEXT("failed with error %d: %s"), dw, lpMsgBuf);

      wchar_t msg [128];
      swprintf(msg,L"Error: %s", dllName );
      LOG_ERROR1(msg);

      swprintf((LPTSTR)lpDisplayBuf, TEXT("failed with error %d: %s"), dw, lpMsgBuf);
      LOG_ERROR1((LPCTSTR)lpDisplayBuf);

      LocalFree(lpMsgBuf);
      LocalFree(lpDisplayBuf);


   } // if hlib
   return hlib;

}

/**
   Load dll and registers an entry in m_dlls_WCYAdds list

   @param dllName Name of library
*/
bool TDllManager::LoadDll( wchar_t *dllName )                                 {
//   if (!CheckVersion ( dllName))                                             {
//      wchar_t msg [128];
//      swprintf(msg, L"Incorrect version %s", dllName);
//      LOG_WARN1(msg);
////      DISPATCH_EVENT(WARN_CODE, EV_KERNEL_INCORRECT_VERSION,msg,this);
//      return false;
//   }
//
//   HINSTANCE hlib  = LoadLibrary( dllName);
//   if ( hlib == NULL )                                                        {
//
//      LPVOID lpMsgBuf;
//      LPVOID lpDisplayBuf;
//      DWORD dw = GetLastError();
//
//      FormatMessage                                                           (
//              FORMAT_MESSAGE_ALLOCATE_BUFFER |
//              FORMAT_MESSAGE_FROM_SYSTEM,
//              NULL,
//              dw,
//              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//              (LPTSTR) &lpMsgBuf,
//              0, NULL );
//
//      lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,(lstrlen((LPCTSTR)lpMsgBuf)+ 40)*sizeof(TCHAR));
//      swprintf((LPTSTR)lpDisplayBuf, TEXT("failed with error %d: %s"), dw, lpMsgBuf);
//
//      wchar_t msg [128];
//      swprintf(msg,L"Error: %s", dllName );
//      LOG_ERROR1(msg);
//
//      swprintf((LPTSTR)lpDisplayBuf, TEXT("failed with error %d: %s"), dw, lpMsgBuf);
//      LOG_ERROR1((LPCTSTR)lpDisplayBuf);
//
//      LocalFree(lpMsgBuf);
//      LocalFree(lpDisplayBuf);
//
//      return false;
//   } // if hlib

   LOG_INFO2(dllName , L" will be loaded");

   HINSTANCE hlib = LoadAndGetInstanceDll( dllName);
   if ( hlib ==NULL)
      return false;

   LOG_INFO2(dllName , L" Ok loaded.");

   if ( s_dllInfo::SearchInList( m_dlls_WCYAdds, hlib))
      return true;
   else                                                                       {

      // IMPORTANT !!: function calls from dynamic libraries are request to write "_" before function name
      WHAT_CAN_YOU_CREATE     wcya  = (WHAT_CAN_YOU_CREATE) GetProcAddress( hlib, "_WhatCanYouCreate" );
      CREATE_DATAMNGR         cdm   = (CREATE_DATAMNGR)     GetProcAddress( hlib, "_CreateDataMngr" );
      CREATE_DATA             cd    = (CREATE_DATA)         GetProcAddress( hlib, "_CreateData" );

      // Set active language for dll
      SET_LANG                lang  = (SET_LANG)            GetProcAddress( hlib, "_SetLang" );
      if (lang)
            lang(ACTIVE_LANG);
      else                                                                    {
//         DISPATCH_EVENT( INFO_CODE, L"SetLang function doesn´t exist in dll:", dllName, m_parent);
         LOG_INFO2( L"SetLang function doesn´t exist in dll:", dllName);
      }
      //set configuration file ( used to initialize LogEngineUDP in each dll)
      SET_FILECONFIG          setFileConfig = (SET_FILECONFIG) GetProcAddress( hlib, "_SetFileConfig" );
      if (setFileConfig)
            setFileConfig( CONFIG_NAME);
      else                                                                    {
//         DISPATCH_EVENT( INFO_CODE, L"SetLang function doesn´t exist in dll:", dllName, m_parent);
         LOG_INFO2( L"SetFileConfig function doesn´t exist in dll:", dllName);
      }

      // Set translations for dll's
      SET_LOAD_TEXT            setLoadText = (SET_LOAD_TEXT)    GetProcAddress( hlib, "_SetLoadText" );
      if (setLoadText)
            setLoadText(LoadText);
      else                                                                    {
         LOG_INFO2( L"SetLoadText function doesn´t exist in dll:", dllName);
//         DISPATCH_EVENT( INFO_CODE, L"SetLoadText function doesn´t exist in dll:", dllName, m_parent);
      }

      if ( wcya==NULL || (cdm == NULL && cd==NULL) )                                   {
         LOG_INFO2( L"Coulndt find some required function", dllName);
//         DISPATCH_EVENT( INFO_CODE, L"Coulndt find some required function", dllName, m_parent);
         return false;
      }

      s_dllInfo dlli;
      dlli.hinst = hlib;
      dlli.wcya  = wcya;
      dlli.cdm   = cdm;
      dlli.cd    = cd;
      wcscpy( dlli.dllName, dllName);
      m_dlls_WCYAdds.push_back( dlli);
      return true;
   } // else
}

//----------------------------------------------------------------------------

void TDllManager::GetLoadedDlls(std::wstring &loadedDlls)                      {

   std::list<s_dllInfo>::iterator i;
   for ( i = m_dlls_WCYAdds.begin(); i!= m_dlls_WCYAdds.end(); i++ )          {
      loadedDlls.append( (*i).dllName);
      loadedDlls.append( L";");
   }

}


/**
   Unload dynamic libraries
   @return
*/
bool TDllManager::UnLoadDlls( )                                               {

   bool ret = true;
   std::list<s_dllInfo>::iterator i;

   i = m_dlls_WCYAdds.begin();
   while ( i != m_dlls_WCYAdds.end() )                                        {

      s_dllInfo dlli = *i;
      HINSTANCE hInst  =    dlli.hinst;
      i = m_dlls_WCYAdds.erase ( i);
      wchar_t dllName[256];
      GetModuleFileName(hInst,dllName, sizeof(dllName)/sizeof(dllName[0]));
      dllName [wcslen(dllName)] = '\0';
      LOG_INFO2(L"Trying to free Dynamic Library: ", dllName);
      if ( ! FreeLibrary( hInst) )                                            {
         LOG_ERROR2(L"Dynamic Library could not be unloaded: ", dllName);
         ret = false;
      }
      else                                                                    {
         LOG_INFO2(L"Dynamic Library Unloaded: ", dllName);
      }
   }
   return ret;
}

void TDllManager::SetLangDlls( )                                              {

   bool ret = true;
   std::list<s_dllInfo>::iterator i;

   for ( i = m_dlls_WCYAdds.begin(); i!= m_dlls_WCYAdds.end(); i++ )          {

      s_dllInfo dlli = *i;
      HINSTANCE hInst  =    dlli.hinst;

   // Set active language for dll
      SET_LANG                lang  = (SET_LANG)            GetProcAddress( hInst, "_SetLang" );
      if (lang)
            lang(ACTIVE_LANG);
   }
}

//----------------------------------------------------------------------------
/**
   To be called from the class that inherits form this one ( tModuleMngr ),
   in its overriden method WhatCanYouCreate.
   It looks for all the dlls in the list ldlls and calls its function WhatCanYouCreate

   @param   list
   @param   name
   @param   manager
*/
void   TDllManager::WhatCanDllsCreate( std::list<s_wcyc_container_class > &list,
                                       char *name , bool manager)             {

   std::list<s_dllInfo>::const_iterator i;
   for ( i= m_dlls_WCYAdds.begin(); i!= m_dlls_WCYAdds.end(); i++ )           {
      (*i).wcya( list, name , manager );
   }
};

//----------------------------------------------------------------------------
/**
   Iterates through list and calls CreateDataManager function for every entry

   @param   name
   @param   code
   @param   dparent
*/
TDataManager *TDllManager::CreateDataMngr( wchar_t *name, char* code,
                                           TDataManager *dparent)             {

   TDataManager *mn;
   std::list<s_dllInfo>::const_iterator i;

   for ( i= m_dlls_WCYAdds.begin(); i!= m_dlls_WCYAdds.end(); i++ )       {
      if ( (*i).cdm)                                                       {
         mn = ((*i).cdm)( name, code, dparent );
         if ( mn != NULL )                                                   {

            mn->AddAttrib( new TAttrib( L"Dll", (*i).dllName));
            return mn;
         }
      }
   }
   wchar_t text[256];
   swprintf(text, L"Error creating Data Manager: with name: %s", name);
   LOG_ERROR1(text);
   return NULL;
}

//----------------------------------------------------------------------------
/**
   Create a parameter with name, code and parent values.

   @param   name     Name of parameter
   @param   code     Code of parameter
   @param   parent   Manager Parent of parameter

*/
TData *TDllManager::CreateData( wchar_t *name, char* code,
                                TDataManager *dparent )                       {

   TData *da;
   std::list<s_dllInfo>::const_iterator i;

   for ( i= m_dlls_WCYAdds.begin(); i!= m_dlls_WCYAdds.end(); i++ )           {
      if ( (*i).cd)                                                           {
         da = ((*i).cd)( name, code, dparent );
         if ( da )
            return da;
      }
   }
   wchar_t text[256];
   swprintf(text, L"Error creating Data: %s", name);
   LOG_ERROR1(text);
   return NULL;
}






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
