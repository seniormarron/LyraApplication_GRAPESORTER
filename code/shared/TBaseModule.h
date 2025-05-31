//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TBaseModule.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TBaseModuleH
#define TBaseModuleH

#include "TComm.h"
#include "TMessageProcessor.h"
#include "TOPCMngr.h"
#ifndef _MSC_VER
#include "TEventdispatcher.h"
#endif

class TDataManager;

class TCommInterface;

/// Defines base functionality for Modules.
class TBaseModule : public TMessageProcessor                                  {

 protected:

   TDataManager     *m_dataMngr;                   ///< Module Manager
   TCommMngr        *m_commMngr;                   ///< Communication Manager
   TCommInterface   *m_commIfc;                    ///< Communication Interface
   TOPCMngr         *m_opcMngr;                    ///< OPC Manager

   wchar_t           m_name[ 16];                  ///< Module Name

 public:
   TBaseModule (wchar_t *moduleName, wchar_t *entryPoint); ///< Constructor
   virtual ~TBaseModule();                                        ///< Destructor

   // -- public methods --
   virtual bool RequestCfg( bool embedded = false)  {return false;};    ///< Request configuration

   // -- inline funtions --
   // inline virtual wchar_t*         GetName      () {  return m_name;       }; ///< Return Module Name
   virtual TCommMngr*           GetCommMngr  () {  return m_commMngr;   }; ///< Get Communicaton Manager
   virtual TCommInterface*      GetCommIfc   () {  return m_commIfc;    }; ///< Get Comunication Interface
   virtual TDataManager*        GetMngr      () {  return m_dataMngr;   }; ///< Get Module Manager
   virtual TOPCMngr*            GetOPCMngr   () {  return m_opcMngr;   }; ///< Get the OPC Manager
   virtual void                 SetOPCMngr   (TOPCMngr* opcm) {  m_opcMngr = opcm;   }; ///< Set the OPC Manager
   virtual bool                 Start();

//----------------------------------------------------------------------------

   void ProcessMessage() ;

   /*
      Pass HWND of the main window for communications purporse to inbox.
      Inbox will send a messagege to MainWindow that will cause processing of input messages.
   */
   void SeHWND(HWND wHandle)                                              {
      if (m_commMngr && wHandle)
         m_commMngr->CreateInBox(wHandle);
   }
   protected:
      virtual bool   ProcMsg( LT::TMessage *msg)=0;                       ///< Process Message

};
#endif





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
