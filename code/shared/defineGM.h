/*******************************************************************************

Multiscan Technologies SL
Copyright  2009. All Rights Reserved.

Project:       MVS
Subsystem      Global Manager (GM)
File:          defineGM.h

Author:        Software Group Multiscan Technologies SL <mvs@multiscan.eu>
Version:       0.0
Created:       01/03/2009  E Palacios
Modified:      11/11/2009  E Palacios

Overview:

*******************************************************************************/
//---------------------------------------------------------------------------
#ifndef defineGMH
#define defineGMH
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#pragma hdrstop
#pragma argsused


#include "TGm.h"
#include "TAux.h"

TGm *gm = NULL;         ///< Reference to GM
std::list<TGm*> gmList; ///< List of GM objects

//----------------------------------------------------------------------------
/**
   Close library.
   Do here all necessary steps to close module correctly.
*/
void CloseGM()                                                                  {

   std::list<TGm*>::iterator i = gmList.begin();
   while( i != gmList.end())                          {
      TGm* gm = *i;                              
      if (  gm )
         delete (gm);
      i = gmList.erase( i);
   }
//     MessageBox( NULL, "Closing Module","closing", MB_OK);
}

//----------------------------------------------------------------------------
/**
   Function to call from the launcher to execute Global Manager

   @param  params for Global Manager
   @hForm  handle of MVS' form
   @hApplication handle of main application
   @return  bool true on success, false otherwise
*/
bool ExecuteModuleGM(char* params, void* hForm, void* hApplication)             {

   char * p_params;
   s_params dllInfo;
   bool result = false;
//   p_params    = strdup(params);

   wchar_t* w_params = TAux::STR_TO_WSTR(params);

   if ( TAux::ExtractParams( w_params, dllInfo ))     {
      char* w_xmlfilename = TAux::WSTR_TO_STR(dllInfo.xmlFileName);
      char* w_entrypoint = TAux::WSTR_TO_STR(dllInfo.entryPoint);
      gm = new TGm(dllInfo.name,w_xmlfilename ,w_entrypoint ,Application->MainForm);
      if (gm)     {
         if (gm->Start()) {
            gmList.push_back( gm);
            result = true;
         }
      }
      if (w_xmlfilename) {
          delete [] w_xmlfilename;
      }
      if (w_entrypoint) {
         delete [] w_entrypoint;
      }
   }

   if( dllInfo.name)
      free( dllInfo.name);

   if( dllInfo.target)
      free( dllInfo.target);

   if( dllInfo.xmlFileName)
      free( dllInfo.xmlFileName);

   if (  w_params )
      delete [] w_params;
//      free( p_params);
   return result;
} // end ExecuteModule


#endif
