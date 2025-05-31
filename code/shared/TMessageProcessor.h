//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TMessageProcessor.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TMessageProcessorH
#define TMessageProcessorH
//---------------------------------------------------------------------------

#include "TMessage.h"

class TMessageProcessor
{
   public:
      TMessageProcessor();
      ~TMessageProcessor();

      virtual bool ProcMsg( LT::TMessage *msg);                       ///< Process Message

};

#endif






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
