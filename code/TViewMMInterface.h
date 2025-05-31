//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TViewMMInterface.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TViewMMInterfaceH
#define TViewMMInterfaceH
//---------------------------------------------------------------------------

#include "TViewInterface.h"

class TViewMMInterface : public TViewInterface                               {



 public:

//   TViewMMInterface(    TDataManager *mngr, TView *view );    ///< Constructor.
   TViewMMInterface(    TDataManager *mngr, TForm *view );    ///< Constructor.
   virtual ~TViewMMInterface();

  protected:

   virtual bool   RefreshInterface ( TNode *tdata);     ///< Updates changes of Tdata in a view.

//   virtual void   SetEnabled     ( bool enabled)   {
//       m_enabled = true; // this interface will always be set to true.
//   }
};

#endif












//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
