//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TSubscriber.cpp
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

#include <vector>

#include "TSubscriber.h"
#include "TAux.h"



#pragma package(smart_init)


//-----------------------------------------------------------------------------
//********************************  TSubscriber Class  ************************
//-----------------------------------------------------------------------------
/**
   Constructor

   @param   local    wchar_t pointer to the local path of the TData or TDataManager
   @param   remote   wchar_t pointer to the remote path where to send changes in the local path
   @param   recursive   Only for TDataManagers.
            true to subscribe to the DataManager and all of this descendant (default)
            false to subscribe just to the direct descendants of the TDataManager
*/
TSubscriber::TSubscriber( const wchar_t *local, const wchar_t *remote,
                          e_priority priority, int maskFlagsAnd,
                          int maskFlagsOr )                                   {

   if( wcslen(local) && wcslen(remote))                                       {
      m_localPath    = _wcsdup(local);
      m_remotePath   = _wcsdup(remote);
   }
   else                                                                       {
      m_localPath    = NULL;
      m_remotePath   = NULL;
   }

   wchar_t modLocal[20],
          modRemote[20];

   if (Path::GetModName( local, modLocal, sizeof(modLocal)) && Path::GetModName( remote , modRemote, sizeof( modRemote)) && (wcscmp(modLocal,modRemote)==0) )
      m_intraModule     =  true;
   else
      m_intraModule     =  false;

   m_maskFlagsAnd =  maskFlagsAnd;
   m_maskFlagsOr  =  maskFlagsOr;
   m_priority     =  priority;

}

//----------------------------------------------------------------------------
/**
   Destructor
*/
TSubscriber::~TSubscriber()                                                   {

   // Free path's memory
   if( m_localPath)
      free( m_localPath);
   m_localPath = NULL;
   if( m_remotePath)
      free( m_remotePath);
   m_remotePath = NULL;
}

//----------------------------------------------------------------------------
/**
   Matching can be total or partial. Just beginning of m_localPath is compared
   @param   nameToMatch    name we are testing if it matches m_localPath
   @param   completeMatch  returns true when complete Match is got
   @return  true           if matches
*/
bool TSubscriber::MatchesLocalName( const wchar_t *nameToMatch , bool &completeMatch)   const    {

   bool ret;
   completeMatch  = false;
   if ( wcslen( nameToMatch) >= wcslen( m_localPath) )                        {
      ret =    ( wcsncmp(m_localPath, nameToMatch, wcslen(m_localPath))==0);
      if (ret && (wcslen( nameToMatch) == wcslen( m_localPath)))
         completeMatch  =  true;
   }
   else
      return false;

   return ret;
}
bool TSubscriber::MatchesRemoteName( const wchar_t *nameToMatch , bool &completeMatch)   const    {

   bool ret;
   completeMatch  = false;
   if ( wcslen( nameToMatch) >= wcslen( m_remotePath) )                        {
      ret =    ( wcsncmp(m_remotePath, nameToMatch, wcslen(m_remotePath))==0);
      if (ret && (wcslen( nameToMatch) == wcslen( m_remotePath)))
         completeMatch  =  true;
   }
   else
      return false;

   return ret;
}


//-----------------------------------------------------------------------------
/**
   For intra_module subscriptions ( remote path and local path in the same module,
   we must inspect local names and remote names

   @param   nameToMatch   name we are testing if it matches m_localPath
   @param   localMatch    return true if matching is found in m_localName
   @param   completeMatch  return true if a complete Match is got for nameToMatch
   @return  NULL         if matches no matching found
      m_remoteName   if nametoMatch matches with m_localName
      m_localName    if nameToMatch matches with m_remoteName
*/
const wchar_t *TSubscriber::IntraModuleSubOppositeName( const wchar_t *nameToMatch , bool &localMatch, bool &completeMatch) const  {

   const wchar_t *ret   =  NULL;
   localMatch        =  false;
   completeMatch     =  false;

   if ( m_intraModule ==false)
      return ret;
   if ( wcslen( nameToMatch) >= wcslen( m_remotePath) )                       {
      if ( wcsncmp(m_remotePath, nameToMatch, wcslen(m_remotePath))==0)       {
         //matching found in m_remotePath. Returns localPath
         ret = m_localPath;
         if ( wcslen( nameToMatch) == wcslen( m_remotePath))
            completeMatch  =  true;

      }
   }
   if ( ret ==NULL)                                                           {
      if ( wcslen( nameToMatch) >= wcslen( m_localPath) )                     {
         if ( wcsncmp(m_localPath, nameToMatch, wcslen(m_localPath))==0)      {
            //matching found in m_localPath. Returns remotePath
            ret = m_remotePath;
            localMatch  = true;
            if ( wcslen( nameToMatch) == wcslen( m_localPath))
                  completeMatch  =  true;
         }
      }
   }
   return ret;
}

