//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TOPCInterface.cpp
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

#include "TOPCInterface.h"

// ---------------------------------------------------------------------------
/**
 Constructor
 @param  mngr  Pointer to a TDataManager where the interface is added
 @param  cmngr Pointer to a TOPCMngr, used to communicate with the OPC Server Dll
 */
TOPCInterface::TOPCInterface( TDataManager *mngr, TOPCMngr *opcmngr )
    : TDataInterface(mngr){

   m_opcMngr = opcmngr;
   m_deleteme = false;
   if( m_opcMngr )
      m_opcMngr->SetOPCInterface(this);
}

// ---------------------------------------------------------------------------
/**
*  Default Destructor
*/
TOPCInterface::~TOPCInterface() {

}

// ----------------------------------------------------------------------------
/**
*  Refresh the interface given a tNode
*/
bool TOPCInterface::RefreshInterface(TNode *tdata)                           {
   return false;       //ToDo: implementar
}
// ----------------------------------------------------------------------------
/**
*  Refresh the interface given a TDataManager
*/
bool TOPCInterface::RefreshAllInterface(TDataManager *mn)                    {
   return false;      //ToDo: implementar
}

// ----------------------------------------------------------------------------
/**
 This method is called when data from a parameter/manager have been modified.
 This method reports all its interfaces of this change for them to be updated.

 @param   node  node  changed. If NULL; it means the manager itself has been modified
 @param   intf  interface  modified node
 @param   forceUpdate forces update manager
 */
bool TOPCInterface::Refresh(TNode *node, TDataInterface *intf,
    bool forceUpdate) {

   if ( !m_enabled )
      return false;

   TData *data = dynamic_cast<TData*>(node);

   bool ret = false;
   if (data && ( data->GetFlags() & FLAG_PUBLISH_REMOTE ) ) {      //if we have to notify the opc server
       return m_opcMngr->UpdateNodeFromTData(data);
   }
   return ret;
}

// ------------------------------------------------------------------------------
/**
 Method created for notifying a set of TDatas in only one message
 */
bool TOPCInterface::Refresh(TDataManager *mn, TDataInterface *intf,
    bool forceUpdate, bool recursive) {

   if ( !m_enabled )
      return false;

   bool ret = true;
   return ret;
}







//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
