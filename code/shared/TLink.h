//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TLink.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TLinkH

#define TLinkH

#include "TMessage.h"

class TCommMngr;

/** --
   Most significant bytes, are type of TLink implementation (socket, cue,....)
   Less significant bytes, are TLink type (Client, Server,...)
*/
enum                                                                          {
   TL_CLIENT = 0x0001,
   TL_SERVER = 0x0002
};                            ///<Tlink's Types & subtypes

/**
* 2 Most significant bytes, are the status of the server part of the TLink.
* 2 Less significant bytes, are the status of the client part of the Tlink.
* Examples:
* - 0x1313 Means that both, client and server, are correctly open.
* - 0x0113 Means that the cliente is correctly open, and no server is configured (so its closed).
* - 0x1113 Means that the client is correctly open, but the server is closed (but configured ans ready to be opened).
* - 0x1111 Means that both, client and server are configured and ready to be open, but still closed.
*/

typedef enum                                                                  {
   ST_NONE              = 0x0000,
   ST_CLIENT_CLOSED     = 0x0001,
   ST_CLIENT_OPENING    = 0x0002,
   ST_CLIENT_OPENED     = 0x0004,
   ST_CLIENT_CLOSING    = 0x0008,
   ST_CLIENT_MASK       = 0xFF00,
   ST_SERVER_CLOSED     = 0x0100,
   ST_SERVER_OPENING    = 0x0200,
   ST_SERVER_OPENED     = 0x0400,
   ST_SERVER_CLOSING    = 0x0800,
   ST_SERVER_MASK       = 0x00FF
} e_tlinkStatus;                    ///< TLink Status


//-----------------------------------------------------------------------------
//**************************  TLinkBase Class  ********************************
//-----------------------------------------------------------------------------

/// Abstract base-class for all the communication links (clients and servers)
/**
   Contains all shared members for TLink and TLinkServer.
*/
class TLinkBase                                                               {

  protected:
   int                  m_status;                   ///< TLink status.
   int                  m_type;                     ///< Tlink type.
   TCommMngr            *m_proc;                    ///< Processor of the messages (tipically TCommMngr).
   wchar_t              *m_param,                   ///< Parameter for the TLink.
                        *m_defaultParam;            ///< Parameter to connect to the deafult server on the other TCommMngr. Useful to send to other TCommMngr.


  public:

      TLinkBase(TCommMngr *proc, wchar_t *param);      ///< Constructor.
      virtual ~TLinkBase();                         ///< Destructor.

  protected:
   virtual bool ProcessParam()  = 0;                ///< Check and process parameter sintax.

  public:

   virtual bool CreateMyParam(wchar_t *param, int capacity)  = 0;             ///< Create the needed param to communicate with this TLink. Useful to the TLink negotiation bettween TCommMngrs.
   virtual int GetStatus();                                                ///< Get the status of the TLink.
   virtual int GetType();                                                  ///< Get the Type of the TLink.
   virtual inline wchar_t *GetDefaultParam(wchar_t *param, unsigned int size);   ///< Get the param to create the Tlink. Useful to send to other TCommMngr's.
   virtual inline void SetDefaultParam(wchar_t *param);                       ///< Get the param to create the Tlink. Useful to send to other TCommMngr's.
   virtual void GetInfoTLink( wchar_t *msg, unsigned int size)  = 0;  ///< GetInfoTlink. Returns a char * with the status information about the TLink.
   virtual bool IsClosed();                                                   ///< method to check if link is closed or not
};

   // -- inline methods
   //-------------------------------------------------------------------------
   /**
   */
   inline int TLinkBase::GetStatus()                                                 {
      return m_status;
   };

   //-------------------------------------------------------------------------
   /**
   */
   inline int TLinkBase::GetType()                                                   {
      return m_type;
    };

   //-------------------------------------------------------------------------
   /**
   */
   inline wchar_t* TLinkBase::GetDefaultParam(wchar_t *param, unsigned int size)           {
      return (m_defaultParam && param && size >= sizeof( m_defaultParam)) ? wcscpy( param, m_defaultParam) : NULL;
   };

   //-------------------------------------------------------------------------
   /**
   */
   inline void TLinkBase::SetDefaultParam(wchar_t *param) {
      if( m_defaultParam)
         free( m_defaultParam);
      m_defaultParam = _wcsdup( param);
};

//------------------------------------------------------------------------------
//****************************  TLink Class  ***********************************
//------------------------------------------------------------------------------

