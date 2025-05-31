//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TLinkFactory.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#pragma hdrstop

#include "TLinkFactory.h"
#include "TComm.h"
#include "TTCPSocketLink.h"

#pragma package(smart_init)

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                              TLinkFactory
//------------------------------------------------------------------------------

/**
   Default Constructor
*/
TLinkFactory::TLinkFactory( TCommMngr *commMngr)            {

   if( commMngr == NULL )  {
      LOG_ERROR1( L"Bad Parameters");
      return;
   }
   m_commMngr     = commMngr;
}

//----------------------------------------------------------------------------
/**
   Destructor
*/
TLinkFactory::~TLinkFactory()                                                 {
}

//----------------------------------------------------------------------------


/**
   TLinkBase creator

      Param Format: LINKTYPE:Param
      Examples:
      TCPSOCKET#192.168.1.11:1000

      TCPSOCKET:     Complete TLink using TCP sockets
      192.168.1.11:  Remote IP
      1000:          Remote Port to sent data

      TCPSERVER#1000
      TCPSERVER:     TLinkServer using TCP sockets
      1000:          Local Port to listen
*/
TLinkBase *TLinkFactory::CreateTLink( wchar_t *dest, const wchar_t *param)     {

   wchar_t moduleName[32];
   m_commMngr->GetModuleName(moduleName,sizeof(moduleName)/sizeof(moduleName[0]));
   // We make a copy to avoid modifications on the original string
   wchar_t *paramCopy = _wcsdup( param);
   if( paramCopy)                                                             {
      wchar_t *p = wcstok( paramCopy, L"#");
      TLinkBase * ret = NULL;
      if( p && !wcscmp( p, L"TCPSOCKET"))                              {
         p = wcstok( NULL, L"\0");

         static __int64 timeFail = 0;
         __int64 t = LT::Time::GetTimeMs();
         if ( t - timeFail > LOG_RECONNECTION_TIME ) {
            LOG_INFO4( L"TCPSocket Created From ",moduleName, L"to destination: ", dest);
            timeFail = t;
         }

         ret = new TTCPSocket( m_commMngr, dest, p);
         free( paramCopy);
      }
      else if( p && !wcscmp( p, L"TCPSERVER"))                              {
         p = wcstok( NULL, L"\0");
         LOG_INFO4( L"TCPServer Created From ",moduleName, L"to destination: ", dest);
         ret = new TTCPServerSocket( m_commMngr, p);
         free( paramCopy);
      }
      return ret;
   }
   return NULL;
}

//----------------------------------------------------------------------------
/**
   Process the data of a MSG_RESPONSE_CONNECT, and creates a new TLink
*/
TLink *TLinkFactory::ResponseConnect( wchar_t *moduleOriginName, wchar_t *IPOrigin,
                                      const wchar_t *data, int dataSize,
                                      int linkNumber)                         {

   wchar_t  *msgData             = new wchar_t[ dataSize+1];
   memcpy( msgData, data, dataSize);
   int length = wcslen(data);
   msgData[dataSize] = 0;
   if (length < dataSize)
      msgData[length] = 0;

   wchar_t          paramLink[64];
   wchar_t  *linkType            = wcstok( msgData, L"#"),
         *receivedParamLink   = wcstok( NULL, L"#");

   TLink *link ;
   wchar_t moduleName[addressSize+1];

   m_commMngr->GetModuleName( moduleName, sizeof( moduleName)/sizeof( moduleName[0]));

   {
      // For TCP Socket Create the client part, with the given data.
      // jump the ':' before the port number
      receivedParamLink++;
      swprintf( paramLink, L"%s#%s:%s", linkType, IPOrigin, receivedParamLink);
      link = (TLink *)CreateTLink( moduleOriginName, paramLink);
      link->OpenClientPart();
   }

   delete  [] msgData;

   return link;
}

