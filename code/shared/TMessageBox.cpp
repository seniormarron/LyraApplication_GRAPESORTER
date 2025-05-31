//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TMessageBox.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#include "TMessageBox.h"


//----------------------------------------------------------------------------
/**
   Constructor
*/
TMessageBox::TMessageBox( wchar_t* name) :
   TSafeList<LT::TMessage*>(name, 1999999)                                     {
}

//----------------------------------------------------------------------------
/**
   Destructor
*/
TMessageBox::~TMessageBox()                                                   {
}

//----------------------------------------------------------------------------
/**
   Pop acording to priority and time on the list
*/
LT::TMessage *TMessageBox::Pop()                                             {

   DWORD dwWaitResult = WaitForSingleObject( m_hmutex,  500L);   // 500 ml time-out interval
   switch (dwWaitResult)                                                      {
      // The thread got mutex ownership.
      case WAIT_OBJECT_0:
#ifdef _MSC_VER 
		 try                                                                { //ToDo revisar si equivalente a __try en visual studio
#else
         try                                                                {
#endif
			 for( int p = (int)PRIOR_HIGH; p <= (int)PRIOR_LOW; p++ )
               for( std::list<LT::TMessage*>::iterator i = m_list.begin(); i != m_list.end();++i)
                  if( (*i)->GetPriority() == (e_priority)p)                   {
                     LT::TMessage *msg = *i;
                     m_list.erase(i);
                     return msg;
                  }
            return NULL;
         }
#ifdef _MSC_VER 
		 finally                                                                { //ToDo revisar si equivalente a __finally en visual studio
#else
		 __finally                                                                {
#endif
            // Release ownership of the mutex object.
            if (! ReleaseMutex(m_hmutex))                                     {
               // Deal with error.
                  LOG_WARN2( L"TMessageBox::Pop() couldn't release mutex", m_name);

            }
            else                                                              {
               while ( ReleaseMutex( m_hmutex))    {
                  LOG_INFO2( L"TMessageBox::Pop() EXTRA_RELEASEMUTEX", m_name);
               };
            }
         }
         break;

      // Cannot get mutex ownership due to time-out.
      case WAIT_TIMEOUT:
         LOG_WARN2( L"TMessageBox::Pop() couln't get mutex", m_name);
         break;
      // Got ownership of the abandoned mutex object.
      case WAIT_ABANDONED:
         return NULL;
   }
   return NULL;
}

//----------------------------------------------------------------------------
/**
   Clear
*/
void TMessageBox::Clear()                                                     {
   TSafeList<LT::TMessage*>::ClearList();
}


//----------------------------------------------------------------------------
/**
   Constructor

   @param HWND handle of window where to notify with a PostMessage
   @param message message sent into a PostMessage
*/
TMessageBoxPost::TMessageBoxPost( HWND hwnd, UINT message) :
   TListWithMutexPost<LT::TMessage*>( hwnd, message)                         {
}

//----------------------------------------------------------------------------
/**
   Destructor
*/
TMessageBoxPost::~TMessageBoxPost()                                           {
}

//----------------------------------------------------------------------------
/**
   Pop acording to priority and time on the list
*/
LT::TMessage *TMessageBoxPost::Pop()                                         {

   DWORD dwWaitResult = WaitForSingleObject( m_hmutex,  500L);   // 500 ml time-out interval
   switch (dwWaitResult)                                                      {
      // The thread got mutex ownership.
      case WAIT_OBJECT_0:
#ifdef _MSC_VER 
		  try                                                                { //ToDo revisar si equivalente a __try en visual studio
#else
		  try                                                                {
#endif                                                               {
            for( int p = (int)PRIOR_HIGH; p <= (int)PRIOR_LOW; p++ )
               for( std::list<LT::TMessage*>::iterator i = m_list.begin(); i != m_list.end();++i)
                  if( (*i)->GetPriority() == (e_priority)p)                   {
                     LT::TMessage *msg = *i;
                     m_list.erase(i);
                     return msg;
                  }
            return NULL;
         }
#ifdef _MSC_VER 
		  finally                                                                { //ToDo revisar si equivalente a __finally en visual studio
#else
		  __finally                                                                {
#endif
            // Release ownership of the mutex object.
            if (! ReleaseMutex(m_hmutex))                                     {
                       // Deal with error.
            }
         }
         break;
      // Cannot get mutex ownership due to time-out.
      case WAIT_TIMEOUT:
      // Got ownership of the abandoned mutex object.
      case WAIT_ABANDONED:
         return NULL;
   }
   return NULL;
}

//----------------------------------------------------------------------------
/**
   Clear
*/
void TMessageBoxPost::Clear()                                                 {
   ClearList();
}






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
