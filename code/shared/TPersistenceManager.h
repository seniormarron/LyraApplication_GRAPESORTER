//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TPersistenceManager.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TPersistenceManagerH
#define TPersistenceManagerH

//---------------------------------------------------------------------------

#include "TPersistence.h"
#include "TEventPersistence.h"
#include "TDataMngControl.h"


class TPersistenceManager : public TDataManager                                {

  protected:

   TPersistence*        m_persistence        ;
   TEventPersistence*   m_eventPersistence   ;
   TData*               m_daSelectType;
   TData*               m_daDeleteType;
   TData*               m_daNewType;
   int                  m_selectType, m_deleteType, m_NewType;
   bool                 m_updating;

   void __fastcall DataModifiedPost( TData* da);                        ///< Event Handler method for Control of modified data
   void  UpdateData()  ;

  public:

   // -- Constructor & Destructor methods
   TPersistenceManager( const wchar_t *name, TDataManager *_parent = NULL);    ///< Constructor.
   virtual   ~TPersistenceManager();                                                      ///< Destructor.

};

#endif





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
