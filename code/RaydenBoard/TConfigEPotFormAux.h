//---------------------------------------------------------------------------

#ifndef TConfigEPotFormAuxH
#define TConfigEPotFormAuxH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "AdvSmoothComboBox.hpp"
#include "AdvSmoothListBox.hpp"
#include <ExtCtrls.hpp>
#include "TAux.h"
//---------------------------------------------------------------------------
class TConfigEPotFormAux : public TForm
{
__published:	// IDE-managed Components
   TShape *Shape1;
   TAdvSmoothComboBox *CBCameraLines;
   TShape *Shape2;
   TLabel *OriName;
   TEdit *Line;
private:	// User declarations
public:		// User declarations
   __fastcall TConfigEPotFormAux(TComponent* Owner);

   void SetCameraLine( const char *line, wchar_t *planeOptions);
   void FillInfo(std::wstring &ePodDemosaic,int ilum);
   void ModifLineNumber(char *oldS,char *newS){
      char *str =  TAux::WSTR_TO_STR(Line->Text.c_str());
      if(strcmp(oldS,str)== 0 || strcmp("-9999",str)== 0) {
         Line->Text = newS;
      }
      delete [] str;
   }
};
//---------------------------------------------------------------------------
extern PACKAGE TConfigEPotFormAux *ConfigEPotFormAux;
//---------------------------------------------------------------------------
#endif
