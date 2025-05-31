//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TEventLink.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TEventLinkH
#define TEventLinkH

#include <windows.h>
#include "tlink.h"
#include "TMessageBox.h"

//---------------------------------------------------------------------------

//-- Tlink's Types & subtypes:
// Most significant bit indicates if the tlink correspond to a local tlink (both modules share memory context)
// Next most significant 7 bits, are type of TLink implementation (socket, cue, events,....)
// Less significant 2 bytes, are TLink type (Client, Server,...)
enum                                                                          {
   TL_EVENT = 0x8200
};
//---------------------------------------------------------------------------

class TEventLinkServer;

// s_teventThread
/// Structure with the thread's necessary data
/**
   see EventThread
*/
//struct s_teventThread                                                         {
//   HANDLE                     handleThread;                    ///< Handle to manage the thread
//   TMessageBox                *localBox;                       ///< Pointer to the message's box
//   TCommMngr                  *proc;                           ///< Pointer to the message processor
//   TEventLinkServer           *link;                           ///< Pointer to the Link Server
//};
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//**************************** TEventServerLink ********************************
//------------------------------------------------------------------------------
/// TLink implementacion, to communicate using Windows system events

/**
   To instance and open a TEventServer, we need local list name
*/
class TEventLinkServer : public TLinkServer                                   {

 public: //todo: cambiar esto a protected cuando funcione

   TMessageBox               *m_localBox;                  ///< Local message's box.

 public:

   // Constructor & destructor methods
   TEventLinkServer(TCommMngr *msgProc);                     ///< Constructor.
   virtual ~TEventLinkServer();                             ///< Destructor.

    // TLink & TLinkBase inheritance
   virtual bool Open();                                     ///< Opens the conecction.
   virtual void Close();                                    ///< Closes the connection.
   virtual bool ProcessParam();                             ///< Check & process parameter sintax.
   /// Create the needed param to communicate with this TLink. Useful to send between diferent TCommMngrs.
   virtual bool CreateMyParam( wchar_t *param, int capacity);

   //  friend EventThread(LPVOID);
};




//----------------------------------------------------------------------------
//****************************** TEventLink **********************************
//----------------------------------------------------------------------------

// TEvent
/// TLink implementacion, to communicate using Windows system events
/**
To instance and open a TEvent TLink, we need:
    - Module destination name,
    - Pointer to the messages list and the name of the event to signal when
   there is new information, available in the list
   - (Optional) Local entryPoint (name of the local event to signal)
*/
class TEventLink : public TLink                                             {

 protected:
    //-- Protected members
   TMessageBox               *m_destBox;                             ///< Destination message's box.
   TEventLinkServer          *m_server;                              ///< Server part of the link.

   virtual bool ProcessParam();                                      ///< Check & process parameter sintax

 public:

   //-- Constructor & destructor
   TEventLink( TCommMngr *msgProc, wchar_t *dest, wchar_t *param);        ///< Constructor.
   virtual ~TEventLink();                                           ///< Destructor.

   // TLink & TLinkBase inheritance
   virtual bool OpenClientPart();                                   ///< Open the client part of the conecction
   virtual bool OpenServerPart();                                   ///< Open the server part of the conecction
   virtual void Close();                                            ///< Close the connection
   virtual bool TxMsg( LT::TMessage *_msg);                        ///< Send a msg through the TLink

   virtual bool CreateMyParam( wchar_t *param, int capacity);           ///< Create the needed param to communicate with this TLink. Useful to send between diferent TCommMngrs
   virtual void CompleteTLink( wchar_t *paramLink);                     ///< Complete TLink
//   virtual inline wchar_t *GetDefaultParam(wchar_t *param, unsigned int size);   ///< Get the param to create the Tlink. Useful to send to other TCommMngr's.
   virtual void GetInfoTLink( wchar_t *msg, unsigned int size); ///< Returns a char * with the status information about the TLink
   virtual void ProcessMsgReceived();                                ///< Prosses received messages.

   // -- inline methods

   //--------------------------------------------------------------------------
   /**
      GetSemaphoreBox
   */
   virtual HANDLE GetSemaphoreBox()                                           {
      return m_server->m_localBox->GetSemaphore();
   }

};
//------------------------------------------------------------------------------
#endif



//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
