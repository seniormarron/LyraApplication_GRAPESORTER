//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TComm.cpp
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
#pragma package(smart_init)

#include "tcomm.h"
#include "process.h"
#include "TAux.h"
#include "TCommInterface.h"

#include "TTCPSocketLink.h"
#include "sTriggerRequest.h"

#include "threadsDefs.h"

#ifndef _MSC_VER
#include "TBaseModule.h"
#endif

#define Wait_PendingList_TIMEOUT 200     ///< Time out for PendingList, the list of pending messages for sending.

UINT msg_shutdown = RegisterWindowMessage(L"MSG_SHUTDOWN");
UINT msg_comm_inbox = RegisterWindowMessage(L"MSG_COMM_INBOX");
// vic: not sure if needed

// TMessage type names just for debugging purposes, be sure the names actually
// correspond to its index value if using.
wchar_t * type_names[] = {
   L"MSG_COMM_IFC_SUBSCRIBE ", L"MSG_COMM_IFC_SUBSCRIBE_NO_NOTIFY ",
   L"MSG_COMM_IFC_UNSUBSCRIBE ", L"MSG_COMM_IFC_NOTIFY_SUBSCRIBERS ",
   L"MSG_COMM_IFC_NOTIFY_SUBSCRIBERS_ERASED ", L"MSG_COMM_REQUEST_CONNECT ",
   L"MSG_COMM_RESPONSE_CONNECT ", L"MSG_COMM_KEEP_ALIVE ",
   L"MSG_COMM_KEEP_ALIVE_ACK ", L"MSG_COMM_GET_ADDRESS_BY_NAME ",
   L"MSG_COMM_NO_ADDRESS_BY_NAME ", L"MSG_COMM_ADDRESS_BY_NAME ",
   L"MSG_MODULE_GO ", L"MSG_MODULE_STOP ", L"MSG_MODULE_INIT ",
   L"MSG_MODULE_CLOSE ", L"MSG_GET_CFG ", L"MSG_GET_CFG_EMBEDDED ", L"MSG_CFG ",
   L"MSG_CFG_EMBEDDED ", L"MSG_CFG_LOADED ", L"MSG_NO_CFG ", L"MSG_SAVE_CFG ",
   L"MSG_TYPE_CONFIG_SAVE_AS ", L"MSG_SAVE_CFG_TO ", L"MSG_SAVE_PARTIAL_CFG ",
   L"MSG_LT_EVENT ", L"MSG_PROCESS_DATA ", L"MSG_TYPE_CONFIG_ACTIVE ",
   L"MSG_GET_TYPE_CONFIG_ACTIVE ", L"MSG_SET_TYPE_CONFIG_ACTIVE ",
   L"MSG_TYPE_CONFIG_NEW ", L"MSG_TYPE_CONFIG_DELETE ",
   L"MSG_TYPE_CONFIG_RENAME ", L"MSG_TYPE_CONFIG_GET_AVAILABLES ",
   L"MSG_TYPE_CONFIG_AVAILABLES ",
   L"MSG_TYPE_CONFIG_GET_TRANSLATIONS_AVAILABLES ",
   L"MSG_TYPE_CONFIG_TRANSLATIONS_AVAILABLES ", L"MSG_LT_TXT_TO_FILE ",
   L"MSG_HELLO ", L"MSG_HELLO_2 ", L"MSG_HELLO_M2M ", L"MSG_SHUTDOWN ",
   L"MSG_COMM_SERVER_NOT_RESPONDING", L"MSG_COM_CLIENTLINK_NOT_RESPONDING",
   L"MSG_COM_CLIENT_CONNECTED ", L"MSG_REQUEST_TRIGGERS ",
   L"MSG_RESPONSE_TRIGGERS ", L"MSG_LAST"};

s_triggerRequest triggerRequest;
s_triggerResponseHeader triggerResponseHeader;

// ----------------------------------------------------------------------------
/**
 function to get a remoteLT structure from a wchar_t
 */
//
s_remoteLT getRemoteLTfromWchar(wchar_t * remoteName,
    wchar_t * remoteEntryPoint) {
   // remoteEntryPoint should look like this:
   // TCPSOCKET#127.0.0.1:31500
   s_remoteLT ret;
   wcscpy(ret.remoteName, remoteName);

   wchar_t * remoteCopy = _wcsdup(remoteEntryPoint);

   // Searching for ip
   wchar_t *p_hash = wcschr(remoteCopy, L'#');
   wchar_t *p_semicolon = wcschr(remoteCopy, L':');
   wchar_t *p_port = &(p_semicolon[1]);
   p_semicolon[0] = 0;
   // we set the : position to 0 so we can perform the wcscpy
   if (p_hash && p_semicolon) {
      wcscpy(ret.ipAddress, &p_hash[1]);
      ret.portNumber = _wtoi(p_port);
   }
   return ret;
}

// ----------------------------------------------------------------------------
/**
 Communication thread
 Thread used for communication tasks (send ).
 - It moves the messages between the communications and the in & out boxes to
 avoid blocking other threads.
 */
unsigned WINAPI TCommMngr::CommThread(LPVOID owner) {

   HANDLE events[2];
   DWORD ev;

   s_commThread* proc = (s_commThread*)owner;

   events[0] = proc->commMngr->m_eventSendEnd;
   events[1] = proc->commMngr->m_outBox->GetSemaphore();

   if (proc->commMngr->m_moduleName)
      LOGGER(proc->commMngr->m_moduleName);

   if (events[0] && events[0] != INVALID_HANDLE_VALUE && events[1] && events[1]
       != INVALID_HANDLE_VALUE) {
      while ((ev = WaitForMultipleObjects(2, events, false, 100)) !=
          (WAIT_OBJECT_0)) {

         if (ev == WAIT_OBJECT_0 + 1) {

#ifdef _DEBUG
            // FILE *fitx;
            // if ( _wfopen_s( &fitx, L"TCommMngr_Thread", L"a") == 0 ) {
            // fwprintf(fitx, L"TCommMngr::CommThread: evento m_outBox\n");
            // fclose( fitx);
            // }

#endif

            LT::TMessage *msg = proc->commMngr->PopOutMsg();
            if (msg && proc->commMngr) {
               proc->commMngr->SendMsg(msg);
            }
         }
         else if (ev == WAIT_TIMEOUT) {
            if (proc->commMngr->ReleaseOutBoxMutex()) {
               LOG_WARN(
                   L"Ojo! liberado mutex en outboxthread timeout (pop del outbox)");
            }

         }
      }

   }
#ifdef _DEBUG
   LOG_INFO(L"End of CommMngr::CommThread thread");
#endif
   ResetEvent(events[1]);
   return 0;
}

// ----------------------------------------------------------------------------
/*
 Communication thread, only used when no Module are used.
 Thread used for communication tasks ( receive).
 - It moves the messages between the communications and the in & out boxes to
 avoid blocking other threads.
 */
unsigned WINAPI TCommMngr::CommRecInBoxThread(LPVOID owner) {
   OutputDebugString(
       L" unsigned WINAPI TCommMngr::CommRecInBoxThread( LPVOID owner) Thread used for communication tasks ( receive).\n");
   HANDLE events[10];
   unsigned event_size = 0;

   int start, end;

   DWORD ev;
   s_commThread* proc = (s_commThread*)owner;

   if (proc->commMngr->m_moduleName)
      LOGGER(proc->commMngr->m_moduleName);

   // EVENTS
   events[0] = proc->commMngr->m_inBox->GetSemaphore();
   event_size++;
   events[1] = proc->commMngr->m_eventRecvInBoxEnd;
   event_size++;

   if (events[0] && events[0] != INVALID_HANDLE_VALUE && events[1] && events[1]
       != INVALID_HANDLE_VALUE) {

      while ((ev = WaitForMultipleObjects(event_size, events, false, 100)) !=
          (WAIT_OBJECT_0 + 1)) {
         if (ev == WAIT_OBJECT_0) {
#ifdef _DEBUG
            // FILE *fitx;
            // if ( _wfopen_s( &fitx, L"TCommMngr_Thread", L"a") == 0 ) {
            // fwprintf(fitx, L"TCommMngr::CommRecInBoxThread: sacando mensaje que in_box\n");
            // fclose( fitx);
            // }
#endif

            if (proc->commMngr) {

               LT::TMessage *msg = NULL;
               while ((msg = proc->commMngr->GetInboxMsg()) != NULL) {
                  start = timeGetTime();
                  std::list<TMessageProcessor*>::iterator i =
                      proc->commMngr->MessageProcessorListBegin();
                  bool processed = false;
                  while (i != proc->commMngr->MessageProcessorListEnd()) {
                     if (*i) {
                        processed = (*i)->ProcMsg(msg) | processed;
                     }
                     i++;
                  }
                  int timeProc = timeGetTime() - start ;

                  if ( timeProc > proc->commMngr->m_logProcInputTimesOlderThanMs )   {
                     wchar_t buff[128];
                     buff[SIZEOF_CHAR(buff)-1]=0;
                     snwprintf(buff,SIZEOF_CHAR(buff)-1,L"Message took %d ms to be proccessed, type: %s", timeProc, type_names[msg->Type()]);
                     LOG_INFO(buff);
                  }

                  // if some messageprocessor has processed the message, we delete them
                  // therefore the current system only allows one TMessageProcessor to process the message without having problems freeing the message
                  // if (processed) //delete allways the message, processed or not
                  delete msg;

               } // inbox while



            }




         }
         else if (ev == WAIT_TIMEOUT) {
            if (proc->commMngr->ReleaseInBoxMutex()) {
               LOG_WARN(
                   L"Ojo! liberado mutex en inboxthread timeout (pop del outbox)");
            }
         }
      }
   }
#ifdef _DEBUG
   LOG_INFO(L"TEnd of CommMngr::CommRecInBoxThread thread");
#endif
   ResetEvent(events[1]);
   return 0;
}

// ----------------------------------------------------------------------------
/**
 Constructor for Communication Manager

 @param  moduleName   Char pointer with the name of the module that created de TCommMngr.
 @param  paramGM      Char pointer with the parameter to create TLink to communicate with the GM.
 @param  entryPoint   Char pointer with the parameter to create the default TLinkServer.
 */
