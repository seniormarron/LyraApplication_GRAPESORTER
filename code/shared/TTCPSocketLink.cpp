//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TTCPSocketLink.cpp
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

#include "TTCPSocketLink.h"
#include "tcomm.h"
#include "TAux.h"
#pragma package(smart_init)

//------------------------------------------------------------------------------
//                             TTCPSocket
//------------------------------------------------------------------------------
#ifdef _MSC_VER
#include "TTCPServerVS.h"
#include "TTCPClientVS.h"

using namespace  System;
using namespace  System::Net;
using namespace  System::Net::Sockets;
using namespace  System::Threading;
using namespace  System::Text;
#endif
/**
   Constructor
*/
TTCPSocket:: TTCPSocket ( TCommMngr *msgProc, wchar_t *dest, wchar_t *param)
           : TLink      ( msgProc, dest, param)                               {
   // init data
   m_type           |= TL_TCPSOCKET;
   m_port         = 0;
   m_server          = NULL;
   m_serverEntryPoint= NULL;
   m_IP.S_un.S_addr  = INADDR_NONE;
#ifdef _MSC_VER
   m_tTCPClientVS = gcnew TTCPClientVS(this);
#else
   // Indy TCP Client
   m_IdTCPClient   = new TIdTCPClient( NULL);

   m_IdTCPClient->UseNagle          = false;
   m_IdTCPClient->OnConnected       = OnClientConnected;
   m_IdTCPClient->OnDisconnected    = OnClientDisconnected;

   m_IdTCPClient->ConnectTimeout = 25; //vic: me la juego a ponerlo bajo para no bloquear todo el modulo de comunicaciones

#endif
}

//-----------------------------------------------------------------------------
/**
   Destructor
*/
TTCPSocket::~TTCPSocket()                                                     {

   Close();

   if (m_serverEntryPoint)
      free(m_serverEntryPoint);

   if( m_server)
      delete m_server;
#ifndef _MSC_VER
   if (m_IdTCPClient)
      delete m_IdTCPClient;
#endif
   if( m_serverEntryPoint)
      free( m_serverEntryPoint);

}


/**
   Open the connection
   @return  True if the socket becomes opening, false otherwise
*/
bool TTCPSocket::OpenClientPart()                                             {

   static unsigned int connectionException = 500;

   ProcessParam();
   if( m_clientConfigured)                                                    {
   //initialization of the address
#ifdef _MSC_VER
      IPAddress ^ipAddress = gcnew IPAddress(m_IP.S_un.S_addr);
      IPEndPoint^ remoteEP = gcnew IPEndPoint(ipAddress, m_port);
#else
      m_IdTCPClient->Host  = inet_ntoa( m_IP);
      m_IdTCPClient->Port  = m_port;
#endif
      m_status             = (m_status & ST_CLIENT_MASK) | ST_CLIENT_OPENING;    //set OPENING bit
      try
     {
#ifdef _MSC_VER
         m_tTCPClientVS->Start(remoteEP);
      } catch (Exception ^exception)                                          {
         //Console::WriteLine(L"TTCPSocket::OpenClientPart() Exception: " + exception->ToString());
         Console::WriteLine(L"Conection Not Ready");
#else
        m_IdTCPClient->Connect();
      } catch (Exception &exception)                                          {
         if ( connectionException++ == 500 ) {
            wchar_t msg [1024];
            swprintf(msg, L"OPEN CLIENT TCP EXCEPTION : %s", exception.Message.c_str() );
            LOG_ERROR1( msg);
            connectionException = 0;
         }
#endif
         m_status  = (m_status & ST_CLIENT_MASK) | ST_CLIENT_CLOSED;
      } //end catch
      catch ( std::exception &ex)                                             {
         //C++ standard exceptions
         if ( connectionException++ == 500 ) {
            wchar_t *message = TAux::STR_TO_WSTR( ex.what());
            wchar_t str[256];
            snwprintf( str,256, L"OPEN CLIENT TCP std::EXCEPTION : %s",message);
            LOG_WARN1( str);
            delete [] message;
            connectionException = 0;
         }
         m_status  = (m_status & ST_CLIENT_MASK) | ST_CLIENT_CLOSED;
      }
      catch (...) {
         LOG_ERROR1(L"OPEN CLIENT TCP EXCEPTION ...");
         m_status  = (m_status & ST_CLIENT_MASK) | ST_CLIENT_CLOSED;
      }
      return !(m_status & ST_CLIENT_CLOSED);
   }
   return false;
}

