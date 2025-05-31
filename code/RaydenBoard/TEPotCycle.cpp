//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TEPotCycle.h"
#include "AdvSmoothListBox.hpp"
#include "TAux.h"
#include "TConfigEPotFormAux.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvSmoothComboBox"
#pragma link "AdvSmoothListBox"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"


TEPotCycle *EPotCycle;

/******************************************************************************
                               TEPotCycle
*******************************************************************************/
//---------------------------------------------------------------------------
/**
   Constructor
   @param owner se le pasa al constructor del TForm
   @param manager, es el manager que se desea vincular a la interfaz que maneja este formulario
*/
//---------------------------------------------------------------------------
__fastcall TEPotCycle::TEPotCycle(TComponent* Owner,  int id)
   : TForm( (Classes::TComponent*)NULL), m_id (id){
   TPanel* m_panelToDock =  (TPanel*)Owner;
   if( m_panelToDock)   {
      this->SetParentComponent(m_panelToDock);
   }
   NCycle->Text = id;
}

void TEPotCycle::FillInfo(std::wstring &ePodDemosaic){
   ePodDemosaic += L"I,";
   wchar_t cant[MilisecWidth];
   swprintf(cant,L"%d",CBIlum->SelectedItemIndex+1);
   ePodDemosaic += cant;
   ePodDemosaic += L",";
   swprintf(cant,L"%0*d",MilisecWidth,InitIlum->Text.ToInt());
   ePodDemosaic += cant;
   ePodDemosaic += L",";
   swprintf(cant,L"%0*d",MilisecWidth,DuratIlum->Text.ToInt());
   ePodDemosaic += cant;
   ePodDemosaic += L";C,1,";
   swprintf(cant,L"%0*d",MilisecWidth,InitCam->Text.ToInt());
   ePodDemosaic += cant;
   ePodDemosaic += L",";
   swprintf(cant,L"%0*d",MilisecWidth,DuratCam->Text.ToInt());
   ePodDemosaic += cant;
   for(int i = 0; i < m_lines.size(); i++){
      m_lines.at(i)->FillInfo(ePodDemosaic,CBIlum->SelectedItemIndex+1);
   }
   ePodDemosaic += L";";
}

//---------------------------------------------------------------------------
/**
   SetCycleLines
   @param ilumTrigger   contains the selected ilum, the offset and the duration (I,4,23332,23334)
   @param cameraTrigger contains the selected camera, the offset and the duration (C,4,23332,23334)
   @param cameraLines   contains the lines for the camera (MONO_1,1.25,1;MONO_2,2.25,2;MONO_3,3.25,3)
   @param ilumOptions   the options to fill the combobox  (NIR,R,G,B)
   @param planeOptions  the options to fill the combobox  (R,G,B,NIRSAT,NIRNOSAT)
*/
void TEPotCycle::SetCycleLines( const char *ilumTrigger, const char *cameraTrigger, const char *cameraLines, wchar_t *ilumOptions, wchar_t *planeOptions)  {
   SetIlum(ilumTrigger,ilumOptions);
   SetCamera(cameraTrigger);
   SetCameraLines(cameraLines,planeOptions);
}

//---------------------------------------------------------------------------
/**
   SetIlum
   @param ilumTrigger   contains the selected ilum, the offset and the duration (I,4,23332,23334)
   @param ilumOptions   the options to fill the combobox  (NIR,R,G,B)
*/
void TEPotCycle::SetIlum(const char *ilumTrigger,wchar_t *ilumOptions){

   std::vector<std::wstring> tokensOptions;
   TAux::ExtractTokens( ilumOptions, tokensOptions, L",");
   CBIlum->Items->Clear();
   for(unsigned int i = 0 ; i < tokensOptions.size(); i++){
      TAdvSmoothListBoxItem      *cbItem;
      cbItem = CBIlum->Items->Add();
      cbItem->Caption = tokensOptions[i].c_str();
   }

   std::vector<std::string> tokensTrigger;
   TAux::ExtractTokens( ilumTrigger, tokensTrigger, ",");
   CBIlum->SelectedItemIndex = atoi(tokensTrigger[1].c_str())-1;
   InitIlum->Text = atoi(tokensTrigger[2].c_str());
   DuratIlum->Text = atoi(tokensTrigger[3].c_str());

}

//---------------------------------------------------------------------------
/**
   SetCamera
   @param cameraTrigger contains the selected camera, the offset and the duration (C,4,23332,23334)
*/
void TEPotCycle::SetCamera(const char *cameraTrigger)   {
   std::vector<std::string> tokensTrigger;
   TAux::ExtractTokens( cameraTrigger, tokensTrigger, ",");
   InitCam->Text = atoi(tokensTrigger[2].c_str());
   DuratCam->Text = atoi(tokensTrigger[3].c_str());
}

