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

#pragma hdrstop
#include "TMessageBoxSimple.h"
#include "TAttrib.h"  // size of char .....

#include <limits>
#pragma package(smart_init)


//
//int filterException(int code, PEXCEPTION_POINTERS ex) {
//    return EXCEPTION_EXECUTE_HANDLER;
//}


//----------------------------------------------------------------------------
/**
   Constructor
*/

TMessageBoxSimple::TMessageBoxSimple(wchar_t* name)                               {

   int maxCount      = std::numeric_limits<int>::max();
   m_name            = _wcsdup(  name);
   m_hmutex          = CreateMutex(       NULL, false, NULL);
   m_hsemaphore      = CreateSemaphore(   NULL, 0 ,maxCount, NULL);
   m_count           = m_list.size();
   m_lockConflicts   = 0;
   memset( m_lockerName, 0x00, sizeof(m_lockerName));

   m_countLock       = 0;
   m_countRelease    = 0;



}

//----------------------------------------------------------------------------
/**
   Destructor
*/
TMessageBoxSimple::~TMessageBoxSimple()                                                   {
   free( m_name);
   // Lock the mutex and never release it
   WaitForSingleObject( m_hmutex,  INFINITE);

   CloseHandle( m_hsemaphore);
   CloseHandle( m_hmutex);
}


bool TMessageBoxSimple::ReleaseMutexBox() {
   if (! ReleaseMutex(m_hmutex)) {
      // debería ser normal que fallara el release porque no lo debería tener pillado
       return false;
   }
   else  {
      while ( ReleaseMutex( m_hmutex))    {
         LOG_INFO2( L"TMessageBoxSimple::ReleaseMutexBox() EXTRA_RELEASEMUTEX", m_name);
      };
   }
   return true;
}




//----------------------------------------------------------------------------
/**
   Pop acording to priority and time on the list
*/
LT::TMessage *TMessageBoxSimple::Pop()                                     {

//   int val = 0x55555555,
//       valnext = 0x33333333;

   DWORD dwWaitResult = WaitForSingleObject( m_hmutex,  500L);   // 500 ml time-out interval
   switch (dwWaitResult)   {
      // The thread got mutex ownership.
      case WAIT_OBJECT_0:
         {
               LT::TMessage *msg= NULL;


               if ( m_countLock != m_countRelease)  {
                  LOG_WARN1(L"Los contadores lock release no cuadran en pop, reajustando");
                  m_countRelease = m_countLock ;
               }
               ++m_countLock;
               DWORD  dw = GetCurrentThreadId( );
               __int64 micro_s = LT::Time::GetTimeMs();
               memset( m_lockerName, 0x00, sizeof(m_lockerName));
               snwprintf(m_lockerName, SIZEOF_CHAR(m_lockerName)-1, L"Pop - th %d, micro_s %Ld, point 0", ( int)dw , micro_s);
               if (!m_list.empty())                                           {
                  for( int p = (int)PRIOR_HIGH; p <= (int)PRIOR_LOW; p++ )       {
                     std::list<LT::TMessage*>::iterator it = m_list.begin();
                     while (it != m_list.end())                                  {
                        if( (*it) &&  (*it)->GetPriority() == (e_priority)p)       {
                           msg = *it;
                           --m_count;
                           snwprintf(m_lockerName, SIZEOF_CHAR(m_lockerName)-1, L"Pop - th %d, micro_s %Ld, point 1", ( int)dw , micro_s);
                           it = m_list.erase(it);
                           snwprintf(m_lockerName, SIZEOF_CHAR(m_lockerName)-1, L"Pop - th %d, micro_s %Ld, point 2", ( int)dw , micro_s);

                           break;
                        }
                        else                                                  {
                           ++it;
                        }
                     }
                     if (msg)                                                 {
                        break;
                     }
                  }

               }
               snwprintf(m_lockerName, SIZEOF_CHAR(m_lockerName)-1, L"Pop  point 3");



              ++m_countRelease;
               // Release ownership of the mutex object.
               if (! ReleaseMutex(m_hmutex))                      {
                  // Deal with error.
                     LOG_WARN4( L"TMessageBoxSimple::Pop() couldn't release mutex", m_name, L" locker: ",m_lockerName);
               }
               return msg;
         }
         break;

      // Cannot get mutex ownership due to time-out.
      case WAIT_TIMEOUT:
         LOG_WARN4( L"TMessageBoxSimple::Pop() couln't get mutex", m_name, L" locker: ",m_lockerName);
         m_lockConflicts++;
         break;
      // Got ownership of the abandoned mutex object.
      case WAIT_ABANDONED:
         LOG_WARN4( L"TMessageBoxSimple::Pop() WAIT_ABANDONED mutex", m_name, L" locker: ",m_lockerName);
         m_lockConflicts++;
         return NULL;
   }
   return NULL;
}

