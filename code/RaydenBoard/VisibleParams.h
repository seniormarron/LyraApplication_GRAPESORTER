//---------------------------------------------------------------------------

#ifndef VisibleParamsH
#define VisibleParamsH
//---------------------------------------------------------------------------

#include "TDataMng.h"

class VisibleParams: public TDataManager {

protected:
   wchar_t    m_cameraLines[512];
   wchar_t    m_iluminations[512];
   int  m_cameraLinesNumber;   ///< calculated value

public:

   VisibleParams(const wchar_t *name, TDataManager *_parent);
   virtual ~VisibleParams();
   virtual void DataModifiedPost( TData *da);

   wchar_t *GetCameraLines(){
      return m_cameraLines;
   }
   wchar_t *GetIluminations(){
      return m_iluminations;
   }
   int CameraLinesNumber() {
      return m_cameraLinesNumber;
   }

};

#endif
