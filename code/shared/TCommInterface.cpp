//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TCommInterface.cpp
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

#include "TCommInterface.h"
#define COMM_BUFFER_SIZE 1024

#pragma package(smart_init)

// ---------------------------------------------------------------------------
/**
 Constructor

 @param   mngr  Pointer to a TDataManager where the interface is added
 @param   cmngr Pointer to a TCommMngr, used to communicate with other modules
 */
TCommInterface::TCommInterface(TDataManager *mngr, TCommMngr *cmngr)
    : TDataInterface(mngr), m_avoidRebList(L"avoidReb") {

   m_cmngr = cmngr;
   m_deleteme = false;
   m_avoidRebList.ClearList();

   cmngr->SetCommInterface(this);
}

// ---------------------------------------------------------------------------
/**
    Default Destructor
 */
TCommInterface::~TCommInterface() {

   m_avoidRebList.ClearList();

}

// ----------------------------------------------------------------------------
/**
 */
bool TCommInterface::RefreshInterface(TNode *tdata)                           {
   return false;       //ToDo: implementar
}
// ----------------------------------------------------------------------------
/**
 */
bool TCommInterface::RefreshAllInterface(TDataManager *mn)                    {
   return false;      //ToDo: implementar
}

// ---------------------------------------------------------------------------
/**
 */
TData *TCommInterface::GetCpyDataObject(char *path) {
   return NULL;
}

// ---------------------------------------------------------------------------
/**
 Pide a interfaces remotas y crea TdataManagers copia
 */
TDataManager *TCommInterface::GetCpyDataManager(char *_name, bool recursive) {
   return NULL;
}

// ---------------------------------------------------------------------------
/**
 */
int TCommInterface::GetDataAsText(char *path, char *text, int size) {
   return -1;
}

// ---------------------------------------------------------------------------
/**
 */
bool TCommInterface::SetDataAsText(char *path, char *text) {
   return false;
}

// ---------------------------------------------------------------------------
/**
 */
bool TCommInterface::SetDataAsSData(char *path, sdata *sda) {
   return false;
}

#include <iostream> //for std::wcerr
// ----------------------------------------------------------------------------
/**
 Process Message

 @param   msg   Message to be processed
 */