//----------------------------------------------------------------------------
/**
   @param   remoteName name to look matching for in list
   @param   localName   buffer where get the local name corresponding the remote name
   @param   sizeMax size of buffer (localName)
   @param   completeMatch (by Reference). Returns true if a complete matching is got.
   @return  1     if a partial match is got for nameToMatch
            2     if a complete match is got for nameToMatch
            0     if NO match is got for fullName in this entry
   Only one line extpected to match ( not allowed more than one subscription.If needed, do it in 'cascada'
   Matching can be complete or partial:
      - complete: there is an entry in list with a name that is exactly the one we are looking for
      - partial:  there is an entry in list that matches with the beginning of the name we are looking for
      Throws an exception if finds a matching entry, but has no enough buffer to return name --> its returnning 0, not an exception
*/
int  TSubscriber::GetLocalMatchRemoteName( const wchar_t *remoteName, wchar_t *localName, unsigned int sizeMax ) const  {

   int ret    =  0;
   //check for matching names
   if ( wcsncmp( m_remotePath, remoteName, wcslen(m_remotePath))==0)          {
      //ret = true;
      if(  wcslen( m_remotePath) == wcslen( remoteName))                      {
         //complete matching
         if ( sizeMax > wcslen( m_localPath) )                                {
         wmemcpy( localName   , (wchar_t*) m_localPath, wcslen( m_localPath) + 1) ;
         ret            =  2;
         }
         else
            LOG_WARN2(L"Size of Local Path Exceeded", m_localPath);
      }
      else                                                                    {
         //partial matching

         const std::vector<std::wstring> vthisremotePath = AuxStrings::explodeW( std::wstring(m_remotePath) , L'\\') ;
         const std::vector<std::wstring> vremoteName     = AuxStrings::explodeW( std::wstring(remoteName)   , L'\\') ;

         //if last name does not match is because is not a complete name so we reject the match -> ret = 0
         if (  vthisremotePath.size() > 1
               && vremoteName.size() > 1
               && vthisremotePath[vthisremotePath.size()-1] != vremoteName[vremoteName.size()-1] )   {
            return 0;
         }

         int inc  =  wcslen( m_remotePath);
         int size =  wcslen( m_localPath) +  wcslen( remoteName) - wcslen( m_remotePath) + 1;
         //localName   = (char *)malloc(size);
         if ( size < (int) sizeMax )                                          {
            memcpy( localName,  m_localPath, wcslen( m_localPath));
            wchar_t *pDest = localName + wcslen( m_localPath);
            const wchar_t *pOri  = remoteName + inc;
            wmemcpy( pDest, pOri, wcslen(remoteName) - inc);
            pDest[wcslen(remoteName) - inc ] = L'\0';
            ret = 1;
         }
         else
            LOG_WARN3(L"Size Paths Exceeded", (wchar_t *)m_localPath , m_remotePath);
      }
   }
   return ret;
}

