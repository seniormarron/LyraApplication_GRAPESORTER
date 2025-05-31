//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TComm.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#ifndef tcommH
#define tcommH

#ifndef _MSC_VER
#include <list.h>
#endif

#include <exception>

#include "TLinkFactory.h"
#include "TMessageBox.h"
#include "TMessageBoxSimple.h"
#include "TMessageProcessor.h"

class TCommInterface;
//class TBaseModule;


#define COM_LOG_RECONNECTIONS
//#define COM_LOG_SUBSCRIPTIONS
//#define COM_LOG_MESSAGEFAILS

//5 segs
#define LOG_RECONNECTION_TIME 5000000

//----------------------------------------------------------------------------

#define  tagGM  L"GM"                         ///< TAG for Global Manager
//#define  TIMEOUT_MUTEX        400            ///< Max delay per message.
//#define  TIMEOUT_PENDING_MSG  30000          ///< Max delay per pending messages. Already set to 40 seconds. TODO: too much time to wait for?
//#define  TIMEOUT_PENDING_PROC_MSG  12000      ///< Max delay per pending proccess data messages.

#define  TIMEOUT_MUTEX        400            ///< Max delay per message.
#define  TIMEOUT_PENDING_MSG  10000          ///< Max delay per pending messages. Already set to 40 seconds. TODO: too much time to wait for?
#define  TIMEOUT_PENDING_PROC_MSG  7000      ///< Max delay per pending proccess data messages.

#define  TIMER_PENDING_MSG    1000           ///< Timer for FuncProcExpiredMsg
#define  TIMER_LINKS_CHECK    2000           ///< Timer for FuncProcExpiredLinks checks
//#define  TIMER_LINKS_MSG    5000           ///< Timer for FuncProcExpiredLinks (client check)

#define  TIMER_LINKS_KEEP_ALIVE     1000           ///< Timer to resend the keep_alive messages (checks sent timer)
#define  TIMER_LINKS_NOT_RESPONSE   3000           ///< Timer to resend a keep_alive message if the server didnt sent any response
#define  TIMER_EXPIRED_LINK         8000           ///< time to be considered as expired

//#define  TIMER_LINKS_KEEP_ALIVE     20000           ///< Timer to resend the keep_alive messages (checks sent timer)
//#define  TIMER_LINKS_NOT_RESPONSE   30000           ///< Timer to resend a keep_alive message if the server didnt sent any response
//#define  TIMER_EXPIRED_LINK         80000           ///< time to be considered as expired


//----------------------------------------------------------------------------
/// Possible status of a TMessage
typedef enum                                                                  {

   TXMSG_FAILED   = 0,                       ///< Message failed to send.
   TXMSG_OK,                                 ///< Message corretly sent.
   TXMSG_PENDING,                            ///< Message pending to be sent (waiting for the TLink to be created).
} e_txMsgStatus;

//----------------------------------------------------------------------------
/// Possible status of the CommMngr
enum class e_commMngrStatus : int                                                              {
   DISCONNECTED       = 0x0001,
   WAITING_CONF      = 0x0002,
   CONNECTED          = 0x0003,
   INVALID           = 0x0004

} ;                    ///< TLink Status


//----------------------------------------------------------------------------
///Structure containing necessary data to deal with the thread. See TCommMngr for more details.
struct s_commThread                                                           {

   HANDLE          handleThread;                                           ///< Handle to manage the thread
   TCommMngr      *commMngr;                                               ///< Pointer to the Communications Manager
//   e_txMsgStatus (__closure *sendFunc)( LT::TMessage *msg, bool askGM);   ///< Function Pointer to Send Function
};

//----------------------------------------------------------------------------
///Structure containing necessary data to deal with the thread. See TCommMngr for more details. NO LONGER NEEDED
//struct s_commRecThread                                                        {
//
//   HANDLE          handleThread;             ///< Handle to manage the thread
//   TCommMngr      *commMngr;                 ///< Pointer to the Communications Manager
//};

//----------------------------------------------------------------------------
//TOdo: delete sino se utiliza
#ifdef _DEBUG
#define MAX_MSG 5000
struct s_time_stat {

