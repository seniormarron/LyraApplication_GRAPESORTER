//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TImageSignalForm.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#include <vcl.h>
#pragma hdrstop

#include "TImageSignalForm.h"
//#include "TMyMachineModuleMngr.h"
#include "TImageSignalMngr.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Chart"
#pragma link "Series"
#pragma resource "*.dfm"
TImageSignalForm *ImageSignalForm;

 extern UINT msg_paint_image;

/******************************************************************************
                               TFormImageSignalInterface
*******************************************************************************/

//---------------------------------------------------------------------------
/**
   Constructor
   @param mngr, es el manager que se desea vincular a la interfaz que maneja este formulario
   @param form
*/
__fastcall TImageSignalFormInterface::TImageSignalFormInterface( TDataManager *mngr, TForm *form )
   :TViewInterface( mngr, form)                                                {
   m_form = dynamic_cast<TImageSignalForm*>(form);
}
/**
   Destructor
*/
TImageSignalFormInterface::~TImageSignalFormInterface(){
   for(int i = 0 ; i < MAX_SIGNALS; i++){
      if(m_signalAndIndex[i].data != NULL){
         delete m_signalAndIndex[i].data;
         m_signalAndIndex[i].data = NULL;
         memset( m_signalAndIndex[i].signalName, 0x00, sizeof(*m_signalAndIndex[i].signalName));
      }
   }
}
/**
   ResetSignals
   delete all the signals
*/
void  TImageSignalFormInterface::ResetSignals(){
   for(int i = 0 ; i < MAX_SIGNALS; i++){
	  if(m_signalAndIndex[i].data != NULL){
		 delete m_signalAndIndex[i].data;
		 m_signalAndIndex[i].data = NULL;
		 memset( m_signalAndIndex[i].signalName, 0x00, sizeof(*m_signalAndIndex[i].signalName));
	  }
   }
}

//-----------------------------------------------------------------------------
/**
   Gets value from a control in a view
   @param      TComponent  control of a View
   @param      text        array of wchar_t where value of the control is returned
   @param      size        size of the array
   @param      data        needed to get aatributes( format, list of ints,...)
   @return     true  when success
*/

bool TImageSignalFormInterface::GetValAsText( TComponent *c, wchar_t *text, int size, TData *data )  {

   if( dynamic_cast<TSpeedButton*>(c))                                   {
	  TSpeedButton   &sb   = *(TSpeedButton *)c;
	  swprintf( text, L"%d", !sb.Down);
	  return true;
   }

   if (TViewInterface::GetValAsText( c, text, size, data ))
	  return true;

   return false;

}
/**
   RefreshInterface
*/
bool   TImageSignalFormInterface::RefreshInterface ( TNode *tdata)             {
   bool ret =   TViewInterface::RefreshInterface( tdata);
   if ( tdata ==NULL || m_view==NULL)
   return ret;
   //clear the form
   if ( tdata->AreYou( L"Clear")){
	  m_form->Clear();
   }
   if(tdata->AreYou( L"BtnReset")){
	  if(m_form != NULL){
		for(int i = 0 ; i < MAX_SIGNALS; i++){
			 m_signalAndIndex[i].max = 0;
			 m_signalAndIndex[i].min = 9999;
		  }
      }
   }
   if ( tdata->AreYou( L"ScaleDepth")){
	  TData *da = dynamic_cast<TData*>(tdata);
	  if(da->AsInt() > 0)
		m_form->setScaleDepth(da->AsInt());
   }
   //Refresh the average, max, min, etc
   else if (tdata->AreYou( L"Calculate_Info")){
	  if(m_form != NULL){
		 calculateInfo();
      }
   }
   //Refresh the average, max, min, etc
   else if (tdata->AreYou( L"Signls")||tdata->AreYou( L"x1")||tdata->AreYou( L"x2")){
	  if(m_form != NULL){
		 calculateInfo();
		 for(int i = 0 ; i < MAX_SIGNALS; i++){
			 m_signalAndIndex[i].max = 0;
			 m_signalAndIndex[i].min = 9999;
		  }
	  }

   }
   //if the tdata name string contains "Signal_"
   else if ( wcsncmp(tdata->GetName(),LB_SIGNAL_NAME, wcslen(LB_SIGNAL_NAME)) == 0)         {
	  wchar_t name[32];
      unsigned int count = SIZEOF_CHAR(name);
      tdata->GetAttribValue(cAttribNames[ATTR_DESCRIPTION], name, count);
	  int i = 0;
	  while( true ){
		 //search the signal if previous created or the first empty space
		 if(wcscmp(m_signalAndIndex[i].signalName, name) == 0 || m_signalAndIndex[i].signalName[0] == 0){
			unsigned int size = 0;

			TAttrib *valueAttrib = tdata->GetAttrib( cAttribNames[ATTR_VALUE]);
			if (valueAttrib ==NULL)
			   return ret;
			size = valueAttrib->GetDataSize();
			if (size==0)
			   return ret;
			int numPixels = size / sizeof(WORD);

			if( m_signalAndIndex[i].signalName[0] == 0 ){
			   wcsncpy( m_signalAndIndex[i].signalName, name, SIZEOF_CHAR( m_signalAndIndex[i].signalName) );
			   m_signalAndIndex[i].signalName[ SIZEOF_CHAR( m_signalAndIndex[i].signalName) -1]= 0;
			   m_signalAndIndex[i].data = new WORD[numPixels];
			   m_signalAndIndex[i].width = numPixels;
			   m_signalAndIndex[i].max = 0;
			   m_signalAndIndex[i].min = 9999;
			}
			else if ( numPixels !=  m_signalAndIndex[i].width) {
			   //The signal size is diferent
			   ResetSignals();
               break;
			}
			TData *da = dynamic_cast<TData*>(tdata);
			if ( da->AsBin( (void*)m_signalAndIndex[i].data, size))   {
			   m_form->Add(m_signalAndIndex[i].data, numPixels, name);
			}
            break;
         }
         else if(i==9)
			break;
		 i++;
	  }
   }
   return ret;
}
// ---------------------------------------------------------------------------
/**
   calculateInfo
  Calculate and refresh the signal info
 */
