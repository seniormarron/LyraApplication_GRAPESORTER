//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TTCPSocketLink.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TTCPSocketLinkH
#define TTCPSocketLinkH

#include "tlink.h"

#ifndef _MSC_VER   //if not visual studio, we include indy sockets structures and methods

#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPServer.hpp>

#else

#include <vcclr.h>
#include <msclr\auto_gcroot.h>

using namespace System;
using namespace System::Net::Sockets;
//ref class TTCPServerVS;

#endif


//---------------------------------------------------------------------------

//-- Tlink's Types & subtypes:
// Most significant bit indicates if the tlink correspond to a local tlink (both modules share memory context)
// Most significant bytes, are type of TLink implementation (socket, cue,....)
// Less significant bytes, are TLink type (Client, Server,...)
enum                                                                          {
   TL_TCPSOCKET = 0x0400
};
//---------------------------------------------------------------------------


// TUPDServerSocket
/// TLinkServer implementacion, to communicate using Indy TCP Sockets
/**
   To instance and open a TTCPServerSocket, we need:
    - Port number
    - IP and port of the server socket on the destination module
*/
class TTCPServerSocket : public TLinkServer                                  {

#ifdef _MSC_VER
   friend ref class TTCPServerVS;
#endif

 protected:
    //-- Protected members
    int        m_port;                                        ///< Local port to listen

   // TLink & TLinkBase inheritance
   virtual bool ProcessParam();                                ///< Check & process parameter sintax

#ifndef _MSC_VER   //if not visual studio, we include indy sockets structures and methods

   TIdTCPServer  *m_IdTCPServer;      ///< Server Indy Socket (TCP)

    //-- Events
   void __fastcall OnExecute        (TIdContext *AContext);                         ///< Execution of the receiving thread
   void __fastcall OnException      (TIdContext *AContext, Exception * exception);  ///< Exceptions of the receiving thread
   void __fastcall OnDisconnection  (TIdContext *AContext);                         ///< disconnection of the receiving thread

#else

   msclr::auto_gcroot<TTCPServerVS ^>  m_tTCPServerVS;               ///< TCP Server which will receive the messages

   void __fastcall receivedMsg(LT::TMessage * msg);           ///< Execution of the receiving thread

#endif



 public:
    // Constructor & destructor
   TTCPServerSocket(TCommMngr *_msgProc, wchar_t *entryPoint);   ///< Constructor
   virtual ~TTCPServerSocket();                               ///< Destructor


    // TLinkServer & TLinkBase inheritance
   virtual bool Open();                                       ///< Opens the conecction
   virtual void Close();                                      ///< Closes the connection
   virtual bool IsClosed();                                   ///< method to check if link is closed or not

   virtual bool CreateMyParam( wchar_t *param, int capacity);     ///< Create the needed param to communicate with this TLink. Useful to send between diferent TCommMngrs


   //Get the port
   inline int GetPort()  { return m_port;};

};
//------------------------------------------------------------------------------


// TUPDSocket
/// TLink implementacion, to communicate using Indy TCP Sockets
/**
To instance and open a TTCPSocket TLink, we need:
    - Module destination name,
    - IP and port of the server socket on the destination module
*/
class TTCPSocket : public TLink
{
#ifdef _MSC_VER
   friend ref class TTCPClientVS;
#endif
 protected:
    //-- Protected members
   wchar_t     *m_serverEntryPoint;
   IN_ADDR           m_IP;                                     ///< IP destination module
   int      m_port;                                           ///< Port destination module

   #ifndef _MSC_VER   //if not visual studio, we include indy sockets structures and methods

    TIdTCPClient     *m_IdTCPClient;                           ///< Client Indy Socket (TCP)

   #else

   msclr::auto_gcroot<TTCPClientVS ^>  m_tTCPClientVS;

   #endif

   TTCPServerSocket *m_server;                                 ///< Server Socket

    //-- Events
#ifndef _MSC_VER   //if not visual studio, we include indy sockets structures and methods
   void __fastcall OnClientConnected(     TObject *Sender);    ///< Client Events (connection)
   void __fastcall OnClientDisconnected(  TObject *Sender);    ///< Client Events (disconnection)
#else
   void __fastcall OnClientConnected(void *Sender);    ///< Client Events (connection)
   void __fastcall OnClientDisconnected(void *Sender);    ///< Client Events (disconnection)
#endif

    // TLink & TLinkBase inheritance
   virtual bool ProcessParam();                                ///< Check & process parameter sintax
 public:
    //-- Constructor & destructor
    TTCPSocket( TCommMngr *msgProc, wchar_t *dest, wchar_t *param);  ///< Constructor
    virtual ~TTCPSocket();                                     ///< Destructor

    // TLink & TLinkBase inheritance
    virtual bool OpenClientPart();                             ///< Open the client part of the connection
    virtual bool OpenServerPart();                             ///< Open the server part of the conecction
    virtual void Close();                                      ///< Close the connection
    virtual bool IsClosed();                                   ///< method to check if link is closed or not
    virtual bool TxMsg( LT::TMessage *_msg);                  ///< Send a msg through the TLink
   /// Create the needed param to communicate with this TLink. Useful to send between diferent TCommMngrs
   virtual bool CreateMyParam( wchar_t *param, int capacity);
   virtual void CompleteTLink( wchar_t *paramLink);               ///< Complete TLink
   /// GetInfoTlink. Returns a char * with the status information about the TLink
   virtual void GetInfoTLink( wchar_t *msg, unsigned int size);
   virtual   void ProcessMsgReceived()  {};
   virtual HANDLE GetSemaphoreBox()     { return NULL;      };
   std::wstring GetIP();


};
//------------------------------------------------------------------------------

#endif







//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------

