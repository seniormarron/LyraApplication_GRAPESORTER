//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TLoadCfgDlg.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TLoadCfgDlgH
#define TLoadCfgDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "TPersistence.h"

//---------------------------------------------------------------------------
class TLoadCfgDlg : public TForm
{
__published:	// IDE-managed Components
   TLabel *LocalDir;
   TPanel *PanelLoadCfg;
   TLabel *infolb;
   TLabel *LabelSelect;
   TButton *okbt;
   TButton *cancelbt;
   TListBox *cfglistbox;
   TCheckBox *localcb;
   TCheckBox *externalcb;
   TLabel *ExternalDir;
   TPanel *Panel1;
   TPanel *Panel2;
   TButton *ButtonDelete;
   TBitBtn *BtnLoad;
   void __fastcall FormDestroy(TObject *Sender);
   void __fastcall localcbClick(TObject *Sender);
   void __fastcall externalcbClick(TObject *Sender);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall ButtonDeleteClick(TObject *Sender);
   void __fastcall BtnLoadClick(TObject *Sender);
private:	// User declarations
protected:
   TPersistence *m_persistence;
   bool         m_user;
   void SetLoadCfg(wchar_t* folder);

public:		// User declarations
   __fastcall TLoadCfgDlg(TComponent* Owner, TPersistence *persistence, bool user);
  };
//---------------------------------------------------------------------------
extern PACKAGE TLoadCfgDlg *LoadCfgDlg;
//---------------------------------------------------------------------------
#endif






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