//------------------------------------------------------------------------------
/**
*/
int  TSubscriber::GetRemoteMatchLocalName( const wchar_t *localName, wchar_t *remoteName, unsigned int sizeMax ) const {

   int  ret = 0;
   //check for matching names
   if ( wcsncmp( m_localPath, localName, wcslen( m_localPath))==0)            {
      //ret = true;
      if(  wcslen(m_localPath) == wcslen( localName))                         {
         //complete matching
         if ( sizeMax > wcslen( m_remotePath) )                               {
            wmemcpy( remoteName   , m_remotePath, wcslen( m_remotePath) + 1) ;
            ret            =  2;
         }
         else                                                                 {
            //TODO: definir excepcion a lanzar . Se elimina el warning W8078 del compilador o se define la excepción.
            throw std::exception();
         }
      }
      else                                                                    {

         const std::vector<std::wstring> vthislocalPath = AuxStrings::explodeW( std::wstring(m_localPath) , L'\\') ;
         const std::vector<std::wstring> vlocalName     = AuxStrings::explodeW( std::wstring(localName)   , L'\\') ;

         //if last name does not match is because is not a complete name so we reject the match -> ret = 0
         if (  vthislocalPath.size() > 1
               && vlocalName.size() > 1
               && vthislocalPath[vthislocalPath.size()-1] != vlocalName[vlocalName.size()-1] )   {
            return 0;
         }
         //partial matching
         int inc  =  wcslen( m_localPath);
         int size =  wcslen( m_remotePath) +  wcslen( localName) - wcslen( m_localPath) + 1;
         //remoteName   = (char *)malloc(size);
         if (  size  < (int)sizeMax )                                         {

            memcpy( remoteName, m_remotePath, wcslen( m_remotePath));
            wchar_t *pDest = remoteName + wcslen( m_remotePath);
            const wchar_t *pOri  = localName + inc;
            wmemcpy( pDest, pOri, wcslen(localName) - inc);
            pDest[wcslen(localName) - inc ] = L'\0';
               ret = 1;
         }
         else                                                                 {
            //TODO: definir excepcion a lanzar . Se elimina el warning W8078 del compilador o se define la excepción.
            throw std::exception( );
         }
      }
   }
   return ret;
}

//-----------------------------------------------------------------------------
//*********************  TSubscriberList Class  *******************************
//-----------------------------------------------------------------------------

/**
   Constructor
*/
TSubscriberList::TSubscriberList()                                            {
   m_subsL.clear();
}

//------------------------------------------------------------------------------
/**
   Destructor
*/
TSubscriberList::~TSubscriberList()                                           {

   std::list<TSubscriber *>::iterator i;
   for (i=m_subsL.begin(); i!=m_subsL.end(); i++)
      delete *i;

   m_subsL.clear();
}

//------------------------------------------------------------------------------
/**
   Adds a subscriber in list
   @param subscriber
*/
bool  TSubscriberList::Add( TSubscriber  *subscriber )                        {

   bool  found = false;
   //check if there is another subscriber in list with same remote path or including remote path or included remote path
   std::list<TSubscriber *>::iterator i;
   for (i=m_subsL.begin(); i!=m_subsL.end(); i++)                       {
      if ( wcscmp( (*i)->GetRemoteName() , subscriber->GetRemoteName() ) == 0 )    {
            found  = true;
      }
   }
   if (! found)
      m_subsL.push_back( subscriber);
   wchar_t buff[512];
   buff[511] = 0;
   if ( ! found ) {
      snwprintf( buff,511, L"TSubscriberList::Add local:%s orig:%s sublist size: %d",subscriber->m_localPath,subscriber->m_remotePath, m_subsL.size());
   }
   else
   {
      snwprintf( buff,511, L"TSubscriberList::Add local:%s orig:%s is already in list",subscriber->m_localPath,subscriber->m_remotePath);
      found = false; //we return true to represent that the subscriber exists in list at the end, despite it was already inside the list
   }

#ifdef COM_LOG_SUBSCRIPTIONS
   LOG_INFO1(buff);
#endif
   return ( ! found);

}

//------------------------------------------------------------------------------
/**
   Searches by name in remoteName field and deletes entry in list
*/
bool TSubscriberList::DeleteByRemoteName( const wchar_t *remoteName)             {

   bool  found = false;
   //check if there is an entry in list with remoteName
   std::list<TSubscriber *>::iterator i;
   i =  i=m_subsL.begin();
   while ( i!=m_subsL.end() )                       {
      if ( wcscmp( (*i)->GetRemoteName() , remoteName) == 0 )    {
            found  = true;
            i = m_subsL.erase( i);
      }
      else
         i++;
   }
   return ( found);
}

//-----------------------------------------------------------------------------
/**
   @param localName: entry to be deleted in m_subsList
   @param remoteName return the remoteNmae matching localName in m_subsList  ( a duplicate of the string)
   @return true if localName exists in m_subsList
*/
bool TSubscriberList::DeleteByLocalName( const wchar_t *localName, wchar_t *&remoteName )   {

   bool  found = false;
   //check if there is an entry in list with remoteName
   std::list<TSubscriber *>::iterator i;
   i =  i=m_subsL.begin();
   while ( i!=m_subsL.end() )                       {
      if ( wcscmp( (*i)->GetLocalName() , localName) == 0 )    {
            remoteName = _wcsdup( (*i)->GetRemoteName());
            found  = true;
            i = m_subsL.erase( i);
      }
      else
         i++;
   }
   return ( found);
}




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
