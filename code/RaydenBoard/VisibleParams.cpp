//---------------------------------------------------------------------------


#pragma hdrstop

#include "TAux.h"
#include "VisibleParams.h"

#define LBL_CAMERA_LINES L"CameraLines"

//---------------------------------------------------------------------------

#pragma package(smart_init)

VisibleParams::VisibleParams(const wchar_t *name, TDataManager *_parent): TDataManager(name,_parent){

   new TData( this, L"PixelSize",     NULL,400,0,10000, NULL, FLAG_NORMAL, L"Size of pixel in mm2");
   new TData( this, LBL_CAMERA_LINES,  m_cameraLines,  L"", SIZEOF_CHAR( m_cameraLines), FLAG_NORMAL, L"Camera line names separated by ','" );
   new TData( this, L"Iluminations", m_iluminations, L"", SIZEOF_CHAR( m_iluminations),FLAG_NORMAL, L"Ilumination names separated by ','" );
   m_cameraLinesNumber = 0;
}

//---------------------------------------------------------------------------


VisibleParams::~VisibleParams(){

}

//---------------------------------------------------------------------------


void VisibleParams::DataModifiedPost( TData *da)   {

   if ( da ==NULL)   {
      return;
   }

   if ( da->AreYou( LBL_CAMERA_LINES)) {
      std::list<std::wstring> tokens;
      TAux::ExtractTokens( m_cameraLines, tokens, L",");
      m_cameraLinesNumber = tokens.size();
   }


}



