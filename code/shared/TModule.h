//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TModule.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TModuleH
#define TModuleH

#include <list>
#include "TBaseModule.h"
#include "TModuleMngr.h"
#include "TTXTEvent.h"
#include "TEventDispatcher.h"
#include "TXmlManager.h"

#include "definesModulesModes.h"

class TMachineControl;
class TMyMachineModuleMngr;
class THistCatLongTerm; //ToDo: borrar
class THistZone; //ToDo: borrar
//----------------------------------------------------------------------------
//*****************************  TModule Class  ******************************
//----------------------------------------------------------------------------
/// Class to provide mplementation for a Module.
class TModule     : public TBaseModule {

  protected:
   bool                    m_correctlyLoaded;      ///< configuration has benn correctly loaded
   TModuleMngr*            m_dataMngr;             ///< Module Manager
   bool                    m_resultInit,           ///< Result for Initialization of module
                           *m_NextModulesOk;       ///< Flag to indicate destination modules are available to communicate with.
   int                     m_numNextmodules;       ///< Number of destination modules
   std::list<std::wstring> m_tokensNextModules;    ///< Contain names of destination modules.

   TPersistence*           m_persistence;          ///< Pointer to manager of persistence
   TEventPersistence*      m_eventPersistence;     ///< Persistence of Events
   bool                    m_remotePersistence;    ///< True if module has remote persistence.
   HANDLE                  m_mutex;

  protected:

   bool  LoadCfg( LT::TStream *stream/*, bool saveBeforeClearToSave = false*/);                 ///< Load CFG
   virtual bool   GetRequiredDestinations   ();          ///< Get available connections with destinations
   virtual bool   CheckNextModules (wchar_t *org);          ///< Checks the answer for the messages sent from GetNextmodules method

  public:
   virtual bool   Start();                               ///< Start-up module. Load Configuration and prepare module to start working.

   /**
   * The constructor receives the module name and the address of the Global Manager.
   * The address will be formatted as: IP number + Port number. During the construction
   * of the TModule, a TLink will be created between the Global Manager and the module.
   *
   * At the same time, each module will create a TLinkServer, so that a listening port
   * will be open in port 80 UDP.
   *
   * @param moduleName name for the brand new module.
   * @param paramGM address of the GM, necessary to open a TLink with him.
   * @param entryPoint Port where the listener (TLinkServer) will be created.
   */
//   TModule (wchar_t *moduleName, wchar_t *paramGM, wchar_t *entryPoint, wchar_t *eventsFilePath, TModuleMngr *modMngr,
//               TPersistence *persistence = NULL, TEventPersistence * eventPersistence = NULL );
   TModule (wchar_t *moduleName, wchar_t *entryPoint, wchar_t *eventsFilePath, TModuleMngr *modMngr,
                  TPersistence *persistence = NULL, TEventPersistence * eventPersistence = NULL );
   virtual ~TModule();                          ///< Destructor.

   virtual bool RequestCfg   ( bool embedded = false);     ///< Request configuration
   bool SendCfg      ();                        ///< Send configuration to TGm
   bool SendEmbeddedCfgToSaveTo( wchar_t *fileName, bool force=false);   ///< Sends embedded configuration to Gm, to be saved in a file ( Save COnfiguration To)
   bool SendPartialCfg();                       ///< Send Partial configuration to TGm
//   void AutoSave     ();                        ///< Send configuration and kill timer in TModuleMngr for AutoSave
   void ResetComunications(bool loadAd = false);                     ///< Reset the communications, sending MSG_CFG_LOADED and deleting all subscriptions and links.


   // -- Signals --
   virtual bool   Init  ();                     ///< Init signal for Module
   virtual bool   Go    ();                     ///< Go signal for Module
   virtual bool   Stop  ();                     ///< Stop signal for Module
   virtual bool   Close ();                     ///< Close signal for Module
   virtual bool SendMsg( LT::TMessage *msg);

  // -- inline functions
  //------------------------------------------------------------------------------
   /**
      Return manager for Module
      @return manager for module
   */
   TModuleMngr*         GetMngr              () { return m_dataMngr;          };       ///< Get Module Manager
   TPersistence*        GetPersistence       () { return m_persistence;       };
   TEventPersistence*   GetEventPersistence  () { return m_eventPersistence;  };

   static void CALLBACK TimerProcCheckTest(PVOID param, BOOLEAN timerOrWaitFired);

void EXECUTE_TESTS()                                                          {
   RequestCfg( );

}

protected:
   virtual bool   ProcMsg( LT::TMessage *msg);
};

#endif




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
