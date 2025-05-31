//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TImageSignalMngr.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#pragma hdrstop

#include "TImageSignalMngr.h"
#include "TModuleMngr.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)



// ---------------------------------------------------------------------------
/**
    Constructor
    @param
    @return
 */
TImageSignalMngr::TImageSignalMngr(const wchar_t *name, TProcess *_parent)
    : TProcess(name, _parent) {
   m_daSignlsComboBox = new TData( this, L"SignlsComboBox",    m_daSignals,      L"R;G;B;NIR", SIZEOF_CHAR(m_daSignals),       FLAG_NORMAL, L"Name of the planes that will appear in the graphic (10 max)" );
   m_daSignalsList    = new TData( this, L"Signls",            NULL, 0,      L"",        FLAG_NORMAL);
   m_daScaleDepth     = new TData( this, L"ScaleDepth",        NULL, 4095, 0,10000,NULL, FLAG_NORMAL | FLAG_RDONLY, L"Scale depth of the Y axis.");
   m_x1               = new TData( this, L"x1",                NULL, 0, 0,   4000, NULL, FLAG_NORMAL,               L"First pixel to calculate the average, deviation, maximum and minimum");
   m_x2               = new TData( this, L"x2",                NULL, 0, 2000,4000, NULL, FLAG_NORMAL,               L"x2 Last pixel to calculate the average, deviation, maximum and minimum");
   m_daClear          = new TData( this, L"Clear",             NULL, 0, 0,   1,    NULL, (FLAG_NORMAL | FLAG_RDONLY | FLAG_SAVE_NO_VALUE) & ~FLAG_SAVE  , L"Tdata for reset the signals. ");
   m_daReset          = new TData( this, L"Reset",             NULL, 0, 0,   1,    NULL, (FLAG_NORMAL | FLAG_RDONLY | FLAG_SAVE_NO_VALUE) & ~FLAG_SAVE  , L"Tdata for clearing the graphic.");
   m_daResetBtn       = new TData( this, L"BtnReset",          NULL, 0, 0,   1,    NULL, FLAG_NORMAL, L"Tdata for reset historic values.");
   m_daCloseButton    = new TData( this, L"BtnClose",          NULL, 0, 0,   1,    NULL, FLAG_NORMAL,               L"Tdata for stop proccess. ");
   m_daCalculate      = new TData( this, L"Calculate_Info",    NULL, 0, 0,   1,    NULL, (FLAG_NORMAL | FLAG_RDONLY | FLAG_SAVE_NO_VALUE) & ~FLAG_SAVE  , L"Tdata for calculate the average, deviation, etc.");
   m_daThresholdMs    = new TData( this, L"ThresholdMs",       NULL, 1000, 0,10000,NULL,  FLAG_NORMAL,                L"Minimum time for refresh form info in miliseconds.");
   m_daShowForm       = new TData( this, L"ShowForm",          NULL, 0, 0,   1,    NULL, (FLAG_NORMAL | FLAG_SAVE_NO_VALUE) & ~FLAG_SAVE  , L"show form for visualice ilumination signals");

   m_daSignal = new TData*[MAX_SIGNALS];
   for(int i = 0 ; i < MAX_SIGNALS ; i++)                                     {
      m_daSignal[i] = NULL;
   }

   this->DataModEventPost = DataModifiedPost;

   m_signalForm == NULL;
   m_img       = NULL;
   m_closed    = false;
   m_lastTime  = 0;
   m_tokensDestinations.clear();

   TAux::ExtractTokens( m_daSignals, m_tokensDestinations );
}
// ---------------------------------------------------------------------------
/**
 Destructor
 */
TImageSignalMngr::~TImageSignalMngr()                                         {
   if (m_daSignal != NULL)                                                    {
      for(int i = 0 ; i < MAX_SIGNALS; i++)                                   {
         if(m_daSignal[i]!= NULL)                                             {
            delete m_daSignal[i];
            m_daSignal[i] = NULL;
         }
      }
   delete [] m_daSignal;
   }
};
// ---------------------------------------------------------------------------
/**
 DataModifiedPost
 */