void TImageSignalFormInterface::calculateInfo(){
   int signal = m_form->getSignalSelected();
   if(signal >= 0)                              {
	  int x1 = m_form->getMin();
	  int x2 = m_form->getMax();
	  int average = getAverage(signal,x1,x2);
      m_form->setAverage(average);
	  m_form->setDeviation(getDeviation(average,signal,x1,x2));
	  m_form->setMin(getMinimum(signal,x1,x2));
	  m_form->setMax(getMaximum(signal,x1,x2));
	  m_form->setHistoricMin(m_signalAndIndex[signal].min);
	  m_form->setHistoricMax(m_signalAndIndex[signal].max);
   }
}

// ---------------------------------------------------------------------------
/**
   getAverage
 Calculate the average of the signal
   @param serie, number of the signal
   @param x1, first number of the signal
   @param x1, last number of the signal
 */
int TImageSignalFormInterface::getAverage(int serie, int x1, int x2){
   if (m_form == NULL || m_signalAndIndex[serie].data == NULL){
      return 0;
   }
   int suma = 0;
   int total = 0;
   int width = m_signalAndIndex[serie].width;
   int limSup = min((int)m_signalAndIndex[serie].width, x2);
   int num = limSup - x1 -1;
   if ( num <=0)                       {
      return 0;
   }
   for (int i = x1+1; i < limSup; i++) {
      suma+= m_signalAndIndex[serie].data[i];
   }
   return suma/num;
}
// ---------------------------------------------------------------------------
/**
   getDeviation
 Calculate the deviation of the signal
   @param average, average of the signal
   @param serie, number of the signal
   @param x1, first number of the signal
   @param x1, last number of the signal
 */
int TImageSignalFormInterface::getDeviation(int average, int serie, int x1, int x2){
   if (m_form == NULL || m_signalAndIndex[serie].data == NULL){
      return 0;
   }

   int powSum = 0;
   int limSup = min((int) m_signalAndIndex[serie].width, x2);
   int num = limSup - x1 -1;

   for (int i = x1; i < limSup+1; i++) {
      powSum +=  pow(m_signalAndIndex[serie].data[i] - average,2.0);
   }
   return sqrt((1.0f/(num-1)*powSum));
}
// ---------------------------------------------------------------------------
/**
   getMaximum
 Calculate the maximum of the signal
   @param serie, number of the signal
   @param x1, first number of the signal
   @param x1, last number of the signal
 */