//---------------------------------------------------------------------------
/**
   Process the data of a MSG_REQUEST_CONNECT, and creates a new TLink

  //TODO: Cuando se añada versión del módulo, comprobar, y si no cuadra responder mensaje especial

   // Data MUST be:
   // "DefaultLinkType#::port#RemotePID#MessageType".
   // Rules to apply:
   // 1st: If both TCommMngr has tha same PID, create a TEventLink socket
   // 2nd: If  PID is different and the type of desired messages is short, create a TUDPSocketLink     NO LONGER IN USE
   // 3d : Otherwise create TCPSocketLink to communicate
*/
TLink *TLinkFactory::RequestConnect( wchar_t *moduleOriginName, wchar_t *IPOrigin,
                                     const wchar_t *data, int dataSize,
                                     int linkNumber)                          {


   LOG_INFO4( L"Request Connect moduleOriginName=", moduleOriginName, L" IPOrigin", IPOrigin);
   wchar_t          *msgData              = new wchar_t[ dataSize+1],
                  IPPort[32],
                  defaultParamLink[ 64],
                  paramLink[ 64],
                 *p;
   DWORD          RemotePID            = 0,
                  LocalPID             = GetCurrentProcessId();
   e_messageType  messageType;
   TLink         *returnLink,
                 *tempLink;
   LT::TMessage *reMsg                = NULL;
   wchar_t moduleName[addressSize+1];
   m_commMngr->GetModuleName( moduleName, sizeof( moduleName)/sizeof(moduleName[0]));


   memcpy( msgData, data, dataSize);
   msgData[dataSize] = 0;

   // First param: Must be the the default link type. It MUST be TCPSOCKET
   p = wcstok( msgData, L"#");

   if( p && !wcscmp( p, L"TCPSOCKET"))                                         {

      // Second param Must be "::port" of the default TCP server
      p = wcstok( NULL, L"#");

      if( (p+2) && _wtoi( p+1))                                                {

         // Also, we need the IP to create a Tlink with sockets
         wcscpy( IPPort, IPOrigin);
         wcscat( IPPort, L":");
         wcscat( IPPort, p+1);
         //IPPort now should be "IP:port"

         // 3d param must be "RemotePID"
         p = wcstok( NULL, L"#");
         if( wcslen( p) == 10)
            swscanf(p, L"0x%08x", &RemotePID);

         // 4th param must be desired message type
         p = wcstok( NULL, L"#");
         messageType = (e_messageType)_wtoi( p);

         // Create a temporal TCPSOCKET link to the default TCP server on the other module
         swprintf( defaultParamLink, L"TCPSOCKET#%s", IPPort);
         tempLink = (TLink *)CreateTLink( moduleOriginName, defaultParamLink);
         tempLink->OpenClientPart();

         //-- No optimization -> TTCPSocketLink
         // On this case, the temporary Link becomes the definitive TLink
         // just add to the TLink's list
         returnLink = tempLink;

         m_commMngr->CreateDefaultServerParam( paramLink, sizeof( paramLink));

         // Create the response message in order to the other side creates their client part
         reMsg   = new LT::TMessage( MSG_COMM_RESPONSE_CONNECT, moduleName, moduleOriginName, PRIOR_NORMAL, (char*)paramLink, wcslen( paramLink)*sizeof(wchar_t));

      }
      else                                                                    {
         // bad port number. return
         delete [] msgData;
         return NULL;
      }
   }
   else                                                                       {
      // bad default TLink type. return NULL;
      delete [] msgData;
      return NULL;
   }

   delete [] msgData;

   // Set on the definitive TLink the default param. Useful to resolve IP:Port
   // when other modules send us MSG_GET_ADDRESS_BY_NAME messages
   returnLink->SetDefaultParam( defaultParamLink);

   // Send via the temporalLink the data to create the definitive Link on the other TCommMngr
   //reMsg->SetMsgNumber( m_commMngr->IncMessageNumber());
   m_commMngr->SetMessageNumber( reMsg );
   tempLink->TxMsg( reMsg);

   // Close & Delete tempLink if not needed anymore
   if( returnLink != tempLink)                                                {
      tempLink->Close();
      delete tempLink;
   }

   // return the definitive TLink
   return returnLink;
}

//----------------------------------------------------------------------------
/**
   Process the response of a MSG_RESPONSE_CONNECT, and completes the existing TLink
*/
void TLinkFactory::ResponseConnect2( TLink *existingTLink, wchar_t *IPOrigin,           //vic: no longer needed
                                     const wchar_t *data, int dataSize)          {
   wchar_t *msgData             = new wchar_t[ dataSize+1];
   memcpy( msgData, data, dataSize);
   msgData[dataSize] = 0;
   int length = wcslen(data);
   if (length < dataSize)
      msgData[length] = 0;

   wchar_t *linkType         = wcstok( msgData, L"#"),
        *receivedParamLink   = wcstok( NULL, L"#"),
        realParamLink[64];

   if( !wcscmp( linkType, L"UDPSOCKET") || !wcscmp( linkType, L"TCPSOCKET"))
      // Add the remote IP to the paramLink for sockets
      swprintf( realParamLink, L"%s%s", IPOrigin, receivedParamLink);
   else
      wcscpy( realParamLink, receivedParamLink);


   existingTLink->CompleteTLink( realParamLink);
   existingTLink->OpenClientPart();


   LOG_INFO1(L"CONNECTION COMPLETED.");
   delete [] msgData;
}






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