bool TCommInterface::ProcMsg(LT::TMessage *msg) {

   if ( !m_enabled )
      return false;

   bool proc = false;
   bool notifySubscription = true;
   switch (msg->Type()) {

   case MSG_COMM_IFC_SUBSCRIBE_NO_NOTIFY :
      notifySubscription = false;
   case MSG_COMM_IFC_SUBSCRIBE: {
//          LOG_INFO1(L"TCommInterface::ProcMsg MSG_COMM_IFC_SUBSCRIBE");
         // add a new entry in m_subsL list
         const char *mens = (char*) msg->DataAsRaw();
         int maskFlagsAnd = *(int *)mens;
         int maskFlagsOr = *(int *)(  mens +sizeof(int));
         const wchar_t *remote = (const wchar_t*)( mens + sizeof(int) * 2);
         int dataSize = (msg->DataSize())/sizeof(wchar_t);

         // check message is correct:
         if (dataSize < (int)(addressSize +  sizeof(int)))    {
            LOG_ERROR1(L"Size in MSG_SUBSCRIBE");
            break;
         }
         if ( ((const wchar_t *)mens)[dataSize - 1] != L'\0' && ((const wchar_t *)mens)[dataSize - 1] != L'\n' )  {
            LOG_ERROR1(L"Size in MSG_SUBSCRIBE, unexpected end of message");
            break;
         }


         // data points to a null terminated string, who contains remote name
         const wchar_t *local = remote + wcslen(remote)+ 1;
         if (local > ((const wchar_t *)mens) + dataSize) {
            LOG_ERROR1(L"Size in MSG_SUBSCRIBE");
            break;
         }
         int msgn = msg->GetMsgNumber();

         wchar_t buffer[2048];
         buffer[2047] = 0;
         snwprintf( buffer, 2047, L" Received subscribe to local: %s with message number: %d", local , msg->GetMsgNumber());
         LOG_INFO1(buffer);

         TSubscriber *subs = new TSubscriber(local, remote, msg->GetPriority(),
             maskFlagsAnd, maskFlagsOr);

         bool added = m_subsList.Add(subs);

         if ( notifySubscription )  {
            // Notify subscribers
            std::list<TDataManager*>::iterator i2;
            for (i2 = m_listDataMngr.begin(); i2 != m_listDataMngr.end(); i2++) {
               TDataManager *mn = *i2;
               if (mn) {
                  TNode *node = mn->GetNode(local);
                  if (node)
                     NotifySubscriber(node, subs, true);
               }
            }
         }
         else  {
//            LOG_INFO2(L"subscripcion sin notificar de nodo remoto:",remote);
            TNode *node;
            std::list<TSubscriber *>::iterator i;
            std::list<TSubscriber *> *list =m_subsList.GetList();
            wchar_t * localName = _wcsdup(local);
            for (i=list->begin(); i!=list->end(); i++)                        {
               if ( (*i)->GetLocalMatchRemoteName(  remote, localName,  wcslen(localName)+1) ) {       //ToDo, ojo al +1 pero dentro hace falta por comprobacion
                  std::list<TDataManager *>::iterator i2;
                  for ( i2 = m_listDataMngr.begin(); i2!= m_listDataMngr.end(); i2++)    {
                     TDataManager *mngr = *i2;
                     node = mngr? mngr->GetNode ( localName ) : NULL;
                     if ( node)                                               {
                        std::wstring fullname;
                        node->GetFullNameAsWString(fullname);
//                        LOG_INFO2(L"subscripcion sin notificar en nodo:", fullname.c_str());
                        //ToDo should we check if the interface is allowed to change the tdata value?
                        //What about the checks that viewinterface does using the rdonlystatus flag of the tdata?
                        //what about if I'm the server part? and if I'm the client?
                        LT::TStream *stream = msg->GetTStream();
                        if (stream)                                           {

                           //avanzamos el stream lo leido que son los dos ints de flags y los nombres local y remoto
                           int readPos = stream->posRead(); //save the
                           stream->SeekRead( readPos +
                                             sizeof(int) * 2 +
                                             (wcslen(remote)*sizeof(wchar_t) ) +
                                             (wcslen(local)*sizeof(wchar_t) ) + 2*sizeof(wchar_t) );

                           //leemos datos también incluidos en el tdata
                           // no los usamos para nada, podriamos comprobar nombre en todo caso
                           wchar_t buffer[COMM_BUFFER_SIZE];

                           //read keyword in line ( Data or DataMnger )
                           int   size;
                           msg->ReadLn(buffer , COMM_BUFFER_SIZE - 1, size);
                           //read remote fullname in next line
                           msg->ReadLn( buffer, COMM_BUFFER_SIZE - 1, size);
                           //remove new line char
                           buffer[ COMM_BUFFER_SIZE - 1 ]=0;
                           //look for local name in subscriber table
                           wchar_t localName[COMM_BUFFER_SIZE ];
                           //Read and discard line containing code of TData or TDataManager
                           unsigned int code;
                           if ( msg->Read( (char *)&code, sizeof( code))!= (int)sizeof(code))
                              return false;
                           msg->ReadLn( localName, COMM_BUFFER_SIZE - 1 , size); //this is doing nothing, the local(or locals) name is get after using GetLocalMatchRemoteName over the buffer (which has the remote name)

                           m_avoidRebList.Push( *i);
                           readPos = stream->posRead(); //save the
                           if( node->Read( stream, this ) == false )    {
                              LOG_INFO2(L"Fallo al leer el dato de la sub sin notificar del nodo: ", fullname.c_str());
                           }
                           stream->SeekRead( readPos);
                           m_avoidRebList.PopThis( *i);
                           proc = true;
                        }
                     }
                  }
               }
            }
            free( localName );

         }
         if (! added )  {           //if we could not add the subscription, we delete it
            delete subs;
         }

         proc = true;
      }
      break;

   case MSG_COMM_IFC_UNSUBSCRIBE: {
         LOG_INFO1(L"  TCommInterface::ProcMsg MSG_COMM_IFC_UNSUBSCRIBE");
         // This message contains one only field: the name of the remote data to unsuscribe
         const wchar_t *remote = (wchar_t*) msg->DataAsRaw();
         int dataSize = msg->DataSize();

         // check message is correct:
         if ((dataSize <= addressSize) || (remote[dataSize - 1] != '\0'))
            break;
         proc = m_subsList.DeleteByRemoteName(remote);
      } break;

   case MSG_COM_CLIENTLINK_NOT_RESPONDING:   {
         LOG_INFO1(L"  TCommInterface::ProcMsg MSG_COM_CLIENTLINK_NOT_RESPONDING");
//         wchar_t  dest[addressSize];
//         msg->GetDest(dest,addressSize);
//         DeleteSubscriptionsFromDest(dest);
         proc = true;

   }
      break;

   case MSG_COMM_SERVER_NOT_RESPONDING:   {
//      LOG_INFO1(L"  TCommInterface::ProcMsg MSG_COMM_SERVER_NOT_RESPONDING");
//      wchar_t  dest[addressSize];
//      msg->GetDest(dest,addressSize);
//      DeleteSubscriptionsFromDest(dest);
//      proc = true;

   }
      break;

   case MSG_COMM_IFC_NOTIFY_SUBSCRIBERS: {
//         LOG_INFO1(L"  TCommInterface::ProcMsg MSG_COMM_IFC_NOTIFY_SUBSCRIBERS");
         //get remote name from message
         wchar_t buffer[COMM_BUFFER_SIZE];

         //read keyword in line ( Data or DataMnger )
         int   size;
         msg->ReadLn(buffer , COMM_BUFFER_SIZE - 1, size);
         //read remote fullname in next line
         msg->ReadLn( buffer, COMM_BUFFER_SIZE - 1, size);
         //remove new line char
         buffer[ COMM_BUFFER_SIZE - 1 ]=0;
         //look for local name in subscriber table
         wchar_t localName[COMM_BUFFER_SIZE ];
         //Read and discard line containing code of TData or TDataManager
         unsigned int code;
         if ( msg->Read( (char *)&code, sizeof( code))!= (int)sizeof(code))
            return false;
         msg->ReadLn( localName, COMM_BUFFER_SIZE - 1 , size); //this is doing nothing, the local(or locals) name is get after using GetLocalMatchRemoteName over the buffer (which has the remote name)

         std::list<TSubscriber *>::iterator i;
         std::list<TSubscriber *> *list =m_subsList.GetList();
         if ( list)                                                           {

            // Only one subscription to a remote manager is allowed. Multiple Subscriptions for TData are allowed instead.
            TNode *node;
            for (i=list->begin(); i!=list->end(); i++)                        {
               if ( (*i)->GetLocalMatchRemoteName(  buffer, localName,  sizeof(localName)) ) {
                  std::list<TDataManager *>::iterator i2;
                  for ( i2 = m_listDataMngr.begin(); i2!= m_listDataMngr.end(); i2++)    {
                     TDataManager *mngr = *i2;
                     node = mngr? mngr->GetNode ( localName ) : NULL;
                     if ( node)                                               {
                        //ToDo should we check if the interface is allowed to change the tdata value?
                        //What about the checks that viewinterface does using the rdonlystatus flag of the tdata?
                        //what about if I'm the server part? and if I'm the client?
                        LT::TStream *stream = msg->GetTStream();
                        if (stream)                                           {
                           m_avoidRebList.Push( *i);
                           int readPos = stream->posRead(); //save the pos

//                           bool forceToSave = true;
//                           node->Read( stream, this,true,&forceToSave );      //force save because is not saving even with modifications
                           node->SetToSave();
                           node->Read( stream, this );
//                           node->NotifyModified(this,false);   //bucle infinito, mala idea
                           stream->SeekRead( readPos);
                           m_avoidRebList.PopThis( *i);

                           proc = true;
                        }
                     }
                  }
               }
            }
            if (proc == false){
               wchar_t buffer2[2048];
               buffer2[2047] = 0;
               snwprintf( buffer2, 2047, L" Received notification from a TData %s which not corresponds to any subscription, the node does not exist or the stream can not be reconstructed", buffer );
               LOG_INFO1(buffer2);
//               LOG_WARN1(L"Received notification from a TData which not corresponds to any subscription, the node does not exist or the stream can not be reconstructed");
            }
         }
      } break;

   case MSG_COMM_IFC_NOTIFY_SUBSCRIBERS_ERASED: {

         /* TODO: descomentar cuando se pruebe
          //erase local Node
          const char *remote      =  msg->DataAsRaw();
          int   dataSize          =  msg->DataSize();

          //check message is correct:
          if ( ( dataSize <=  addressSize  ) ||  ( remote[dataSize-1] != '\0' ) )
          break;

          list<TDataManager *>::iterator i2;
          for ( i2= m_listDataMngr.begin(); i2!= m_listDataMngr.end(); i2++)   {
          TDataManager *mn = *i2;
          if ( mn)    {
          char localName[1024];
          std::list<TSubscriber *>::iterator i;
          std::list<TSubscriber *> *list =m_subsList.GetList();
          if ( list)  {
          // When a node A is subscribed to a manager in which a node B is erased, node A must be notified about it, and delete node B from him.
          i =   list->begin();
          while ( i != list->end())  {

          int  match= (*i)->GetLocalMatchRemoteName(  remote, localName,  sizeof(localName)) ;
          if ( (bool) ( match  ) )   {
          m_avoidRebList.Push( *i);
          mn->Delete( localName, this); // todo: pensar este erase quizás se tiene que hacer solamente cuando   el match = 1, matching parcial.
          //notificar interfaces
          //NotifyInterfacesNodeErased(mn, localName,this);
          proc = true;
          //if completeMatching, then erase subscription from list
          if ( match == 2 )
          i = list->erase( i);
          else
          i++;
          }
          else
          i++;
          }
          }
          }
          }
          */ } break;
   }
   return proc;
}

