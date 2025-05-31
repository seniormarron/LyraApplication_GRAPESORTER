//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TMessageBox.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TMessageBoxH
#define TMessageBoxH

#include "TSafeListWithMutexPost.h"
#include "TLists.h"
#include "TMessage.h"

//----------------------------------------------------------------------------
/// Base-class for all the objets that want to receive messages
/**
*
*/
class TMessageBox : public TSafeList<LT::TMessage*>                          {
 public:
   //-- Constructor & Destructor
   TMessageBox( wchar_t* name);                 ///< Constructor
   ~TMessageBox();                          ///< Destructor

   //-- Public functions
   /// Pop
   LT::TMessage *Pop();
   /// Clear
   void Clear();
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

/**
As TMessageBox, but with some differences
   - inherits from TListWithMutexPost, so it has not a sempahore
     where to listen to, nor an event to stop listening
   - instedad, it notifies through a PostMessage to a HWND

*/
class TMessageBoxPost : public TListWithMutexPost<LT::TMessage*>             {
 public:
   //-- Constructor & Destructor
   TMessageBoxPost( HWND hwnd, UINT message);               ///< Constructor
   ~TMessageBoxPost();                                      ///< Destructor

   //-- Public functions
   /// Pop
   LT::TMessage *Pop();
   /// Clear
   void Clear();
};

#endif





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
