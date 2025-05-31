//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TEventLink.cpp
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

#include "TEventLink.h"
#include "tcomm.h"
#include <process.h>
#include <exception>

//----------------------------------------------------------------------------

#pragma package(smart_init)

/// Sufix for the thread end event
const char *endEventLinkSufix = {"_END"};


//----------------------------------------------------------------------------
//****************************** TEventLink **********************************
//----------------------------------------------------------------------------

/**
   Constructor
*/
TEventLink::TEventLink( TCommMngr *msgProc, wchar_t *dest, wchar_t *param)
           :TLink     ( msgProc, dest, param)                                 {
   // init data
   m_type           |= TL_EVENT;
   m_destBox         = NULL;
   m_server          = NULL;

   // This kind of Tlink is always configured for server
   m_serverConfigured = true;
}

//----------------------------------------------------------------------------
/**
   Destructor
*/
TEventLink::~TEventLink()                                                     {

   Close();
   if( m_server)
      delete m_server;
}

//----------------------------------------------------------------------------
/**
   Process messages received by link.
*/
void TEventLink::ProcessMsgReceived()                                         {

   LT::TMessage *msg = m_server->m_localBox->Pop();
   if( msg && msg->Check())                                                {

      msg->TraceReceived();
      // Add IPOrigin of the message
      IN_ADDR IP;
      IP.S_un.S_addr    = inet_addr( "127.0.0.1");
      msg->SetIPOrigin( IP);
      // update Statistics
      m_server->m_numMsgRx++;
      m_server->m_bytesRx += msg->Size();

      // And just call message processor, we don't proccess any message
      // The processor is responsible of create the box & thread
      // to process the message to avoid blocking this thread
      m_proc->ProcMsg( msg);
   }

}

//----------------------------------------------------------------------------
/**
   Open the client part of the conecction
   @return true if the socket becomes opening, false otherwise
*/
bool TEventLink::OpenClientPart()                                             {

   ProcessParam();
   if( m_clientConfigured)                                                    {
      m_status = (m_status & ST_CLIENT_MASK) | ST_CLIENT_OPENED;
      return true;
   }
   LOG_ERROR1( L"Couldn't open client");
   return false;
}

//----------------------------------------------------------------------------
/**
   Open the server part of the conecction
   @return true if the socket becomes opening, false otherwise
*/
bool TEventLink::OpenServerPart()                                             {
   m_server          = new TEventLinkServer( m_proc);
   if( m_server->Open())
      m_status       = (m_status & ST_SERVER_MASK) | ST_SERVER_OPENED;
   else
      m_status       = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSED;
   return true;
}

//----------------------------------------------------------------------------
/**
   Close the connection
*/
void TEventLink::Close()                                                      {

   if( m_status & ST_CLIENT_OPENED)                                           {
      m_status = (m_status & ST_CLIENT_MASK) | ST_CLIENT_CLOSED;
      m_destBox = NULL;
   }

   if( m_server)                                                              {
      m_server->Close();
      m_status = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSED;
   }
}

//----------------------------------------------------------------------------
/**
   Check & process parameter sintax
   @return true if at least
*/
bool TEventLink::ProcessParam()                                               {

   // Param MUST be: RemoteBoxPointer
   m_clientConfigured = false;
   wchar_t  *temp = _wcsdup( m_param);
   if( wcslen( temp) == 10)
      swscanf(temp, L"0x%08x", &m_destBox);
   free( temp);

   //-- Check client part
   if ( m_destBox)
   // Client part is OK.
   m_clientConfigured = true;

   bool ret = m_serverConfigured | m_clientConfigured;
   if (ret ==false)                                                        {
       wchar_t num[64];
       swprintf( num, L"%d", (int)m_clientConfigured);
       LOG_ERROR2( L"m_clientConfigured=", num);
   }
   return ret;
}

//----------------------------------------------------------------------------
/**
   Send a msg through the TLink

   @param msg Message to send through the TLink
   @return true if the socket becomes opening, false otherwise
*/
bool TEventLink::TxMsg( LT::TMessage *msg)                                   {

   try {
//      // Check if the Tlink is open, and if we point to the right destination
      if( m_status & ST_CLIENT_OPENED && msg->IsDest( m_dest) && msg->Check() )  {
         // update Statistics
         m_numMsgTx++;
         m_bytesTx += msg->Size();
         msg->TraceSent();
         m_destBox->Push( msg);
         //As it's a shared memory link DON'T delete the message
         return true;
      }
   } catch (std::exception &exception)                                            {
      m_status             = (m_status & ST_CLIENT_MASK) | ST_CLIENT_CLOSED;
      wchar_t msg [1024];
      swprintf(msg, L"%s : %s",L"TX  EVENTLINK EXCEPTION", exception.what());
      LOG_ERROR1( msg);
   }

   return false;
}

