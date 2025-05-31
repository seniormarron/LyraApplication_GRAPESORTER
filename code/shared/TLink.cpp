//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TLink.cpp
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

#include "TLink.h"
#include "TAux.h"
//----------------------------------------------------------------------------
/**
   Constructor
*/
TLinkBase::TLinkBase( TCommMngr *proc, wchar_t *param)                           {

   m_proc            = proc;
   m_status          = ST_NONE;
   if (param)
      m_param        = _wcsdup( param);
   else
      m_param        = NULL;
   m_defaultParam = NULL;
}

//----------------------------------------------------------------------------
/**
   Destructor
*/
TLinkBase::~TLinkBase()                                                       {
   if( m_param)
      free( m_param);
   if (m_defaultParam)
      free(m_defaultParam);
}

// OJO CON ESTA FUNCIÓN, LOS FLAGS DE CERRADO, ABIERTO, ABRIENDO NO TIENEN SENTIDO PUESTO QUE NO SE ESTAN ACTUALIZANDO CORRECTAMENTE
bool TLinkBase::IsClosed()   {
    return ( ((m_status & ST_SERVER_MASK) | ST_SERVER_CLOSED) |
             ((m_status & ST_CLIENT_MASK) | ST_CLIENT_CLOSED)  );
}

//----------------------------------------------------------------------------
/**
   Constructor
*/
TLinkServer::TLinkServer( TCommMngr *proc, wchar_t *entryPoint)
            :TLinkBase  ( proc, entryPoint)                                   {

   m_type                      =     TL_SERVER;
   m_status                   |=     ST_SERVER_CLOSED;
   m_serverConfigured          =     false;

   // Reset Statistics
   m_numMsgRx                  =     0;
   m_bytesRx                   =     0;
}

//----------------------------------------------------------------------------
/**
   Get information about TLink

   @return a char * with the status information about the TLink.
*/
void TLinkServer::GetInfoTLink( wchar_t *msg, unsigned int size)         {

   wchar_t                    text[256];
   wchar_t                    cfg[64];
   wchar_t                    status[64];
   unsigned int               realSize;

   memset( text,  0x00, sizeof( text));
   memset( cfg,   0x00, sizeof( cfg));
   memset( status,0x00, sizeof( status));

   //Status
   switch( m_status & ~ST_SERVER_MASK)                                        {
      case ST_SERVER_CLOSED:
         swprintf( status, L"CLOSED");
         break;
      case ST_SERVER_OPENING:
         swprintf( status, L"OPENING");
         break;
      case ST_SERVER_OPENED:
         swprintf( status, L"OPENED");
         break;
      case ST_SERVER_CLOSING:
         swprintf( status, L"CLOSING");
         break;
      default:
         swprintf( status, L"UNKNOWN");
   }

   swprintf( cfg, L"%s (%s)", m_param, m_serverConfigured ? L"OK" : L"NOT OK");

   snwprintf( text, SIZEOF_CHAR(text)-1, L"Server Configuration: %s\nServer Status: %s\nRX: %d messages (%d bytes)\n", cfg, status, m_numMsgRx, m_bytesRx);
   wcsncat( msg, text, size - wcslen( msg));

}

//----------------------------------------------------------------------------
/**
   Constructor
*/
TLink::TLink      ( TCommMngr *msgProc, wchar_t *dest, wchar_t *param)
      :TLinkBase  ( msgProc, param)                                           {

   m_type                 =   TL_CLIENT;
   m_status               |=  (ST_CLIENT_CLOSED | ST_SERVER_CLOSED);

   m_serverConfigured      =  m_clientConfigured = false;

   // Copy module dest name
//   strncpy( m_dest, dest, sizeof( m_dest));
   wcsncpy( m_dest, dest, sizeof( m_dest)/sizeof(m_dest[0]));
   m_dest[ sizeof( m_dest)-1] = '0';   // needed if dest is larger than sizeof( dest)

   // Reset Statistics
   m_numMsgTx              =     0;
   m_bytesTx               =     0;

   m_last_sent_time = timeGetTime();
   m_last_received_time = timeGetTime();
}

//----------------------------------------------------------------------------
/**
   Get default parmeter.
*/
wchar_t *TLink::GetDefaultParam(  wchar_t *param, unsigned int size)                {

   if( m_defaultParam && wcslen( m_dest)  + wcslen( m_defaultParam) + 2 < size){
      memset( param, 0x00, size*sizeof(wchar_t));
      wcscpy( param, m_dest);
      wcscat( param, L"#");
      wcscat( param, m_defaultParam);
      return param;
   }
   return NULL;
}