// TCommMngr::TCommMngr    ( wchar_t *moduleName, wchar_t *paramGM, wchar_t *entryPoint)
TCommMngr::TCommMngr(wchar_t *moduleName, wchar_t *entryPoint)
    : LinkFactory(this) {

   if ((moduleName == NULL) || (wcslen(moduleName) == 0) || (entryPoint == NULL)
       || (wcslen(entryPoint) == 0)) {
      LOG_ERROR1(L"Bad Parameters");
      return;
   }
   m_started = false;
   m_status = e_commMngrStatus::DISCONNECTED;

   wcsncpy(m_moduleName, moduleName, SIZEOF_CHAR(m_moduleName));
   m_moduleName[SIZEOF_CHAR(m_moduleName) - 1] = 0;
   // needed if source is larger than dest
   // if ( paramGM)
   // m_paramGM = _wcsdup( paramGM);
   // else
   // m_paramGM = NULL;

   m_reEnter = false;
   m_msgNumber = 1;

   // m_usingModule     = usingModule;

   // Create Message's boxes
   wchar_t cadena[32];
   cadena[SIZEOF_CHAR(cadena)-1] = 0;
   snwprintf(cadena,SIZEOF_CHAR(cadena)-1, L"%s_inbox", moduleName);

   // m_inBox  = new TMessageBox( cadena);
   m_inBox = new TMessageBoxSimple(cadena);

   snwprintf(cadena,SIZEOF_CHAR(cadena)-1, L"%s_outbox", moduleName);
   // m_outBox       = new TMessageBox( cadena);
   m_outBox = new TMessageBoxSimple(cadena);

   m_linkLocalHandleList = new TSafeList<HANDLE>(L"localHandle");

   // Create list of pending messages
   m_pendingList = new std::list<LT::TMessage*>;
   // mutex to synchronize access
   m_mutexPendingList = CreateMutex(NULL, false, NULL);

   // mutex to synchronize access
//   m_mutexLinkList = CreateMutex(NULL, false, NULL);

    m_swrLockLinkList  = new SRWLOCK();
    InitializeSRWLock( m_swrLockLinkList );

   // -- Create thread for communication (send)
   m_eventSendEnd = CreateEvent(NULL, true, false, NULL);
   // m_eventRecvEnd = CreateEvent( NULL, true, false, NULL);
   m_eventRecvInBoxEnd = CreateEvent(NULL, true, false, NULL);

   // Creates the send thread suspended until start
   unsigned dx;
   m_commTh.commMngr = this; // commMngr    -> to acces the Inbox & outBox
   m_commTh.handleThread = (HANDLE) _beginthreadex(NULL,
       // address of thread security attributes
       (DWORD)0x10000, // initial thread stack size, in bytes
       CommThread, // address of thread function
       &m_commTh, // argument for new thread
       CREATE_SUSPENDED, // creation flags
       &dx); // address of returned thread identifier

   SetThreadName(dx, "OutBoxThread");

   m_commRecInBoxTh.commMngr = this;
   // commMngr    -> to acces the Inbox & outBox
   m_commRecInBoxTh.handleThread =
       (HANDLE) _beginthreadex(NULL, // address of thread security attributes
       (DWORD)0x10000, // initial thread stack size, in bytes
       CommRecInBoxThread, // address of thread function
       &m_commTh, // argument for new thread
       CREATE_SUSPENDED, // creation flags
       &dx); // address of returned thread identifier
   SetThreadName(dx, "InBoxThread");

   // Create TLinkServer with the given entryPoint
   if (entryPoint)
      m_server = (TLinkServer*)LinkFactory.CreateTLink(L"", entryPoint);
   else {
      m_server = NULL;
      LOG_WARN1(L"Server for communications not created.");
   }

   SetServerMode(true);

   m_lastServerNotRespondingTime = 0;

   // Create list of  message processors
   m_messageProcessorsList = new std::list<TMessageProcessor*>;

   m_hWnd = NULL;
#ifdef _DEBUG
   ClearTimeStat();
#endif
   // Start "Queue Timer" for processing expired messages
   CreateTimerQueueTimer(&m_timerHandle, NULL, TimerProcCheckExpiredMsg, this,
       // 0,    // &arg
       0, TIMER_PENDING_MSG, WT_EXECUTEINIOTHREAD);
   // option to be changed if there are problems with other timers

   // Start "Queue Timer" for processing expired links
   CreateTimerQueueTimer(&m_timerLinksHandle, NULL, TimerProcCheckExpiredLinks,
       this, // 0,    // &arg
       0, TIMER_LINKS_CHECK, WT_EXECUTEINIOTHREAD);
   // option to be changed if there are problems with other timers


   //public values to debug in some machines, we set here some default values
   m_pendingListSize    = 0;
   m_pendingSizeDebug   = 30;
   m_inBoxSize          = 0;
   m_outBoxSize         = 0;
   m_BoxSizeDebug       = 200;
   m_logProcInputTimesOlderThanMs = 30;     //30 ms

   m_connectedToServer = false;
}

// ----------------------------------------------------------------------------
/**
 Frees up memory
 */
TCommMngr::~TCommMngr() {

   // Destroy "Queue Timer"
   DeleteTimerQueueTimer(NULL, m_timerHandle, INVALID_HANDLE_VALUE);
   // Destroy "Queue Timer"
   DeleteTimerQueueTimer(NULL, m_timerLinksHandle, INVALID_HANDLE_VALUE);

   // First we send the end events to the threads to finish them:
   SetEvent(m_eventSendEnd);
   SetEvent(m_eventRecvInBoxEnd);

   if (m_commRecInBoxTh.handleThread)
      WaitForSingleObject(m_commRecInBoxTh.handleThread, INFINITE);

   if (m_commTh.handleThread) {
      WaitForSingleObject(m_commTh.handleThread, INFINITE);
   }

   //vamos a esperar a que se cierren para realmente cerrar el manejador
   CloseHandle(m_eventSendEnd);
   CloseHandle(m_eventRecvInBoxEnd);


   CloseHandle(m_commTh.handleThread);
   m_commTh.handleThread = NULL;

   if (m_commRecInBoxTh.handleThread) {
      CloseHandle(m_commRecInBoxTh.handleThread);
      m_commRecInBoxTh.handleThread = NULL;
   }
   // time to close the server link (receiving thread must be killed)
   if (m_server)
      delete m_server;

   // now the thread should be closed so we can empty and delete the lists
   // Synchronize
//   if (m_mutexLinkList)
//      WaitForSingleObject(m_mutexLinkList, 500);

    AcquireSRWLockExclusive( m_swrLockLinkList );

   // delete all TLinks
   std::list<TLink*>::iterator i = m_linkList.begin();
   while (i != m_linkList.end()) {
      if ((*i)) {
         (*i)->Close();
         delete *i;
      }
      i = m_linkList.erase(i);
   }
   // // Close mutex
//   if (m_mutexLinkList) {
//      ReleaseMutex(m_mutexLinkList);
//      CloseHandle(m_mutexLinkList);
//   }
    ReleaseSRWLockExclusive( m_swrLockLinkList );

   // Delete pending messages' list
   if (m_pendingList) {
      // Synchronize
      if (m_mutexPendingList)
         WaitForSingleObject(m_mutexPendingList, 500);

      std::list<LT::TMessage*>::iterator i = m_pendingList->begin();
      while (i != m_pendingList->end()) {
         if (*i) {
            LT::TMessage *msg = *i;
            i = m_pendingList->erase(i);
            delete msg;
         }
         else
            i++;
      }

      m_pendingList->clear();
      delete m_pendingList;

   }

   if (m_mutexPendingList)
      ReleaseMutex(m_mutexPendingList);

   // Close the mutex for the pending list
   if (m_mutexPendingList)
      CloseHandle(m_mutexPendingList);

   // delete messages on boxes

   if (m_outBox) {
      m_outBox->Clear();
      delete m_outBox;
   }

   if (m_inBox) {
      m_inBox->Clear();
      delete m_inBox;
   }

   if (m_linkLocalHandleList) {
      m_linkLocalHandleList->ClearList();
      delete m_linkLocalHandleList;
   }

   if (m_messageProcessorsList) {
      m_messageProcessorsList->clear();
      delete m_messageProcessorsList;
   }


   // if ( m_paramGM)
   // free( m_paramGM);

}

// ----------------------------------------------------------------------------
/**
 Sets the commInterface to use it when not using module as InBox thread receiver.
 @param   commIfc     TCommInterface pointer
 */
void TCommMngr::SetCommInterface(TCommInterface * commIfc) {
   m_commIfc = commIfc;
   AddMessageProcessor(m_commIfc);
}

// ----------------------------------------------------------------------------
/**
 Starts the process of the commMngr:
 - Start the Sending thread
 - Start the reception thread
 - Starts the default server
 */
bool TCommMngr::Start() {

   m_started = false;

   // Resume the in and out box threads
   ResumeThread(m_commTh.handleThread);
   ResumeThread(m_commRecInBoxTh.handleThread);

   // Opens the default server
   if (m_server && m_server->Open()) {
      // if( m_paramGM && wcslen(m_paramGM))                                     {
      //
      // //-- Try to communicate with the GM
      // // Create a virtual MSG_ADDRES_BY_NAME message with the data taht we already
      // // have to start the protocol to communicate for the first time with a module
      // wchar_t text[64];
      // wchar_t txt[64];
      // swprintf( text, L"%s#%s\0", tagGM, m_paramGM);
      // swprintf( txt, L"%s#%s\0", tagGM, m_paramGM);
      // LT::TMessage *virtualMsg  = new LT::TMessage(    MSG_COMM_ADDRESS_BY_NAME, L"VIRTUAL",
      // m_moduleName, PRIOR_NORMAL, (char*)txt, wcslen( text)*sizeof(wchar_t));
      //
      // // Call the process message function
      // if( ProcMsg( virtualMsg))                                            {
      // Sleep((unsigned int) 500);
      // m_started = true;
      // }
      // }
      // else  {
      // // We're the GM, already started
      m_started = true;
      // }
   }
   // both server and clients starts disconnected,
   // the TModule will change the server to connected when ready(configuration ready)
   // the clients will change to connected when ready(when they get and decode the configuration mensaje)
   // m_status    = e_commMngrStatus::DISCONNECTED;

   // it seems that the module init is what calls this Tcomm::Start so the configuration is already ready
   if (m_server) {
      m_status = e_commMngrStatus::CONNECTED;
   }
   else {
      m_status = e_commMngrStatus::DISCONNECTED;
   }

   return m_started;
}

// ----------------------------------------------------------------------------
/**
 CreateInbox when using Module
 */
void TCommMngr::CreateInBox(HWND hThread) {
   // if (  hThread )                                                                  {
   // m_hWnd   = hThread;
   // m_inBox  = new TMessageBoxPost( hThread , msg_comm_inbox );
   // }
   // else
   // LOG_ERROR1(L"Couldn't create InBox");
   LOG_WARN1(
       L"The inbox managed from outside using the HWND hThread is not longer supported. Now it has its own receiving thread");
}

// ----------------------------------------------------------------------------
/**
 Push a message to the inbox list
 @param  msg TMessage `pointer
 */
bool TCommMngr::PushInMsg(LT::TMessage *msg)         {
   m_inBoxSize = m_inBox->GetSize();
   if( m_inBoxSize > m_BoxSizeDebug )                 {
      wchar_t buff[128];
      buff[ SIZEOF_CHAR(buff)-1] = 0;
      snwprintf(buff, SIZEOF_CHAR(buff)-1,L" InBox has %d messages",m_inBoxSize);
      LOG_WARN1(buff);
   }
   if (m_inBox && msg && msg->Check())                {
      return m_inBox->Push(msg);
   }
   return false;
}

// ----------------------------------------------------------------------------
/**
 Push a message to the output list

 @param   msg     TMessage pointer
 */
bool TCommMngr::PushOutMsg(LT::TMessage *msg) {
#ifdef _DEBUG
   // wchar_t origen[50],
   // destino[50];
   // msg->GetDest( destino, 50);
   // msg->GetOrg( origen, 50);

   // FILE *fitx;
   // if ( _wfopen_s( &fitx,origen, L"a") == 0 ) {
   // fwprintf(fitx, L"TCommMngr::PushOutMsg: origen=%s destino=%s tipo=%d\n", origen, destino, msg->Type());
   // fclose( fitx);
   // }

#endif
   if (m_started)
      if (m_outBox && msg && msg->Check()) {
         // #ifdef _DEBUG
         // FILE *fitx = _wfopen( origen, L"a");
         // fwprintf(fitx, L"TCommMngr::PushOutMsg, msg Checked: origen=%s destino=%s tipo=%d\n", origen, destino, msg->Type());
         // fclose( fitx);
         // #endif

         bool ret = m_outBox->Push(msg);
         m_outBoxSize = m_outBox->GetSize();
         if( m_outBoxSize > m_BoxSizeDebug )  {
            wchar_t buff[128];
            memset( buff, 0x00, sizeof(buff));
            snwprintf(buff,SIZEOF_CHAR(buff)-1,L" OutBox has %d messages",m_outBoxSize);
            LOG_WARN1(buff);
         }

         if (!ret) {
            LOG_WARN2(L"TCommMngr::PushOutMsg failed of msg type ",
                type_names[msg->Type()]);
         }
         return ret;

      }

   return false;
}

bool TCommMngr::ReleaseOutBoxMutex() {
   return m_outBox->ReleaseMutexBox();
}