// ----------------------------------------------------------------------------
/**
 This method is called when data from a parameter/manager have been modified.
 This method reports all its interfaces of this change for them to be updated.

 @param   node  node  changed. If NULL; it means the manager itself has been modified
 @param   intf  interface  modified node
 @param   forceUpdate forces update manager
 */
bool TCommInterface::Refresh(TNode *node, TDataInterface *intf,
    bool forceUpdate) {



	if ( !m_enabled )
      return false;

   TData *data = dynamic_cast<TData*>(node);

   bool ret = false;
   if (data) {

      // Notice to all subscribers or a Data, or any parent,
      // sending them the new value, in a mensaje MSG_NOTIFY_SUBSCRIBERS.

      wchar_t fn[1024];
      if (!data->GetFullName(fn, sizeof(fn)))
         return ret;
         std::list<TSubscriber*>::iterator i,
                                           i2;
         ret = true;
         for (i = (m_subsList.GetList())->begin(); i != (m_subsList.GetList())->end(); i++) {
            bool completeMatch = true;
            //only one entry in local subscriptions for a bidirectional subscribe
            //So, check both columns in table
            if ( (! ( (*i)->MatchesLocalName(fn, completeMatch) && completeMatch==true)) &&
                  (! ( (*i)->MatchesRemoteName(fn, completeMatch)  && completeMatch ==true)))  {      //ToDo, ahora mismo encajan tdatas con nombres que se contienen, si esta suscrito todos los que empiezen por ese nombre tb se notificarán. Está hecho asi para poder subscribir managers enteros sin tener que subscribir todos sus tdatas. Hay que comprobar que los matching parciales corresponden con un nombre completo de tdata
               continue;
            }
            if (forceUpdate) {
               ret = NotifySubscriber(data, *i, false) && ret;

            }
            else     {
               int pop = m_avoidRebList.PopThis(*i);
               if ( pop == 0)  {
                        ret = NotifySubscriber(data, *i, false) && ret;
               }
            }
         }
   }
   return ret;
}

