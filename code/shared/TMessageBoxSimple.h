//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TMessageBoxSimple.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TMessageBoxSimpleH
#define TMessageBoxSimpleH

#include <list>
#include "TMessage.h"

//----------------------------------------------------------------------------
/// Base-class for all the objets that want to receive messages
/**
*
*/

class TMessageBoxSimple                        {

  protected:
   HANDLE         m_hmutex;               ///< mutex to read/write in list
   wchar_t*       m_name;                 ///< list name
   std::list<LT::TMessage *>   m_list;   ///< list
   HANDLE         m_hsemaphore;           ///< semaphore that indicates if there is any item in list
   wchar_t        m_lockerName[128];       ///< name of process that has locked this list

 public:
   int            m_count;                ///< number ot items in list
   int            m_lockConflicts;        ///< number of conflics when locking

   unsigned long int    m_countLock,
                        m_countRelease;

 public:
   //-- Constructor & Destructor
   TMessageBoxSimple( wchar_t* name );             ///< Constructor
   ~TMessageBoxSimple();                          ///< Destructor

   //-- Public functions
   /// Pop
   LT::TMessage *Pop();                                            ///< pops the first message with higher to lowest priority
   bool  Push           ( LT::TMessage *  t );   ///< push an element in list

   bool ReleaseMutexBox();


   /// Clear
   bool Clear();

   HANDLE   GetSemaphore() {  return m_hsemaphore; }; ///< Gets a pointer to the semaphore of this list
   HANDLE   GetMutex    () {  return m_hmutex;     }; ///< Gets a pointer to the mutex of this list
   int      GetSize     () {  return m_count;      };


};

#endif






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
