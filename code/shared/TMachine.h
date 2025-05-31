//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TMachine.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#ifndef TMachineH
#define TMachineH

#include "tdataMngControl.h"
#include "definesModulesModes.h"
#include "TMessage.h"
#include "TEventDispatcher.h"

//----------------------------------------------------------------------------
//***************************  TMachine Class  ****************************
//----------------------------------------------------------------------------
/// TMachine defines a basic machine .
class TMachine : public  TDataManagerControl                                  {

  protected:
//   int                     m_number_modules;                         ///< Number of modules currently loaded
   bool                    m_updating;                               ///< Flag to indicate that Machine is currently updating
   bool                    m_flag_start_up;                     ///< Check if  has already been initializate
   bool                    m_closing;                                ///< Flag to indicate that Machine is in closing process.

   // Both lists must contain the modules
   std::list<wchar_t*>      m_registeredModulesList;                  ///< List of the current modules registered
   std::list<std::wstring>  m_configuredModulesList;                  ///< List of the current modules in the configuration

  public:
   TMachine(const wchar_t *name,  TDataManagerControl *dparent  );             ///< Constructor for TMachine
   virtual ~TMachine();                                              ///< Default destructor

  protected:

   void  Reset();                                                    ///< Reset machine properties
   bool  GetNextModule     ( const wchar_t*  name_module, wchar_t* next_module);  ///< Return next module
   void  GetPreviousModule ( const wchar_t*  name_module, wchar_t* prev_module);  ///< Return previous module
   int   GetState          ( const wchar_t * name);                            ///< Return state of module named "name"

   virtual  void RegisterModule     ( const wchar_t* name);                   ///< Register a module in machine
   virtual  void UpdateStateMachine ();                              ///< Update state machina
   virtual  bool isRegistered       (const  wchar_t* name);                   ///< Check if the module is in registered modules list
   virtual  bool isConfiguredModule ( const wchar_t* name);                   ///< Check if the module is in configured module list
   void __fastcall DataModifiedPost ( TData* da);                    ///< Event Handler method for Control of modified data

   virtual  bool GoModule     ( const wchar_t* nameModule = NULL);             ///< Go this module
   virtual  bool StopModule   ( const wchar_t* nameModule = NULL);             ///< Stop this module
   virtual  bool CloseModule  ( const wchar_t* nameModule = NULL);             ///< Close this module
   virtual  bool InitModule   ( const wchar_t* nameModule = NULL);             ///< Init this module
   bool  SendShutDownMsg     (wchar_t* name);

  public:
   virtual  bool Start( );                                           ///< Start machine
   virtual  bool MyGo     ();             ///< Go this module
   virtual  bool MyStop   ();             ///< Stop this module
   virtual  bool MyClose  ();             ///< Close this module
   virtual  bool MyInit   ();             ///< Init this module

   bool ShutDown     ();


   virtual  bool ProcMsg( LT::TMessage *msg);                       ///< Processing of messages
   virtual  void CheckStateModule(TData* state); ///< Check and set state

  
};

#endif







//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