//-----------------------------------------------------------------------------
/**
   Open the connection
   @return  True if the socket becomes opening, false otherwise
*/
bool TTCPSocket::OpenServerPart()                                             {

   ProcessParam();
   if( m_serverConfigured)                                                    {
      if( m_serverEntryPoint)                                                 {

         m_server          = new TTCPServerSocket( m_proc, m_serverEntryPoint);

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

//-----------------------------------------------------------------------------
/**
   Close the connection
*/
void TTCPSocket::Close()                                                      {


#ifdef _MSC_VER
   if (m_tTCPClientVS->Connected)                                            {
      m_status             = (m_status & ST_CLIENT_MASK) | ST_CLIENT_CLOSING;
     m_tTCPClientVS->Stop();
   }
#else
   try                                                                        {
      //commented because it gets stuck sometimes
//      if (m_IdTCPClient->Connected())                                         {
         m_status = (m_status & ST_CLIENT_MASK) | ST_CLIENT_CLOSING;
         //clean the buffer so the client can disconect without waiting the buffer to be empty
         if ( m_IdTCPClient->IOHandler )  {
            m_IdTCPClient->IOHandler->InputBuffer->Clear();
         }
         m_IdTCPClient->Disconnect();
         m_IdTCPClient->DisconnectNotifyPeer();
//      }
   }catch (Exception &exception) {
      wchar_t msg [1024];
      swprintf(msg, L"%s : %s",L"Close TCP EXCEPTION", exception.Message.c_str() );
      LOG_ERROR1( msg);
   }
#endif
   if( m_server)                                                              {
//      m_status             = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSING;
//      m_server->Close();
      m_status             = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSED;
   }
}

//-----------------------------------------------------------------------------
/**
   Check & process parameter sintax
   @return  True if at leaest one part of the Tlink (Server or client) can be opened
*/
bool TTCPSocket::ProcessParam()                                               {

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
   pEndRemotePort      = pEndRemoteIP    ? wcschr( pEndRemoteIP+1,   L':')    : NULL;
   if( !pEndRemotePort)
      // Is posible that there isn't a ':' because there is no local server port
      pEndRemotePort      = pEndRemoteIP    ? wcschr( pEndRemoteIP+1,   L'\0')    : NULL;
   else
      pEndLocalServerPort = pEndRemotePort  ? wcschr( pEndRemotePort+1, L'\0')   : NULL;

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
   if( *temp)
      m_port = _wtoi( temp);

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


//-----------------------------------------------------------------------------
/**
   Send a msg through the TLink

   @param   msg   Message to send through the TLink
   @return  True if the socket becomes opening, false otherwise
*/
bool TTCPSocket::TxMsg( LT::TMessage *msg)                                 {

   try {

      // Check if the TLink is open, and if we point to the right destination
      if( m_status & ST_CLIENT_OPENED && msg->IsDest( m_dest))
     {

#ifdef _MSC_VER
        if (!m_tTCPClientVS->Connected)
#else
        if (!m_IdTCPClient->Connected())
#endif
      {
         OnClientDisconnected(NULL);
         return false;
      }

        int          size    = msg->Size();

//        wchar_t text [1024];
//       swprintf(text, L"%s : %d",L"Sending message of type:", (int)msg->Type() );
//       OutputDebugStringW(text);

        // Message As Raw
        if( size != -1)
      {
         const char *rawData = msg->AsRaw();
            // Message to TBytes
            if (rawData)
         {
#ifndef _MSC_VER
            Sysutils::TBytes dataToSend = RawToBytes(rawData, size);
            if (dataToSend.Length != 0)
#else
            if (size > 0)
#endif
            {
#ifdef _MSC_VER
               array<Byte> ^data = gcnew array<Byte>(size + 4);
               // Data to send
               data[0] = 0; data[1] = 0; data[2] = 0;   //we add the '   #' expected by the server as init of each message
               data[3] = '#';
               for (int id = 0; id < size ; ++id)
               {
                  data[id + 4] = rawData[id];
               }

               m_tTCPClientVS->Send(data);
#else

               //-- Send
               m_IdTCPClient->IOHandler->WriteBufferOpen();

               // if the first char is '\0' ReadBytes on the server fails, so we always send a # at the beginning
               m_IdTCPClient->IOHandler->Write( '#');
               // Writes the data
               m_IdTCPClient->IOHandler->Write( dataToSend);

               m_IdTCPClient->IOHandler->WriteBufferClose();

//               // flushes the buffer
//               m_IdTCPClient->IOHandler->WriteBufferFlush();
#endif

               msg->TraceSent();
               // update Statistics
               m_numMsgTx++;
               m_bytesTx += size;

            }
            else
               LOG_ERROR2(L"TRYING TO SEND NULL CONTENT. INFO: dest, type", m_dest);
            }
            else
               LOG_ERROR2(L"TRYING TO SEND NULL CONTENT. INFO: dest, type", m_dest);
            // free up memory
            delete msg;                   // As its a socket connection delete the message

            UpdateSentTime();
            return true;
         }
         else
            LOG_ERROR1( L"size==-1");
      }
      else                                                                       {
         if (!( m_status & ST_CLIENT_OPENED) )                                    {
            LOG_ERROR1( L"Link closed");
         }
         else
       {
            LOG_ERROR1( L"Incorrect destination");
         }
      }
   }
#ifdef _MSC_VER
      catch (Exception ^exception)                                            {
#else
     catch (Exception &exception)                                 { //does not work on indy socket exception, tryed to catch all possible indy exception but no one worked.
#endif
         m_status             = (m_status & ST_CLIENT_MASK) | ST_CLIENT_CLOSED;
         wchar_t text [1024];
#ifdef _MSC_VER
         swprintf(text, L"%s : %s",L"Invalid Connection. TX TCP EXCEPTION", exception->ToString() );
#else
         swprintf(text, L"%s : %s",L"Invalid Connection. TX TCP EXCEPTION", exception.Message.c_str() );
#endif
         LOG_ERROR1( text);
   }
   OnClientDisconnected(NULL);
   return false;
}

//-----------------------------------------------------------------------------
/**
   Create the needed param to communicate with this TLink.
   Useful to send between diferent TCommMngrs

   @return    true if the needed param has ben created and copied on the given string. false otherwise
*/
bool TTCPSocket::CreateMyParam( wchar_t *destParam, int capacity)                {
   wchar_t myparam[128];

   m_server->CreateMyParam( myparam, sizeof( myparam));

   if( wcslen( myparam) < (unsigned) capacity)                                {
      wcscpy( destParam, myparam);
      return true;
   }
   return false;
}

//-----------------------------------------------------------------------------
/**
   Complete TLink

   @param paramLink Needed string with the necessary information to complete the TLink
*/
void TTCPSocket::CompleteTLink( wchar_t *paramLink)                              {

   // m_param now should be TCPSOCKET#::localPort
   // paramLink should be TCPSOCKET#destIP:destPort
   // With the data in paramLink we will complete the paramLink to:
   // TCSOCKET#destIP:destPort:localPort
   if( m_param)
      free( m_param);
   m_param = (wchar_t *)malloc( (wcslen( paramLink)+ 7)* sizeof(wchar_t)); // 7 = ':' + '\0' + 5 for the port
   swprintf( m_param, L"%s:%d", paramLink, m_server->GetPort());
}

//-----------------------------------------------------------------------------
/**
   Socket connects (Event)
*/

#ifdef _MSC_VER
void __fastcall TTCPSocket::OnClientConnected(void *Sender) {
#else
void __fastcall TTCPSocket::OnClientConnected(TObject *Sender) {
#endif

   // just change status to real
   m_status   = (m_status & ST_CLIENT_MASK) | ST_CLIENT_OPENED;
   if ( m_serverEntryPoint )                                                  {
      LOG_INFO2(L"Client Connected:", m_serverEntryPoint);
   }
   else                                                                       {
      LOG_INFO1(L"Client Connected");
   }
}

//-----------------------------------------------------------------------------
/**
   Socket disconnects (Event)
*/

#ifdef _MSC_VER
void __fastcall TTCPSocket::OnClientDisconnected(void *Sender){
#else
void __fastcall TTCPSocket::OnClientDisconnected(TObject *Sender) {
#endif
   // just change status to real
   m_status   = (m_status & ST_CLIENT_MASK) | ST_CLIENT_CLOSED;
   //   m_IdTCPClient->DisconnectNotifyPeer();
    LOG_INFO1(L"Client Disconnected.");
}

//-----------------------------------------------------------------------------
/**
   Get information about the TLink

   @return a char * with the status information about the TLink
*/
void TTCPSocket::GetInfoTLink( wchar_t *msg, unsigned int size)          {
   // Add especific information about the TLink
   wchar_t  text[256];

   swprintf( text,L"Type of TLink: TCPLINK\n");
   wcsncat( msg, text, size- wcslen( msg));

   // Call parent
   TLink::GetInfoTLink( msg, size);

   // Add server information
   if ( m_server)       {
      m_server->GetInfoTLink( msg, size);
   }
}

bool TTCPSocket::IsClosed()   {
   //ToDo arreglo temporal hasta que m_status sirva para representar el estado.
   if ( m_server )  {
      return m_server->IsClosed();
   }
   else {
      return m_status & ST_CLIENT_CLOSED ;
   }
}

std::wstring TTCPSocket::GetIP() {
   return AuxStrings::utf8_to_utf16(std::string(inet_ntoa( m_IP)));
}


//------------------------------------------------------------------------------
//                          TTCPServerSocket
//------------------------------------------------------------------------------

/**
   Constructor

   @param    msgProc    TMsgProcessor class to send notifictions when anything is received
   @param   entryPoint  Parameter needed to create the TLinkServer. For TCP server sockets MUST be the port
*/
TTCPServerSocket:: TTCPServerSocket ( TCommMngr *msgProc, wchar_t *entryPoint)
                 : TLinkServer      ( msgProc, entryPoint)                    {

   m_type  |= TL_TCPSOCKET;

#ifdef _MSC_VER
   m_tTCPServerVS = gcnew TTCPServerVS(this);
#else
   m_IdTCPServer                 = new TIdTCPServer( NULL);
   m_IdTCPServer->UseNagle          = false;
   m_IdTCPServer->Active            = false;
   m_IdTCPServer->OnExecute         = OnExecute;
   m_IdTCPServer->OnException       = OnException;
   m_IdTCPServer->OnDisconnect      = OnDisconnection;
//   m_IdTCPServer->Bindings->Items
#endif

}

//-----------------------------------------------------------------------------
/**
   Destructor
*/
TTCPServerSocket::~TTCPServerSocket()                                         {
   Close();
#ifndef _MSC_VER
   delete m_IdTCPServer;
#else
   delete m_tTCPServerVS;
#endif
}

//-----------------------------------------------------------------------------
/**
   Opens the connection

   @return  True if the server really opens, false otherwise (bad port)
*/
bool TTCPServerSocket::Open()                                                 {

   if( ProcessParam())                                                        {
      try                                                                     {
         if( m_port)                                                          {
            m_serverConfigured            = true;
#ifdef _MSC_VER
            m_tTCPServerVS->m_port = m_port;
            m_tTCPServerVS->StartListening();
#else
            m_IdTCPServer->DefaultPort    = m_port;
            m_IdTCPServer->Active          = true;
#endif
            m_status                      = (m_status & ST_SERVER_MASK) | ST_SERVER_OPENED;
         }
      }
#ifdef _MSC_VER
     catch (Exception ^exception)                                            {
#else
     catch (Exception &exception)                                 { //does not work on indy socket exception, tryed to catch all possible indy exception but no one worked.
#endif
         m_status   = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSED;
         wchar_t msg [1024];

#ifdef _MSC_VER
       swprintf(msg, L"%s : %s", "OPEN TCP EXCEPTION", exception->ToString());
#else
       swprintf(msg, L"%s : %s", "OPEN TCP EXCEPTION", exception.Message.c_str());
#endif
         LOG_ERROR1(msg);
      }
   }
   return (m_status & ST_SERVER_OPENED);
}

//-----------------------------------------------------------------------------
/**
   Closes the connection
*/
void TTCPServerSocket::Close()                                                {
   if( !(m_status & ST_SERVER_CLOSED))                                        {
      try                                                            {
#ifdef _MSC_VER
        m_tTCPServerVS->Stop();
#else
         m_IdTCPServer->Active          = false;
         m_IdTCPServer->StopListening();
#endif
         // Wait for the thread to terminate
         ::Sleep(30);
      }
#ifdef _MSC_VER
     catch (Exception ^exception)                                            {
#else
     catch (Exception &exception)                                 { //does not work on indy socket exception, tryed to catch all possible indy exception but no one worked.
#endif
         wchar_t msg [1024];
#ifdef _MSC_VER
       swprintf(msg, L"%s : %s", "CLOSE TCP EXCEPTION", exception->ToString());
#else
       swprintf(msg, L"%s : %s", "CLOSE TCP EXCEPTION", exception.Message.c_str());
#endif
         LOG_ERROR1(msg);
      }
   }
   m_status   = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSED;

}

#ifndef _MSC_VER    //if builder...
//-----------------------------------------------------------------------------
/**
Message arrives event
*/
void __fastcall TTCPServerSocket::OnExecute(TIdContext *AContext)            {
   bool disconnected = false;
   AContext->Connection->CheckForGracefulDisconnect(disconnected);
   if(disconnected)  {
      return;
   }


   // Message will always start with # to avoid a '\0' char on the first byte.
   if (AContext->Connection->IOHandler->ReadChar() == L'#')                 {

      // Next read size of the stream
      Sysutils::TBytes stream;
      int tam = AContext->Connection->IOHandler->ReadLongInt(false);
      //         int tam = AContext->Connection->IOHandler->ReadInteger( false);
      // Add the size to the whole data
      stream = ToBytes(tam);
      // Read the rest of the message
      AContext->Connection->IOHandler->ReadBytes(stream, tam - 4, true);
      char   *smsg;
      // Read message and create s_message structure
      smsg = (char *)malloc(stream.Length);
      BytesToRaw(stream, smsg, stream.Length);

      // Ceate TMessage
      LT::TMessage  *myMsg = new LT::TMessage((s_message*)smsg, stream.Length);
      // Check
      if (myMsg->Check())                                                  {

         myMsg->TraceReceived();
         // Add IPOrigin of the message
         IN_ADDR IP;
         //IP.S_un.S_addr    = inet_addr( AContext->Binding()->PeerIP.c_str());
         char *ip = TAux::WSTR_TO_STR(AContext->Binding->PeerIP.c_str());
         IP.S_un.S_addr = inet_addr(ip);
         if (ip) {
            delete[] ip;
         }
         myMsg->SetIPOrigin(IP);

         // update Statistics
         m_numMsgRx++;
         m_bytesRx += tam;

         // And just call message processor, we don't proccess any message
         // The processor is responsible of create the list & thread
         // to process the message to avoid blocking this thread
         m_proc->ProcMsg(myMsg);
      }

      // free up memory
      // We don't free up mymsg. It's freed up automatically when not necessary
      free(smsg);
   }

}

//-----------------------------------------------------------------------------
/**
Excepction in receiver thread
*/
void __fastcall TTCPServerSocket::OnException(TIdContext *AContext, Exception * exception)            {

   bool isconnected = AContext->Connection->Connected();
   wchar_t msg[2048];
   swprintf(msg, L"%s : %s", L"TCP EXECUTE EXCEPTION", exception->Message.c_str());
   LOG_ERROR1(msg);


}
//-----------------------------------------------------------------------------
/**
 disconnection in receiver thread
*/
void __fastcall TTCPServerSocket::OnDisconnection(TIdContext *AContext)            {

   wchar_t msg[2048];
   snwprintf(msg,2048, L"TCP DISCONNECTION: ip: %s port: %d", AContext->Binding->PeerIP.c_str(), AContext->Binding->PeerPort );
   LOG_ERROR1(msg);

   m_proc->DisconnectionFromIPandPort(std::wstring(AContext->Binding->PeerIP.c_str()), AContext->Binding->PeerPort );

}

#else //visual studio

//-----------------------------------------------------------------------------
/**
   Message arrives event
*/
void __fastcall TTCPServerSocket::receivedMsg(LT::TMessage * myMsg)           {

   try                                                                        {
         if( myMsg->Check())                                                  {

            // update Statistics
            m_numMsgRx++;
            m_bytesRx += myMsg->GetTStream()->Size();

            // And just call message processor, we don't proccess any message
            // The processor is responsible of create the list & thread
            // to process the message to avoid blocking this thread

            if (m_proc)
               m_proc->ProcMsg( myMsg);
         }

         // free up memory
   }
   catch ( Exception ^exception)                                              {

      wchar_t msg [2048];
      swprintf(msg, L"%s : %s", "TCP EXECUTE EXCEPTION", exception->ToString());
      LOG_ERROR1(msg);
   }

}

#endif

//-----------------------------------------------------------------------------
/**
   Check & process parameter sintax
*/
bool TTCPServerSocket::ProcessParam()                                         {

   // Param MUST be: LocalServerPort
   m_port = _wtoi( m_param);
   return m_port;
}

//-----------------------------------------------------------------------------
/**
   Create the needed param to communicate with this TLink.
   Useful to send between diferent TCommMngrs

   @return true if the needed param has ben created and copied on the given string; false otherwise
*/
bool TTCPServerSocket::CreateMyParam( wchar_t *destParam, int capacity)          {

   //vicente : added processparam to force the m_port cast from m_param
   ProcessParam();
   wchar_t myparam[128];
   swprintf( myparam, L"TCPSOCKET#:%d", m_port);

   if( wcslen( myparam) < (unsigned)capacity)                                 {
      wcscpy( destParam, myparam);
      return true;
   }
   return false;
}
//-----------------------------------------------------------------------------



bool TTCPServerSocket::IsClosed()   {
    return m_status & ST_SERVER_CLOSED;
}






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------


