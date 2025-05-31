//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TUDPSocketLink.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#include <stdio.h>

#pragma hdrstop

#include "TUDPSocketLink.h"
#include "tcomm.h"
#include "TAux.h"

#pragma package(smart_init)

//----------------------------------------------------------------------------
//                             TUDPSocket
//----------------------------------------------------------------------------

/**
   Constructor
*/
TUDPSocket::TUDPSocket( TCommMngr *msgProc, wchar_t *dest, wchar_t *param)
           :TLink     ( msgProc, dest, param)                                 {
   // init data
   m_type           |= TL_UDPSOCKET;
   m_port            = 0;
   m_server          = NULL;
   m_serverEntryPoint= NULL;
   m_IP.S_un.S_addr  = INADDR_NONE;

   // UDP Client
   m_IdUDPClient                 = new TIdUDPClient( NULL);
   m_IdUDPClient->Active         = false;
   m_IdUDPClient->OnConnected    = OnSocketConnected;
   m_IdUDPClient->OnDisconnected = OnSocketDisconnected;
}

//----------------------------------------------------------------------------
/**
   Destructor
*/
TUDPSocket::~TUDPSocket()                                                     {

   Close();
   delete m_IdUDPClient;

   if( m_serverEntryPoint)
      free( m_serverEntryPoint);

   if( m_server)
      delete m_server;
}

//----------------------------------------------------------------------------
/**
   Open the conection
   @return true if the socket becomes opening, false otherwise
*/
bool TUDPSocket::OpenClientPart()                                             {

   ProcessParam();
   if( m_clientConfigured)                                                    {

      m_IdUDPClient->Host  = inet_ntoa( m_IP);
      m_IdUDPClient->Port  = m_port;
      m_status             = (m_status & ST_CLIENT_MASK) | ST_CLIENT_OPENING;
      m_IdUDPClient->Connect();
      return true;
   }
   return false;
}

//----------------------------------------------------------------------------
/**
   Open the conecction
   @return true if the socket becomes opening, false otherwise
*/
bool TUDPSocket::OpenServerPart()                                             {

   ProcessParam();
   if( m_serverConfigured)                                                    {
      if( m_serverEntryPoint)                                                 {
         m_server          = new TUDPServerSocket( m_proc, m_serverEntryPoint);
         m_status          = (m_status & ST_SERVER_MASK) | ST_SERVER_OPENING;
         if( m_server->Open())                                                {
            m_status       = (m_status & ST_SERVER_MASK) | ST_SERVER_OPENED;
            return true;
         }
         else
            m_status       = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSED;
      }
   }
   else
      // Server status = none
      m_status &= ST_SERVER_MASK;

   return false;
}

//----------------------------------------------------------------------------
/**
   Close the connection
*/
void TUDPSocket::Close()                                                      {

   if( m_IdUDPClient->Connected())                                            {
      m_status             = (m_status & ST_CLIENT_MASK) | ST_CLIENT_CLOSING;
      m_IdUDPClient->Disconnect();
   }
   if( m_server)                                                              {
      m_status             = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSING;
      m_server->Close();
      m_status             = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSED;
   }
}

//----------------------------------------------------------------------------
/**
   Check & process parameter sintax

   @return true if at leaest one part of the Tlink (Server or client) can be opened
*/
bool TUDPSocket::ProcessParam()                                               {

   // Param MUST be: Remote_IP:Remote_Port:LocalServerPort
   // Remote_IP and Remote_Port are the client part
   // LocalServerPort is the server part

   m_serverConfigured = m_clientConfigured = false;
   // Get a pointer to the end of each parameter. With these pointers we can copy
   // each parameter to a temporal string, to process them. It's not posible to
   // use strtok, becasuse all param are optional, and the token is always ':'

   wchar_t  *pEndRemoteIP,
            *pEndRemotePort,
            *pEndLocalServerPort = NULL,
            temp[32];

   pEndRemoteIP        = wcschr( m_param, L':');
   pEndRemotePort      = pEndRemoteIP  ? wcschr( pEndRemoteIP+1, L':')       : NULL;
   if( !pEndRemotePort)
      // Is posible that there isn't a ':' because there is no local server port
      pEndRemotePort      = pEndRemoteIP  ? wcschr( pEndRemoteIP+1, L'\0')   : NULL;
   else
      pEndLocalServerPort = pEndRemotePort ? wcschr( pEndRemotePort+1,L'\0') : NULL;

   memset( temp, 0x00, sizeof( temp));

   // Remote IP
   if( pEndRemoteIP)
      wcsncpy( temp, m_param, pEndRemoteIP - m_param);
   if( *temp)  {
      char* ctemp = TAux::WSTR_TO_STR(temp);
      if (ctemp) {
         m_IP.S_un.S_addr    = inet_addr( ctemp);
         delete [] ctemp;
      }
   }

   memset( temp, 0x00, sizeof( temp));

   // Remote Port
   if( pEndRemotePort && pEndRemoteIP)
      wcsncpy( temp, pEndRemoteIP+1, pEndRemotePort - (pEndRemoteIP+1));
   if( *temp) {
      m_port = _wtoi( temp);
   }

   //-- Check client part
   if ( m_IP.S_un.S_addr != INADDR_NONE && m_IP.S_un.S_addr != INADDR_ANY && m_port)
      // IP & local port OK. Client part OK
      m_clientConfigured = true;

   memset( temp, 0x00, sizeof( temp));

   //-- Check Server part
   // LocalServerPort
   if( pEndLocalServerPort && pEndRemotePort)
      wcsncpy( temp, pEndRemotePort+1, pEndLocalServerPort - (pEndRemotePort+1));
   if( *temp)                                                                 {
      m_serverEntryPoint = _wcsdup( temp);
      if( m_serverEntryPoint && _wtoi( m_serverEntryPoint))
         // Server Part OK
         m_serverConfigured = true;
      else                                                                    {
         free( m_serverEntryPoint);
         m_serverEntryPoint = NULL;
      }
   }
   return m_serverConfigured | m_clientConfigured;
}

