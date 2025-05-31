//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TConfigEPotForm.h"
#include "ConfigEPotMngr.h"
#include "AdvSmoothListBox.hpp"
#include "TAux.h"
#include "TConfigEPotFormAux.h"
#include "TEPotCycle.h"
#include "TDemosaic.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TConfigEPotForm *ConfigEPotForm;



/******************************************************************************
                               TConfigEPotInterface
*******************************************************************************/

//---------------------------------------------------------------------------
/**
   Constructor
   @param mngr, es el manager que se desea vincular a la interfaz que maneja este formulario
   @param form
*/
__fastcall TConfigEPotInterface::TConfigEPotInterface( TDataManager *mngr, TForm *form )
   :TViewInterface( mngr, form)   {
   SetComponents();
   m_mngr = mngr;
}

//---------------------------------------------------------------------------
/**
   Destructor
*/
TConfigEPotInterface::~TConfigEPotInterface(){
   ClearVector();
}

//---------------------------------------------------------------------------
/**
   ClearVector
*/
void TConfigEPotInterface::ClearVector(){
   for(unsigned int i = 0; i < m_cycles.size(); i++){
      m_cycles.at(i)->Release();
   }
   m_cycles.clear();
}


//---------------------------------------------------------------------------
/**
   SetComponents
*/
void TConfigEPotInterface::SetComponents()   {
   m_scrollBar        = dynamic_cast<TScrollBar *> (m_view->FindComponent(L"VScrollBar"));
   m_panelOptions     = dynamic_cast<TPanel *>     (m_view->FindComponent(L"PanelOptions"));
   m_panelAdd         = dynamic_cast<TPanel *>     (m_view->FindComponent(L"PanelAdd"));
   m_combEpotDemosaic = dynamic_cast<TEdit *>      (m_view->FindComponent(L"CombEpotDemosaic"));
   m_shots            = dynamic_cast<TEdit *>      (m_view->FindComponent(L"Shots"));
   m_dir              = dynamic_cast<TEdit *>      (m_view->FindComponent(L"Direction"));
}

//---------------------------------------------------------------------------
/**
   RefreshInterface
*/
bool   TConfigEPotInterface::RefreshInterface ( TNode *node){
   bool ret = TViewInterface::RefreshInterface( node);
   TData *da = dynamic_cast<TData*>(node);
   if ( da ==NULL || m_view==NULL)   {
      return ret;
   }
   else if ( da->AreYou( LBL_COMB_EPOT_DEMOSAIC))   {
      ResetCyclos( da);
      UpdateScrollbar();
   }
   return ret;
}

//---------------------------------------------------------------------------
/**
   CloseWithSave
*/
void TConfigEPotInterface::CloseWithSave()   {
   std::wstring ePodDemosaic;

   for(unsigned int i = 0; i < m_cycles.size(); i++){
      m_cycles[i]->FillInfo(ePodDemosaic);
   }
   m_combEpotDemosaic->Text = ePodDemosaic.c_str();
   ClearVector();
}

//---------------------------------------------------------------------------
/**
   Close
*/
void TConfigEPotInterface::Close()   {
   ClearVector();
}


//---------------------------------------------------------------------------
/**
   ResetCyclos
*/
void TConfigEPotInterface::ResetCyclos( TData *data)  {

   if ( !data) {
      return;
   }
   unsigned int sizeComb = ConfigEPotMngr::LinesInfoMax;
   char *combined = new char[sizeComb];
   if ( !data->AsText( combined, sizeComb))  {
      delete [] combined;
      combined = new char[sizeComb];
   }

   unsigned int sizeIlum       = SIZEOF_CHAR(m_ilums)       -1;
   unsigned int planeNameSize  = SIZEOF_CHAR(m_planeNames)  -1;
   unsigned int CameraLineSize = SIZEOF_CHAR(m_cameraLines) -1;

   data->GetAttribValue( LBL_ILUMS,        m_ilums,       sizeIlum);
   data->GetAttribValue( LBL_PLANE_NAMES,  m_planeNames,  planeNameSize);
   data->GetAttribValue( LBL_CAMERA_LINES, m_cameraLines, CameraLineSize);

   std::vector<std::string> tokCombined;
   TAux::ExtractTokens( combined, tokCombined, ";");

   std::vector<std::wstring> tokensIlum;
   TAux::ExtractTokens( m_ilums, tokensIlum, L",");

   ClearVector();

   unsigned int sizeForm = 0,
                 numForm = 0;

   //Foreach line (each 2 tokens from combined)              
   for(int i = 0; i<tokCombined.size(); i+=2,numForm++)   {
      TEPotCycle *form = new TEPotCycle(m_panelAdd,numForm+1);
      form->Show();
      m_cycles.push_back(form);

      std::string camLines;
      GenerateCamLines(camLines,tokCombined[i+1].c_str());
      
      form->SetCycleLines(tokCombined[i].c_str(),tokCombined[i+1].c_str(), camLines.c_str() , m_ilums, m_planeNames);
      form->Top = numForm*form->Height;
      sizeForm += form->Height;
   }
   m_shots->Text =  tokCombined.size()/2;
   m_panelAdd->Height = sizeForm;

   delete [] combined;
}