   unsigned long   messageCount;             ///< Number of messages to be include for calculate the mean and median
   double   accumulationTime;                ///< Total amount of time
   double   time_t[MAX_MSG];                 ///< Partial values to calculate median.
   double   tMin;                            ///<
   double   tMax;                            ///<
   double   tmean;                           ///<
   double   tmedian;                         ///<
   wchar_t     infoMsg[512];                    ///<
   double   tCreateSendMin;                  ///<
   double   tCreateSendMax;                  ///<
   wchar_t     infoMsgCreateSend[512];          ///<

};                                           ///< Structure for retrieving statiscal data, only DEBUG mode
#endif


struct s_remoteLT                                                            {

   wchar_t  remoteName[addressSize];
//   int      TCP;  ConUDP no funciona, asi que dejo solamente TCP
   wchar_t  ipAddress[20];
   int      portNumber;
};

// function to get a remoteLT structure from a wchar_t
s_remoteLT getRemoteLTfromWchar(wchar_t * remoteName, wchar_t * remoteEntryPoint);


//----------------------------------------------------------------------------
//********************************  TCommMngr Class  *************************
//----------------------------------------------------------------------------
/// Class to provide operations for communication purposes between components.
/**
*  Tasks:
*  - Manages a list of TLinks, and routes the messages to the correct module.
*  - Creates needed Tlinks when necessary, and destroys them at the end.
*  - Manages a TLinkServer, where by default arrives messages from other modules.
*      TCommMngr only process Tlink Creation Messages. For the other messages,
*      just call the registered processors
*  - Manages a list of message processors. Every object in a module, that want to
*    receive messages, has to register on the TCommMngr.
*/
class TCommMngr                                                               {

  protected:
//   int                        m_outBoxPop;
   wchar_t                    m_moduleName[ addressSize];   ///< Module/Component name which CommMngr belongs to.
//   wchar_t*                   m_paramGM;                    ///< Parameters for connection to GLobal Manager

   std::list<TLink *>         m_linkList;                   ///< Tlink's list, list of links connected with this CommMngr.
   TLinkServer*               m_server;                     ///< TLinkServer, default server
   TLinkFactory               LinkFactory;                  ///< Interface used to create TLinks
   HWND                       m_hWnd;

   //Thread manage
//   s_commThread               m_commRecTh;                  ///< Thread for receiving messages from shared memory (serverlink)
   HANDLE*                    m_handleRecBox;               ///<
   s_commThread               m_commTh;                     ///< Structure with the data for the communication thread (send)
   s_commThread               m_commRecInBoxTh;             ///< Structure with the data for the communication thread  (receive)

   HANDLE                     m_eventSendEnd;
   HANDLE                     m_eventRecvEnd;
   HANDLE                     m_eventRecvInBoxEnd;

   // Messages box
//   TMessageBox               *m_inBox;                       ///< Message box for the input messages
//   TMessageBox               *m_outBox;                     ///< Message box for the output messages
   TMessageBoxSimple         *m_inBox;                       ///< Message box for the input messages
   TMessageBoxSimple         *m_outBox;                     ///< Message box for the output messages


   std::list<LT::TMessage*> *m_pendingList;                ///< Message list for the pending messages (waiting for the TLink)
   int                        m_msgNumber;                  ///< Auto increasing number of message number sended by the TCommMngr/module

   // Misc
   bool                       m_reEnter,                    ///< Used to avoid reentry on TxMsg
                              m_started;                    ///< Indicates if the commMngr has been started otr not
   HANDLE                     m_mutexPendingList;           ///< Mutex to sinchronyze the access to the pending message's list
//   HANDLE                     m_mutexLinkList;              ///< Mutex to Sinchronyze the access to the link's list
   HANDLE                     m_timerHandle;                ///< Queue Timer Handle for the Callback Function
      HANDLE                  m_timerLinksHandle;                ///< Queue Timer Handle for the Callback Function


   PSRWLOCK                   m_swrLockLinkList;


   bool                       m_usingModule;                ///<
   TCommInterface            *m_commIfc;                    ///< Tcomminterface to process received messages

   bool                       m_serverMode;                  ///< Used to change behaviour when acting as server (main LT) or as external client (e.g. OPC Server)
   std::list<TMessageProcessor*> *m_messageProcessorsList;   ///< Message processors list which will be called when receiving a message.
   e_commMngrStatus           m_status;                      ///< Status of the commMngr