int TImageSignalFormInterface::getMaximum(int serie, int x1, int x2){
    if (m_form == NULL || m_signalAndIndex[serie].data == NULL){
      return 0;
    }

   int max =  m_signalAndIndex[serie].data[x1];
   int limSup = min((int)m_signalAndIndex[serie].width, x2);

   for (int i = x1 ; i < limSup+1; i++) {
	  if( m_signalAndIndex[serie].data[i] > max ){
         max = m_signalAndIndex[serie].data[i];
	  }
	  if(m_signalAndIndex[serie].max < max)
		m_signalAndIndex[serie].max = max;
   }
   return max;
}
// ---------------------------------------------------------------------------
/**
   getMinimum
 Calculate the minimum of the signal
   @param serie, number of the signal
   @param x1, first number of the signal
   @param x1, last number of the signal
 */
int TImageSignalFormInterface::getMinimum(int serie, int x1, int x2){

    if (m_form == NULL || m_signalAndIndex[serie].data == NULL){
      return 0;
    }
   int mini = m_signalAndIndex[serie].data[x1];
   int limSup = min((int)m_signalAndIndex[serie].width, x2);

	for (int i = x1; i < limSup+1; i++) {
	  if( m_signalAndIndex[serie].data[i] < mini ) {
         mini = m_signalAndIndex[serie].data[i];
	  }
	  if(mini != 120 && m_signalAndIndex[serie].min > mini)
		m_signalAndIndex[serie].min = mini;
   }
   return mini;
}




/******************************************************************************
                               TImageSignalForm
*******************************************************************************/
//---------------------------------------------------------------------------
/**
   Constructor
   @param owner se le pasa al constructor del TForm
   @param manager, es el manager que se desea vincular a la interfaz que maneja este formulario
*/
__fastcall TImageSignalForm::TImageSignalForm(TComponent* Owner, TDataManager *mn)
    : TView(Owner)                                                              {
   numSeries       = 0;
   m_intf      = new TImageSignalFormInterface( mn, this);
   for(int i = 0 ; i < MAX_SIGNALS; i++){
		 m_signalAndIndex[i].data = NULL;
		 m_series[i] = NULL;
		 memset( m_signalAndIndex[i].signalName, 0x00, sizeof(*m_signalAndIndex[i].signalName));
   }
   Close();
}
/**
   Destructor
*/
__fastcall TImageSignalForm::~TImageSignalForm(){
	//no borro el interface porque al no tener el SetDeleteMe a false, se destruira automaticamente
	for(int i = 0 ; i < MAX_SIGNALS ; i++){
		if(m_series[i])
			delete m_series[i];
	}
}
 void __fastcall TImageSignalForm::OnChange(TObject *Sender)                {
   m_intf->OnChange( Sender);
}



//---------------------------------------------------------------------------
/**
   Add
   Add a signal point in the grid
   @param signal signal to add the point
   @param value of the point
*/
void TImageSignalForm::Add(WORD* data,int size, UnicodeString name ){
	int i = 0;
	while( true ){
		//search the signal if previous created or the first empty space
		if(wcscmp(m_signalAndIndex[i].signalName, name.c_str()) == 0 || m_signalAndIndex[i].signalName[0] == 0){
			wcsncpy( m_signalAndIndex[i].signalName, name.c_str(), SIZEOF_CHAR( m_signalAndIndex[i].signalName) );
			m_signalAndIndex[i].signalName[ SIZEOF_CHAR( m_signalAndIndex[i].signalName) -1]= 0;
			m_signalAndIndex[i].width = size;
			m_signalAndIndex[i].data = data;
			if( m_series[i] == NULL){
				m_series[i] = new TLineSeries(this);
				Chart1->AddSeries(m_series[i]);
			}
			else{
				m_series[i]->Clear();
            }
			//serie->LinePen->Width = 2;
			if(name.Compare("R") == 0){
				m_series[i]->LinePen->Color = clRed;
			}
			else if(name.Compare("G") == 0){
				m_series[i]->LinePen->Color = clGreen;
			}
			else if(name.Compare("B") == 0){
				m_series[i]->LinePen->Color = clBlue;
			}
			for(int j = 0 ; j < m_signalAndIndex[i].width ; j++){
            	m_series[i]->Add(m_signalAndIndex[i].data[j]);
			}
			//m_series[i]->AddArray((double *)m_signalAndIndex[i].data,m_signalAndIndex[i].width);
			m_series[i]->Title = name;
			return;
		}
		else if(i==MAX_SIGNALS+1)
			return;
		i++;
	}

}
//---------------------------------------------------------------------------
/**
   Clear
   clear the form
*/
void TImageSignalForm::Clear()      {
	 for(int i = 0 ; i < MAX_SIGNALS ; i++){
		if(m_series[i])m_series[i]->Clear();
	 }
}

