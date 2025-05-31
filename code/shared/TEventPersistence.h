//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TEventPersistence.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TEventPersistenceH                              
#define TEventPersistenceH

/// Basic include libraries
#include "tcomm.h"
#include "TPersistence.h"

//----------------------------------------------------------------------------
//********************************  TEventPersistence Class  *****************
//----------------------------------------------------------------------------
/**
   TEventPersistence is used to store & querying Events in the environment
*/
/// Interface for Event Persistence
class TEventPersistence                                                       {

 public:

   virtual bool ProcMsg( LT::TMessage *inputMsg, LT::TMessage *&responseMsg) = 0;
   virtual ~TEventPersistence() {};
   virtual void Init() = 0;
};

#endif



//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
