//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      LTInit.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#include "definesLang.h"
#include "vcl.h"

#ifndef LTInitH
#define LTInitH
//---------------------------------------------------------------------------

extern wchar_t FILE_NAME_CFG  [1024];
extern wchar_t APP_NAME       [256];
extern wchar_t PATH_EVENTS_FILE [1024];

extern wchar_t NAME_MODULE[256];
extern wchar_t ENTRY_POINT_MODULE [256];
extern wchar_t TARGET_LOCATION[256];

//extern wchar_t PORT_MODULE[256];
extern wchar_t GM_ENTRY_POINT [256];
extern wchar_t GM_NAME[256];

// opc conf
extern wchar_t OPC_HOST_NAME[256];
extern wchar_t OPC_URL [256];
extern wchar_t OPC_APP_NAME[256];
extern wchar_t OPC_ADDRESSSPACE[256];

extern void    INIT_AIS    (wchar_t* filename, int lang_code= _EN);             ///< Initialize Lang options.
extern void    Init_Lang   (int lang_code= _EN);             ///< Initialize Lang options.
extern void    LOAD_TRANSLATIONS (TComponent * view);



//extern void SET_OPTIONS(wchar_t *);
//extern void LAUNCH_MODULES(wchar_t *file =NULL);


#endif



















//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------

