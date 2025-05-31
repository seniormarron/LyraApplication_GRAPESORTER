//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TCommInterface.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TCommInterfaceH
#define TCommInterfaceH

#include "TDataMng.h"
#include "TDataInterface.h"
#include "TSubscriber.h"
#include "TComm.h"
#include "TMessageProcessor.h"

//----------------------------------------------------------------------------
//********************************  TCommInterface Class  ********************
//----------------------------------------------------------------------------
/**
   Class TCommInterface adds communication funcionality to standard TDataInterface. It supports:
   Param subscption from both local and remote modules to get change notifications
   Inherited TDataInterface to work as a param server to other modules
*/
/// Class TCommInterface adds communication funcionality to standard TDataInterface.
class TCommInterface : public TDataInterface, public TMessageProcessor         {

  protected:
   TCommMngr*                    m_cmngr;                      ///< Access to manager for comunications
   TSubscriberList               m_subsList;                   ///< Subscribers list

  public:

   TListWithMutex<TSubscriber *> m_avoidRebList;               ///< List to avoid rebotes, suposing only one thread processing messages


   TCommInterface( TDataManager *mngr, TCommMngr *cmngr);       ///< Constructor for Interface
   virtual ~TCommInterface();                                   ///< Destructor for Interface

  protected:

   bool NotifySubscriber( TNode *tdata, const TSubscriber *subs, bool recursive );                       ///Notify to local subscriber a subscription from remote subscriptor
   bool NotifySubscriberNodeErased( const char *fullName, const TSubscriber *subs, bool &completeMatch  );   ///<
   bool SubscribeInner( const wchar_t *oriFullName, TNode *tLocalNode,
                           e_priority priority,
                           int maskFlagsAnd,
                           int maskFlagsOr,
                           e_typeMsg type);                   ///<   subscribe a localNode to a remote or local Node


  public:
   bool Subscribe( const wchar_t *oriFullName, TNode *tLocalNode,
                           e_priority priority  = PRIOR_NORMAL,
                           int maskFlagsAnd     = 0xFFFFFFFF,
                           int maskFlagsOr      = 0x00000000);                   ///<   subscribe a localNode to a remote or local Node

   bool SubscribeWithoutNotification( const wchar_t *oriFullName, TNode *tLocalNode,
                           e_priority priority  = PRIOR_NORMAL,
                           int maskFlagsAnd     = 0xFFFFFFFF,
                           int maskFlagsOr      = 0x00000000);                   ///<   subscribe a localNode to a remote or local Node

   bool Unsubscribe( TNode *tLocalNode);                                         ///<
   virtual TData *GetCpyDataObject(char *path);                                  ///< Pide a interfaces remostos el sdata y crea Tdatas copia
   virtual TDataManager *GetCpyDataManager( char *_name, bool recursive = true); ///< Pide a interfaces remosto y crea TdataManagers copia

   bool UnsubscribeAll();                                                        ///<
   bool DeleteSubscriptionsFromDest(wchar_t * dest);                             ///< deletes all subscriptions corresponding to dest (without sending any unsubscribe message)
   bool DeleteAllSubscriptions();                                                ///< deletes all subscriptions (without sending any unsubscribe message)

   virtual int  GetDataAsText(char *path, char *text, int size);                 ///<
   virtual bool SetDataAsText(char *path, char *text);                           ///<
   virtual bool SetDataAsSData(char *path, sdata *sda);                          ///<

   virtual void Add( TDataManager  *mngr);

   bool  Refresh( TDataManager *mn, TDataInterface *intf, bool forceUpdate, bool recursive );                                    ///< Sends modificatins on a manager, with posibility of doing it recursively

   virtual bool Refresh          ( TNode *node, TDataInterface *intf,        bool forceUpdate = true);         ///<
   virtual bool RefreshInterface ( TNode *node);         ///<
   virtual bool   RefreshAllInterface(TDataManager *mn);

   // Se llama desde el TDatamanager para comminterface y viewinterface, las dos la tienen que tener
   virtual bool SetViewErased( const char *fullName,              TDataInterface *intf = NULL );                                 ///<


   virtual bool ProcMsg( LT::TMessage *msg);                                                                                    ///<
   bool GetInfoSubs( wchar_t *str, int size);                                                                                       ///<Gets information from subscribers into str

   virtual void   SetEnabled     ( bool enabled);                                ///< Sets the enabled flag to parameter value

 // -- inline methods
  protected:
   //-----------------------------------------------------------------------------
  /**
      Adds a new Subscriber to the list of subscribers
      @param subs subscriber
   */
   bool AddSubscriber( TSubscriber *subs)                                     {

      return m_subsList.Add( subs);
   }

   //-----------------------------------------------------------------------------
   /**
      param localName: entry to be delted in m_subsList
      param remoteName return the remoteNmae matching localName in m_subsList  (a duplicate)
      return true if localName exists in m_subsList
   */
   bool DelSubscriptionByLocalName( wchar_t *localName, wchar_t *&remoteName)        {
      return m_subsList.DeleteByLocalName( localName, remoteName);
   }

};


#endif





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