bool TCommMngr::ReleaseInBoxMutex() {
   return m_inBox->ReleaseMutexBox();
}

// ----------------------------------------------------------------------------
/**
 Pop a message from the inbox list
 */
LT::TMessage *TCommMngr::PopInMsg(int type, int priority) {
   LT::TMessage * msg;
   if (m_inBox)
      msg = m_inBox->Pop();

#ifdef _DEBUG
   if (msg)
      CalculateStatistics(msg);
#endif

   m_inBoxSize = m_inBox->GetSize();
   return msg;
}

// ----------------------------------------------------------------------------
/**
 Pop a message from the outbox list
 */
LT::TMessage *TCommMngr::PopOutMsg(int type, int priority) {
   LT::TMessage *msg = NULL;
   try {
      msg = NULL;
      if (m_outBox) {
         msg = m_outBox->Pop();
      }
   }
   catch (...) {
      LOG_ERROR(L"Could'nt pop message from m_outBox");
   }
   m_outBoxSize = m_outBox->GetSize();

   return msg;
}

// ----------------------------------------------------------------------------
/**
 Send Message

 @param   msg   Message to be sent
 @return  true if the the message has been sent succesfully, false otherwise.
 */
e_txMsgStatus TCommMngr::SendMsg(LT::TMessage *msg) {

#ifdef _DEBUG

   // FILE *fitx;
   // if ( _wfopen_s( &fitx,L"TCommMngr_Thread", L"a") == 0 ) {
   // if ( msg)   {
   // wchar_t origen[50],
   // destino[50];
   // msg->GetDest( destino, 50);
   // msg->GetOrg( origen, 50);
   //
   // fwprintf(fitx, L"TCommMngr::SendMsg: started =%d, origen=%s destino=%s tipo=%d, size=%d,  \n", m_started, origen, destino, msg->Type(), msg->DataSize());
   // }
   // else  {
   // fwprintf( fitx, L"TCommMngr::SendMsg: started =%d NULL msg%d,  \n", m_started);
   // }
   // fclose( fitx);
   // }
#endif

   SetMessageNumber(msg);

   if (!msg || !m_started)
      return TXMSG_FAILED;

   wchar_t dest[addressSize];

   // Messages sent to myself, eg. events of modules. So that all messages are processed by windowproc
   if (wcscmp(msg->GetDest(dest, sizeof(dest)), m_moduleName) == 0) {
      if (!PushInMsg(msg)) {
         LOG_WARN2(L"PushInMsg failed in SendMsg (message to myself)",
             type_names[msg->Type()]);
         return TXMSG_FAILED;
      }
      return TXMSG_OK;
   }

   TLink* linkFound = NULL;
//   if (m_mutexLinkList) {
//
//      DWORD wait = WaitForSingleObject(m_mutexLinkList, TIMEOUT_MUTEX);
//      if (wait == WAIT_OBJECT_0) {

          AcquireSRWLockShared( m_swrLockLinkList );
         try {

            int try_again;

            std::list<TLink*>::iterator i;

            for (i = m_linkList.begin(); i != m_linkList.end(); i++) {
               if ((*i) && (*i)->IsDest(msg->GetDest(dest, addressSize))) {
                  linkFound = (*i);
                  int messagetype = msg->Type();
                  if (messagetype == MSG_REQUEST_TRIGGERS) {
                     // escribir time de envio de mensaje ( s_triggerReques.m_sentTime)
                     __int64 t = LT::Time::GetTime();

                     msg->WriteAtDataPosition((char*)& t, sizeof(t),
                         (char*)&triggerRequest.m_sentTime -
                         (char*)&triggerRequest);
                  }
                  else if (messagetype == MSG_RESPONSE_TRIGGERS) {
                     // escribir tiempo de envio de mensaje
                     __int64 t = LT::Time::GetTime();

                     msg->WriteAtDataPosition((char*)& t, sizeof(t),
                         (char*)&triggerResponseHeader.m_sentServerTime -
                         (char*)&triggerResponseHeader);

                  }
                  if ((*i)->TxMsg(msg)) {
//                     (*i)->UpdateSentTime(); // update last sent time, WE ARE DOING THIS IN TXMsg
                     return TXMSG_OK;
                  }
                  LOG_ERROR1(L"Failed to send the message");
               }
            }
            // LOG_DEBUG1(L"TCommMngr::SendMsg mutex release");
         }
         __finally {
//            if (!ReleaseMutex(m_mutexLinkList)) {
//               LOG_ERROR1(L"Couldnt release mutex in TCommMngr::SendMsg");
//            }
//            else {
//               while (ReleaseMutex(m_mutexLinkList)) {
//                  LOG_INFO1(L"TCommMngr::SendMsg EXTRA_RELEASEMUTEX");
//               }
//            }
             ReleaseSRWLockShared( m_swrLockLinkList );
         }
//      }
//      else {
//         // if (m_serverMode){
//         LOG_ERROR1(L"SendMsg lock mlinklist failed");
//         // }
//
//      }
//   } // if mutex

   if (linkFound && (linkFound->Closed())) {
#ifdef _DEBUG

      FILE *fitx;
      wchar_t origen[50], destino[50];
      msg->GetDest(destino, 50);
      msg->GetOrg(origen, 50);

      // if ( _wfopen_s( &fitx,origen, L"a") == 0 ) {
      // fwprintf(fitx, L"link not found or closed linkFound=%d\n", linkFound);
      // fclose( fitx);
      // }

#endif

      linkFound->OpenClientPart();
      // linkFound->OpenServerPart();
   }

   // Solving subscription problem ....
   e_txMsgStatus ret = TXMSG_FAILED;

   if (m_mutexPendingList) {

      if (ReleaseMutex(m_mutexPendingList)) {
         LOG_WARN1(L"Com SendMsg m_mutexPendingList PRERELEASE sin sentido");
      }
      DWORD dwWaitResult = WaitForSingleObject(m_mutexPendingList,
          Wait_PendingList_TIMEOUT);
      if (dwWaitResult == WAIT_OBJECT_0) {
         // ToDo: check for not psuh twice into this list
         bool found = false;
         bool finish = false;

         try {

#ifdef _DEBUG
            wchar_t num[64];
            num[SIZEOF_CHAR(num)-1] = 0;
            snwprintf(num,SIZEOF_CHAR(num)-1, L"%d", (int)m_pendingList->size());
            // LOG_INFO2(L"SendMsg: size of m_pendingList =", num );
#endif

            std::list<LT::TMessage*>::iterator i;
            for (i = m_pendingList->begin(); i != m_pendingList->end(); i++) {
               if ((*i)->GetMsgNumber() == msg->GetMsgNumber()) {
                  found = true;
                  LogMsg(L"Msg Found in pending list", msg);
               }
               wchar_t destLink[addressSize];
               if (wcscmp(msg->GetDest(dest, sizeof(dest)),
                   (*i)->GetDest(destLink, sizeof(destLink))) == 0) {
                  // We don't want to try opening link more then once
                  finish = true;
               }
            }
            if (found == false) {
#ifdef COM_LOG_MESSAGEFAILS
               LogMsg(L"Message Fail : save in Pending List", msg);
#endif
               m_pendingList->push_back(msg);
               // ReleaseMutex( m_mutexPendingList);      //ya se hace bajo tanto si se encuentra como si no
            }
#ifdef _DEBUG
            else {
               ret = TXMSG_FAILED;
               LogMsg(L"Message already in pending list: ", msg);
            }
#endif
         }
         __finally {
            if (!ReleaseMutex(m_mutexPendingList)) {
               // Deal with error.
               LOG_WARN1(
                   L"TCommMngr::SendMsg couldn't release m_mutexPendingList");

            }
            else {
               while (ReleaseMutex(m_mutexPendingList)) {
                  LOG_INFO1(
                      L"TCommMngr::SendMsg m_mutexPendingList EXTRA_RELEASEMUTEX");
               };
            }
         }
         wchar_t num2[64];
         num2[SIZEOF_CHAR(num2)-1] = 0;
         snwprintf(num2,SIZEOF_CHAR(num2)-1, L"%d", (int)finish);
         // LOG_INFO2(L"Pending list: finish=", num2 );
         if (finish)
            return ret;
      }
      else
         LOG_ERROR1(L"Message Fail : couldn't save in Pending List");
   }
   return ret;
}

// ----------------------------------------------------------------------------

/**
 Crea un link a partir de la estructura especificada   y lo añade a la lista de links
 */
bool TCommMngr::CreateLink(s_remoteLT remoteLT) {


  int timeCreateLink = timeGetTime();

   // LOG_DEBUG1(L"TCommMngr::CreateLink");
   const unsigned int sleeptimeout_ms = 0;
   int max_try_connection = 1;
//   const unsigned int sleeptimeout_ms = 1000;
//   int max_try_connection = 3;
#define bzero(n,m) memset(n,0,m)     /* Unix/BSD 4.3 definition */

   if (ExistLink(remoteLT.remoteName))
      DeleteLink(remoteLT.remoteName);

   std::wstring str;
   str.append(L"TCPSOCKET");
   str.append(L"#");
   str.append(remoteLT.ipAddress);
   str.append(L":");

   wchar_t port[10];
   port[SIZEOF_CHAR(port)-1] = 0;
   snwprintf(port,SIZEOF_CHAR(port)-1, L"%d", remoteLT.portNumber);
   str.append(port);

   // Create Client Link and pass parameters
   TLink *ClientLink = (TLink*)LinkFactory.CreateTLink(remoteLT.remoteName,
       str.c_str());

   if (!ClientLink) {
      LOG_ERROR2(L"Can't create Link to ", str.c_str());
      m_connectedToServer = false;
      return false;
   }

   // Try to conect
   bool connected = false;
//   do {
      if (ClientLink->OpenClientPart()) {
         connected = true;
//         break;
      } else{
         static __int64 timeFail = 0;
         __int64 t = LT::Time::GetTimeMs();
         if ( t - timeFail > LOG_RECONNECTION_TIME ) {
//            LOG_ALERT3(L"Can't connect to ", str.c_str(), L" trying again...");
            LOG_ALERT2(L"Can't connect to ", str.c_str());
            timeFail = t;
         }
      }
//      Sleep(sleeptimeout_ms);
//   }
//   while (--max_try_connection);

   if (!connected) { // this has no sense now
//      LOG_ERROR2(L"Can't Open Client Part: ", str.c_str());       //commented because the we already logged fail on log_alert3
      delete ClientLink;
      m_connectedToServer = false;
      return false;
   }
   DWORD PID = GetCurrentProcessId(); // Get current process ID
   wchar_t dataMsg[64];

   wchar_t myEntryPoint[32];

   LT::TMessage *Mess_request_connect;
   int messageType = MESSAGETYPE_LARGE;

   // Clear all for sure
   bzero(dataMsg, sizeof(dataMsg));
   bzero(myEntryPoint, sizeof(myEntryPoint));
   // If server can't be init break all.
   if (!m_server) {
      LOG_ERROR1(L"M_server not found?");
      delete ClientLink;
      m_connectedToServer = false;
      return false;
   }

   // Get information from server adress ip , port and save in myEntryPoint
   m_server->CreateMyParam(myEntryPoint, sizeof(myEntryPoint));

   // Prepare message for reqauest push all information about server .
   snwprintf(dataMsg,SIZEOF_CHAR(dataMsg)-1, L"%s#0x%08x#%d", myEntryPoint, PID, (int)messageType);

   // Create message
   Mess_request_connect = new LT::TMessage(MSG_COMM_REQUEST_CONNECT,
       m_moduleName, remoteLT.remoteName, PRIOR_NORMAL, (char*) dataMsg,
       wcslen(dataMsg)*sizeof(wchar_t));

   // If can't be init break all process.
   if (!Mess_request_connect) {
      LOG_ERROR1(L"Can't allocate memory");
      delete ClientLink;
      m_connectedToServer = false;
      return false;
   }

   // Increment message Number
   SetMessageNumber(Mess_request_connect);

   // Send Message and check result .
   if (!ClientLink->TxMsg(Mess_request_connect)) {
      LOG_ERROR1(L"Can't send message");
   }
   // once created, we add the link to the list
   m_linkList.push_back(ClientLink);


   timeCreateLink = timeGetTime() - timeCreateLink;

   wchar_t buff[128];
   buff[SIZEOF_CHAR(buff)-1]=0;
   snwprintf(buff,SIZEOF_CHAR(buff)-1,L"CreateLinkMutex took %d ms to be proccessed", timeCreateLink);
   LOG_INFO1(buff);

   m_connectedToServer = true;
   return true;
}