// ------------------------------------------------------------------------------
/**
 Method created for notifying a set of TDatas in only one message
 */
bool TCommInterface::Refresh(TDataManager *mn, TDataInterface *intf,
    bool forceUpdate, bool recursive) {

   if ( !m_enabled )
      return false;

   bool ret = true;
   // Avisamos a todos los suscriptores de un TData o algun padre
   // enviandoles el nuevo valor, en un mensaje  MSG_NOTIFY_SUBSCRIBERS

   if (mn) {

      wchar_t fn[1024];
      if (!mn->GetFullName(fn, sizeof(fn)))
         return ret;

      // create answer message and send
      std::list<TSubscriber*>::iterator i;
      for (i = (m_subsList.GetList())->begin();  i != (m_subsList.GetList())->end(); i++) {
         bool completeMatch = true;
         if (!(*i)->MatchesLocalName(fn, completeMatch))
            continue;
         int pop = m_avoidRebList.PopThis(*i);
         if (((pop == 0) || forceUpdate) /* &&  (*i)->MatchesLocalName(fn) */) {
            ret = NotifySubscriber(mn, *i, recursive) && ret;
         }
      }
   }
   return ret;
}

// ----------------------------------------------------------------------------------
/**
 */
bool TCommInterface::NotifySubscriber(TNode *node, const TSubscriber *subs,
    bool recursive) {

   if ( !m_enabled )
      return false;

   bool ret = false;

   wchar_t fn[1024];
   if (!node->GetFullName(fn, sizeof(fn)))
      return false;


	#ifdef _DEBUG
//	FILE *fitx = _wfopen(  L"TCommInterface", L"a");
//	fwprintf(fitx, L"TCommInterface::NotifySubscriber: node=%s m_enabled=%d\n",
//		node->GetName(), m_enabled);
//	fclose( fitx);
	#endif

   // local subscription
   if (subs->IsIntraModuleSubscription()) {     //i think is no used anymore
      std::list<TDataManager*>::iterator i2;

      // if subscription is local, only TData's are allowed
      TData *tLocalData = dynamic_cast< TData*>(node);
      if (tLocalData != NULL) {

         // IF SUBSCRIPTION IS LOCAL, WE MUST LOOKUP IN BOTH FIELDS ( LOCAL AND REMOTE)
         bool localMatch, completeMatch;
         const wchar_t *otherExtremName =
             subs->IntraModuleSubOppositeName(fn, localMatch, completeMatch);
         if (otherExtremName) {
            for (i2 = m_listDataMngr.begin(); i2 != m_listDataMngr.end(); i2++)
            {

               TData *tdata = (*i2)->GetDataObject(otherExtremName);
               if (tdata) {
                  // m_avoidReb = subs;
                  // ToDo: when changed from m_avoidReb to m_avoidRebList, Intramodule subscriptions
                  // has been disabled.
                  char c[100];
                  unsigned int size= sizeof( c);


                  //Be careful: in internal subscriptions, only
                  //value atribute will be notified to subscriptors
                  if ( tLocalData->AsText( c, size))                             {
                     TSubscriber *subsConst = const_cast < TSubscriber* > ( subs );
                     m_avoidRebList.Push( subsConst);

                     tdata->SetAsText( c, this, true);
                     m_avoidRebList.PopThis( subsConst);
                  }
                  else                 {
                     wchar_t str[100];
                     memset(str, 0x00, sizeof(str));
                     snwprintf( str, SIZEOF_CHAR(str)-1, L"TData %s couldnt be read for tnotify subscriptors",
                                  tLocalData->GetName());
                     LOG_WARN( str);
                  }

                  ret = true;
               }
            }
         }
      }
   }
   else { // Remote subscription
      bool completeMatch;
      if (subs->MatchesLocalName(fn, completeMatch)) {

         wchar_t org[addressSize], dest[addressSize];

         if (  Path::GetModName(fn, org, sizeof(org)) &&
               Path::GetModName(subs->GetRemoteName(), dest, sizeof(dest))) {
        //    LOG_WARN3(fn,L" -------- ENVIAR A ", dest);
            LT::TMessage *answerMsg = new LT::TMessage(MSG_COMM_IFC_NOTIFY_SUBSCRIBERS, org, dest, subs->GetPriority(), 200);
            LT::TStream *stream = answerMsg->GetTStream();
            // if unidireccional subscription, mask ReadOnly flag
            // Otra solucion posible sería llamar a Write con una opcion para indicar que no se escribieran los flags,
            // y en TattribManager no borrarlo todo, sino que ir buscando para modificar, y dejar el resto
            int maskFlagsAnd = subs->GetMaskFlagsAnd();
            int maskFlagsOr = subs->GetMaskFlagsOr();
            // node->Write( stream, recursive, true, TNode::FLAG_PUBLISHED | TNode::FLAG_NOTIFYREM , maskFlagsAnd , maskFlagsOr );
            //23/06/2016 vic, changed cleartobesaved to false because notifications are removing the toSave value and they shouldn't
            node->Write(stream, recursive, true, false, false, FLAG_NOTIFY, maskFlagsAnd, maskFlagsOr);

            ret = m_cmngr->PushOutMsg(answerMsg);
            if( !ret )  {
               LOG_WARN1(L"Failed to PushOutMsg in TCommInterface::NotifySubscriber MSG_COMM_IFC_NOTIFY_SUBSCRIBERS");
            }


         }
      }
   }

   return ret;
}