//----------------------------------------------------------------------------
/**
   Send a msg through the TLink

   @param   msg   Message to send through the TLink
   @return true if the socket becomes opening, false otherwise
*/
bool TUDPSocket::TxMsg( LT::TMessage *msg)                                   {

   // Check if the TLink is open, and if we point to the right destination
   if( m_status & ST_CLIENT_OPENED && msg->IsDest( m_dest))                   {
      int size = msg->Size();

      // Message As Raw
      if( size != -1)                                                         {
         const char *rawData = msg->AsRaw();
         // Message to TBytes
         Sysutils::TBytes data = RawToBytes(rawData, size);
         // Send
         m_IdUDPClient->SendBuffer(data);
         // update Statistics
         m_numMsgTx++;
         m_bytesTx += size;

         msg->TraceSent();
         // free up memory
         delete msg;     // As its a socket connection delete the message

         return true;
      }
   }
   return false;
}

//----------------------------------------------------------------------------
/**
   Create the needed param to communicate with this TLink.
   Useful to send between diferent TCommMngrs

   @return true if the needed param has ben created and copied on the given string. false otherwise
*/
bool TUDPSocket::CreateMyParam( wchar_t *destParam, int capacity)                {

   wchar_t myparam[128];

   m_server->CreateMyParam( myparam, sizeof( myparam));

   if( wcslen( myparam) < (unsigned) capacity)                                {
      wcscpy( destParam, myparam);
      return true;
   }
   return false;
}

//----------------------------------------------------------------------------
/**
   Complete TLink

   @param paramLink Needed string with the necessary information to complete the TLink
*/
void TUDPSocket::CompleteTLink( wchar_t *paramLink)                              {

   // m_param now should be UDPSOCKET#::localPort
   // paramLink should be UDPSOCKET#destIP:destPort
   // With the data in paramLink we will complete the paramLink to:
   // UDPSOCKET#destIP:destPort:localPort

   if( m_param)
      free( m_param);

   m_param = (wchar_t *)malloc( (wcslen( paramLink)+ 7)* sizeof(wchar_t)); // 7 = ':' + '\0' + 5 for the port
   swprintf( m_param, L"%s:%d", paramLink, m_server->GetPort());
}

//----------------------------------------------------------------------------
/**
   Socket connects (Event)
*/
void __fastcall TUDPSocket::OnSocketConnected(TObject *Sender)                {
   // just change status to real
   m_status   = (m_status & ST_CLIENT_MASK) | ST_CLIENT_OPENED;
}

//----------------------------------------------------------------------------
/**
Socket disconnects (Event)
*/
void __fastcall TUDPSocket::OnSocketDisconnected(TObject *Sender)             {
   // just change status to real
   m_status   = (m_status & ST_CLIENT_MASK) | ST_CLIENT_CLOSED;
}

//----------------------------------------------------------------------------
/**
   Get information about TLink
   @return a char * with the status information about the TLink
*/
void TUDPSocket::GetInfoTLink( wchar_t *msg, unsigned int size)          {

   // Add especific information about the TLink
   wchar_t text[256];
   unsigned int realSize;

   swprintf( text, L"Type of TLink: UDPLINK\n");
   wcsncat( msg, text, size- wcslen( msg));

   // Add server information
   m_server->GetInfoTLink( msg, size);


}