/**
 Crea un link a partir de la estructura especificada
 */
bool TCommMngr::CreateLinkMutex(s_remoteLT remoteLT) {
//   if (m_mutexLinkList) {
//      // LOG_DEBUG1(L"TCommMngr::CreateLinkMutex mutex init");
//      DWORD wait = WaitForSingleObject(m_mutexLinkList, TIMEOUT_MUTEX);
//      if (wait == WAIT_OBJECT_0) {

         AcquireSRWLockExclusive(m_swrLockLinkList);
         bool created = false;
         try {
            created = CreateLink(remoteLT);
            // LOG_DEBUG1(L"TCommMngr::CreateLinkMutex mutex release");
         }
         __finally {
//            if (!ReleaseMutex(m_mutexLinkList)) {
//               LOG_WARN1(L"TCommMngr::CreateLinkMutex couldn't release mutex");
//            }
//            else {
//               while (ReleaseMutex(m_mutexLinkList)) {
//                  LOG_INFO1(L"TCommMngr::CreateLinkMutex EXTRA_RELEASEMUTEX");
//               };
//            }
             ReleaseSRWLockExclusive(m_swrLockLinkList);
         }
         return created;
//      }
//      else {
//         LOG_ERROR1(L"Couldn't lock m_mutexLinkList");
//      }
//   }
//   else {
//      LOG_ERROR1(L"Doesn't exist m_mutexLinkList");
//   }
//   return false;
}

/**
 Process Message

 @param msg: TMessage pointer with the message to be processed
 @return true if at least one TMsgProc has process the message, false otherwise
 */