//----------------------------------------------------------------------------
/**
   Create the needed param to communicate with this TLink.
   Useful to send between diferent TCommMngrs

   @return true if the needed param has ben created and copied on the given string. false otherwise
*/
bool TEventLink::CreateMyParam( wchar_t *destParam, int capacity)                {

   wchar_t myparam[128];
   m_server->CreateMyParam( myparam, sizeof( myparam));

   if( wcslen( myparam) < (unsigned)capacity)                                 {
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
void TEventLink::CompleteTLink( wchar_t *paramLink)                              {

   // m_param now should be empty
   // paramLink should be EVENTLINK#remoteBoxAddress
   // With the data in paramLink we will complete the paramLink to:
   // EVENTLINK#remoteBoxAddress
   if( m_param)
      free( m_param);
   m_param = _wcsdup( paramLink);

}

//wchar_t* TEventLink::GetDefaultParam(wchar_t *param, unsigned int size)           {
//      return (m_defaultParam && param && size >= sizeof( m_defaultParam)) ? wcscpy( param, m_defaultParam) : NULL;
//};

//----------------------------------------------------------------------------
/**
   Get information about the TLink

*/
void TEventLink::GetInfoTLink( wchar_t *msg, unsigned int size)          {
   // Add especific information about the TLink
   wchar_t  text[256];

   swprintf( text, L"Type of TLink: SHARED MEMORY\n");
   wcsncat( msg, text, size - wcslen( text));

   TLink::GetInfoTLink( msg, size);

   // Add server information
   if ( m_server)          {
      m_server->GetInfoTLink( msg, size);
   }
}


//----------------------------------------------------------------------------
//****************************** TEventLinkServer ****************************
//----------------------------------------------------------------------------

/**
   Constructor
   @param msgProc        TMsgProcessor class to send notifictions when anything is received
   @param	entryPoint     Parameter needed to create the TLinkServer. For UDP server sockets MUST be the port
*/
TEventLinkServer::TEventLinkServer( TCommMngr *msgProc)
                 :TLinkServer     ( msgProc, L"")                              {
   m_type     |= TL_EVENT;
   m_localBox  = NULL;
}


//----------------------------------------------------------------------------
/**
   Destructor
*/
TEventLinkServer::~TEventLinkServer()                                         {
    Close();
}

//----------------------------------------------------------------------------
/**
   pens the connection

   @return true If the server really opens, false otherwise (bad port)
*/
bool TEventLinkServer::Open()                                                 {

   // Close if it was opened
   Close();

   // CreateBox
   m_serverConfigured = true;
   wchar_t modulename[64];
   this->m_proc->GetModuleName(modulename,sizeof(modulename)/sizeof(modulename[0]));
   m_localBox = new TMessageBox(modulename);

   if(m_localBox)
      m_status    = (m_status & ST_SERVER_MASK) | ST_SERVER_OPENED;
   else
      m_status    = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSED;

   HANDLE hwd =m_localBox->GetSemaphore();
   m_proc->m_linkLocalHandleList->Push(hwd );
   return (m_status & ST_SERVER_OPENED);
}

//----------------------------------------------------------------------------
/**
   Closes the connection
*/
void TEventLinkServer::Close()                                                {
   // Stop the thread and destroy the box
   if( m_localBox)                                                            {
      delete m_localBox;
      m_localBox = NULL;
   }
   m_status   = (m_status & ST_SERVER_MASK) | ST_SERVER_CLOSED;
}

//----------------------------------------------------------------------------
/**
   Check & process parameter sintax
*/
bool TEventLinkServer::ProcessParam()                                         {
   //Nothing to do, everything it's on the TMessageBox Class
   return true;
}

//----------------------------------------------------------------------------
/**
   Create the needed param to communicate with this TLink.
   Useful to send between diferent TCommMngrs

   @return true if the needed param has ben created and copied on the given string. false otherwise
*/
bool TEventLinkServer::CreateMyParam( wchar_t *destParam, int capacity)          {
   if( m_localBox)                                                            {
      wchar_t myparam[128];

      swprintf( myparam, L"EVENTLINK#0x%08x", m_localBox);

      if( wcslen( myparam) < (unsigned)capacity)                              {
         wcscpy( destParam, myparam);
         return true;
      }
   }
   return false;
}



//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