// ------------------------------------------------------------------------------
/**
 /// Al suscribirse, en el TCommInterface destino se guarda la referencia del objeto solicitado, para que cuando se modifique se nos enve la informacin solicitada
 */
void TCommInterface::Add(TDataManager *mngr) {

   if (mngr) {
      m_listDataMngr.push_back(mngr); // Add to my list
      mngr->Add(this); // Tell mngr to add me
   }
   else                                                                       {
      LOG_WARN1(L"Manager NULL.");
   }
}

// ------------------------------------------------------------------------------
/**
 */
bool TCommInterface::SetViewErased(const char *fullName, TDataInterface *intf) {

   bool ret = true;
   // Avisamos a todos los suscriptores de un TData o algun padre
   // enviandoles el nuevo valor, en un mensaje  MSG_NOTIFY_SUBSCRIBERS

   // create answer message and send
   std::list<TSubscriber*>::iterator i;
   i = (m_subsList.GetList())->begin();
   while (i != (m_subsList.GetList())->end()) {
      bool next = true;
      bool completeMatch = false;
      int pop = m_avoidRebList.PopThis(*i);
      if (((pop == 0))) {
         // if ( ( ( *i!= m_avoidReb)   )   /*  &&  (*i)->MatchesLocalName(fn)*/ )      {
         ret = NotifySubscriberNodeErased(fullName, *i, completeMatch) && ret;
         if (ret && completeMatch) {
            i = (m_subsList.GetList())->erase(i);
            next = false;
         }
      }
      if (next)
         i++;
   }
   return ret;

}

// ----------------------------------------------------------------------------------
/**
 Notify subscribers of a node erased.

 @param   fullName of erased Node that must be notified
 @param   subs     entry in subscriberList to check
 @param   completeMatch  true if a completeMatch is got for fullName in this entry
 @return  true     if a  match is got for fullName in this entry, and then a notification has been done
 */
bool TCommInterface::NotifySubscriberNodeErased(const char *fn,
    const TSubscriber *subs, bool &completeMatch) {
   bool ret = false;
   /** TODO: descomentar cuando se compruebe
    completeMatch  = false;

    if( subs->IsIntraModuleSubscription()) {

    //local subscription //IF SUBSCRIPTION IS LOCAL, WE MUST LOOKUP IN BOTH FIELDS ( LOCAL AND REMOTE)
    list<TDataManager *>::iterator i2;
    bool  localMatch              =  false;
    const char *otherExtremName   = subs->IntraModuleSubOppositeName( fn , localMatch, completeMatch );
    if ( otherExtremName )  {
    for( i2 = m_listDataMngr.begin(); i2 != m_listDataMngr.end(); i2++)    {

    TData *tdata      = (*i2)->GetDataObject( otherExtremName );
    TDataManager *mn  = tdata? tdata->GetParent():NULL;

    if ( mn )  {
    //ToDo: intramodule subscriptions doesn't work any more since m_avoidReb has been replaced by m_avoidRebList
    char fn2[1024];
    if ( tdata->GetFullName( fn2, sizeof( fn2)))
    //erase Node corresponding to other extreme
    mn->Delete( fn2 , this);
    ret = true;
    }
    }
    }
    }
    else     {
    if (   subs->MatchesLocalName( fn, completeMatch ) )     {
    //remote subscription
    char  org[addressSize],
    dest[addressSize];
    if ( Path::GetModName( fn, org, sizeof(org)) && Path::GetModName( subs->GetRemoteName(), dest, sizeof( dest)))      {

    LT::TMessage *answerMsg = new LT::TMessage( MSG_NOTIFY_SUBSCRIBERS_ERASED, org , dest, subs->GetPriority(), 200 );
    LT::TStream  *stream = answerMsg->GetTStream();
    stream->WriteStr( fn );
    ret = m_cmngr->PushOutMsg(answerMsg);
    }
    }
    }
    */
   return ret;
}

