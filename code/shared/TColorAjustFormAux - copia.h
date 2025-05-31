//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
            Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TColorAjustForm.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              02/01/2023
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TColorAjustFormAuxH
#define TColorAjustFormAuxH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "AdvSmoothButton.hpp"
#include <ExtCtrls.hpp>
#include "AdvSmoothToggleButton.hpp"
#include "TPreclassific.h"
#include "LTBeltDiscr_RGBHSI.h"
#include "TDataMng.h"

#define   C_LAST_VALUE       289
#define   C_FIRST_VALUE        1
#define   C_PASO_COLUMNA      10
#define   C_VAL_PER_COLUMN     9
#define   C_VAL_PER_ROW       29

#define   CATEGORIAS_COLOR    10

class TMyMachineModuleMngr;




class TColorAjustFormAux : public TDataManager
{
//__published:         // IDE-managed Components
   TPanel *Panel1;

   TComboBox* m_comboBox;

   std::vector<TPreclassificMngr*> m_preclassificMngrs;

   std::vector<LTBeltDiscr_RGBHSI*> m_ltBeltDiscrMngrs;

   TMyMachineModuleMngr *m_modMngr;

   void __fastcall BotonCerrarClick(TObject *Sender);
   void __fastcall onChange(TObject *Sender);
private:             // User declarations

protected:

//   TData       *m_daSMinFilaGris,
//               *m_daSMaxFilaGris,
//               *m_daSMinFilaColor,
//               *m_daSMaxFilaColor;

//   int m_sMinFilaGris,
//	   m_sMaxFilaGris,
//	   //tas max y min
//	   m_sMinFilaColor,
//	   m_sMaxFilaColor;



public:              // User declarations

   TColorAjustFormAux(const wchar_t*name, TMyMachineModuleMngr *modMngr);
   ~TColorAjustFormAux();
   void __fastcall Init(TPanel *panel);
   void __fastcall C1Click(TObject *Sender);
   void __fastcall DblClick(TObject *Sender);
   void __fastcall InitComboBox();
   void __fastcall UpAllButtons();
   void __fastcall GetMinMax(int &hmin, int &hmax,/* int &smin, int &smax,*/ int &imin, int &imax);

};
//---------------------------------------------------------------------------
extern PACKAGE TColorAjustFormAux *ColorAjustFormAux;
//---------------------------------------------------------------------------
#endif












//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