bool TCommMngr::ProcMsg(LT::TMessage *msg) {
//   LOG_INFO2(L"TCommMngr::ProcMsg type:",type_names[msg->Type()]);
   LOG_DEBUG2(L"TCommMngr::ProcMsg type:", type_names[msg->Type()]);
#ifdef _DEBUG
   FILE *fitx;
   wchar_t origen[50], destino[50];
   msg->GetDest(destino, 50);
   msg->GetOrg(origen, 50);
   // if ( _wfopen_s( &fitx,destino, L"a") == 0 ) {
   //
   // fwprintf(fitx, L"ProcMsg: origen=%s destino=%s tipo=%d, size=%d\n", origen, destino, msg->Type(), msg->DataSize());
   // fclose( fitx);
   // }
#endif

   // Definitions
#define bzero(n,m) memset(n,0,m)     /* Unix/BSD 4.3 definition */
//   int max_try_connection = 5;
//   const unsigned int sleeptimeout_ms = 1000;

   // Check whether message is empty: not valid for some reason
   if (!msg) {
      LOG_ERROR1(L"Null Message");
      delete msg;
      return false;
   }

   e_typeMsg T = msg->Type();

   // Get the source of address.
   wchar_t moduleOriginName[addressSize];
   msg->GetOrg(moduleOriginName, sizeof(moduleOriginName) / sizeof
       (moduleOriginName[0]));

   // status check
   if (!m_serverMode) { // Client protocol special cases
      // ToDo include keep alive acks as allowed?
      // if we are disconected is because we are waiting for a response conection
      if (m_status == e_commMngrStatus::DISCONNECTED && T !=
          MSG_COMM_RESPONSE_CONNECT) {
         LOG_ERROR1(
             L"Not Conected clients only listen to MSG_COMM_RESPONSE_CONNECT messages");
         delete msg;
         return false;
      }
      // if we are waiting for conf is because we are waiting for a configuration message
      if (m_status == e_commMngrStatus::WAITING_CONF && T != MSG_CFG) {
         LOG_ERROR1(
             L"Not Conected clients only listen to MSG_COMM_RESPONSE_CONNECT messages");
         delete msg;
         return false;
      }

   }
   else { // server checks
      if (m_status != e_commMngrStatus::CONNECTED) {
         LOG_ERROR1(L"The Server is not ready, message discarded");
         delete msg;
         return false;
      }
   }

   // update received time to detect not responding connections
   UpdateSenderReceivedTime(moduleOriginName);
   // vic comentado para ver si es esto lo que provoca fallos, habrá que volver a ponerlo

   // if the message is not for this CommMngr ProcMsg, we push it in the inbox
   // so the in thread can call ProcMsg to all TMessageProcessors in its list
   if (!(T == MSG_COMM_ADDRESS_BY_NAME || T == MSG_COMM_GET_ADDRESS_BY_NAME ||
       T == MSG_COMM_KEEP_ALIVE || T == MSG_COMM_KEEP_ALIVE_ACK ||
       T == MSG_COMM_REQUEST_CONNECT || T == MSG_COMM_RESPONSE_CONNECT ||
       T == MSG_REQUEST_TRIGGERS || T == MSG_RESPONSE_TRIGGERS)) {

      if (!PushInMsg(msg)) {
         LOG_WARN2(L"PushInMsg failed in TCommMngr::ProcMsg ",
             type_names[msg->Type()]);
         delete msg;
         return false;
      }
      return true;
   }

   wchar_t p[16];
   p[SIZEOF_CHAR(p)-1] = 0;
   snwprintf(p,SIZEOF_CHAR(p)-1, L"%d", T);

   switch (T) {
   case MSG_REQUEST_TRIGGERS: {
         // write s_triggerRequest.m_receivedTime
         __int64 t = LT::Time::GetTime();

#ifdef _DEBUG
         // wchar_t text[100];
         // swprintf(text,L"TTCommMngr::ProcMsg MSG_REQUEST_TRIGGERS t=%Ld", t);
         // LOG_INFO1 (text);
#endif
         msg->WriteAtDataPosition((char*)& t, sizeof(t),
             (char*)&(triggerRequest.m_receivedTime) -
             (char*)(&triggerRequest));
         if (!PushInMsg(msg)) {
            LOG_WARN1(
                L"PushInMsg failed in TCommMngr::ProcMsg, msg MSG_REQUEST_TRIGGERS");
            delete msg;
            return false;
         }

         return true;
      } break;
   case MSG_RESPONSE_TRIGGERS: {

         // as soon as possible, write _triggerResponseHeader.m_receivedTime
         __int64 t = LT::Time::GetTime();

#ifdef _DEBUG
         // wchar_t text[100];
         // swprintf(text,L"TTCommMngr::ProcMsg MSG_RESPONSE_TRIGGERS t=%Ld", t);
         // LOG_INFO1 (text);
#endif

         msg->WriteAtDataPosition((char*)&t, sizeof(t),
             (char*)&(triggerResponseHeader.m_receivedTime) -
             (char*)(&triggerResponseHeader));
         if (!PushInMsg(msg)) {
            LOG_WARN1(
                L"PushInMsg failed in TCommMngr::ProcMsg, msg MSG_RESPONSE_TRIGGERS");
            delete msg;
            return false;
         }

         return true;
      } break;
      // -----------------------------------------------------------------------
   case MSG_COMM_ADDRESS_BY_NAME: {
         LOG_ERROR1(L"ProcMsg MSG_COMM_ADDRESS_BY_NAME not implemented");
         // // If message is a MSG_ADDRESS_BY_NAME and destination is for "me" do it:
         // if( T == MSG_COMM_ADDRESS_BY_NAME && msg->IsDest( m_moduleName))     {
         // //  Get information about destination module and parameters.
         // wchar_t *buffer_of_data = new wchar_t[msg->DataSize()+1];
         // TLink *ClientLink;
         // if ( !buffer_of_data )  {
         // LOG_ERROR1(L"Can't alocate memory");
         // delete msg;
         // return false;
         // }
         //
         // // Copy name of destination module and param to buffer
         // bzero(buffer_of_data,msg->DataSize()+1);
         // int wrealSize = ( msg->DataSize() > 0) ?  (msg->DataSize() / sizeof(wchar_t)) : 0;
         // if (wrealSize <= 0)                                   {
         // delete msg;
         // return false;
         // }
         // memcpy(buffer_of_data,(wchar_t*)msg->DataAsRaw(), msg->DataSize());
         // buffer_of_data  [wrealSize] =  0;
         //
         // wchar_t* dest_module_name;
         // wchar_t *linkParameter;
         //
         // // Find parameter in string
         // linkParameter = wcschr(buffer_of_data,L'#');
         // if ( !linkParameter )                                             {
         // LOG_ERROR1(L"Bad format of data");
         // delete msg;
         // return false;
         // }
         //
         // // Assign all
         // *linkParameter=0;
         // linkParameter++;
         // dest_module_name = (wchar_t*)buffer_of_data;
         //
         // // Create Client Link and pass parameters
         // ClientLink =(TLink *)LinkFactory.CreateTLink(dest_module_name, linkParameter);
         //
         // if(!ClientLink )                                                  {
         // LOG_ERROR1(L"Can't create Link");
         // delete[] buffer_of_data;
         // delete msg;
         // return false;
         // }
         // //  Try to connect N-times with CONST timeout
         // do                                                                {
         // if (ClientLink->OpenClientPart() )
         // break;
         // LOG_ALERT1(L"Can't connect try again...");
         // Sleep(sleeptimeout_ms);
         // }
         // while (max_try_connection--);
         // // If can't be possible connected close all operation.
         // if(!max_try_connection)                                        {
         // LOG_ERROR1(L"Can't Open Client Part");
         // delete ClientLink;
         // delete msg;
         // delete[] buffer_of_data;
         // return false;
         // }
         // DWORD PID = GetCurrentProcessId();  // Get current process ID
         // wchar_t dataMsg[64];
         // wchar_t myEntryPoint[32];
         // LT::TMessage *Mess_request_connect;
         // int messageType = MESSAGETYPE_LARGE;
         //
         // // Clear all for sure
         // bzero(dataMsg,sizeof(dataMsg));
         // bzero(myEntryPoint,sizeof(myEntryPoint));
         // // If server can't be init break all.
         // if(!m_server)                                                     {
         // LOG_ERROR1( L"M_server not found?");
         // delete ClientLink;
         // delete msg;
         // delete[] buffer_of_data;
         // return false;
         // }
         //
         // // Get information from server adress ip , port and save in myEntryPoint
         // m_server->CreateMyParam(myEntryPoint, sizeof(myEntryPoint));
         //
         // // Prepare message for reqauest push all information about server .
         // swprintf(dataMsg, L"%s#0x%08x#%d", myEntryPoint, PID, (int)messageType);
         //
         // // Create message
         // Mess_request_connect = new LT::TMessage( MSG_COMM_REQUEST_CONNECT, m_moduleName, dest_module_name, PRIOR_NORMAL,(char*) dataMsg, wcslen( dataMsg)*sizeof(wchar_t));
         //
         // // If can't be init break all process.
         // if(!Mess_request_connect)                                         {
         // LOG_ERROR1(L"Can't allocate memory");
         // delete ClientLink;
         // delete[] buffer_of_data;
         // delete msg;
         // return false;
         // }
         //
         // // Increment message Number
         // SetMessageNumber( Mess_request_connect );
         //
         // // Send Message and check result .
         // if(!ClientLink->TxMsg(Mess_request_connect))
         // LOG_ERROR1(L"Can't send message");
         //
         // // Clear all ..
         // delete ClientLink;
         // delete msg;
         // //msg=NULL;
         // delete[] buffer_of_data;
         // return true;
         // }

      } break;

      // ----------------------------------------------------------------------
   case MSG_COMM_GET_ADDRESS_BY_NAME: {
         LOG_ERROR1(L"ProcMsg MSG_COMM_GET_ADDRESS_BY_NAME not implemented");
         // if( T == MSG_COMM_GET_ADDRESS_BY_NAME  && msg->IsDest(m_moduleName)) {
         //
         // typedef list<TLink *>::iterator LinkIT;
         // // Prepare name of module
         // wchar_t *moduleName  = new wchar_t[msg->DataSize()+1];
         // if(!moduleName)                                                   {
         // LOG_ERROR1(L"Can't allocate memory");
         // delete msg;
         // return false;
         // }
         //
         // // Check for sure if message has some data.
         // if(!msg->DataAsRaw() || !msg->DataSize())                         {
         // LOG_ERROR1(L"Miss data ??");
         // delete msg;
         // delete moduleName;
         // return false;
         // }
         //
         // // Get module name and linkparameter from the message
         // _wmemcpy(moduleName, (wchar_t*)msg->DataAsRaw(), msg->DataSize());
         // moduleName[msg->DataSize()] = 0;
         //
         // wchar_t param[64];
         // wchar_t dest[addressSize];
         //
         // // Clear param and dest
         // bzero(param,sizeof(param));
         // bzero(dest,sizeof(dest));
         //
         // if ( m_mutexLinkList)                                             {
         // DWORD wait = WaitForSingleObject( m_mutexLinkList, TIMEOUT_MUTEX);
         //
         // if ( wait == WAIT_OBJECT_0)                                    {
         // // Find the Connection (TLink) , by means of name of module.
         // bool ModuleIsFound = false;  // If we have this connetion set true
         // LinkIT i = m_linkList.begin();
         // for(i ; i != m_linkList.end(); i++)                         {
         // if( !(*i))                                               {
         // LOG_ERROR1(L"Null Pointer in LinkList!");
         // break;
         // }
         // if((*i)->IsDest(moduleName))                                {
         // ModuleIsFound=true;
         // break;
         // }
         // }
         // // Fetch param of this module .
         // if(ModuleIsFound && (*i)->GetDefaultParam(param, sizeof(param))) {
         // // Fetch destination of message.
         // msg->GetOrg(dest,addressSize);
         //
         // // Create message
         // LT::TMessage *reMsg = new LT::TMessage( MSG_COMM_ADDRESS_BY_NAME, m_moduleName, dest, PRIOR_NORMAL, (char *)param, wcslen( param)*sizeof(wchar_t));
         //
         // if(!reMsg)                                                  {
         // LOG_ERROR1(L"Can't allocate memory");
         // delete msg;
         // delete moduleName;
         // if ( m_mutexLinkList )
         // ReleaseMutex(m_mutexLinkList);
         // return false;
         // }
         // // Send this message .
         // if ( !PushOutMsg(reMsg)) {     // send it to myself (the TCOMMInterface)
         // delete reMsg;
         // LOG_WARN1( L"PushOutMsg failed in procmsg, MSG_COMM_ADDRESS_BY_NAME" );
         // }
         // }
         //
         // // If module can't be found , send message "I can't find module"
         // LT::TMessage *MessageAnswerNoModuleFound;
         // bzero(dest,sizeof(dest));
         //
         // if (!ModuleIsFound)                                            {
         // msg->GetOrg(dest, addressSize);
         // MessageAnswerNoModuleFound = new LT::TMessage(   MSG_COMM_NO_ADDRESS_BY_NAME, m_moduleName, dest, PRIOR_HIGH, "", 0 );
         // if( !MessageAnswerNoModuleFound)
         // LOG_ERROR1(L"Can't allocate memory");
         // if ( !PushOutMsg(MessageAnswerNoModuleFound)) {     // send it to myself (the TCOMMInterface)
         // delete MessageAnswerNoModuleFound;
         // LOG_WARN1( L"PushOutMsg failed in procmsg, MSG_COMM_NO_ADDRESS_BY_NAME" );
         // }
         // }
         // ReleaseMutex(m_mutexLinkList);
         // }
         // else //wait != WIAT_OBJECT_0
         // LOG_ERROR1(L"Couldn't lock m_mutexLinkList");
         // }
         //
         // delete msg;
         // delete moduleName;
         // return true;
         // }
      } break;

      // ----------------------------------------------------------------------
   case MSG_COMM_KEEP_ALIVE: {
         // The current design (03/09/2015) should only receiving   MSG_COMM_KEEP_ALIVE from external clients
         if (T == MSG_COMM_KEEP_ALIVE) {
            // LOG_INFO1(L"  TCommMngr::ProcMsg  MSG_COMM_KEEP_ALIVE");
            LT::TMessage * req = new LT::TMessage(MSG_COMM_KEEP_ALIVE_ACK,
                m_moduleName, moduleOriginName, PRIOR_NORMAL, "", 0);
            if (!PushOutMsg(req)) { // send it to myself (the TCOMMInterface)
               delete req;
               LOG_WARN1(
                   L"PushOutMsg failed in procmsg, MSG_COMM_KEEP_ALIVE_ACK");
            }

            delete msg;
            return true;
         }
      } break;
      // ----------------------------------------------------------------------
   case MSG_COMM_KEEP_ALIVE_ACK: {
         // The current design (03/09/2015) should only be receiving  MSG_COMM_KEEP_ALIVE_ACK from server
         if (T == MSG_COMM_KEEP_ALIVE_ACK) {
            // LOG_INFO1(L"  TCommMngr::ProcMsg  MSG_COMM_KEEP_ALIVE_ACK");

            delete msg;
            return true;
         }
      } break;

      // ----------------------------------------------------------------------
   case MSG_COMM_REQUEST_CONNECT: {

         if (T == MSG_COMM_REQUEST_CONNECT) {
            LOG_DEBUG1(L"  TCommMngr::ProcMsg  MSG_COMM_REQUEST_CONNECT");
//            if (m_mutexLinkList) {
//
//               DWORD wait = WaitForSingleObject(m_mutexLinkList, TIMEOUT_MUTEX);
//               if (wait == WAIT_OBJECT_0) {
                   AcquireSRWLockExclusive(m_swrLockLinkList);
                  try {
                     // LOG_DEBUG1(L"TCommMngr::procmsg MSG_COMM_REQUEST_CONNECT mutex init");
                     bool islink = ExistLink(moduleOriginName);
                     TLink *link = NULL;
                     bool createLink = true;
                     if (islink)
                        createLink = DeleteLink(moduleOriginName);
                     if (createLink) {
                        wchar_t * inet =
                            TAux::STR_TO_WSTR(inet_ntoa(msg->GetIPOrigin()));
                        if (inet) {
                           link = LinkFactory.RequestConnect(moduleOriginName,
                               inet, (wchar_t*)msg->DataAsRaw(),
                               msg->DataSize(), m_linkList.size());
                           m_linkList.push_back(link);

                           // if Im the server, we send an inner message to notify that a client connected
                           if (m_serverMode) {
                              // we simulate a receiving MSG_COM_CLIENT_CONNECTED messaje, so our TMessageProccesor will read and act consequently
                              LT::TMessage *reMsg =
                                  new LT::TMessage(MSG_COM_CLIENT_CONNECTED,
                                  m_moduleName, moduleOriginName,
                                  PRIOR_NORMAL, "", 0);

                              if (!PushInMsg(reMsg)) {
                                 LOG_WARN1(
                                     L"PushInMsg failed in TCommMngr::ProcMsg, msg MSG_COM_CLIENT_CONNECTED");
                                 // return false;         //esto es muy peligroso, no se porque estaba así sin finally siquiera y sin liberar mutex
                              }
                           }

                           delete[]inet;
                        }
                     }

                  }
                  __finally {
//                     if (!ReleaseMutex(m_mutexLinkList)) {
//                        LOG_WARN1(
//                            L"TCommMngr::ProcMsg couldn't release mutexlink");
//                     }
//                     else {
//                        while (ReleaseMutex(m_mutexLinkList)) {
//                           LOG_INFO1(
//                               L"TCommMngr::ProcMsg mutexlink  EXTRA_RELEASEMUTEX");
//                        };
//                     }
                      ReleaseSRWLockExclusive(m_swrLockLinkList);
                  }
                  // LOG_DEBUG1(L"TCommMngr::procmsg MSG_COMM_REQUEST_CONNECT mutex release");

//               }
//               else
//                  LOG_ERROR1(L"Couldn't lock m_mutexLinkList");
//            }
//            else
//               LOG_ERROR1(L"Doesn't exist m_mutexLinkList");
            delete msg;
            return true;
         }
      } break;

      // ----------------------------------------------------------------------
   case MSG_COMM_RESPONSE_CONNECT: {

         if (T == MSG_COMM_RESPONSE_CONNECT) {
            LOG_DEBUG1(L"  TCommMngr::ProcMsg  MSG_COMM_RESPONSE_CONNECT");
            TLink *existingTLink = NULL;
//            if (m_mutexLinkList) {
//               //
//               DWORD wait = WaitForSingleObject(m_mutexLinkList, TIMEOUT_MUTEX);
//               if (wait == WAIT_OBJECT_0) {
                   AcquireSRWLockExclusive(m_swrLockLinkList);
                  try {
                     // LOG_DEBUG1(L"TCommMngr::procmsg MSG_COMM_RESPONSE_CONNECT mutex init");
                     // Search on our TLinks list to find (or not) a existing TLink to this origin
                     for (std::list<TLink*>::iterator i = m_linkList.begin();
                     i != m_linkList.end(); i++)
                        if ((*i) && (*i)->IsDest(moduleOriginName)) {
                           existingTLink = (*i);
                           break;
                        }
                     // LOG_DEBUG1(L"TCommMngr::procmsg MSG_COMM_RESPONSE_CONNECT mutex release");
                  }
                  __finally {
//                     if (!ReleaseMutex(m_mutexLinkList)) {
//                        LOG_WARN1(
//                            L"TCommMngr::ProcMsg couldn't release mutexlink");
//                     }
//                     else {
//                        while (ReleaseMutex(m_mutexLinkList)) {
//                           LOG_INFO1(
//                               L"TCommMngr::ProcMsg mutexlink  EXTRA_RELEASEMUTEX");
//                        };
//                     }
                      ReleaseSRWLockExclusive(m_swrLockLinkList);
                  }
//               }
//               else
//                  LOG_ERROR1(L"Couldn't lock m_mutexLinkList");
//            }
//            else
//               LOG_ERROR1(L"Doesn't exist m_mutexLinkList");

            // if I am a client the next step after receiving conection is to request the configuration
            if (!m_serverMode) {
               // we simulate a receiving MSG_CFG_LOADED messaje, so our TMessageProccesor will read and act consequently
               LT::TMessage *reMsg =
                   new LT::TMessage(MSG_CFG_LOADED, m_moduleName, m_moduleName,
                   PRIOR_NORMAL, "", 0);
               if (!PushInMsg(reMsg)) {
                  LOG_WARN1(
                      L"PushInMsg failed in TCommMngr::ProcMsg, msg MSG_COMM_RESPONSE_CONNECT");
                  delete msg;
                  return false;
               }

               m_status = e_commMngrStatus::WAITING_CONF;

            }

            delete msg;
            return true;
         }
      } break;

   default: ;
   } // end switch

   LOG_ALERT1(L"[Message type not supported]!!!!!");
   return false;
#undef bzero
}

// ---------------------------------------------------------------------------
/**
 Get information about the communication

 @return char * with the information about the comm. Manager and each Tlink
 */