//---------------------------------------------------------------------------
/**
*  Close the panel
*/
void __fastcall TImageSignalForm::CloseBtn__Click(TObject *Sender)             {
   m_intf->OnChange( Sender);
   Close();
}
//---------------------------------------------------------------------------
/**
   getSignalSelected
   return the combobox selected signal
*/
int TImageSignalForm::getSignalSelected(){
    return Signls->ItemIndex;
}
//---------------------------------------------------------------------------
/**
   getMax
   return the x2 tedit value
*/
int TImageSignalForm::getMax(){
	if (x2->Text.c_str()[0] != 0)
		return x2->Text.ToDouble();
	else
		return 0;
}
//---------------------------------------------------------------------------
/**
   getMin
   return the x1 tedit value
*/
int TImageSignalForm::getMin(){
	if (x1->Text.c_str()[0] != 0)
		return x1->Text.ToDouble();
	else
		return 0;
}
//---------------------------------------------------------------------------
/**
   setAverage
   set value to averaje tedit
*/
void TImageSignalForm::setAverage(int x){
	 average->Text = x;
}
//---------------------------------------------------------------------------
/**
   setDeviation
   set value to deviation tedit
*/
void TImageSignalForm::setDeviation(int x){
     deviation->Text = x;
}
//---------------------------------------------------------------------------
/**
   setMin
   set value to minimum tedit
*/
void  TImageSignalForm::setMin(int x){
     minimum->Text = x;
}
//---------------------------------------------------------------------------
/**
   setHistoricMax
   set value to maximum tedit
*/
void  TImageSignalForm::setHistoricMax(int x){
	 HistoricMax->Text = x;
}
//---------------------------------------------------------------------------
/**
   setHistoricMin
   set value to minimum tedit
*/
void  TImageSignalForm::setHistoricMin(int x){
	 if(x != 9999)
		HistoricMin->Text = x;
	 else
	 	HistoricMin->Text = 0;
}
//---------------------------------------------------------------------------
/**
   setMax
   set value to maximum tedit
*/
void  TImageSignalForm::setMax(int x){
	 maximum->Text = x;
}
//---------------------------------------------------------------------------
/**
   setScaleDepth
*  set the maximum value for Y axis
*/
 void  TImageSignalForm::setScaleDepth(int x)                     {
   Chart1->LeftAxis->Maximum = x;
}
//---------------------------------------------------------------------------
/**
   gridXClick
*  Show the grid in X axis
*/
void __fastcall TImageSignalForm::gridXClick(TObject *Sender){
	 Chart1->LeftAxis->Grid->Visible = gridX->Checked;
}
//---------------------------------------------------------------------------
/**
   gridYClick
*  Show the grid in Y axis
*/
void __fastcall TImageSignalForm::gridYClick(TObject *Sender){
    Chart1->BottomAxis->Grid->Visible = gridY->Checked;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
   FormShow
*  Show the form
*/
void __fastcall TImageSignalForm::FormShow(TObject *Sender){
   m_intf->SetEnabled(true);
}
//---------------------------------------------------------------------------

void __fastcall TImageSignalForm::FormClose(TObject *Sender, TCloseAction &Action){
   m_intf->SetEnabled(false);
}
//---------------------------------------------------------------------------

void __fastcall TImageSignalForm::BtnResetClick(TObject *Sender){
	m_intf->OnChange( Sender);
	HistoricMax->Text = 0;
	HistoricMin->Text = 0;
}
//---------------------------------------------------------------------------





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------

