/*******************************************************************************
Multiscan Technologies SL
Copyright  2009. All Rights Reserved.

Project:       MultiscanBeltSorter
Subsystem      MVS
File:          TEPotCycle.h

Author:        Software Group Multiscan Technologies SL <mvs@multiscan.eu>
Created:       10/01/2018 J Puerto

Overview:


*******************************************************************************/
//---------------------------------------------------------------------------

#ifndef TEPotCycleH
#define TEPotCycleH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "AdvSmoothComboBox.hpp"
#include "AdvSmoothListBox.hpp"
#include "ConfigEPotMngr.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "TViewInterface.h"

class TConfigEPotFormAux;

//---------------------------------------------------------------------------
class TEPotCycle : public TForm{
__published:
   TShape *Shape7;
   TLabel *DestLine_lb;
   TLabel *DestPlane_lb;
   TShape *Shape2;
   TShape *Shape4;
   TLabel *OrigLine_lb;
   TShape *Shape1;
   TShape *Shape5;
   TShape *Shape6;
   TImage *CamImg;
   TImage *IlumImg;
   TAdvSmoothComboBox *CBIlum;
   TImage *Init1Img;
   TEdit *InitIlum;
   TImage *Durat1Img;
   TSpeedButton *MinDuratIlum;
   TShape *MinDuratIlum_sh;
   TSpeedButton *PlusDuratIlum;
   TShape *PlusDuratIlum_sh;
   TEdit *DuratIlum;
   TImage *Init2Img;
   TSpeedButton *MinInitCam;
   TShape *MinInitCam_sh;
   TSpeedButton *PlusInitCam;
   TShape *PlusInitCam_sh;
   TEdit *InitCam;
   TImage *Durat2Img;
   TSpeedButton *MinDuratCam;
   TShape *MinDuratCam_sh;
   TSpeedButton *PlusDuratCam;
   TShape *PlusDuratCam_sh;
   TEdit *DuratCam;
   TLabel *Miliseconds1_lb;
   TLabel *Miliseconds2_lb;
   TPanel *PanelOptions;
   TPanel *PanelAdd;
   TScrollBar *VScrollBar;
   TEdit *LinesInfo;
   TEdit *NumCycle;
   TEdit *NumCycles;
   TLabel *Label1;
   TLabel *Label2;
   TSpeedButton *MinInitIlum;
   TShape *MinInitIlum_sh;
   TSpeedButton *PlusInitIlum;
   TShape *PlusInitIlum_sh;
   TTimer *Timer4Btns;
   TShape *Shape3;
   TShape *Shape8;
   TShape *Shape9;
   TShape *Shape11;
   TShape *Shape10;
   TEdit *NCycle;
   void __fastcall VScrollBarChange(TObject *Sender);
   void __fastcall FormDestroy(TObject *Sender);
   void __fastcall MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
   void __fastcall MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
   void __fastcall Timer4BtnsTimer(TObject *Sender);


public:

   static const int MilisecWidth = 5;

   __fastcall TEPotCycle(TComponent* Owner,int id);
   void SetNumCycles(int cant);
   void DemosaicString(wchar_t* cadena);
   void SetCycleLines( const char *ilumTrigger, const char *cameraTrigger, const char *cameraLines, wchar_t *ilumOptions, wchar_t *planeOptions);
   void ChangeTotalCycles(bool inverted, int cantAnt,int cant);
   void FillInfo(std::wstring &ePodDemosaic);
   void SetId(int id)   {
      m_id = id;
   }

private:

   TSpeedButton                   *m_sb;
   TMouseEvent                    m_event;

   unsigned int m_id;

   std::vector<TConfigEPotFormAux*> m_lines;

   void SetIlum(const char *ilumTrigger,wchar_t *ilumOptions);
   void SetCamera(const char *cameraTrigger);
   void SetCameraLines(const char *cameraLines,wchar_t *planeOptions);

};
//---------------------------------------------------------------------------
extern PACKAGE TEPotCycle *EPotCycle;
//---------------------------------------------------------------------------
#endif