//----------------------------------------------------------------------------
/**
   Push
*/
bool TMessageBoxSimple::Push( LT::TMessage * t )                  {

   bool ret = false;

   DWORD dwWaitResult = WaitForSingleObject( m_hmutex,  500L);   // 500 ms time-out interval
   switch (dwWaitResult)   {
      // The thread got mutex ownership.
      case WAIT_OBJECT_0:
         {
         if ( m_countLock != m_countRelease)  {
            LOG_WARN1(L"Los contadores lock release no cuadran en Push, reajustando");
            m_countRelease = m_countLock ;
         }
         ++m_countLock;
         DWORD  dw = GetCurrentThreadId( );
         memset( m_lockerName, 0x00, sizeof(m_lockerName));
         __int64 micro_s = LT::Time::GetTimeMs();
         snwprintf(m_lockerName, SIZEOF_CHAR(m_lockerName)-1, L"Push - th %d, micro_s %Ld", ( int)dw , micro_s);
            m_list.push_back( t );
            ++m_count ;
         ret = true;
         ++m_countRelease;
         // Release ownership of the mutex object.
         if (! ReleaseMutex(m_hmutex)) {
            // Deal with error.
               LOG_WARN4( L"TMessageBoxSimple::Push() couldn't release mutex", m_name, L" locker: ",m_lockerName);
         }
         if (ret)             {
            LONG prevValue;
            if ( !ReleaseSemaphore( m_hsemaphore, 1L, &prevValue) ) {
               wchar_t str[150];
               snwprintf(str, 150,L"Push: ReleaseSemaphore failed, %s prev=%d",m_name, prevValue);
               LOG_WARN1( str);

               LPVOID lpMsgBuf;
               LPVOID lpDisplayBuf;
               DWORD dw = GetLastError();

               FormatMessage                                                           (
                       FORMAT_MESSAGE_ALLOCATE_BUFFER |
                       FORMAT_MESSAGE_FROM_SYSTEM,
                       NULL,
                       dw,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPTSTR) &lpMsgBuf,
                       0, NULL );

               lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,(lstrlen((LPCTSTR)lpMsgBuf)+ 40)*sizeof(TCHAR));
               swprintf((LPTSTR)lpDisplayBuf, TEXT("failed with error %d: %s"), dw, lpMsgBuf);
               LOG_ERROR1((LPCTSTR)lpDisplayBuf);

               LocalFree(lpMsgBuf);
               LocalFree(lpDisplayBuf);

            }

//            if ( prevValue > 15000 )   {
//               wchar_t str[150];
//               snwprintf(str, 150,L"Push: The amount of messages stored is %d ", prevValue);
//               LOG_WARN1( str);
//            }
         }
         }
         break;

      // Cannot get mutex ownership due to time-out.
      case WAIT_TIMEOUT:
         LOG_WARN4( L"TMessageBoxSimple::Push() couln't get mutex", m_name, L" locker: ",m_lockerName);
         m_lockConflicts++;
         break;
      // Got ownership of the abandoned mutex object.
      case WAIT_ABANDONED:
         LOG_WARN4( L"TMessageBoxSimple::Push() WAIT_ABANDONED mutex", m_name, L" locker: ",m_lockerName);
         m_lockConflicts++;
         return ret;
   }

   return ret;
}


//----------------------------------------------------------------------------
/**
   Clear
*/
bool TMessageBoxSimple::Clear()                                                     {

   DWORD dwWaitResult = WaitForSingleObject( m_hmutex,  INFINITE);
   if ( dwWaitResult!=WAIT_OBJECT_0  )    {
      LOG_ERROR3( L"TSafeList locked in ClearListAndErase" , m_name, m_lockerName);
      return false;
   }
   else                   {
      try {
         if ( m_countLock != m_countRelease)  {
            LOG_WARN1(L"Los contadores lock release no cuadran");
         }
         ++m_countLock;
         m_list.clear();
         //reset semaphore
         while (WaitForSingleObject( m_hsemaphore,  0)==WAIT_OBJECT_0);

         m_count        = m_list.size();

      }
      __finally   {
         ++m_countRelease;
         ReleaseMutex( m_hmutex);
      }
      return true;
   }

}





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
