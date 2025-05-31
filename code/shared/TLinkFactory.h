//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TLinkFactory.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TLinkFactoryH
#define TLinkFactoryH

#include "TLink.h"

class TCommMngr;

//------------------------------------------------------------------------------

/// Message type optimization
typedef enum                                                                  {
    MESSAGETYPE_LARGE = 0,    ///< Possible large messages -> TCPLink prefered if different procces
    MESSAGETYPE_SHORT         ///< Short messages -> UDPLink prefered if different procces
} e_messageType;

// TLinkFactory
/// Interface used to create correct TLink types, from the "param" parameter
class TLinkFactory                                                            {
 public:
   int         m_portBase;                                  ///< Base number to create socket servers
   TCommMngr  *m_commMngr;                                  ///< Message Processor for the TLinks
   TLinkFactory( TCommMngr *msgProc);                       ///< Default Constructor
   ~TLinkFactory();                                         ///< Destructor
   TLinkBase *CreateTLink  ( wchar_t *dest, const wchar_t *param);        ///< Creates a complete Tlink from a paramLink
   TLink *ResponseConnect  ( wchar_t *moduleOriginName,  wchar_t *IPOrigin, const wchar_t *data, int dataSize, int linkNumber);  ///< Process the data of a MSG_RESPONSE_CONNECT, and creates a new TLink
   TLink *RequestConnect   ( wchar_t *moduleOriginName,  wchar_t *IPOrigin, const wchar_t *data, int dataSize, int linkNumber);   ///< Process the data of a MSG_REQUEST_CONNECT, and creates a new TLink
   void ResponseConnect2   ( TLink *existingTLink,       wchar_t *IPOrigin, const wchar_t *data, int dataSize);   ///< Process the response of a MSG_RESPONSE_CONNECT, and completes the existing TLink

};

#endif






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