/// Abstract class to represent the client part in the communication between modules
/**
*  TLink represents an abstract interface to communicate with other modules.
*  TLink will always be client type, and it will connect to their matching
*  TlinkServer on the destination module we want to communicate.
*
* A list of TLink's will be managed on the TCommMngr class. When the TCommMngr
* has a TLink to a module, is able to communicate with it.
*
* If the TCommMngr doesn't have a Tlink to another module, will ask for the
* necessary information to create it to the GM.
*/
class TLink : public TLinkBase                                                {

  protected:
   wchar_t                 m_dest[ addressSize];                      ///< Module name destination.
   bool                    m_serverConfigured,                        ///<
                           m_clientConfigured;                        ///<
   unsigned int            m_numMsgTx,                                ///< Num of msg transmited.
                           m_bytesTx;                                 ///< Bytes transmited.

   unsigned     m_last_received_time;                                  ///< Time flag for control of "expired links".
   unsigned     m_last_sent_time;                                      ///< Time flag for control of "expired links".

  public:

   TLink( TCommMngr *proc, wchar_t *dest, wchar_t *param);                  ///< Constructor.

   virtual bool TxMsg(LT::TMessage *msg)       = 0;                  ///< Send a message to the destination.
   virtual bool OpenServerPart()                = 0;                  ///< Open server part of the communication.
   virtual bool OpenClientPart()                = 0;                  ///< Open client part of the communication.
   virtual void Close()                         = 0;                  ///< Close communication.
   virtual void CompleteTLink(wchar_t *paramLink)  = 0;                  ///< Complete TLink.
   virtual void GetInfoTLink( wchar_t *msg, unsigned int size);  ///< GetInfoTlink. Returns a char * with the status information about the TLink.
   virtual wchar_t *GetDefaultParam(  wchar_t *param, unsigned int size);   ///<
   virtual void ProcessMsgReceived() {};                              ///< Process messages received by link.
   virtual HANDLE GetSemaphoreBox () {return NULL;};                  ///< Get Semaphore for management of events

   bool    ExpiredReceivedTime(UINT timeout);                               ///< Check if the link has "expired" due to received time
   bool    ExpiredSentTime(UINT timeout);                               ///< Check if the link has "expired" due to sent time
   void    UpdateReceivedTime() ;                              ///< Updates the last received time
   void    UpdateSentTime() ;                                  ///< Updates the last sent time

   virtual bool Closed()   {
      return  !(m_status & ST_CLIENT_OPENED) ;
    }

//   virtual void PrintStatus{
//      return  (m_status & ~ST_CLIENT_OPENED) ;
//    }


     // -- inline methods
   //-------------------------------------------------------------------------
   /**
      Check destination of the TLink.
   */
   virtual bool IsDest(const wchar_t *dest)                                      {
      if (dest == NULL || m_dest == NULL)
         return false;
      if( wcslen( dest) == wcslen( m_dest))
            return (bool)!wcscmp(dest, m_dest);
      else if( wcslen( dest) > wcslen( m_dest))
         // if org is larger than m_org, compare until strlen( m_org) and the next char MUST be '\'
         return (!wcsncmp(dest, m_dest, wcslen( m_dest)) && *(dest + wcslen( m_dest)) == L'\\');
      else
         //org never could be shorter than m_org
      return false;
   }

   //------------------------------------------------------------------------------
   /**
      Get a copy of the link destination

      @param   dest  Destination of message
      @param   size  Size of module destination name ( number of wchar's)
      @return  Copy of message destination or NULL if any error
   */
   inline wchar_t *TLink::GetDest( wchar_t *dest, int size)                    {
      return (dest && size >= addressSize) ? wcscpy( dest, m_dest) : NULL;
   }


};




//------------------------------------------------------------------------------
//***********************  TLinkServer Class  **********************************
//------------------------------------------------------------------------------
/// Abstract class to represent a server in the communication between modules
/**
   TLinkServer represents an abstract interface to receive mesages from other
   modules. All servers will inherit from TLinkServer. TLinkServer is just
   the interface,	so its only responsible of waiting for the messages,
   checking its integrity, and send it to the processor. TLinkServer doesn't
   process any message.
*/

class TLinkServer : public TLinkBase                                          {

  public:
//   protected: todo: cambair esto a protected
   bool                 m_serverConfigured;
   // Statistics
   unsigned int         m_numMsgRx,                                     ///< Num of msg received.
                        m_bytesRx;                                      ///< Bytes received.

 public:

   TLinkServer( TCommMngr *proc, wchar_t *entryPoint);                  ///< Constructor.
   virtual bool Open()           = 0;                                   ///< Open server part of the communication.
   virtual void Close()          = 0;                                   ///< Close communication.
   virtual void GetInfoTLink( wchar_t *msg, unsigned int size); ///< GetInfoTlink. Returns a char * with the status information about the TLink.
};
#endif




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