//---------------------------------------------------------------------------
/**
   GenerateCamLines
*/
void TConfigEPotInterface::GenerateCamLines(std::string &camLines,const char *cameraStr ){

   std::vector<std::wstring> tokensDestNames;
   TAux::ExtractTokens( m_cameraLines, tokensDestNames, L",");

   std::vector<std::wstring> tokensPlaneNames;
   TAux::ExtractTokens( m_planeNames, tokensPlaneNames, L",");
   
   //Generate the CamLines info (MONO_1,1.25,2;MONO_2,2.25,2;...)
   std::vector<std::string> tokCamLines;
   TAux::ExtractTokens(cameraStr, tokCamLines, ",");
   std::vector<std::string> linesNumber;
   GetLinesNumber( tokCamLines, linesNumber);
   int linesNumberSize = linesNumber.size();
    //j = 5 (the first 4 stands for cam info, j+=3 (each line uses 3 tokens)
   for(int j = 0; j < tokensDestNames.size() ; j++) {
      char *str =  TAux::WSTR_TO_STR(tokensDestNames[j].c_str());
      camLines += str;
      delete [] str;
      camLines += ',';
      camLines += j < linesNumberSize?linesNumber[j].c_str():"";
      camLines += ',';
      unsigned int numOfToken = j*3 + 5; //the first start number of interest is in 5 (0+5) and there are 3 num in between each (j*3)
      if(numOfToken >= tokCamLines.size()){
         camLines += '1';
      }
      else{
         for(int k = 0 ; k < tokensPlaneNames.size(); k++)  {
            char *str =  TAux::WSTR_TO_STR(tokensPlaneNames[k].c_str());
            if(  stricmp( str , tokCamLines[numOfToken].c_str() ) == 0  ) {
               char str[8];
               sprintf( str, "%d", k);
               camLines += str;
            }
            delete [] str;
         }
      }
      camLines += ';';
   }
}
//---------------------------------------------------------------------------
/**
   GetLinesNumber
*/
void TConfigEPotInterface::GetLinesNumber( std::vector<std::string> &tokCamLines, std::vector<std::string> &linesNumber) {

   int size = tokCamLines.size();
   int i=FirstIndexDestLine;
   while ( i  < size)   {
      linesNumber.push_back( tokCamLines[i]);
      i = i+IndexDestLineGap; //each subline contains 3 elements: destination line, plane name and ilumination
   }

}


//---------------------------------------------------------------------------
/**
   UpdateCycles
*/
void TConfigEPotInterface::UpdateCycles(bool add){
   int cant = m_cycles.size();
   int cantAnt = add?cant-1:cant+1;
   for(unsigned int i = 0; i < m_cycles.size(); i++){
      m_cycles.at(i)->ChangeTotalCycles(m_dir->Text.ToInt(), cantAnt, cant);
   }
   m_shots->Text =  cant;
   UpdateScrollbar();
}

