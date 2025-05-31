//---------------------------------------------------------------------------


#pragma hdrstop

#include "process_line_scan.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
// ---------------------------------------------------------------------------
#pragma argsused

#include "TDemosaic.h"
#include "TFillRGBFromPlanes.h"
#include "TLinearConfigDistCoord.h"



/// Functions Exported
#if defined(__cplusplus)
extern "C" {
#endif

   __declspec(dllexport) void WhatCanYouCreate
       (std::list<s_wcyc_container_class> &list, char *name, bool manager);
   __declspec(dllexport) TDataManager * CreateDataMngr(wchar_t *name, char* code, TDataManager * dparent);
   __declspec(dllexport) TData * CreateData(wchar_t *name, char* code, TDataManager * dparent);
   __declspec(dllexport) void SetLang(unsigned int lang);
   __declspec(dllexport) void SetLoadText(LOAD_TEXT pfload_text);
   __declspec(dllexport) void SetFileConfig( wchar_t *fileName);

#if defined(__cplusplus)
}
#endif

/**
 *  WhatCanYouCreate
 *
 * */
void WhatCanYouCreate(std::list<s_wcyc_container_class> &l, char *name,
    bool manager)                                                             {

   if (manager)                                                               {
      bool firstLevel = (stricmp (name, FIRST_LEVEL_MANAGER) == 0);
      if (stricmp(typeid(TDemosaic).name() , name) == 0 || firstLevel)        {
         s_wcyc_container_class s1(typeid(TDemosaic).name() );
         s1.AddToList(l);
      }

      if (stricmp(typeid(TFillRGBFromPlanes).name() , name) == 0 || firstLevel)        {
         s_wcyc_container_class s1(typeid(TFillRGBFromPlanes).name() );
         s1.AddToList(l);
      }

      if ((stricmp(name, typeid(TLinearConfigDistCoord).name()) == 0  || firstLevel)) {
         s_wcyc_container_class s2(typeid(TLinearConfigDistCoord).name());
         s2.AddToList(l);
      }

   }
}

// ------------------------------------------------------------------------------

TDataManager* CreateDataMngr(wchar_t *name, char* code, TDataManager *dparent){


   if ( stricmp( code, typeid(TDemosaic).name())==0)                  {
      TProcess *p = dynamic_cast<TProcess*>(dparent);
      if (p) {
         return new TDemosaic( name, p);
      }
      else     {
         LOG_ERROR(L"TDemosaic parent is not a TProcess");
         return NULL;
      }
   }

   if ( stricmp( code, typeid(TFillRGBFromPlanes).name())==0)                  {
      TProcess *p = dynamic_cast<TProcess*>(dparent);
      if (p) {
         return new TFillRGBFromPlanes( name, p);
      }
      else     {
         LOG_ERROR(L"TFillRGBFromPlanes parent is not a TProcess");
         return NULL;
      }
   }
   if ( stricmp( code, typeid(TLinearConfigDistCoord).name())==0)                   {
      TProcess *mnc = dynamic_cast<TProcess*>(dparent);
      if (mnc) {
         return new TLinearConfigDistCoord( name, mnc);
      }
      else                                                                    {
         LOG_ERROR(L"Parent is not a TProcess");
         return NULL;
      }
   }


   return NULL;
}

// ---------------------------------------------------------------------------
/**
 Set active language for the library
 @param language_code code for language currently active
 */
void SetLang(unsigned int language_code)                                      {

   ACTIVE_LANG = language_code;

}

// ---------------------------------------------------------------------------
/**
 Set Translations
 @param language_code code for language currently active
 */
void SetLoadText(LOAD_TEXT pfLoadText)                                        {

   SetTranslations(pfLoadText);
}

/**
   SetFileCOnfig
   Initializes LogEngineUDP with the configuration file
*/
void SetFileConfig( wchar_t *fileName)                                        {

   LogEngineUDP::GetInstance().InitWithFile(fileName);


}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)  {
   return 1;
}