// ----------------------------------------------------------------------------------
/**
 Subscribe a localNode to a remote or local Node
 Local subscriptions only allowed for TData (not for TDataManagers)
 Remote subscriptions allowed for both TData and TDataManager

 @param   oriFullName name of remote or local node to wich sobuscribe
 @param   tLocalNode  local Node that is subscribed
 @ return true        when success
 */
bool TCommInterface::SubscribeInner(const wchar_t *oriFullName, TNode *tLocalNode,
    e_priority priority, int maskFlagsAnd, int maskFlagsOr, e_typeMsg type) {

   bool ret = false;
   wchar_t *moduloOri;
   const wchar_t *p;
   wchar_t localFullName[1024];
   if (tLocalNode == NULL)
      return false;

   if (!tLocalNode->GetFullName(localFullName, sizeof(localFullName)))
      return false;

   if (oriFullName == NULL || wcscmp(oriFullName, L"") == 0)
      return false;

   // 1. Extraer nombre del modulo de oriFullName. Simulacion. Se supone que la raiz del fullname es siempre
   // el nombre del modulo, y que es inalterable.
   if ((p = wcschr(oriFullName, L'\\')) != NULL) {
      int size = (p - oriFullName);
      moduloOri = new wchar_t[size+1]; //  (wchar_t *) malloc(size + 1);
      memcpy(moduloOri, (wchar_t*)oriFullName, size*sizeof(wchar_t) + 1);
      moduloOri[size] = 0;
   }
   else {
      if (wcslen(oriFullName) < addressSize)
         moduloOri = _wcsdup(oriFullName);
      else
         return false;
   }

#ifdef COM_LOG_SUBSCRIPTIONS
    wchar_t buff[512];
    buff[511] = 0;
    snwprintf( buff,511, L"SubscribeInner trying to subs local:%s orig:%s", localFullName, oriFullName );
    LOG_INFO1(buff);
#endif

   // 2. Crear una entrada en la tabla de suscripcion
   // m_subscriptionsL.push_back( new TSubscriber( localFullName, oriFullName, true));

   // maskFlags are sended to remote, but not stored in local subscription
   TSubscriber *subs = new TSubscriber(localFullName, oriFullName, priority);
   // char *localFullName2    = "ANI1\\Nombre_TData2";
   bool intraModuleSubs = subs->IsIntraModuleSubscription();
//5/10/2016:  comment this code, as only one entry will exist for each bidirectional internal subscription
//   if (intraModuleSubs) {
//      subs->SetMaskFlagsAnd(maskFlagsAnd);
//      subs->SetMaskFlagsOr(maskFlagsOr);
//      TData *tLocalData = dynamic_cast<TData*>(tLocalNode);
//      if (tLocalData != NULL) {
//         // 3. If local subscription: get Tdata and update views
//         // TData *tdata = m_dataMngr->GetDataObject( oriFullName);
//         ret = false;
//         std::list<TDataManager*>::iterator i2;
//         for (i2 = m_listDataMngr.begin(); i2 != m_listDataMngr.end(); i2++) {
//            TData *tdata = (*i2)->GetDataObject(oriFullName);
//
//            if (tdata) {
//               ret = true;
//               tLocalData->SetAsTData(*tdata, maskFlagsAnd, maskFlagsOr);
//               TSubscriber *subsLocal = new TSubscriber(oriFullName, localFullName, priority);
//               if (!AddSubscriber(subsLocal))                                 {
//                  LOG_WARN4(L"Local Subscription from ", oriFullName, localFullName,L" NOT done. ");
//               }
//            }
//            // else
//            // ret = false;
//         }
//      }
//      else
//         ret = false;
//   }
   if (!AddSubscriber(subs))
      return ret;

   if (!intraModuleSubs) {

      // 3. Remote subscription: Crear mensaje de suscripcion

      int size = sizeof(maskFlagsAnd)+sizeof(maskFlagsOr) +
          wcslen(localFullName) + 1 + wcslen(oriFullName) + 1;

      wchar_t name[addressSize];
      m_cmngr->GetModuleName(name, sizeof(name));

      LT::TMessage *msg;

      if( type == MSG_COMM_IFC_SUBSCRIBE_NO_NOTIFY )  {

            msg = new LT::TMessage(MSG_COMM_IFC_SUBSCRIBE_NO_NOTIFY, name, moduloOri, priority, 200);

            msg->Write((char *)&maskFlagsAnd, sizeof(maskFlagsAnd));
            msg->Write((char *)&maskFlagsOr, sizeof(maskFlagsOr));
            msg->Write(localFullName);
            msg->Write(oriFullName);

            LT::TStream *stream = msg->GetTStream();
            ret = false;
            std::list<TDataManager*>::iterator i2;
            for (i2 = m_listDataMngr.begin(); i2 != m_listDataMngr.end(); i2++) {
               TData *tdata = (*i2)->GetDataObject(localFullName);

               if (tdata) {    // esto es una locura, solo funcionaría si solo es un tdata, no se deberían hacer subscripciones a rutas y cosas así
                  ret = true;
                  bool reswrite = tdata->Write(stream, true, true, false, true, FLAG_NOTIFY, maskFlagsAnd, maskFlagsOr);   //ToDo: revisar segundo parametro recursive
                  if ( reswrite == false ) {
                     LOG_INFO2(L"Fallo al escribir dato en SUBSCRIBE_NO_NOTIFY de nodo:",localFullName);
                  }
               }
            }

            if ( !ret ) {
               LOG_ERROR1(L"Subscribe no notify failed getting tdata");
            }


      }
      else  {
         msg   = new LT::TMessage(type, name, moduloOri, priority, size*sizeof(wchar_t));

         msg->Write((char *)&maskFlagsAnd, sizeof(maskFlagsAnd));

         msg->Write((char *)&maskFlagsOr, sizeof(maskFlagsOr));

         msg->Write(localFullName);

         msg->Write(oriFullName);

      }

      // 4. ENVIAR MENSAJE DE SUSCRIPCION
#ifdef COM_LOG_SUBSCRIPTIONS
      LOG_INFO1(L"Sending subscription petiton to pushout");
#endif
      ret = m_cmngr->PushOutMsg(msg);
      if( !ret )  {
         LOG_WARN1(L"Failed to PushOutMsg in TCommInterface::SubscribeInner MSG_COMM_IFC_SUBSCRIBE_NO_NOTIFY or other");
      }
   }
   //free(moduloOri);
   delete [] moduloOri;
   return ret;

}