void TCommMngr::GetInfoComm(wchar_t *msg, unsigned int size) {

   int usedSize;
   wmemset(msg, 0x00, size);

   // -- TcommMngr info
   snwprintf(msg,size, L"Module: %s\nLinks open: %d\n\nDEFAULT SERVER:\n",
       m_moduleName, m_linkList.size() + 1);

   // -- Default Server Info
  m_server->GetInfoTLink( msg , size);

   // -- Rest of the links info
   // Get Mutex list
//   if (m_mutexLinkList)
//      WaitForSingleObject(m_mutexLinkList, 500);

    AcquireSRWLockShared(m_swrLockLinkList);

   // Ask every link for its state
   int count = 0;
   wchar_t firstLineLink[64];
   firstLineLink[ SIZEOF_CHAR( firstLineLink)-1] = 0;
   std::list<TLink*>::iterator i = m_linkList.begin();
   while (i != m_linkList.end())
      if ((*i)) {
         snwprintf(firstLineLink, SIZEOF_CHAR( firstLineLink)-1, L"\nTLINK Nº %d\n", ++count);
         wcsncat(msg, firstLineLink, size - wcslen( msg));
         (*i++)->GetInfoTLink( msg, size);
      }

    #ifdef _DEBUG
    UpdateStatistics(msg, size);
    #endif

   // Release mutex
//   if (m_mutexLinkList)
//      ReleaseMutex(m_mutexLinkList);
    ReleaseSRWLockShared(m_swrLockLinkList);

}

// ----------------------------------------------------------------------------
/**
 Check if Exist TLink to dest.
 @param dest
 @return true if exist TLint to dest
 */
bool TCommMngr::ExistLink(wchar_t* dest) {

   if (dest == NULL)
      return false;

   std::list<TLink*>::iterator i;
   for (i = m_linkList.begin(); i != m_linkList.end(); i++) {
      if ((*i) && (*i)->IsDest(dest))
         return true;
   }
   return false;
}

// ----------------------------------------------------------------------------
/**
 Check if Exist TLink to dest.
 @param dest
 @return true if exist TLint to dest
 */
bool TCommMngr::DeleteLink(wchar_t* dest) {

   if (dest == NULL)
      return false;

   std::list<TLink*>::iterator i = m_linkList.begin();
   while (i != m_linkList.end()) {
      if ((*i) && (*i)->IsDest(dest)) {
         (*i)->Close();
         TLink *toDelete = *i;
         m_linkList.erase(i);
         delete toDelete;
         return true;
      }
      i++;
   }
   return false;
}

// ----------------------------------------------------------------------------
/**
 CALLBACK for the "Queue Timer"
 */
void CALLBACK TCommMngr::TimerProcCheckExpiredMsg(PVOID param,
    BOOLEAN timerOrWaitFired) {

   TCommMngr *tcommMngr = (TCommMngr*)param;
   if (tcommMngr) {
      tcommMngr->m_pendingListSize = tcommMngr->m_pendingList->size();
         //ya se loguea dentro del TryAgainPendingList
//      if (tcommMngr->m_pendingListSize > tcommMngr->m_pendingSizeDebug) {
//         wchar_t buff[128];
//         snwprintf(buff, 128, L"Pending List size = %d",
//             tcommMngr->m_pendingListSize);
//         LOG_INFO(buff);
//      }

      tcommMngr->TryAgainPendingList();
      tcommMngr->CheckExpiredMsg();
   }
}

// ----------------------------------------------------------------------------
/**
 Check if a message has expired. if true, message is deleted.
 */
void TCommMngr::CheckExpiredMsg() {
   // Synchronize

   if (m_mutexPendingList) {
      DWORD wait = WaitForSingleObject(m_mutexPendingList, 500);
      if (wait == WAIT_OBJECT_0) {
         try {
            std::list<LT::TMessage*>::iterator i = m_pendingList->begin();
            while (i != m_pendingList->end()) {
               if ((*i)->Type() == MSG_PROCESS_DATA) {
                  if ((*i)->Expired(TIMEOUT_PENDING_PROC_MSG)) {
                     LT::TMessage *msg = *i;
                     LogMsg(L" -------------- process data message deleted. Time exceeded."
                         , msg);
                     i = m_pendingList->erase(i);
                     delete msg;
                  }
                  else {
                     // Next message
                     ++i;
                  }
               }
               else {
                  if ((*i)->Expired(TIMEOUT_PENDING_MSG)) {
                     LT::TMessage *msg = *i;
                     LogMsg(L" -------------- Message deleted. Time exceeded.",
                         msg);
                     i = m_pendingList->erase(i);
                     delete msg;
                  }
                  else {
                     // Next message
                     ++i;
                  }
               }

            }
         }
         __finally {
            if (!ReleaseMutex(m_mutexPendingList)) {
               LOG_WARN1(
                   L"TCommMngr::CheckExpiredMsg couldn't release m_mutexPendingList");
            }
            else {
               while (ReleaseMutex(m_mutexPendingList)) {
                  LOG_INFO1(
                      L"TCommMngr::CheckExpiredMsg m_mutexPendingList  EXTRA_RELEASEMUTEX");
               };
            }
         }
      }
   }
}

// ----------------------------------------------------------------------------
/**
 Try to send messages from pending list
 */
void TCommMngr::TryAgainPendingList() {

   // Synchronize
   if (m_mutexPendingList) {
      DWORD wait = WaitForSingleObject(m_mutexPendingList, 500);
      if (wait == WAIT_OBJECT_0) {
         try {
            ++m_pendingLockCount;
            m_pendingListSize = m_pendingList->size();
            if (m_pendingListSize > m_pendingSizeDebug) {
               wchar_t buff[128];
               snwprintf(buff, 128, L"Pending List size = %d",
                   m_pendingListSize);
               LOG_INFO1(buff);
            }
            std::list<LT::TMessage*>::iterator i = m_pendingList->begin();
            while (i != m_pendingList->end()) {

               wchar_t dest[32];

#ifdef COM_LOG_MESSAGEFAILS

#ifdef _DEBUG
               LT::TMessage *msg = *i;
               wchar_t cadena[256] = L"Try again";
               wchar_t org[32];
               (*i)->GetDest(dest, sizeof(dest));
               (*i)->GetOrg(org, sizeof(org));
               swprintf(cadena,
                   L"Try again: tipo = %s, org=%s, dest=%s, m_moduleName =%s",
                   type_names[msg->Type()], org, dest, m_moduleName);
               LOG_INFO1(cadena);
#else
               LOG_INFO1(L"Try again");
#endif

#endif


               LT::TMessage *m = *i;
               i = m_pendingList->erase(i);
               // if (m_serverMode) {
//               if (m_mutexLinkList) {
//
//                  DWORD wait = WaitForSingleObject(m_mutexLinkList,
//                      TIMEOUT_MUTEX);
//                  if (wait == WAIT_OBJECT_0) {
                      AcquireSRWLockShared(m_swrLockLinkList);
                     try {
                        // LOG_DEBUG1(L"TCommMngr::TryAgainPendingList mutex init");
                        if (ExistLink(dest)) {
                           if (!PushOutMsg(m))
                           { // send it to myself (the TCOMMInterface)
                              delete m;
                              LOG_WARN1(
                                  L"PushOutMsg failed in procmsg, Pending message deleted");
                           }
                        }
                        else {
#ifdef COM_LOG_MESSAGEFAILS
                           LOG_WARN1(
                               L"Fail sending pending msg to nonexistent link. Message deleted");
#endif
                           delete m;
                        }
                        // LOG_DEBUG1(L"TCommMngr::TryAgainPendingList mutex rel");
                     }
                     __finally {
//                        if (!ReleaseMutex(m_mutexLinkList)) {
//                           LOG_WARN1(
//                               L"TCommMngr::TryAgainPendingList couldn't release m_mutexLinkList");
//                        }
//                        else {
//                           while (ReleaseMutex(m_mutexLinkList)) {
//                              LOG_INFO1(
//                                  L"TCommMngr::TryAgainPendingList m_mutexLinkList  EXTRA_RELEASEMUTEX");
//                           };
//                        }
                         ReleaseSRWLockShared(m_swrLockLinkList);
                     }
//                  }
//                  else {
//                     LOG_INFO1(
//                         L"Failed to get LinkList in Pending List Check. Message deleted");
//                     delete m;
//                  }
//               }
//            }                  //if not server
//            else
//               PushOutMsg(m);
         }
      }
      __finally {
         ++m_pendingReleaseCount;
         if (!ReleaseMutex(m_mutexPendingList)) {
            LOG_WARN1(
                L"TCommMngr::TryAgainPendingList couldn't release m_mutexPendingList");
         }
         else {
            while (ReleaseMutex(m_mutexPendingList)) {
               LOG_INFO1(
                   L"TCommMngr::TryAgainPendingList m_mutexPendingList  EXTRA_RELEASEMUTEX");
            };
         }
      }
   }
}
}

// ----------------------------------------------------------------------------
/**
 CALLBACK for the "Queue Timer"
 */
void CALLBACK TCommMngr::TimerProcCheckExpiredLinks(PVOID param,
    BOOLEAN timerOrWaitFired) {
#ifndef _DEBUG
TCommMngr *tcommMngr = (TCommMngr*)param;
if (tcommMngr) {
   tcommMngr->CheckExpiredLinks();
}
#endif
}

// ----------------------------------------------------------------------------
/**
 Check if the links have expired. Any expired Link is deleted.
 */