//----------------------------------------------------------------------------
//                            TUDPServerSocket
//----------------------------------------------------------------------------

/**
   Constructor

   @param   msgProc     TMsgProcessor class to send notifictions when anything is received
   @param   entryPoint  Parameter needed to create the TLinkServer. For UDP server sockets MUST be the port
*/
TUDPServerSocket::TUDPServerSocket( TCommMngr *msgProc, wchar_t *entryPoint)
                 :TLinkServer     ( msgProc, entryPoint)                      {
   m_type  |= TL_UDPSOCKET;

   m_IdUDPServer                 = new TIdUDPServer( NULL);
   m_IdUDPServer->Active         = false;

   //TODO:  If we need to send bigger messages, we need should make a protol over udp or use TCP

   m_IdUDPServer->BufferSize     = 65536;    // Max allowed packet size by RFC is 65507
   m_IdUDPServer->OnUDPRead      = OnUDPRead;
}

//----------------------------------------------------------------------------
/**
   Destructor
*/
TUDPServerSocket::~TUDPServerSocket()                                         {
   Close();
   delete m_IdUDPServer;
}

//----------------------------------------------------------------------------
/**
   Opens the conecction
   @return true If the server really opens, false otherwise (bad port)
*/
bool TUDPServerSocket::Open()                                                 {
   if( ProcessParam())                                                        {
      try                                                                     {
         if( m_port)                                                          {
            m_serverConfigured            = true;
            m_IdUDPServer->DefaultPort    = m_port;
            m_IdUDPServer->Active         = true;
            m_status                      = (m_status & ST_SERVER_MASK) | ST_SERVER_OPENED;
         }
      }
      catch (...)                                                             {
         m_status   = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSED;
      }
   }
   return (m_status & ST_SERVER_OPENED);
}

//----------------------------------------------------------------------------
/**
   Close the conection
*/
void TUDPServerSocket::Close()                                                {
   if( !(m_status & ST_SERVER_CLOSED))
      m_IdUDPServer->Active = false;
   m_status   = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSED;
}

//----------------------------------------------------------------------------
/**
System::DynamicArray<System::Byte>
   Message arrives event
*/
//void __fastcall TUDPServerSocket::OnUDPRead( TObject *Sender, Sysutils::TBytes AData,
//                                             TIdSocketHandle *ABinding)       {
#include "TAux.h"
void __fastcall TUDPServerSocket::OnUDPRead(TIdUDPListenerThread* AThread, _dt_Idudpserver_1 AData, Idsockethandle::TIdSocketHandle* ABinding) {

   wchar_t   *smsg;
   // Read message and create s_message structure
   smsg = (wchar_t *)malloc( AData.Length);
   BytesToRaw( AData, smsg, AData.Length);

   // Ceate TMessage
   LT::TMessage  *myMsg = new LT::TMessage((s_message*)smsg, AData.Length);
   // Check
   if( myMsg->Check())                                                        {
      myMsg->TraceReceived();
      // Add IPOrigin of the message
      IN_ADDR IP;

      char* ctemp = TAux::WSTR_TO_STR(ABinding->PeerIP.c_str());
      if (ctemp) {
          IP.S_un.S_addr    = inet_addr (ctemp);
         delete [] ctemp;
      }
      //IP.S_un.S_addr    = inet_addr( ABinding->PeerIP.c_str());
      myMsg->SetIPOrigin( IP);

      // update Statistics
      m_numMsgRx++;
      m_bytesRx += AData.Length;

      // And just call message processor, we don't proccess any message
      // The processor is responsible of create the list & thread
      // to process the message to avoid blocking this thread
      m_proc->ProcMsg( myMsg);
   }

   // free up memory
   // We don't free up mymsg. It's freed up automatically when not necessary
   free( smsg);
}

//----------------------------------------------------------------------------
/**
   Check & process parameter sintax
*/
bool TUDPServerSocket::ProcessParam()                                          {

   // Param MUST be: LocalServerPort
   m_port = _wtoi( m_param);
   return m_port;
}

//----------------------------------------------------------------------------
/**
   Create the needed param to communicate with this TLink.
   Useful to send between diferent TCommMngrs

   @return true if the needed param has ben created and copied on the given string; false otherwise
*/
bool TUDPServerSocket::CreateMyParam( wchar_t *destParam, int capacity)          {

   wchar_t myparam[128];

   swprintf( myparam, L"UDPSOCKET#:%d", m_port);
   if( wcslen( myparam) < (unsigned)capacity)                                 {
      wcscpy( destParam, myparam);
      return true;
   }
   return false;
}






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------

