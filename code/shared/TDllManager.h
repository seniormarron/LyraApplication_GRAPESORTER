//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TDllManager.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef TDllManagerH
#define TDllManagerH
//---------------------------------------------------------------------------

#include <windows.h>
#include <mmsystem.h>

#include "TDataMng.h"
//----------------------------------------------------------------------------

/// The s_dllInfo structure contains the basic information to be loaded as dynamic library.
struct s_dllInfo                                                              {
   wchar_t              dllName[64];
   HINSTANCE            hinst;      ///< Handle to the dll
   WHAT_CAN_YOU_CREATE  wcya;       ///< Function pointer what dll can create
   CREATE_DATAMNGR      cdm;        ///< Function pointer for creation of managers
   CREATE_DATA          cd;         ///< Function pointer for creation of parametes

    /*
      Check if dll instance is in list
      @param l
      @param h instance
      @return
   */
   static bool SearchInList( std::list<s_dllInfo> &l, HINSTANCE h )           {

      std::list<s_dllInfo>::const_iterator i;
      for ( i = l.begin(); i != l.end() ; i++)                                {
         const s_dllInfo d = *i;
         if ( d.hinst  == h  )
            return true;
      }
      return false;
   }
};

//----------------------------------------------------------------------------
//*****************************  TDllManager Class  **************************
//----------------------------------------------------------------------------
//------------------------------------------------------------------------------
/**
   TDllManager provides basic funcionality for creation of managers and parameters
*/
/// This class provides functionality for management of dynamic libraries and
/// functionality for creating managers and parameters

class TDllManager                                                             {

  protected:
   wchar_t                 m_dllNamesAct[256];     ///< List with the dynamic libraries to be loaded
   std::list<s_dllInfo> m_dlls_WCYAdds;         ///< List pointint to the funcitons pointer of the dll's
   int                  m_kernel_Major_Number;  ///< Major Number of Kernel Version
   int                  m_kernel_Minor_Number;  ///< Minor Number of Kernel Version
   int                  m_kernel_Release;       ///< Release of the Kernel
   TDataManager         *m_parent;

  public:
   TDllManager(  TDataManager *parent );                            ///< Constructor
   virtual ~TDllManager( );                     ///< Destructor

  protected:
   bool UnLoadDlls    () ;                       ///< UnLoad dynamic libraries
   bool CheckVersion( wchar_t *dllName);           ///< Checks major number and minor number of the version
   bool LoadDll      ( wchar_t *dllName );         ///< Load dll and registers an entry in m_dlls_WCYAdds list
   void GetLoadedDlls(std::wstring &loadedDlls);

  public:

   bool SetDlls      ( char *dllNames, std::wstring &loadedDlls);       ///< Set and load dynamic libraries
   TDataManager *CreateDataMngr  ( wchar_t* name, char* code, TDataManager*  dparent);                ///< Iterates through list and calls CreateDataManager function for every entry
   TData        *CreateData      ( wchar_t* name, char* code, TDataManager* dparent );                ///< Create a parameter with name, code and parent values.
   void   WhatCanDllsCreate      ( std::list<s_wcyc_container_class > &list , char *name , bool manager); ///< It looks for all the dlls in the list ldlls and calls its function WhatCanYouCreate
   HINSTANCE LoadAndGetInstanceDll( wchar_t *dllname);
   HINSTANCE _LoadAndGetInstanceDll( wchar_t *dllname);
   void SetLangDlls( );

};

#endif






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
