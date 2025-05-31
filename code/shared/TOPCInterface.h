//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TOPCInterface.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TOPCInterfaceH
#define TOPCInterfaceH

#include "TDataMng.h"
#include "TDataInterface.h"
#include "TOPCMngr.h"

//----------------------------------------------------------------------------
//********************************  TOPCInterface Class  ********************
//----------------------------------------------------------------------------
/**
   @class TOPCInterface
   Class TOPCInterface communicates changes to the TOPCMngr class.
   TOPCInterface links changes to the TOPCMngr.
*/
class TOPCInterface : public TDataInterface     {

  protected:
   TOPCMngr*    m_opcMngr;                                  ///< Access to manager for DLL comunications

  public:

   TOPCInterface( TDataManager *mngr, TOPCMngr *opcmngr);   ///< Constructor for Interface
   virtual ~TOPCInterface();                                ///< Destructor for Interface

   bool  Refresh( TDataManager *mn, TDataInterface *intf, bool forceUpdate, bool recursive );  ///< Sends modifications on a manager, with posibility of doing it recursively

   virtual bool Refresh             ( TNode *node, TDataInterface *intf,   bool forceUpdate = true);  ///< Refresh of the tNode from a interface
   virtual bool RefreshInterface    ( TNode *node);                              ///< Refresh the interface given a tNode
   virtual bool RefreshAllInterface (TDataManager *mn);                          ///< Refresh the interface given a TDataManager

};

#endif





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
