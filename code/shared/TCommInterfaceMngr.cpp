//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TCommInterfaceMngr.cpp
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

#include "TCommInterfaceMngr.h"

#define FLAG_COMM_LINK     0x0000080 //todo
//---------------------------------------------------------------------------

#pragma package(smart_init)

/**
 //TODO
*/
TCommInterfaceMngr::TCommInterfaceMngr( wchar_t* name, TDataManagerControl *mngr, TCommMngr *cmngr):
      TCommInterface(mngr, cmngr),  TDataManagerControl (  name,      mngr)       {

   //#TCPSOCKET#127.0.0.1:31100
   m_daDest = new TData( this, L"Dest",   m_target,  L"ACO", SIZEOF_CHAR( m_target),  FLAG_NORMAL );
   m_daTarget = new TData( this, L"IPTarget",   m_target,  L"TCPSOCKET#127.0.0.1:31600",        SIZEOF_CHAR( m_target),  FLAG_NORMAL );
   m_CommMngr = cmngr;
   m_started = false;
}

TCommInterfaceMngr::~TCommInterfaceMngr(){
}

bool TCommInterfaceMngr::ProcMsg( LT::TMessage *msg) {

   bool ret = false;

   if (m_CommMngr)
      ret = m_CommMngr->ProcMsg(msg);

   return (TCommInterface::ProcMsg(msg) || ret);
}

bool TCommInterfaceMngr::Start( )                                            {
    if (!m_started && m_CommMngr)                                              {
      if  (GetRootMngr() != NULL)                                             {
         const wchar_t* modName =  GetRootMngr()->GetName();
         //-- Try to communicate with the Dest
         wchar_t txt[64];
         swprintf( txt, L"%s#%s\0", L"ACO",  L"TCPSOCKET#127.0.0.1:31600");
         //ToDo check this is no the case when no using multimodul
//         LT::TMessage *virtualMsg =  new LT::TMessage(    MSG_COMM_ADDRESS_BY_NAME, L"VIRTUAL", modName , PRIOR_NORMAL, (char*)txt, wcslen( txt)*sizeof(wchar_t));
//         if( m_CommMngr->ProcMsg( virtualMsg))                                            {
//            m_started = true;
//         }
      }
   }
   return m_started;
}

bool TCommInterfaceMngr::MyInit( )                                            {

   if (m_started)
      SubscribeNodes();

   return m_started;
}


void  TCommInterfaceMngr::SubscribeNodes()                         {
   // 1.- Recorre la lista de rutas origen

   //TODO: modificar para leer de los tdatas
   wchar_t * sourceRoute = L"ArtificialIntelligenceSoftware\\vcam";
   wchar_t * targetRoute = L"ACO\\vcam";

   TNode* node = GetNode(sourceRoute);
   if (node)
      LinkNode(node, targetRoute);
}

void  TCommInterfaceMngr::LinkNode(TNode* node, wchar_t* targetRoute)        {

   if (!node)
      return;


   TDataManager *mngr = dynamic_cast<TDataManager*>(node);
   if (mngr)                                                                  {

      std::list<TData *>::const_iterator it_da;
      for (it_da = mngr->DataBegin();it_da!=mngr->DataEnd(); it_da++)         {
         if ( (*it_da) && ( (*it_da)->GetFlags() & FLAG_COMM_LINK ))          {
               TCommInterface::Subscribe(targetRoute, (*it_da));
         }
      }

      std::list<TDataManager*>::iterator it_mngr;
      for (it_mngr = mngr->DataManagerBegin() ;it_mngr != mngr->DataManagerEnd(); it_mngr++)   {
         if ( (*it_mngr) && ( (*it_mngr)->GetFlags() & FLAG_COMM_LINK ))      {
            TCommInterface::Subscribe( targetRoute, (*it_mngr));
            LinkNode((*it_mngr),targetRoute );
         }
      }
   }

   TData *data = dynamic_cast<TData*>(node);
   if ( data && ( data->GetFlags() & FLAG_COMM_LINK ))
      TCommInterface::Subscribe(targetRoute, data);
}






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