void TCommMngr::CheckExpiredLinks() {

wchar_t dest[addressSize];

wchar_t cadena[100] = L"Try again";

// bool anyLinkToDelete = false;

TLink* linkFound = NULL;
//if (m_mutexLinkList) {
//
//   DWORD wait = WaitForSingleObject(m_mutexLinkList, TIMEOUT_MUTEX);
//   if (wait == WAIT_OBJECT_0) {
       AcquireSRWLockShared(m_swrLockLinkList);
      // LOG_DEBUG1(L"TCommMngr::CheckExpiredLinks mutex init");
      try {
         bool deleted;
         std::list<TLink*>::iterator i = m_linkList.begin();
         while (i != m_linkList.end()) {

            if ((*i)) {
               (*i)->GetDest(dest, addressSize);
               deleted = false;
               if (m_serverMode) { // server part
                  if ((*i)->ExpiredReceivedTime(TIMER_EXPIRED_LINK)) {
                     // anyLinkToDelete = true;
                     i = m_linkList.erase(i);
                     // ToDo: borrar link delete
                     LT::TMessage * req =
                         new LT::TMessage(MSG_COM_CLIENTLINK_NOT_RESPONDING,
                         m_moduleName, dest, PRIOR_NORMAL, "", 0);
                     bool msgcheck = req->Check();
                     if (!PushInMsg(req))
                     { // send it to myself (the TCOMMInterface)
                        delete req;
                        LOG_WARN1(
                            L"PushInMsg failed in CheckExpiredLInks, MSG_COM_CLIENTLINK_NOT_RESPONDING");
                     }
                     snwprintf(cadena,100, L"Link Expired deleting: org=%s, dest=%s",
                         m_moduleName, dest);
                     LOG_INFO1(cadena);
                     deleted = true;
                  }
               }

               else { // clients part
                  if ((*i)->ExpiredSentTime(TIMER_LINKS_KEEP_ALIVE)
                      && m_status == e_commMngrStatus::CONNECTED) {

                     LT::TMessage * req =
                         new LT::TMessage(MSG_COMM_KEEP_ALIVE, m_moduleName,
                         dest, PRIOR_NORMAL, "", 0);
                     // LOG_INFO1(L"Sending MSG_COMM_KEEP_ALIVE message");
                     if (!PushOutMsg(req))
                     { // send it to myself (the TCOMMInterface)
                        delete req;
                        LOG_WARN1(
                            L"PushOutMsg failed in CheckExpiredLInks, MSG_COMM_KEEP_ALIVE");
                     }
                  }

                  if ((*i)->ExpiredReceivedTime(TIMER_LINKS_NOT_RESPONSE)
                      && m_status == e_commMngrStatus::CONNECTED) {

                     // if we dont have response for a while, we send a keep alive
                     LT::TMessage * req =
                         new LT::TMessage(MSG_COMM_KEEP_ALIVE, m_moduleName,
                         dest, PRIOR_NORMAL, "", 0);
                     LOG_INFO1(
                         L"TIMER_LINKS_NOT_RESPONSE Sending MSG_COMM_KEEP_ALIVE message");
                     if (!PushOutMsg(req))
                     { // send it to myself (the TCOMMInterface)
                        delete req;
                        LOG_WARN1(
                            L"PushOutMsg failed in CheckExpiredLInks, MSG_COMM_KEEP_ALIVE");
                     }

                  }

                  if ((*i)->ExpiredReceivedTime(TIMER_EXPIRED_LINK)) {

                     // if we didnt received any message while we are suppossed to be sending keep alive we notify the MyMachine
                     // we dont delete the link, we will try to reconnect and recreate them (which deletes link)
                     LT::TMessage * req =
                         new LT::TMessage(MSG_COMM_SERVER_NOT_RESPONDING,
                         m_moduleName, dest, PRIOR_NORMAL, "", 0);
                     m_status = e_commMngrStatus::DISCONNECTED;
                     LOG_INFO1(
                         L"Sending MSG_COMM_SERVER_NOT_RESPONDING message from expired Link");
                     if (!PushInMsg(req))
                     { // send it to myself (the TOPCUAServer will read this one)
                        delete req;
                        LOG_WARN1(
                            L"PushInMsg failed in CheckExpiredLInks, TIMER_EXPIRED_LINK");
                     }

                  }

               }
               if (!deleted) {
                  i++;
               }
            }
            else {
               i++; // if we have a null pointer in the list
            }

         }
         // LOG_DEBUG1(L"TCommMngr::CheckExpiredLinks mutex rele");
      }
      __finally {
//         if (!ReleaseMutex(m_mutexLinkList)) {
//            LOG_WARN1(
//                L"TCommMngr::CheckExpiredLinks couldn't release m_mutexLinkList");
//         }
//         else {
//            while (ReleaseMutex(m_mutexLinkList)) {
//               LOG_INFO1(
//                   L"TCommMngr::CheckExpiredLinks m_mutexLinkList  EXTRA_RELEASEMUTEX");
//            };
//         }
          ReleaseSRWLockShared(m_swrLockLinkList);
      }
//   } // if open
//} // if mutex

// if (anyLinkToDelete) {
// DeleteExpiredLinks();
// }

}

// ----------------------------------------------------------------------------
/**
 Check if the links have expired. Any expired Link is deleted.
 */
void TCommMngr::DeleteExpiredLinks() {

wchar_t dest[addressSize];

wchar_t cadena[100] = L"Try again";

TLink* linkFound = NULL;
// if( m_mutexLinkList)                                                       {
//
// DWORD wait = WaitForSingleObject( m_mutexLinkList, TIMEOUT_MUTEX);
// if ( wait == WAIT_OBJECT_0 )                                            {
 AcquireSRWLockExclusive(m_swrLockLinkList);
// LOG_DEBUG1(L"TCommMngr::CheckExpiredLinks mutex init");
try {
   bool deleted;
   std::list<TLink*>::iterator i = m_linkList.begin();
   while (i != m_linkList.end()) {

      if ((*i)) {
         (*i)->GetDest(dest, addressSize);
         deleted = false;
         if (m_serverMode) { // server part
            if ((*i)->IsClosed()) {
               LOG_INFO1(L"Server Link is closed");
            }
            if ((*i)->ExpiredReceivedTime(TIMER_EXPIRED_LINK)) {
               i = m_linkList.erase(i);
               // ToDo: borrar link delete
               LT::TMessage * req =
                   new LT::TMessage(MSG_COM_CLIENTLINK_NOT_RESPONDING,
                   m_moduleName, dest, PRIOR_NORMAL, "", 0);
               bool msgcheck = req->Check();
               if (!PushInMsg(req)) { // send it to myself (the TCOMMInterface)
                  delete req;
                  LOG_WARN1(
                      L"PushInMsg failed in CheckExpiredLInks, MSG_COM_CLIENTLINK_NOT_RESPONDING");
               }
               snwprintf(cadena,100, L"Link Expired deleting: org=%s, dest=%s",
                   m_moduleName, dest);
               LOG_INFO1(cadena);
               deleted = true;
            }
         }

         else { // clients part
            if ((*i)->IsClosed()) {
               LOG_INFO1(L"Client Link is closed");
            }
            if ((*i)->ExpiredSentTime(TIMER_LINKS_KEEP_ALIVE)
                && m_status == e_commMngrStatus::CONNECTED) {

               LT::TMessage * req = new LT::TMessage(MSG_COMM_KEEP_ALIVE,
                   m_moduleName, dest, PRIOR_NORMAL, "", 0);
               // LOG_INFO1(L"Sending MSG_COMM_KEEP_ALIVE message");
               if (!PushOutMsg(req)) { // send it to myself (the TCOMMInterface)
                  delete req;
                  LOG_WARN1(
                      L"PushOutMsg failed in CheckExpiredLInks, MSG_COMM_KEEP_ALIVE");
               }
            }

            if ((*i)->ExpiredReceivedTime(TIMER_LINKS_NOT_RESPONSE)
                && m_status == e_commMngrStatus::CONNECTED) {

               // if we dont have response for a while, we send a keep alive
               LT::TMessage * req = new LT::TMessage(MSG_COMM_KEEP_ALIVE,
                   m_moduleName, dest, PRIOR_NORMAL, "", 0);
               LOG_INFO1(
                   L"TIMER_LINKS_NOT_RESPONSE Sending MSG_COMM_KEEP_ALIVE message");
               if (!PushOutMsg(req)) { // send it to myself (the TCOMMInterface)
                  delete req;
                  LOG_WARN1(
                      L"PushOutMsg failed in CheckExpiredLInks, MSG_COMM_KEEP_ALIVE");
               }

            }

            if ((*i)->ExpiredReceivedTime(TIMER_EXPIRED_LINK)) {

               // if we didnt received any message while we are suppossed to be sending keep alive we notify the MyMachine
               // we dont delete the link, we will try to reconnect and recreate them (which deletes link)
               LT::TMessage * req =
                   new LT::TMessage(MSG_COMM_SERVER_NOT_RESPONDING,
                   m_moduleName, dest, PRIOR_NORMAL, "", 0);
               m_status = e_commMngrStatus::DISCONNECTED;
               LOG_INFO1(
                   L"Sending MSG_COMM_SERVER_NOT_RESPONDING message from expired Link");
               if (!PushInMsg(req))
               { // send it to myself (the TOPCUAServer will read this one)
                  delete req;
                  LOG_WARN1(
                      L"PushInMsg failed in CheckExpiredLInks, TIMER_EXPIRED_LINK");
               }

            }

         }
         if (!deleted) {
            i++;
         }
      }
      else {
         i++; // if we have a null pointer in the list
      }

   }
   // LOG_DEBUG1(L"TCommMngr::CheckExpiredLinks mutex rele");
}
__finally {
   // if (! ReleaseMutex(m_mutexLinkList))                                     {
   // LOG_WARN1( L"TCommMngr::CheckExpiredLinks couldn't release m_mutexLinkList");
   // }
   // else                                                              {
   // while ( ReleaseMutex( m_mutexLinkList))    {
   // LOG_INFO1( L"TCommMngr::CheckExpiredLinks m_mutexLinkList  EXTRA_RELEASEMUTEX");
   // };
   // }
    ReleaseSRWLockExclusive(m_swrLockLinkList);
}
// }
// }

}

// ----------------------------------------------------------------------------
/**
 Update sender received time in order to check the expired ones
 */
void TCommMngr::UpdateSenderReceivedTime(wchar_t* sender) {
   if (sender == NULL) {
      return;
   }
//if (m_mutexLinkList) {
//
//   DWORD wait = WaitForSingleObject(m_mutexLinkList, TIMEOUT_MUTEX);
//   if (wait == WAIT_OBJECT_0) {
       AcquireSRWLockShared(m_swrLockLinkList);
      try {
         // LOG_DEBUG1(L"TCommMngr::UpdateSenderReceivedTime mutex init");
         std::list<TLink*>::iterator i;
         for (i = m_linkList.begin(); i != m_linkList.end(); i++) {
            if ((*i) && (*i)->IsDest(sender)) {
               (*i)->UpdateReceivedTime();
               // LOG_DEBUG1(L"TCommMngr::UpdateSenderReceivedTime mutex release");
               // ReleaseMutex(m_mutexLinkList);
               ReleaseSRWLockShared(m_swrLockLinkList);
               return; // It should only be one link with the sender destination
            }

         }
         LOG_WARN2(L" UpdateSenderReceivedTime Sender does not exist, sender: ",
             sender);
         // LOG_DEBUG1(L"TCommMngr::UpdateSenderReceivedTime mutex release");
//      }
//      __finally {
////         if (!ReleaseMutex(m_mutexLinkList)) {
////            LOG_WARN1(
////                L"TCommMngr::UpdateSenderReceivedTime couldn't release m_mutexLinkList");
////         }
////         else {
////            while (ReleaseMutex(m_mutexLinkList)) {
////               LOG_INFO1(
////                   L"TCommMngr::UpdateSenderReceivedTime m_mutexLinkList  EXTRA_RELEASEMUTEX");
////            };
////         }
//          ReleaseSRWLockShared(m_swrLockLinkList);
//      }

   }
   __except( EXCEPTION_EXECUTE_HANDLER)                                       {
      unsigned long code = GetExceptionCode();
      wchar_t slastCode[128];
      snwprintf( slastCode,128, L"Comm::UpdateSenderReceivedTime exception with code: %d", code);
      LOG_ERROR1(slastCode);
//      DISPATCH_EVENT(ERROR_CODE, L"EV_EXCEPTION_CAPTURED:", slastCode, m_module->GetMngr());
   }
//   }
//   else {
//      LOG_ERROR1(L" UpdateSenderReceivedTime can't lock m_mutexLinkList");
//   }
//}   ReleaseSRWLockShared(m_swrLockLinkList);
}

// ----------------------------------------------------------------------------
/**
 Method for logger purporses when a message is sent.

 @param info Information of text type to be added to msg.
 @param msg message from which source, destination & type of the message will be logged.
 */
void TCommMngr::LogMsg(wchar_t * info, LT::TMessage *msg) {
#ifdef _DEBUG
   wchar_t cadena[150];
   wchar_t org[32], dest[32];
   msg->GetDest(dest, sizeof(dest));
   msg->GetOrg(org, sizeof(org));
   int type = msg->Type();
   if (type >= 0 && type < sizeof(type_names) / sizeof(type_names[0])) {
      snwprintf(cadena,150, L" %s: tipo=%s, org=%s, dest=%s ", info,
          type_names[msg->Type()], org, dest);
      // swprintf(cadena, L" %s: tipo=%d, org=%s, dest=%s ",info, msg->Type(), org,dest);
      LOG_WARN1(cadena);
   }
   else {
      snwprintf(cadena,150, L" %s: tipo=UNKNOWN, org=%s, dest=%s ", info, org, dest);
      LOG_ERROR1(cadena);
   }

#endif
}

// ----------------------------------------------------------------------------
#ifdef _DEBUG

/**
 Update statistics in GetCommInfo
 */