   int                        m_lastServerNotRespondingTime; ///< time to check last disconnection and avoid connection/disconnection loop

   unsigned long int          m_pendingLockCount,
                              m_pendingReleaseCount;

   bool                       m_connectedToServer;           ///< Know if i am connected to the server

public:
   TSafeList<HANDLE>         *m_linkLocalHandleList;

   int                        m_pendingListSize,
                              m_pendingSizeDebug;

   int                        m_inBoxSize,
                              m_outBoxSize,
                              m_BoxSizeDebug;

   int                        m_logProcInputTimesOlderThanMs;




  public:
//   TCommMngr( wchar_t *moduleName, wchar_t *paramGM, wchar_t *entryPoint);   ///< Constructor
   TCommMngr( wchar_t *moduleName, wchar_t *entryPoint);   ///< Constructor
   ~TCommMngr();                                                             ///< Destructor

   void SetCommInterface(TCommInterface * commIFC);                               ///< Sets the commInterface to call it when not using module

  protected:

   bool            PushInMsg  ( LT::TMessage *msg);                          ///< Push a message to the module inbox list
   LT::TMessage  *PopOutMsg  ( int type = 0,         int priority = 0);      ///< Pop a message from the outbox list
   e_txMsgStatus   SendMsg    ( LT::TMessage *msg);                          ///< Routes a message by the correct TLink
   void            LogMsg     (wchar_t * info, LT::TMessage *msg ) ;            ///< Method for help support log messages, only commMngr class.

   bool  ExistLink(wchar_t* dest);                                               ///< true if an existing link with dest is already in m_linklist, false otherwise
   bool  DeleteLink(wchar_t* dest);
   void  TryAgainPendingList();
   void  CheckExpiredMsg();
   static void CALLBACK TimerProcCheckExpiredMsg(PVOID param, BOOLEAN timerOrWaitFired);   ///<Callback Function used by the "Queue Timer" for checking expired messages
   static unsigned WINAPI CommThread(LPVOID owner);                                        ///< Communicaction thread function   (send)
   static unsigned WINAPI CommRecThread(LPVOID owner);                                     ///< Communicaction thread function   (receive from Shared Memory, for Multimodule implementations)
   static unsigned WINAPI CommRecInBoxThread(LPVOID owner);                                ///< Communicaction thread function   (receive)


   static void CALLBACK TimerProcCheckExpiredLinks(PVOID param, BOOLEAN timerOrWaitFired);   ///<Callback Function used by the "Queue Timer" for checking expired messages
   void  CheckExpiredLinks();
   void  DeleteExpiredLinks();
   void UpdateSenderReceivedTime(wchar_t* sender);
   bool CreateLink(s_remoteLT remoteLT);                             ///< create a link for connecting to remote module information

public:



   bool Start();                                                              ///< Starts the communication Manager

   bool           PushOutMsg  ( LT::TMessage *msg);                          ///< Push a message to the outbox list
   bool           ReleaseOutBoxMutex  ( );                          ///< Releases box mutex, DO NOT USE!!!!!!!!!!!
   bool           ReleaseInBoxMutex  ( );                          ///< Releases box mutex, DO NOT USE!!!!!!!!!!!
   LT::TMessage* PopInMsg    ( int type = 0,   int priority = 0);            ///< Pop a message from the inbox list
   bool           ProcMsg     ( LT::TMessage *msg);                          ///< Function to process messages called from the TLinks

   void   GetInfoComm ( wchar_t *msg,      unsigned int size);           ///< GetInfoComm. Returns a char * with the information about the comm. Manager and each Tlink

   void   CreateInBox(HWND hThread);

   LT::TMessage * GetInboxMsg();

   HWND   GetHWnd()        {  return m_hWnd;                                  }

   bool CreateLinkMutex(s_remoteLT remoteLT);                             ///< create a link for connecting to remote module information using the mutex to accesss the list
   void SendCFGLoadedToAllLinks();                                           ///< Send MSG_CFG_LOADED to all links
   void DeleteAllLinks();                                                   ///< Deletes all links

   void DisconnectionFromIPandPort(std::wstring ip, int port);
   bool SendMsgToMyself( LT::TMessage *msg)                                  {
      return PushInMsg( msg);
   }


   // --inline functions --
  public:

   //--------------------------------------------------------------------------
   /**
      Creates the param needed to communicate with this TLink.
      Useful to send messages among diferent TCommMngrs
      @param param
      @param capacity
      @return
   */
   bool CreateDefaultServerParam( wchar_t *param, int capacity)                  {
      return m_server ? m_server->CreateMyParam( param, capacity) : false;
   };

   //--------------------------------------------------------------------------
   /**
      Check whether a module is local or remote. The other module is local to mine
      (both modules share memory context) if the first bit of the m_type member
      is set to 1. If no TLink is opened with the other module, this function returns -1.
      @param moduleName
      @return
   */
    int IsLocalModule(const wchar_t *moduleName)                               {
      for( std::list<TLink *>::iterator i = m_linkList.begin(); i != m_linkList.end(); i++)
         if( (*i) && (*i)->IsDest( moduleName))
            return (bool)((*i)->GetType() & 0x8000);

      return -1;
   };


   //--------------------------------------------------------------------------
   /**
      Increases message sequence number.
      @return
   */
   int IncMessageNumber()                                                    {
      return m_msgNumber++;
   };

   //--------------------------------------------------------------------------
   /**
      Sets message number for a message
      @param msg
   */
    void SetMessageNumber( LT::TMessage *msg)                                {
       if (msg->GetMsgNumber()==-1) {
           msg->SetMsgNumber( ++m_msgNumber );
           // msg->WriteHeader();  Se hace dentro de SetMsgNumber ya.
       }
   }

   //--------------------------------------------------------------------------
   /**
      Get name of the module of TCommMngr
      @param name
      @param size in wchar_t unities
      @return 
   */
   bool GetModuleName( wchar_t *name, DWORD count)                             {
	  if( count >= sizeof( m_moduleName)/ sizeof(wchar_t))                     {
         wcscpy( name, m_moduleName);
         return true;
      }
      return false;
   };

   //--------------------------------------------------------------------------
   /**
   Set the CommMngr into the server mode.
   @param servermode
   @return
   */
   void SetServerMode(bool mode = true){
      m_serverMode = mode;
   };

      //--------------------------------------------------------------------------
   /**
   Get the CommMngr server mode.
   @param servermode
   @return
   */
   bool GetServerMode(){
      return m_serverMode;
   };
   //--------------------------------------------------------------------------
   /**
   Add a MessageProcessor to the list.
   @param TMessageProcessor
   @return
   */
   bool AddMessageProcessor(TMessageProcessor * p){
      //ToDo  add checking to avoid adding twice the same processor
      m_messageProcessorsList->push_back(p);
      return true;
   }

   std::list<TMessageProcessor*>::iterator MessageProcessorListBegin(){
      return m_messageProcessorsList->begin();
   }

   std::list<TMessageProcessor*>::iterator MessageProcessorListEnd(){
      return m_messageProcessorsList->end();
   }

   //--------------------------------------------------------------------------
   bool CheckInBox() {
      return ( m_inBox->GetSize() != 0 );
   }

   //--------------------------------------------------------------------------
   /**
   Set the Comm status, for conection protocol purposes
   @param stat, new status of the server, disconected, waiting_conf, conected,...
   @return
   */
   void SetConnectionStatus(e_commMngrStatus stat){

      m_status = stat;
   }

   // -- variables & methods for statistical purposes, only DEBUG MODE --
   //--------------------------------------------------------------------------

   bool isStarted()  {
      return m_started;
   }

   //--------------------------------------------------------------------------
   /**
   Check if AD connected to the classifier
   @return
   */
   inline bool isConnectToServer() {
      return m_connectedToServer;
   }

  protected:
   #ifdef _DEBUG
   s_time_stat    m_tstats;
   #endif

   #ifdef _DEBUG
   void  UpdateStatistics     (wchar_t* msg, int size);
   void  CalculateStatistics  (LT::TMessage* msg);
   #endif

  public: 
   void  ClearTimeStat        () {
   #ifdef _DEBUG
      memset(&m_tstats,0,sizeof(m_tstats));
      m_tstats.tMin=1000;
      m_tstats.tMax=0;
      m_tstats.tmean=0;
      m_tstats.tmedian=0;
      m_tstats.messageCount = 0;
      m_tstats.tCreateSendMin = 1000;
      m_tstats.tCreateSendMax = 0;
   #endif
   }

};

#endif





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
