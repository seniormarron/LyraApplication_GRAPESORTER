//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TCommInterfaceMngr.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#ifndef TCommInterfaceMngrH
#define TCommInterfaceMngrH
// ---------------------------------------------------------------------------

#include "TCommInterface.h"
#include "TDataMngControl.h"

// ----------------------------------------------------------------------------
// ********************************  TCommInterfaceMngr Class  ****************
// ----------------------------------------------------------------------------
/**
 Class TCommInterface adds communication funcionality to standard TDataInterface. It supports:
 Param reading from both local and remote modules
 Param writing from both local and remote modules
 Param subscption from both local and remote modules to get change notifications
 Inherited TDataInterface to work as a param server to other modules
 */
/// Class TCommInterface adds communication funcionality to standard TDataInterface.
class TCommInterfaceMngr : public TCommInterface, public TDataManagerControl {

protected:


   TData *m_daTarget, *m_daDest;
   wchar_t m_target[1024];
   TCommMngr* m_CommMngr;
   bool m_started;

public:
   TCommInterfaceMngr(wchar_t* name, TDataManagerControl *mngr,
       TCommMngr *cmngr);
   ///< Constructor for Interface
   virtual ~TCommInterfaceMngr();
   ///< Destructor for Interface

   virtual bool ProcMsg(LT::TMessage *msg);

   virtual bool Start();
   virtual bool MyInit() ;

   virtual bool MyClose() {
       m_started = false;
      return !m_started;
   }

   void  SubscribeNodes()  ;
   void LinkNode(TNode* node, wchar_t* targetRoute);
};

#endif






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
