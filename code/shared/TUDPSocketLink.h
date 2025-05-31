//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TUDPSocketLink.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TUDPSocketLinkH
#define TUDPSocketLinkH

#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdUDPBase.hpp>
#include <IdUDPClient.hpp>
#include <IdUDPServer.hpp>

#include "tlink.h"

//-- Tlink's Types & subtypes:
// Most significant bit indicates if the tlink correspond to a local tlink (both modules share memory context)
// Most significant bytes, are type of TLink implementation (socket, cue,....)
// Less significant bytes, are TLink type (Client, Server,...)
enum                                                                          {
   TL_UDPSOCKET = 0x0100
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//                           TUPDServerSocket
/// TLinkServer implementacion, to communicate using Indy UDP Sockets
//------------------------------------------------------------------------------
/**
   To instance and open a TUDPServerSocket, we need:
      - Port number
      - IP and port of the server socket on the destination module
*/
class TUDPServerSocket : public TLinkServer                                   {
  protected:
   //-- Protected members
   int               m_port;          ///< Local port to listen
   TIdUDPServer  *m_IdUDPServer;      ///< Server Indy Socket (UDP)

   //-- Events
   /// Message arrives event
   //void __fastcall OnUDPRead(TObject *Sender, Sysutils::TBytes AData,TIdSocketHandle *ABinding);
   void __fastcall OnUDPRead(TIdUDPListenerThread* AThread, _dt_Idudpserver_1 AData, Idsockethandle::TIdSocketHandle* ABinding);

   // TLink & TLinkBase inheritance
   virtual bool ProcessParam();        ///< Check & process parameter sintax

  public:
   TUDPServerSocket(TCommMngr *_msgProc, wchar_t *entryPoint);  ///< Constructor
   virtual ~TUDPServerSocket();                              ///< Destructor

   // TLinkServer & TLinkBase inheritance
   virtual bool Open();                                      ///< Opens the connection
   virtual void Close();                                     ///< Closes the connection
   /// Create the needed param to communicate with this TLink. Useful to send between diferent TCommMngrs
   virtual bool CreateMyParam( wchar_t *param, int capacity);

   //Get the port
   inline GetPort()  { return m_port;};
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//                               TUPDSocket
///      TLink implementacion, to communicate using Indy UDP Sockets
//------------------------------------------------------------------------------

/**
   To instance and open a TUDPSocket TLink, we need:
      - Module destination name,
      - IP and port of the server socket on the destination module
*/
class TUDPSocket : public TLink                                               {
 protected:
   //-- Protected members
   wchar_t              *m_serverEntryPoint;     ///<
   IN_ADDR           m_IP;                    ///< IP destination module
   int               m_port;                  ///< Port destination module
   TIdUDPClient      *m_IdUDPClient;          ///< Client Indy Socket (UDP)
   TUDPServerSocket  *m_server;               ///< Server Socket

   //-- Events
   void __fastcall OnSocketConnected(      TObject *Sender);   ///< Socket connects (Event)
   void __fastcall OnSocketDisconnected(   TObject *Sender);   ///< Socket disconnects (Event)

   // TLink & TLinkBase inheritance
   virtual bool ProcessParam();                                ///< Check & process parameter sintax
 public:
   //-- Constructor & destructor
   TUDPSocket( TCommMngr *msgProc, wchar_t *dest, wchar_t *param);   ///< Constructor
   virtual ~TUDPSocket();                                      ///< Destructor

   // TLink & TLinkBase inheritance
   virtual bool OpenClientPart();                              ///< Open the client part of the connection
   virtual bool OpenServerPart();                              ///< Open the server part of the conecction
   virtual void Close();                                       ///< Close the connection
   virtual bool TxMsg( LT::TMessage *_msg);                   ///< Send a msg through the TLink
   /// Create the needed param to communicate with this TLink. Useful to send between diferent TCommMngrs
   virtual bool CreateMyParam( wchar_t *param, int capacity);
   virtual void CompleteTLink( wchar_t *paramLink);               ///< Complete TLink
   /// GetInfoTlink. Returns a char * with the status information about the TLink
   virtual void GetInfoTLink( wchar_t *msg, unsigned int size);
   virtual   void ProcessMsgReceived()         {};
   virtual HANDLE GetSemaphoreBox()      { return NULL;      };

};

#endif





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
