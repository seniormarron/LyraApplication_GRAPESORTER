//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TConfigEPotFormAux.h"
#include "TAux.h"
#include "vector.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvSmoothComboBox"
#pragma link "AdvSmoothListBox"
#pragma resource "*.dfm"



TConfigEPotFormAux *ConfigEPotFormAux;
//---------------------------------------------------------------------------
/**
   TConfigEPotFormAux
*/
__fastcall TConfigEPotFormAux::TConfigEPotFormAux(TComponent* Owner)
   : TForm((Classes::TComponent*)NULL){
  TPanel* m_panelToDock =  (TPanel*)Owner;
   if( m_panelToDock)   {
      this->SetParentComponent(m_panelToDock);
   }
}

//---------------------------------------------------------------------------
/**
   FillInfo
*/
void TConfigEPotFormAux::FillInfo(std::wstring &ePodDemosaic,int ilum) {
   ePodDemosaic += L",";
   ePodDemosaic += Line->Text.c_str();
   ePodDemosaic += L",";
   ePodDemosaic += CBCameraLines->Items->SelectedItem->Caption.c_str();
   ePodDemosaic += L",";
   wchar_t cant[4];
   swprintf(cant,L"%d",ilum);
   ePodDemosaic += cant;
}

//---------------------------------------------------------------------------
/**
   SetCameraLine
   @param line   contains the lines for the camera (MONO_1,1.25,1)
   @param planeOptions  the options to fill the combobox  (R,G,B,NIRSAT,NIRNOSAT)
*/
void TConfigEPotFormAux::SetCameraLine( const char *line, wchar_t *planeOptions){
   std::vector<std::string> tokensLine;
   TAux::ExtractTokens( line, tokensLine, ",");
   OriName->Caption = tokensLine[0].c_str();
   Line->Text    = tokensLine[1].c_str();

   std::vector<std::wstring> tokensOptions;
   TAux::ExtractTokens( planeOptions, tokensOptions, L",");
   CBCameraLines->Items->Clear();
   for(unsigned int i = 0 ; i < tokensOptions.size(); i++){
      TAdvSmoothListBoxItem      *cbItem;
      cbItem = CBCameraLines->Items->Add();
      cbItem->Caption = tokensOptions[i].c_str();
   }
   CBCameraLines->SelectedItemIndex = atoi(tokensLine[2].c_str());
}
//---------------------------------------------------------------------------