// ----------------------------------------------------------------------------------
/**
 Subscribe a localNode to a remote or local Node
 Local subscriptions only allowed for TData (not for TDataManagers)
 Remote subscriptions allowed for both TData and TDataManager

 @param   oriFullName name of remote or local node to wich sobuscribe
 @param   tLocalNode  local Node that is subscribed
 @ return true        when success
 */
bool TCommInterface::Subscribe(const wchar_t *oriFullName, TNode *tLocalNode,
    e_priority priority, int maskFlagsAnd, int maskFlagsOr) {
    return SubscribeInner ( oriFullName, tLocalNode, priority, maskFlagsAnd, maskFlagsOr,MSG_COMM_IFC_SUBSCRIBE );
}
// ----------------------------------------------------------------------------------
/**
 Subscribe a localNode to a remote or local Node, and sends its current value
 Receiver reads current value from message, instead of answering with its value
 Local subscriptions only allowed for TData (not for TDataManagers)
 Remote subscriptions allowed for both TData and TDataManager

 @param   oriFullName name of remote or local node to wich sobuscribe
 @param   tLocalNode  local Node that is subscribed
 @ return true        when success
 */
bool TCommInterface::SubscribeWithoutNotification(const wchar_t *oriFullName, TNode *tLocalNode,
    e_priority priority, int maskFlagsAnd, int maskFlagsOr) {
    return SubscribeInner ( oriFullName, tLocalNode, priority, maskFlagsAnd, maskFlagsOr,MSG_COMM_IFC_SUBSCRIBE_NO_NOTIFY );
}



// -----------------------------------------------------------------------------
/**

 @param tLocalNode
 @return
 */
bool TCommInterface::Unsubscribe(TNode *tLocalNode) {

   bool ret = false;
   wchar_t *moduloOri;
   const wchar_t *p;
   wchar_t localFullName[MAX_LENGTH_FULL_PATH];
   if (tLocalNode == NULL)
      return false;

   if (!tLocalNode->GetFullName(localFullName, sizeof(localFullName)))
      return false;

   // 1. Borrar entrada en la tabla de suscripcion, si se encuentra
   wchar_t *oriFullName = NULL;
   if (!DelSubscriptionByLocalName(localFullName, oriFullName))
      return false;

   wchar_t modLocal[20], modRemote[20];
   bool intraModuleSubs = Path::GetModName(localFullName, modLocal,
       sizeof(modLocal)) && Path::GetModName(oriFullName, modRemote,
       sizeof(modRemote)) && (wcscmp(modLocal, modRemote) == 0);

   if (!intraModuleSubs) {

      // Crear mensaje de borrado de suscripcion
      int size = wcslen(localFullName) + 1;

      LT::TMessage *msg = new LT::TMessage(MSG_COMM_IFC_UNSUBSCRIBE, modLocal,
          modRemote, PRIOR_NORMAL, size*sizeof(wchar_t));

      msg->Write(localFullName);

      // 4. ENVIAR MENSAJE DE SUSCRIPCION
      ret = m_cmngr->PushOutMsg(msg);
      if( !ret )  {
         LOG_WARN1(L"Failed to PushOutMsg in TCommInterface::Unsubscribe MSG_COMM_IFC_UNSUBSCRIBE");
      }
      // ret = true;
      // const LT::TStream *stream = msg->GetTStream();
      // stream->SaveToFile( "subscribe.txt");
   }

   if (oriFullName)
      delete oriFullName;

   return ret;

}

