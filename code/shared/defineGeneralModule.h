/*******************************************************************************

Multiscan Technologies SL
Copyright  2009. All Rights Reserved.

Project:       MVS
Subsystem      MVS (Adquisition Device - AD)
File:          defineAD.h

Author:        Software Group Multiscan Technologies SL <mvs@multiscan.eu>
Version:       0.0
Created:       01/03/2009  E Palacios
Modified:      26/05/2010  E Palacios

Overview:
Contains library functions for create, execute and close Adquisition Devices
*******************************************************************************/
//---------------------------------------------------------------------------
#ifndef defineGeneralModuleH
#define defineGeneralModuleH

#include "TModuleVCL.h"
#include "TAux.h"
#include "TXmlManager.h"

#pragma hdrstop
#pragma argsused
//---------------------------------------------------------------------------

std::list<TModuleVCL*> g_generalModuleList; ///< List of Adquisition devices currently in execution.

/**
   Close all adquisition devices currently in execution
*/
void CloseGeneralModule()                                                     {

   std::list<TModuleVCL*>::iterator i = g_generalModuleList.begin();
   while( i != g_generalModuleList.end())   {
      TModuleVCL* module = *i;
      if (  module )
         delete (module);
      i = g_generalModuleList.erase( i);
   }

   if (g_generalModuleList.size() > 0) {
     LOG_ERROR(L"Some Adquisition Device not closed.");
   }
}

/**
   Create and execute Adquistion Device (AD)

   @param params        params for the Adquisition device
   @param hForm         parent main form for MVS.
   @param hApplication  parent application for MVS
   @return  bool true if  AD was correctly created, otherwise
*/
bool ExecuteGeneralModule(char* params, void* hForm, void* hApplication)      {

   char * p_params;
   bool  ret   = false;
   //p_params    = strdup(params);

   TModuleVCL* generalModule = 0;
   s_params moduleInfo;

   wchar_t* w_params = TAux::STR_TO_WSTR(params);
   if ( TAux::ExtractParams( w_params, moduleInfo ))  {
      char* w_xmlfilename = TAux::WSTR_TO_STR(moduleInfo.xmlFileName);
      char* w_entrypoint = TAux::WSTR_TO_STR(moduleInfo.entryPoint);
      char* w_target    = TAux::WSTR_TO_STR(moduleInfo.target);

      TPersistence* persistence = new TXmlManager(w_xmlfilename);
      generalModule = new TModuleVCL(moduleInfo.name, w_target, w_entrypoint,Application->MainForm, NULL,persistence);
      if ( generalModule )  {
         generalModule->StartVCL();
         g_generalModuleList.push_back( generalModule);
         ret = true;
      }
      if (w_xmlfilename) {
         delete [] w_xmlfilename;
      }
      if (w_entrypoint) {
         delete [] w_entrypoint;
      }
      if (w_target) {
         delete [] w_target;
      }
   }
   if( moduleInfo.name)
      free( moduleInfo.name);
   if( moduleInfo.target)
      free( moduleInfo.target);
   if( moduleInfo.entryPoint)
      free( moduleInfo.entryPoint);

   if (  w_params )
      delete [] w_params;
//      free( p_params);
   return ret;
}

//----------------------------------------------------------------------------
/**
   Create and execute Adquistion Device (AD)

   @param params        params for the Adquisition device
   @param hForm         parent main form for MVS.
   @param hApplication  parent application for MVS
   @return  bool true if  AD was correctly created, otherwise
*/
bool ExecuteAloneModule(char* params, TView *f, TModuleMngr *modMngr)         {

//   char * p_params;
//   bool  ret   = false;
//   p_params    = strdup(params);
//
//   TModuleVCL* moduleVCL = 0;
//   s_params dllInfo;
//
//   if ( TAux::ExtractParams( p_params, dllInfo ))                             {
//      //crear una clase heredada, donde el
//      moduleVCL = new TModuleVCL(dllInfo.name, (char *)NULL, dllInfo.entryPoint, (TView *)NULL, modMngr, dllInfo.xmlFileName  );
//
//      if ( moduleVCL )                                                        {
//         moduleVCL->StartAloneVCL( f);
//         g_generalModuleList.push_back( moduleVCL);
//         ret = true;
//      }
//   }
//   if( dllInfo.name)
//      free( dllInfo.name);
//   if( dllInfo.target)
//      free( dllInfo.target);
//   if( dllInfo.entryPoint)
//      free( dllInfo.entryPoint);
//
//   free( p_params);
//   return ret;
   return true;
}

#endif