//----------------------------------------------------------------------------
/**
  Get information about TLink.
  @param size is number of characters
*/
void TLink::GetInfoTLink( wchar_t *msg, unsigned int size)               {

   wchar_t                 text[256];
   wchar_t                 cfg[64];
   wchar_t                 status[64];
   unsigned int            realSize;

   memset( text,  0x00, sizeof( text));
   memset( cfg,   0x00, sizeof( cfg));
   memset( status,0x00, sizeof( status));

   // Configuration
   snwprintf( cfg, SIZEOF_CHAR(cfg)-1, L"%s (%s)", m_param, m_clientConfigured ? "OK" : "NOT OK");

   //Status
   switch( m_status & ~ST_CLIENT_MASK)                                        {
      case ST_CLIENT_CLOSED:
         swprintf( status, L"CLOSED");
         break;
      case ST_CLIENT_OPENING:
         swprintf( status, L"OPENING");
         break;
      case ST_CLIENT_OPENED:
         swprintf( status, L"OPENED");
         break;
      case ST_CLIENT_CLOSING:
         swprintf( status, L"CLOSING");
         break;
      default:
         swprintf( status, L"UNKNOWN");
   }

   snwprintf( text,SIZEOF_CHAR(text)-1, L"Dest. Module: %s\nClient Configuration %s\nClient Status: %s\nTX: %d messages (%d bytes)\n",
                  m_dest, cfg, status, m_numMsgTx, m_bytesTx);

   wcsncat( msg, text, size - wcslen( msg));

}
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
/*  Check is link has expired  due to received time
*  @param TIMEOUT  in milliseconds
*  @return true is the link has expired, false otherwise
*/
bool    TLink::ExpiredReceivedTime(UINT timeout)                                   {

//   if( m_status & ST_CLIENT_CLOSED) {  //vic: we now assume that if the link is closed -> the link has expired so the application layer can detect "disconnections"
//      LOG_INFO1(L"Client Link is closed");
////      return true;
//   }
//
//   if( m_status & ST_SERVER_CLOSED) {  //vic: we now assume that if the link is closed -> the link has expired so the application layer can detect "disconnections"
//      LOG_INFO1(L"Server Link is closed");
////      return true;
//   }

   unsigned int t_time,
                diff,
                current_time;

   t_time =  (m_last_received_time > 0) ? m_last_received_time : 0;
   if (t_time > 0) {
      current_time = timeGetTime()   ;
      diff = ( current_time - t_time)   ; //milliseconds diff
      if ( diff < 0 )   {
         UpdateReceivedTime(); // if time is invalid we set a new previous one and we return non expired
         return false;
      }
      if (diff >= (timeout))     { // true if time exceeded, msg expired
//         wchar_t date[256];
//         swprintf( date, L"ExpiredReceivedTime lastRecvtime %u diff %u timeout %u", m_last_received_time,diff,timeout);
//         LOG_INFO1(date);
         return true;
      }
   }
   #ifdef _DEBUG
//   LOG_WARN1( L"The link has not expired");
   #endif
   return false;
}

//---------------------------------------------------------------------------
/*  Check is link has expired due to sent time
*  @param TIMEOUT  in milliseconds
*  @return true is the link has expired, false otherwise
*/
bool    TLink::ExpiredSentTime(UINT timeout)                                   {

//   if( m_status & ST_CLIENT_CLOSED) {  //vic: we now assume that if the link is closed -> the link has expired so the application layer can detect "disconnections"
//      LOG_INFO1(L"Client Link is closed");
////      return true;
//   }
//
//   if( m_status & ST_SERVER_CLOSED) {  //vic: we now assume that if the link is closed -> the link has expired so the application layer can detect "disconnections"
//      LOG_INFO1(L"Server Link is closed");
////      return true;
//   }


   unsigned int t_time,
                diff,
                current_time;

   t_time =  (m_last_sent_time > 0) ? m_last_sent_time : 0;
   if (t_time > 0) {
      current_time = timeGetTime()   ;
      diff = ( current_time - t_time)   ; //milliseconds diff
      if ( diff < 0 )   {
          UpdateSentTime(); // if time is invalid we set a new previous one and we return non expired
          return false;
      }
      if (diff >= (timeout))     { // true if time exceeded, msg expired
         return true;
      }
   }
   #ifdef _DEBUG
//   LOG_WARN1( L"The link has not expired");
   #endif
   return false;
}

 //---------------------------------------------------------------------------
/*  Update the receive time of this link
*/
void    TLink::UpdateReceivedTime(){

   m_last_received_time = timeGetTime()  ;   //ms
//   wchar_t date[128];
//   swprintf( date, L"UpdateReceivedTime %u",m_last_received_time);
//   LOG_INFO1(date);

}

 //---------------------------------------------------------------------------
/*  Update the sent time of this link
*/
void    TLink::UpdateSentTime(){

   m_last_sent_time = timeGetTime()   ;

}




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------




