//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TBaseModule.cpp
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
#include <string>

#include "TBaseModule.h"
#include "TDataMng.h"

#pragma package(smart_init)
//----------------------------------------------------------------------------
/**
   Construct a Module with the given parameters

   @param moduleName Module Name
   @param entryPoint Entry Point of module
*/
TBaseModule::TBaseModule( wchar_t *moduleName, wchar_t *entryPoint)  {

   wcscpy( m_name, moduleName);
   m_commMngr  = NULL;
   m_commIfc   = NULL;
   m_opcMngr   = NULL;

}

//----------------------------------------------------------------------------
/**
   Destructor
*/
TBaseModule::~TBaseModule()                                                   {
}

//----------------------------------------------------------------------------
/**
   Process Message. Used for patch some bugs encountered using trhreads and forms from Borland.
   This methods is only used inside Forms and threads.
*/
void TBaseModule::ProcessMessage()                                            {

   // Quien procesa -> Elimina el mensaje.
   LT::TMessage *msg;
   if (!m_commMngr)
      return;

   if( (bool)(msg = m_commMngr->PopInMsg()))    {
      wchar_t sourceModuleName[ addressSize];
      msg->GetOrg( sourceModuleName, SIZEOF_CHAR(sourceModuleName));
      if (!ProcMsg( msg))  // Si se elimina aquí, los mensajes procesados por listas en threads se eliminan antes de ser procesados.
         if (msg)          // Solo se elimina el mensaje si hay algún error.
            delete msg;
   }
}

//------------------------------------------------------------------------------
/**
   BaseModule start.
   If exists, starts the opcMngr.
   Then request the configuration.
   Finally, starts the commMngr
*/
bool   TBaseModule::Start()                                                 {

   bool ret = true;

   //opc needs to be enabled before receiving the configuration so it can publish
   //the tdata lists
   if( m_opcMngr && !m_opcMngr->Start())                                     {
      ret = false;
   }

   RequestCfg();

   if ( m_commMngr &&  !m_commMngr->Start())                                {
      ret = false;
   }

   return ret;
}





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------