void TCommMngr::UpdateStatistics(wchar_t* msg, int size) {

   // return; //todelete
   wchar_t statistics[1024];
   statistics[SIZEOF_CHAR(statistics)-1] = 0;

   snwprintf( statistics,SIZEOF_CHAR(statistics)-1,
       L"\n---Message Time Statistics---- \n TMin=[%f] ms\n TMax=[%f] ms\n TMean=[%f] ms\n TMedian=[%f] ms\n Total Number of Message= [%lu] \n ",
       m_tstats.tMin, m_tstats.tMax, m_tstats.tmean, m_tstats.tmedian,
       m_tstats.messageCount);

   wchar_t str2[128];
   str2[SIZEOF_CHAR(str2)-1] = 0;

   snwprintf(str2,SIZEOF_CHAR(str2)-1, L"\n %s \n", m_tstats.infoMsg);
   wcsncat(statistics, str2, SIZEOF_CHAR(statistics)-1 - wcslen(statistics) );

   snwprintf(str2, SIZEOF_CHAR(str2)-1, L"TCreatedAndSendMin=[%f] ms\n TCreatedAndSendMax=[%f] ms\n",
       m_tstats.tCreateSendMin, m_tstats.tCreateSendMax);
   wcsncat(statistics, str2,SIZEOF_CHAR(statistics)-1 - wcslen(statistics));

   snwprintf(str2, SIZEOF_CHAR(str2)-1, L"\n %s", m_tstats.infoMsgCreateSend);
   wcsncat(statistics, str2,SIZEOF_CHAR(statistics)-1 - wcslen(statistics));

   wcsncat(msg, statistics, size - wcslen( msg));
}

// ----------------------------------------------------------------------------
/**
 Compute statistics every time PopInMsg is executed.
 */
void TCommMngr::CalculateStatistics(LT::TMessage* msg) {

if (m_tstats.messageCount >= MAX_MSG)
   ClearTimeStat();

wchar_t org[addressSize], dest[addressSize];

msg->GetOrg(org, sizeof(org));
msg->GetDest(dest, sizeof(dest));

double sent_t = msg->getTimeofSending();
double created_t = msg->getTimeofCreation();
double received_t = msg->getTimeofDelivery();
double ttime = ((created_t > 0) && (received_t > 0)) ?
    (received_t - created_t) : 0;

if (ttime > 0) {

   // Mean
   m_tstats.messageCount++;
   m_tstats.accumulationTime += ttime;
   m_tstats.tmean = m_tstats.accumulationTime / (double) m_tstats.messageCount;

   // Max & Min values
   m_tstats.tMin = min(ttime, m_tstats.tMin);

   if (ttime > m_tstats.tMax) {
      m_tstats.tMax = ttime;
      swprintf(m_tstats.infoMsg,
          L"Info Message TMax  \n ------------ \n org= %s, dest= %s TYPE= %d \n ------------ \n",
          org, dest, msg->Type());
   }

   // Median
   m_tstats.time_t[m_tstats.messageCount - 1] = ttime;
   m_tstats.tmedian = 0;
   // TAux::Median(m_tstats.time_t,m_tstats.messageCount );

   // Time from a Message is created and instant of sending

   double created_t = msg->getTimeofCreation();
   if (sent_t > 0) {
      double tmp = sent_t - created_t;
      // Max & Min values
      m_tstats.tCreateSendMin = min(tmp, m_tstats.tCreateSendMin);
      if (tmp > m_tstats.tCreateSendMax) {
         m_tstats.tCreateSendMax = tmp;
         m_tstats.infoMsgCreateSend[ SIZEOF_CHAR(m_tstats.infoMsgCreateSend)-1] = 0;
         snwprintf(m_tstats.infoMsgCreateSend, SIZEOF_CHAR(m_tstats.infoMsgCreateSend)-1,
             L"Info Message TMax CreatedSend  \n ------------ \n org= %s, dest= %s TYPE= %d \n ------------ \n",
             org, dest, msg->Type());
      }
   }
}
}
#endif
// ------------------------------------------------------------------------------

/**
 GetInboxMsg
 Extracts one Message from the inbox

 */
LT::TMessage *TCommMngr::GetInboxMsg() {
m_inBoxSize = m_inBox->GetSize();
if (m_inBox)
   return m_inBox->Pop();
return NULL;
}

// ----------------------------------------------------------------------------
/**
 Sends a configuration changed message to all links
 */
void TCommMngr::SendCFGLoadedToAllLinks() {
wchar_t destLink[addressSize];

//if (m_mutexLinkList) {
//   DWORD wait = WaitForSingleObject(m_mutexLinkList, TIMEOUT_MUTEX);
//   if (wait == WAIT_OBJECT_0) {
       AcquireSRWLockShared(m_swrLockLinkList);
      LOG_DEBUG1(L"TCommMngr::SendCFGLoadedToAllLinks mutex init");
      try {
         int try_again;

         std::list<TLink*>::iterator i;

         for (i = m_linkList.begin(); i != m_linkList.end(); i++) {
            if ((*i) && (*i)->GetDest(destLink, sizeof(destLink))) {
               LT::TMessage *msg =
                   new LT::TMessage(MSG_CFG_LOADED, m_moduleName, destLink,
                   PRIOR_NORMAL, "", 0);
               if ((*i)->TxMsg(msg) == false)
                  LogMsg(L"Can't send MSG_CFG_LOADED message", msg);
            }
         }
         // LOG_DEBUG1(L"TCommMngr::SendCFGLoadedToAllLinks mutex rele");
      }
      __finally {
//         if (!ReleaseMutex(m_mutexLinkList)) {
//            LOG_WARN1(
//                L"TCommMngr::SendCFGLoadedToAllLinks couldn't release m_mutexLinkList");
//         }
//         else {
//            while (ReleaseMutex(m_mutexLinkList)) {
//               LOG_INFO1(
//                   L"TCommMngr::SendCFGLoadedToAllLinks m_mutexLinkList  EXTRA_RELEASEMUTEX");
//            };
//         }
          ReleaseSRWLockShared(m_swrLockLinkList);
      }
//   }
//   else {
//      if (m_serverMode) {
//         LOG_ERROR1(L"SendMsg lock mlinklist failed");
//      }
//
//   }
//}
}

// ----------------------------------------------------------------------------
/**
 Delete all links without sending any notification
 */
void TCommMngr::DeleteAllLinks() {
//if (m_mutexLinkList) {
//   DWORD wait = WaitForSingleObject(m_mutexLinkList, TIMEOUT_MUTEX);
//   if (wait == WAIT_OBJECT_0) {
       AcquireSRWLockExclusive(m_swrLockLinkList);
      // LOG_DEBUG1(L"TCommMngr::DeleteAllLinks mutex init");
      try {
         // delete all TLinks
         std::list<TLink*>::iterator i = m_linkList.begin();
         while (i != m_linkList.end()) {
            if ((*i)) {
               (*i)->Close();
               delete *i;
            }
            i = m_linkList.erase(i);
         }
         // LOG_DEBUG1(L"TCommMngr::DeleteAllLinks mutex release");
      }
      __finally {
//         if (!ReleaseMutex(m_mutexLinkList)) {
//            LOG_WARN1(
//                L"TCommMngr::DeleteAllLinks couldn't release m_mutexLinkList");
//         }
//         else {
//            while (ReleaseMutex(m_mutexLinkList)) {
//               LOG_INFO1(
//                   L"TCommMngr::DeleteAllLinks m_mutexLinkList  EXTRA_RELEASEMUTEX");
//            };
//         }
          ReleaseSRWLockExclusive(m_swrLockLinkList);
      }
//   }
//   else {
//      if (m_serverMode) {
//         LOG_ERROR1(L"SendMsg lock mlinklist failed");
//      }
//
//   }
//}

}

/**
 the server detected a disconnection and notifies the commMngr diconnected peer IP and port
 */
void TCommMngr::DisconnectionFromIPandPort(std::wstring ip, int port) {

//if (m_mutexLinkList) {
//   DWORD wait = WaitForSingleObject(m_mutexLinkList, TIMEOUT_MUTEX);
//   if (wait == WAIT_OBJECT_0) {
       AcquireSRWLockExclusive(m_swrLockLinkList);
      try {
         std::list<TLink*>::iterator i = m_linkList.begin();
         while (i != m_linkList.end()) {
            TTCPSocket * ss = dynamic_cast<TTCPSocket*>(*i);
            if (ss && ss->GetIP() == ip)
            { // ToDo : only filtering by IP, but we should detect its name from a map of links ip+port -> dest

               wchar_t dest[addressSize];
               ss->GetDest(dest, sizeof(dest));

               ss->Close();
               delete ss;
               i = m_linkList.erase(i);

               if (m_serverMode) { // server part
                  LT::TMessage * req =
                      new LT::TMessage(MSG_COM_CLIENTLINK_NOT_RESPONDING,
                      m_moduleName, dest, PRIOR_NORMAL, "", 0);
                  if (!PushInMsg(req))
                  { // send it to myself (the TOPCUAServer will read this one)
                     delete req;
                     LOG_WARN1(
                         L"PushInMsg failed in CheckExpiredLInks, MSG_COM_CLIENTLINK_NOT_RESPONDING");
                  }
                  // i++;                        // vic, borramos también el link (aunque no se muy bien porque no lo haciamos)
                  // i = m_linkList.erase( i);
               }
               else {
                  // vic 22/08/16 comento el retry porque no creo que tenga sentido que mantengamos el link habiendo borrado el socket
                  int current = timeGetTime();
                  // if(   (current - m_lastServerNotRespondingTime)/1000 > TIMER_EXPIRED_LINK)  {   //usamos este threshold pero podría ser otro

                  wchar_t buff[512];
                  buff[SIZEOF_CHAR(buff)-1] = 0;
                  snwprintf(buff, SIZEOF_CHAR(buff), L"Last reconnection retry:%Ld",
                      (current - m_lastServerNotRespondingTime) / 1000);
                  LOG_INFO1(buff);

                  LT::TMessage * req =
                      new LT::TMessage(MSG_COMM_SERVER_NOT_RESPONDING,
                      m_moduleName, dest, PRIOR_NORMAL, "", 0);
                  m_status = e_commMngrStatus::DISCONNECTED;
                  m_lastServerNotRespondingTime = current;
                  LOG_INFO1(L"Sending MSG_COMM_SERVER_NOT_RESPONDING message");
                  if (!PushInMsg(req))
                  { // send it to myself (the TOPCUAServer will read this one)
                     delete req;
                     LOG_WARN1(
                         L"PushInMsg failed in CheckExpiredLInks, MSG_COMM_SERVER_NOT_RESPONDING");
                  }

                  LOG_INFO2(L"Deleted link with ip ", ip.c_str());
                  // i = m_linkList.erase( i);
                  // }
                  // else  {
                  // wchar_t buff[512];
                  // snwprintf(buff, 512, L"NOT SENDING SERVER NOT RESPONDING because diff:%Ld and diff allowed is %d",(current - m_lastServerNotRespondingTime)/1000, TIMER_EXPIRED_LINK);
                  // LOG_INFO1(buff );
                  // i++;
                  // }
               }

            }
            else {
               i++;
            }
         }
      }
      __finally {
//         if (!ReleaseMutex(m_mutexLinkList)) {
//            LOG_WARN1(
//                L"TCommMngr::DisconnectionFromIPandPort couldn't release m_mutexLinkList");
//         }
//         else {
//            while (ReleaseMutex(m_mutexLinkList)) {
//               LOG_INFO1(
//                   L"TCommMngr::DisconnectionFromIPandPort m_mutexLinkList  EXTRA_RELEASEMUTEX");
//            };
//         }
          ReleaseSRWLockExclusive(m_swrLockLinkList);
      }

#ifdef LOG_RECONNECTIONS

      wchar_t buff[512];
      buff[SIZEOF_CHAR(buff)-1] = 0;
      snwprintf(buff, SIZEOF_CHAR(buff)-1, L"DisconnectionFromIPandPort %s:%d",
          ip.c_str(), port);
      LOG_INFO1(buff);
#endif

//   }
//}
}






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