void __fastcall TImageSignalMngr::DataModifiedPost(TData* da)                 {

   if (da == NULL)                                                            {
      return;
   }
   if (da == m_daSignlsComboBox)                                              {
      m_tokensDestinations.clear();
      TAux::ExtractTokens( m_daSignals, m_tokensDestinations );
   }
   if (da == m_daClear && m_daClear->AsInt())                                 {
      m_daClear->SetAsInt(0);
   }
   if (da == m_daReset && m_daReset->AsInt())                                 {
      m_daReset->SetAsInt(0);
   }
   if (da == m_daCalculate && m_daCalculate->AsInt())                         {
      m_daCalculate->SetAsInt(0);
   }
   if (da == m_daCloseButton && m_daCloseButton->AsInt())                     {
      m_daCloseButton->SetAsInt(0);
      m_closed = true;
   }
   if (da == m_daResetBtn && m_daResetBtn->AsInt())                           {
      m_daResetBtn->SetAsInt(0);
   }

   if (da == m_daShowForm   && m_daShowForm->AsInt()  )                       {
      ShowFormImageSignal();
      m_daShowForm->SetAsInt(0);
   }


}
// ---------------------------------------------------------------------------
/**
 MyStop
 */
bool TImageSignalMngr::MyStop() {
   m_daSignlsComboBox->SetRdOnly(false);
   return TProcess::MyStop();


}
// ---------------------------------------------------------------------------
/**
 MyInit
 */
bool TImageSignalMngr::MyInit() {
   m_daSignlsComboBox->SetRdOnly(true);

   return TProcess::MyInit();
}
// ---------------------------------------------------------------------------
/**
 MyGo
 */
bool TImageSignalMngr::MyGo() {
   return TProcess::MyGo();
}
// ---------------------------------------------------------------------------
/**
 MyProcess
 */
TProcessData * TImageSignalMngr::MyProcess(TProcessData *processData) {
   if(m_closed)                                                                {
	  return processData;
   }
   m_img = dynamic_cast<TImg*>(processData);
   if (m_img==NULL) {
	   return processData;
   }
   if ( (m_img->GetTime() - m_lastTime) / 1000 < m_daThresholdMs->AsInt())				   {
	   return processData;
   }
   m_lastTime = m_img->GetTime();

   //Free memory for previous signals
   if (m_daSignal != NULL){
	  for(int i = 0 ; i < MAX_SIGNALS; i++){
		 if(m_daSignal[i]!= NULL){
			delete m_daSignal[i];
			m_daSignal[i] = NULL;
		 }
	  }
   }
   //Clear the form
   m_daClear->SetAsInt(1);
   int i = 1;
   int bitCount = 0;
   //For each signal
   for ( std::list<std::wstring>::iterator j = m_tokensDestinations.begin(); j != m_tokensDestinations.end(); j++ )   {
	  std::wstring cadena = L"Signal_";
	  cadena.append( *j);
	  WORD * data = NULL;
	  //Get the plane with same name of signal
	  TImgPlane *plane = m_img->GetImgPlane((*j).c_str());
	  if (plane && plane->BytesPerPixel()==2)                                {
		 if ( plane->BitCount() > bitCount) 								 {
			bitCount = plane->BitCount();
		 }
		 unsigned int width = plane->Width();
		 if(width){
			data = new WORD[width];
			//save the plane info
			for (int i = 0; i < width; i++) {
			   LPWORD p =  (LPWORD)plane->GetPixelPtr( i, 0);
			   if(p){
				  data[i] = *p;
			   }
			}
			//new tdata setting as binary the plane info
			m_daSignal[i] = new TData ( this, cadena.c_str() ,    NULL,   0, FLAG_RDONLY|FLAG_NOTIFY, const_cast<wchar_t *>( (*j).c_str()));
			m_daSignal[i]->SetAsBin(data, width * plane->BytesPerPixel());
			m_x1->SetMax((int)width-1);
			m_x2->SetMax((int)width-1);
		 }
	  }
	  if(data != NULL){
		 delete [] data;
	  }
	  i++;
   }
   m_daScaleDepth->SetAsInt( pow(2.0, bitCount) * 1.1);
   //set the combobox list
   if(m_daSignals)
	  m_daSignalsList->SetList(m_daSignals);
   //calculate the info of the selected signal (average, max, min, etc)
   m_daCalculate->SetAsInt(1);
   return processData;
}


void TImageSignalMngr::SetClosed(bool aux){
	m_closed = aux;
}




void TImageSignalMngr::ShowFormImageSignal()                                  {

//   std::list<TDataManager *>::iterator it = m_modMngr->DataManagerBegin();
//   std::list<TDataManager *>::iterator itEnd = m_modMngr->DataManagerEnd();
//   while ( it != itEnd)                                                       {
//      TImageSignalMngr* imageSignalMngr = dynamic_cast<TImageSignalMngr*>(*it);
//      if (imageSignalMngr)                                                    {

         // Create form
         if(m_signalForm == NULL)                                             {
            //m_signalForm = new TImageSignalForm(this,imageSignalMngr);
            m_signalForm = new TImageSignalForm(NULL, this);
         }
         // Show form
         m_signalForm->Show();
         SetClosed(false);

//        break;
//      }
//      it++;
//   }

}







//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