//---------------------------------------------------------------------------
/**
   SetCameraLines
   @param cameraLines   contains the lines for the camera (MONO_1,1.25,G;MONO_2,2.25,R;MONO_3,3.25,B)
   @param planeOptions  the options to fill the combobox  (R,G,B,NIRSAT,NIRNOSAT)
*/
void TEPotCycle::SetCameraLines(const char *cameraLines,wchar_t *planeOptions){
   std::vector<std::string> tokensLines;
   TAux::ExtractTokens( cameraLines, tokensLines, ";");
   int size = 0;
   for(unsigned int i = 0 ; i < tokensLines.size(); i++){
      TConfigEPotFormAux *form = new TConfigEPotFormAux(PanelAdd);
      form->Show();
      form->Top = i*form->Height;
      m_lines.push_back(form);

      size+=form->Height;
      form->SetCameraLine(tokensLines[i].c_str(),planeOptions);
   }
   PanelAdd->Height = size;
   if(size > PanelOptions->Height)  {
      VScrollBar->Visible = true;
      VScrollBar->PageSize = PanelAdd->Height/tokensLines.size();
      VScrollBar->SetParams(0, 0, PanelAdd->Height - (PanelOptions->Height - (PanelAdd->Height/tokensLines.size())));
   }
}

//---------------------------------------------------------------------------
/**
   ChangeTotalCycles
*/
void TEPotCycle::ChangeTotalCycles(bool inverted, int cantAnt, int cant)   {
   for (int i = 1; i <= m_lines.size();i++){
      if(m_id == 1){
         char str[10];
         if(!inverted){
            sprintf( str, "%d", i);
         }
         else{
            sprintf( str, "%d", 2-i);
         }
         m_lines[i-1]->ModifLineNumber(str, str);
         continue;
      }
      float aux,
            auxAnt;
      if(!inverted){
         aux = i +  (m_id-1) * 1.0 /cant;
         auxAnt = i +  (m_id-1) * 1.0 /cantAnt;
      }
      else{
         aux = (1 +  (m_id-1) * 1.0 /cant) - (i-1);
         auxAnt = (1 +  (m_id-1) * 1.0 /cantAnt) - (i-1);
      }
      char str[10],
           strAnt[10];
      sprintf( str, "%.2f", aux);
      sprintf( strAnt, "%.2f", auxAnt);
      m_lines[i-1]->ModifLineNumber(strAnt, str);
   }
}


//---------------------------------------------------------------------------
void __fastcall TEPotCycle::VScrollBarChange(TObject *Sender){
   PanelAdd->Top = -VScrollBar->Position;
}
//---------------------------------------------------------------------------
void __fastcall TEPotCycle::FormDestroy(TObject *Sender){
   m_lines.clear();
}
//---------------------------------------------------------------------------
void __fastcall TEPotCycle::MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y){
 // guardar evento y activar timer
   if( (m_sb = dynamic_cast<TSpeedButton*>(Sender)) != NULL) {
      m_event                 = m_sb->OnMouseDown;
      Timer4Btns->Enabled     = true;
   }
   if(Sender == PlusDuratIlum){
      DuratIlum->Text = DuratIlum->Text.ToInt() + 1;
   }
   else if(Sender == MinDuratIlum){
      DuratIlum->Text = DuratIlum->Text.ToInt() - 1;
   }
   else if(Sender == PlusInitIlum){
      InitIlum->Text = InitIlum->Text.ToInt() + 1;
   }
   else if(Sender == MinInitIlum){
      InitIlum->Text = InitIlum->Text.ToInt() - 1;
   }
   else if(Sender == PlusDuratCam){
      DuratCam->Text = DuratCam->Text.ToInt() + 1;
   }
   else if(Sender == MinDuratCam){
      DuratCam->Text = DuratCam->Text.ToInt() - 1;
   }
   else if(Sender == PlusInitCam){
      InitCam->Text = InitCam->Text.ToInt() + 1;
   }
   else if(Sender == MinInitCam){
      InitCam->Text = InitCam->Text.ToInt() - 1;
   }
}

//---------------------------------------------------------------------------
void __fastcall TEPotCycle::MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y){
   Timer4Btns->Enabled            = false;
   Timer4Btns->Interval           = 100;
   m_sb                           = NULL;

}

//---------------------------------------------------------------------------
void __fastcall TEPotCycle::Timer4BtnsTimer(TObject *Sender){
   if( m_sb)  {
      if( Timer4Btns->Interval > 1)          // aumentar velocidad de repetici?
         Timer4Btns->Interval /= 1.5;

      TShiftState Shift;                  // llamar evento
      m_event( m_sb, Controls::mbLeft, Shift, 0, 0);
   }

}