bool TCommInterface::UnsubscribeAll()                                         {

   if ( !m_enabled )
      return false;

   std::list<TSubscriber*>::iterator i;
   std::list<TSubscriber*> *list = m_subsList.GetList();
   if (list)                                                {
      i = list->begin();
      while (i != list->end()) {
         if ((*i) && (*i)->IsIntraModuleSubscription())
            i = list->erase(i);
         else {
            // remote subscription
            wchar_t org[addressSize], dest[addressSize];

            if (Path::GetModName((*i)->GetLocalName(), org,
                addressSize) && Path::GetModName((*i)->GetRemoteName(), dest,
                addressSize)) {

               LT::TMessage *answerMsg =
                   new LT::TMessage(MSG_COMM_IFC_UNSUBSCRIBE, org, dest,
                   (*i)->GetPriority(), 200);
               if (answerMsg) {
                  answerMsg->Write((*i)->GetLocalName());
                  answerMsg->Write((*i)->GetRemoteName());
                  bool ret = m_cmngr->PushOutMsg(answerMsg);
                  if( !ret )  {
                     LOG_WARN1(L"Failed to PushOutMsg in TCommInterface::UnsubscribeAll MSG_COMM_IFC_UNSUBSCRIBE");
                  }
               }
            }
            i++;
         }
      } // (while)
   }

   return true;

}

// -----------------------------------------------------------------------------
/**
 Gets information from subscribers into str
 @return true if str had enough space to store information
 */
bool TCommInterface::GetInfoSubs(wchar_t *str, int size) {

   memset(str, 0x00, size);
   swprintf(str, L"\n\n");

   bool ret = true;
   wchar_t temp[1024];
   temp[1023] = 0;
   std::list<TSubscriber*>::iterator i;
   i = (m_subsList.GetList())->begin();
   while (i != (m_subsList.GetList())->end()) {
      TSubscriber *sub = *i;
      if (sub) {

         snwprintf(temp,1023, L"Subs local: #%s#\nSubs rem: #%s#\n",
             sub->GetLocalName(), sub->GetRemoteName());
         if ((signed)(wcslen(temp) + wcslen(str)) < size) {
            wcscat(str, temp);
         }
         else {
            ret = false;
            break;
         }
      }
      i++;

   }
   return ret;

}

// -----------------------------------------------------------------------------
/**
 Delete All Subscriptions which belong to dest
 @param dest, destination to remove in subscriptions
 @return true if TCommInterface is enabled and false otherwise
 */
bool TCommInterface::DeleteSubscriptionsFromDest (wchar_t * dest)            {
 //ToDo check valor de retorno

 if ( !m_enabled )   {
      LOG_INFO1(L"DeleteSubscriptionsFromDest disabled!");
      return false;
 }
   LOG_INFO2(L"DeleteSubscriptionsFromDest deleting subscription from dest: ", dest);
   bool deleted_ =false;
   std::list<TSubscriber*>::iterator i;
   std::list<TSubscriber*> *list = m_subsList.GetList();
   if (list)                                                {
      i = list->begin();
      while (i != list->end()) {
         deleted_ =false;
         if (*i){
            // remote subscription
            wchar_t dest_link[addressSize];

            if (Path::GetModName((*i)->GetRemoteName(), dest_link,
                addressSize)) {
               if(wcscmp(dest_link, dest) ==0 ){
                  TSubscriber* sub = *i;
                  i = list->erase(i);
                  delete sub;
                  deleted_ =true;
               }

            }
         }
         if(!deleted_)
               i++;
      } // (while)
   }

   return true;

}


// -----------------------------------------------------------------------------
/**
Delete All Subscriptions which belong to dest
@return true if str had enough space to store information
*/
bool TCommInterface::DeleteAllSubscriptions()            {

   if (!m_enabled)
      return false;
   std::list<TSubscriber*>::iterator i;
   std::list<TSubscriber*> *list = m_subsList.GetList();
   if (list)                                                {
      i = list->begin();
      while (i != list->end()) {
         TSubscriber* sub = *i;
         i = list->erase(i);
         delete sub;
      }
   }

   return true;

}


//------------------------------------------------------------------------------
/**
*  The commInterface is never disabled.
   Parche para no deshabilitar el comminterface en los cambios de tipo por ejemplo
*/
void TCommInterface::SetEnabled (bool enabled)                                {

   m_enabled = true;

};






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