//---------------------------------------------------------------------------
/**
   AddCyclos
*/
void TConfigEPotInterface::AddCyclos()  {
   if(m_cycles.size()>=MaxCycles){ //For prevent to much cycles
      return;
   }
   std::vector<std::wstring> tokensIlum;
   std::vector<std::wstring> tokensDestNames;
   TAux::ExtractTokens( m_ilums, tokensIlum, L",");
   TAux::ExtractTokens( m_cameraLines, tokensDestNames, L",");

   TEPotCycle *form = new TEPotCycle(m_panelAdd,m_cycles.size()+1);
   form->Show();
   m_cycles.push_back(form);
   form->Top = m_panelAdd->Height;
   m_panelAdd->Height += form->Height;

   std::string lines;

   for(int i = 0 ; i < tokensDestNames.size() ; i++) {
      char *str =  TAux::WSTR_TO_STR(tokensDestNames[i].c_str());
      lines += str;
      delete [] str;
      lines += ",-9999,0;";
   }

   form->SetCycleLines("I,1,0,0","C,1,0,0", lines.c_str() , m_ilums, m_planeNames);
   UpdateCycles(true);
}

//---------------------------------------------------------------------------
/**
   DeleteCyclos
*/
void TConfigEPotInterface::DeleteCyclos()   {
   if( m_cycles.size() == 0 )    {
      return;
   }
   m_panelAdd->Height -= m_cycles.at(m_cycles.size()-1)->Height;
   m_cycles.at(m_cycles.size()-1)->Release();
   m_cycles.pop_back();
   UpdateCycles(false);
}

//---------------------------------------------------------------------------
/**
   UpdateScrollbar
*/
void TConfigEPotInterface::UpdateScrollbar()    {
    if( m_panelAdd->Height > m_panelOptions->Height )   {
      m_scrollBar->Visible = true;
      m_scrollBar->PageSize = m_panelAdd->Height/m_cycles.size();
      m_scrollBar->SetParams(0, 0, m_panelAdd->Height - (m_panelOptions->Height - (m_panelAdd->Height/m_cycles.size())));
   }
   else    {
      m_scrollBar->Visible = false;
   }
}


/******************************************************************************
                               TConfigEPotForm
*******************************************************************************/
//---------------------------------------------------------------------------
/**
   Constructor
   @param owner se le pasa al constructor del TForm
   @param manager, es el manager que se desea vincular a la interfaz que maneja este formulario
*/
//---------------------------------------------------------------------------
__fastcall TConfigEPotForm::TConfigEPotForm(TComponent* Owner, TDataManager *mn)
   : TView(Owner){

   m_PotIntf = new TConfigEPotInterface( mn, this);
   m_PotIntf->SetEnabled(false);

   TPanel* m_panelToDock =  (TPanel*)Owner;
   if( m_panelToDock)   {
      this->SetParentComponent(m_panelToDock);
   }

}
//---------------------------------------------------------------------------
void __fastcall TConfigEPotForm::VScrollBarChange(TObject *Sender){
   PanelAdd->Top = - VScrollBar->Position;
}
//---------------------------------------------------------------------------
void __fastcall TConfigEPotForm::OnChange(TObject *Sender){
   m_PotIntf->OnChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TConfigEPotForm::FormShow(TObject *Sender){
   m_PotIntf->SetEnabled(true);
}
//---------------------------------------------------------------------------

void __fastcall TConfigEPotForm::FormClose(TObject *Sender, TCloseAction &Action){
   m_PotIntf->SetEnabled(false);
}
//---------------------------------------------------------------------------


void __fastcall TConfigEPotForm::FormCloseBtn(TObject *Sender){
   m_PotIntf->Close();
   Close();
}
//---------------------------------------------------------------------------

void __fastcall TConfigEPotForm::FormDestroy(TObject *Sender){
   delete m_PotIntf;
   m_PotIntf = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TConfigEPotForm::ShotsPerCycleChange(TObject *Sender){
   m_PotIntf->OnChange(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TConfigEPotForm::btn_next_verifClick(TObject *Sender){
   Close();
   m_PotIntf->CloseWithSave();
}
//---------------------------------------------------------------------------

void __fastcall TConfigEPotForm::ShotsPerCycle_A01Click(TObject *Sender){
   m_PotIntf->AddCyclos();
}
//---------------------------------------------------------------------------
void __fastcall TConfigEPotForm::ShotsPerCycle_I01Click(TObject *Sender){
   m_PotIntf->DeleteCyclos();
}
//---------------------------------------------------------------------------



