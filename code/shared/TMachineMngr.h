//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TMachineMhgr.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TMachineMngrH
#define TMachineMngrH

/// Basic include libraries
#include "TModuleMngr.h"
#include "TMachine.h"
#include "TPersistence.h"
#include "TcommInterface.h"
#include "definesModulesModes.h"
#include "TUser.h"
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
//********************************  TMachineMngr Class  **********************
//----------------------------------------------------------------------------

//TODO: TEventEngine(): will process messages  msg LYRA_LOG_EVENT by categories
/// This class provide management for machines
class TMachineMngr : public TModuleMngr                                       {

  protected:

   TPersistence*     m_persistence;                      ///<
   TCommInterface*   m_commIfc;                          ///<
   TCommMngr*        m_commMngr;                         ///<
   TData*            m_daInfoMachine;                    ///<


   TData*   m_td1,
         *m_td1l;
   int td1, td1l;

  public:

   // -- Constructor & Destructor methods
   TMachineMngr( const wchar_t *name = NULL, TDataManagerControl *_parent = NULL);    ///< Constructor.
   virtual   ~TMachineMngr();                                                  ///< Destructor.

  protected:
   void __fastcall DataModifiedPost( TData* da);                        ///< Event Handler method for Control of modified data

  public:

   void     SetActiveConfig (wchar_t* name);
   virtual bool SendMsg( LT::TMessage *msg) ;                          ///< Send a message through commMngr

   // -- Subscriptions machines
   virtual bool Subscribe( const wchar_t *oriFullName, TNode *tLocalNode, e_priority priority = PRIOR_NORMAL);

   // -- Methods for creating dynamic managers and parameters (see TDataManager.cpp)
   virtual void          WhatCanYouCreate( std::list<s_wcyc_container_class > &list , char *name , bool manager); ///< Provides creation of managers in inherited classes and dynamic libraries.
   virtual TDataManager *CreateDataMngr(  wchar_t *name, char* code, TDataManager *dparent   );               ///< Creates managers type.
   virtual TData        *CreateData(      wchar_t *name, char* code, TDataManager *dparent   );               ///< Creates a TData parameter with name, code and parent values.

   // -- Management of machine parameters
   virtual bool   LoadCfg        (  LT::TStream *ms = NULL, bool saveBeforeClearToSave = false);           ///< Load configuration.
   virtual bool   SaveCfg        () {return false;};                    ///< Save the configuration.
   virtual bool   ProcMsg        (  LT::TMessage *msg);                ///< Process Message.

   bool           StartMachines  (  TCommMngr* commMngr = NULL,
                                    TCommInterface* CommIfc = NULL,
                                    TPersistence* persistence = NULL);  ///< Initialice machine parameters (Load configuration parameters).

   virtual  bool  MyInit( );
   virtual  bool  MyGo( );
   virtual  bool  MyStop( );
   virtual  bool  MyClose( );
   virtual bool  ShutDown( );

   e_state  GetStateMachines( );


 };
#endif



//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
