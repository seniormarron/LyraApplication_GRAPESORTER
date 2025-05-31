//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TSubscriber.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



#ifndef TSubscriberH
#define TSubscriberH

#include <list>
#include <exception>
#include "TMessage.h"

class TSubscriberList;

//-----------------------------------------------------------------------------
//********************************  Path Class  *******************************
//-----------------------------------------------------------------------------
///Provides functionality to search for the module name from a given path
class Path                                                                    {

  public:
   /**
      @param  name      full name where to look for the module Name
      @param  modNmae   returns the module name extracted from name
      @param  size      size of modName parameter, in wchar_t unitites
   */
   static bool GetModName(const wchar_t *name, wchar_t *modName, unsigned int size) {
      bool ret    = false;
      bool found  = false;
      const wchar_t *p;
      if ( ( p = wcschr( name, L'\\')) !=NULL )                                {
         if ( p== name )                                                      {
            //search again: first char was '\\'
            if ( ( p = wcschr( p, L'\\')) !=NULL)                              {
               found = true;
               name++;
            }
         }
         else
            found = true;
      }
      if ( found)                                                             {
         if ( (unsigned)(p - name) < size)                                          {
            int lsize = p - name ;
            memcpy(modName, (wchar_t*)name, lsize*sizeof(wchar_t) + 1 );
            modName[lsize]= 0;
            ret = true;
         }
      }
      else                                                                    {
         if ( wcslen(name) < size)                                            {
            memcpy(modName, (wchar_t*)name, wcslen(name) * sizeof(wchar_t));
            modName[wcslen(name)]= 0 ;
            ret = true;
         }
      }
      return ret;
   }
};

//-----------------------------------------------------------------------------
//**************************  TSubscriber Class  ******************************
//-----------------------------------------------------------------------------
/// Object to manage a the subscription of a remote param to a local param
/**
*  TSubscribers represents a subscription between one local path (TData or TDataManager)
*  and a remote path (Tdata or TDataManager)
*/
class TSubscriber                                                             {

  protected:

   wchar_t        *m_localPath,       ///< Local path of the TData/TDataManager
                  *m_remotePath;      ///< Remote path of the TData/TDataManager: format   <modName>@<fullName of TData or TDataManager> [ '\']
   int            m_maskFlagsAnd,
                  m_maskFlagsOr;      ///< Only from remotePath to localPath
   bool           m_intraModule;      ///< remotePath and localPath in the same module ( local subscription )
   e_priority     m_priority;         ///< Priority of the subscription messages generated

  public:

   TSubscriber( const wchar_t *local, const wchar_t *remote, e_priority priority = PRIOR_NORMAL,
                                                      int maskFlagsAnd     = 0xFFFFFFFF,
                                                      int maskFlagsOr      = 0x00 );                                 ///< Constructor
   ~TSubscriber();                                                                                                   ///< Destructor

   bool MatchesLocalName                  ( const wchar_t *nameToMatch, bool &completeMatch )                  const;   ///<
   bool MatchesRemoteName                  ( const wchar_t *nameToMatch, bool &completeMatch )                  const;   ///<
   const wchar_t *IntraModuleSubOppositeName ( const wchar_t *nameToMatch, bool &localMatch, bool &completeMatch) const;   ///<

   int  GetLocalMatchRemoteName( const wchar_t *remoteName, wchar_t *localName, unsigned int sizeMax  ) const;             ///<
   int  GetRemoteMatchLocalName( const wchar_t *localName,  wchar_t *remoteName, unsigned int maxSize ) const;             ///<


   //-- Inline functions

   //--------------------------------------------------------------------------
   /**
   */
   const wchar_t *GetLocalName()  const                                          {
      return m_localPath;
   };

   //--------------------------------------------------------------------------
   /**
   */
   const wchar_t *GetRemoteName() const                                          {
      return m_remotePath;
   };

   //--------------------------------------------------------------------------
   /**
      Check if subscriber is in the same module than  subscribee
   */
   bool IsIntraModuleSubscription( ) const                                    {
      return m_intraModule;
   }

   //--------------------------------------------------------------------------
   /**
   */
   int GetMaskFlagsAnd() const                                                {
      return m_maskFlagsAnd;
   }

   //--------------------------------------------------------------------------
   /**
   */
   int GetMaskFlagsOr() const                                                 {
      return m_maskFlagsOr;
   }

   //--------------------------------------------------------------------------
   /**
   */
   void SetMaskFlagsAnd(int maskFlagsAnd)                                        {
      m_maskFlagsAnd = maskFlagsAnd;
   }

   //--------------------------------------------------------------------------
   /**
   */
   void SetMaskFlagsOr(int maskFlagsOr)                                          {
      m_maskFlagsOr = maskFlagsOr;
   }

   //--------------------------------------------------------------------------
   /**
   */
   e_priority GetPriority() const                                             {
      return m_priority;
   };

   friend class TSubscriberList;
};

//-----------------------------------------------------------------------------
//*************************  TSubscriberList Class  ***************************
//-----------------------------------------------------------------------------
class TSubscriberList                                                         {

  private:
   std::list<TSubscriber *> m_subsL;                                    ///< Subscriber list

  public:
   TSubscriberList();                                                   ///< Constructor
   ~TSubscriberList();                                                  ///< Destructor

   // -- inline functions --
   bool  Add( TSubscriber  *subscriber );                               ///< Adds a subscriber in list
   bool  DeleteByRemoteName( const wchar_t *remoteName);                   ///< Searches by name in remoteName field and deletes entry in list
   bool  DeleteByLocalName( const wchar_t *localName, wchar_t *&remoteName);  ///<

   //----------------------------------------------------------------------------
   /**
      Get Subscriber List
      @return  Get Subscriber List
   */
   std::list<TSubscriber *> *GetList()                                        {
      return &m_subsL;
   }
};

#endif



//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------

