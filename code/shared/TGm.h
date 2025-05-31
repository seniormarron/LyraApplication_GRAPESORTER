//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TGm.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TGmH
#define TGmH

/// Basic include libraries
#include "TMachineMngr.h"
#include "TBaseModule.h"
//----------------------------------------------------------------------------

// -- Forward Declarations
class TEventPersistence;
class TConfigPersistence;
class TPersistence;
class TGm;

//----------------------------------------------------------------------------
//********************************  TGm Class  *******************************
//----------------------------------------------------------------------------

///  Class for management of Global Manager (GM)
class TGm   : public TBaseModule                                               {

  protected:
   TMachineMngr*           m_machineMngr;          ///< Pointer to Machine Manager
   TPersistence*           m_persistence;          ///< Pointer to manager of persistence
   TEventPersistence*      m_eventPersistence;     ///< Persistence of Events
   bool                    m_started;              ///< Flag to indicate GM has been started
  public:

   // -- constructor & Destructor methods
   TGm   ( wchar_t *moduleName, wchar_t *param, wchar_t *entryPoint, wchar_t *eventsFilePath,TMachineMngr* mngr = NULL);  ///< Construtor for Global Manager Class
   virtual ~TGm  ( );                                                     ///< Destructor

   // -- public methods --
   virtual bool  ProcMsg ( LT::TMessage *msg);    ///< Process Messages for Global Managers
   virtual bool Start () ;                         ///< Start GM
   virtual bool   Init  ();                     ///< Init signal for Module
   virtual bool   Go    ();                     ///< Go signal for Module
   virtual bool   Stop  ();                     ///< Stop signal for Module
   virtual bool   Close ();                     ///< Close signal for Module

   //-- inline methods --
   //----------------------------------------------------------------------------
   /**
      See  TBaseModule for more Details
   */
   virtual TDataManager*  GetMngr     ()                                      {
      return m_machineMngr;
   };

   TPersistence* getPersistence(){
      return m_persistence;
   }


};
#endif






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
