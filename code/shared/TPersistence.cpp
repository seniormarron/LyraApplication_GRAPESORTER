//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TPersistence.cpp
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
#include "TPersistence.h"
#include "THardlockChecker.h"
#include "TShowMsgForm.h"
#include "definesLang.h"   //use of loadtext in Logs

//------------------------------------------------------------------------------
#pragma package(smart_init)

   //validate code in xml file
//ToDo: encrypt value ( what happens with " character, that delimitates string in xml?)
//aunque esté encriptado, se puede copiar y pegar de otro que funcione
bool TPersistence::LoadConfigFrom( wchar_t* fileNamePath)                      {

   m_machineCodeValidated = false;
   if (MyLoadConfigFrom( fileNamePath))                                       {
      char *machineCode = GetFileCode();
      m_machineCodeValidated = THardlockChecker::ValidateMachineCode( machineCode);
      if (m_machineCodeValidated)                                             {
         free( machineCode);
         return true;
      }
      else                                                                    {
         // Comento esto para saltar hardlock LYRA
//         LOG_ERROR1(L"ERROR_NO_HARDLOCK");
//         ShowMsg(LoadText(L"ERROR_NO_HARDLOCK"), L"ERROR", MY_MB_OK);

         // Saltar protección hardlock return = true;
         return true;
      }
   }
   return false;
}




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------


