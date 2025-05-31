//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      LTInit.cpp
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

#include "LTInit.h"
#include <string>
#include <windows.h>

#include <dir.h>
#include <SysUtils.hpp>
#include "LogEngineUDP.h"

#include "TAux.h"

#include <buttons.hpp>
#include <comctrls.hpp>
#include "AdvSmoothLabel.hpp"
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
#include "AdvSmoothTabPager.hpp"

#include <stdio.h>

bool     isUDPServerAlive = false;
wchar_t  UDPSERVERNAME[128];
wchar_t lang_code[8];
///<  flag where to store lang code value from console or configuration file.

wchar_t  FILE_NAME_CFG[1024]     = L"LyraTechsArtificialIntelligence.xml";
wchar_t  APP_NAME[256]           = L"Lyra Techs Artificial Intelligence Software";
wchar_t  PATH_EVENTS_FILE[1024]    = L"";

wchar_t NAME_MODULE[256]         = L""; // Distributed version
wchar_t ENTRY_POINT_MODULE [256] = L"";
wchar_t TARGET_LOCATION[256]     = L"";

wchar_t GM_NAME[256]             = L"";
wchar_t GM_ENTRY_POINT [256]     = L"";
//wchar_t  GM_PATH_EVENTS_TXT[1024]    = L""

// opc conf --> default values
wchar_t OPC_HOST_NAME[256]    = L"";
wchar_t OPC_URL [256]         = L"";
wchar_t OPC_APP_NAME[256]     = L"";
wchar_t OPC_ADDRESSSPACE[256] = L"urn:LyraTechs";




// ---------------------------------------------------------------------------
#pragma package(smart_init)

#include "windows.h"
#include "psapi.h"
#include <tlhelp32.h>

//information about dll
typedef struct {
   char libName[64];
   /// Library FileName
   char functionName[64];
   /// Function Name
   char params[128];
   /// Params

} s_dll_info;

// Params Info Container
struct s_params {
   wchar_t* name;
   ///< Module Name
   wchar_t* target;
   ///< Target for GM
   wchar_t* entryPoint;
   ///< EntryPoint for Module
   wchar_t* xmlFileName;
   ///< XML File Name with configuration
   wchar_t* eventsFilePath;
   ///< Path for events.txt file
};

/**
   @struct s_opc_params
   Structure to set all configuration params needed by the OPC Server.
*/
struct s_opc_params {
  const wchar_t* hostname;       ///< HostName of the opc server
  const wchar_t* url;            ///< URL endpoint of the opc server
  const wchar_t* appName;        ///< Application Name of the opc server
  const wchar_t* addressSpace;   ///< AddressSpace for the OPC objects
};


/**
    Extract the params for loading
    @param params params to the function
    @param dllInfo params container
    @return  bool true on success, false otherwise
*/

bool ExtractParams(wchar_t* pParams, struct s_params &dllInfo) {

      wchar_t* targetTag      = L"target:";
      wchar_t* entryPointTag  = L"entrypoint:";
      wchar_t* xmlTag         = L"xml:";
      wchar_t* eventsFilePathTag = L"eventsfilepath:";

      wchar_t* pSearched;
      bool processLineResult = true;

      dllInfo.name = NULL;
      dllInfo.target = NULL;
      dllInfo.entryPoint = NULL;
      dllInfo.xmlFileName = NULL;
      dllInfo.eventsFilePath = NULL;

      int size;
      if (wcslen(pParams) > 0) {
         // Searching name
         if ((pSearched = wcschr(pParams, L' ')) != NULL) {
            size = pSearched - pParams;
            dllInfo.name = (wchar_t *) malloc((size + 1)*sizeof(wchar_t));
            wmemcpy(dllInfo.name, pParams, size);
            dllInfo.name[size] = 0;
         }

         if (dllInfo.name != NULL) {
            // Searching for Target
            if (((pSearched = wcsstr(pParams, targetTag))) != NULL) {
               pSearched = pSearched + wcslen(targetTag);
               wchar_t* pBlanckSpace = pSearched ?
                   wcschr(pSearched, L' ') : NULL;
               size = wcslen(pSearched) - wcslen(pBlanckSpace);
               dllInfo.target = (wchar_t *) malloc((size + 1)*sizeof(wchar_t));
               wmemcpy(dllInfo.target, pSearched, size);
               dllInfo.target[size] = 0;
            }
            // Searching for EntryPoint
            wchar_t* pEntryPointSearched = wcsstr(pParams, entryPointTag);
            if (pEntryPointSearched != NULL) {
               pEntryPointSearched = pEntryPointSearched + wcslen(entryPointTag);
               wchar_t* pBlanckSpace = pEntryPointSearched ?
                   wcschr(pEntryPointSearched, L' ') : NULL;
               size = (pBlanckSpace == NULL) ? wcslen(pEntryPointSearched) :
                   (wcslen(pEntryPointSearched) - wcslen(pBlanckSpace));
               dllInfo.entryPoint = (wchar_t *) malloc((size + 1)*sizeof(wchar_t));
               wmemcpy(dllInfo.entryPoint, pEntryPointSearched, size);
               dllInfo.entryPoint[size] = 0;

            }
            // else
            // processLineResult =  false;
            // Searching for xml
            wchar_t* pxmlFileNameSearched = wcsstr(pParams, xmlTag);
            if (pxmlFileNameSearched != NULL) {
               pxmlFileNameSearched = pxmlFileNameSearched + wcslen(xmlTag);
               wchar_t* pBlanckSpace = pxmlFileNameSearched ?
                   wcschr(pxmlFileNameSearched, L' ') : NULL;
               size = (pBlanckSpace == NULL) ? wcslen(pxmlFileNameSearched) :
                   (wcslen(pxmlFileNameSearched) - wcslen(pBlanckSpace));
               dllInfo.xmlFileName = (wchar_t *) malloc((size + 1)*sizeof(wchar_t));
               wmemcpy(dllInfo.xmlFileName, pxmlFileNameSearched, size);
               dllInfo.xmlFileName[size] = 0;

            }
//            wchar_t* pxmlFileNameSearched = wcsstr(pParams, xmlTag);
//            if (pxmlFileNameSearched != NULL) {
//               pxmlFileNameSearched = pxmlFileNameSearched + wcslen(xmlTag);
//               size = wcslen(pxmlFileNameSearched);
//               pxmlFileNameSearched[size] = 0;
//               dllInfo.xmlFileName = (wchar_t *) malloc((size + 1)*sizeof(wchar_t));
//               wmemcpy(dllInfo.xmlFileName, pxmlFileNameSearched, size);
//               dllInfo.xmlFileName[size] = 0;
//            }


            // Searching for EventsFilePath
            wchar_t* peventsFilePathFileNameSearched = wcsstr(pParams, eventsFilePathTag);
            if (peventsFilePathFileNameSearched != NULL) {
               peventsFilePathFileNameSearched = peventsFilePathFileNameSearched + wcslen(eventsFilePathTag);
               size = wcslen(peventsFilePathFileNameSearched);
               peventsFilePathFileNameSearched[size] = 0;
               dllInfo.eventsFilePath = (wchar_t *) malloc((size + 1)*sizeof(wchar_t));
               wmemcpy(dllInfo.eventsFilePath, peventsFilePathFileNameSearched, size);
               dllInfo.eventsFilePath[size] = 0;
            }
         }
         else
            processLineResult = false;
      }

      return processLineResult;
 }


// ----------------------------------------------------------------------------
/**
 Check if a process is executing
 @param name of the process
 */
bool isRunning(wchar_t *szProcess) {

   HANDLE hProcessSnap;
   HANDLE hProcess;
   PROCESSENTRY32 pe32;
   DWORD dwPriorityClass;
   bool ret = false;

   hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
   // Takes a snapshot of all the processes

   if (hProcessSnap == INVALID_HANDLE_VALUE) {
      return ret;
   }

   pe32.dwSize = sizeof(PROCESSENTRY32);

   if (!Process32First(hProcessSnap, &pe32)) {
      CloseHandle(hProcessSnap);
      return ret;
   }

   do {
      if (!wcscmp(pe32.szExeFile, szProcess))
      { // checks if process at current position has the name of to be killed app
         ret = true;
      }
   }
   while (Process32Next(hProcessSnap, &pe32) && !ret);
   // gets next member of snapshot

   CloseHandle(hProcessSnap); // closes the snapshot handle
   return ret;
}

// ----------------------------------------------------------------------------
/**
 Replace all tabular characters by blank space
 '\t' -> blank space
 */
void preprocessLine(char* line) {

   int size = strlen(line);
   if (size > 0) {
      for (int i = 0; i < size; i++) {
         if (line[i] == '\t') {
            line[i] = 0x20;
         }
      }
   }
}

static char* SearchBlankSpace(char * pattern) {

   char* pStr = strchr(pattern, '\t');
   if (pStr)
      return pStr;

   pStr = strchr(pattern, ' ');
   if (pStr)
      return pStr;

   return NULL;
}



void LoadOptions(  s_params moduleInfo) {

      if (moduleInfo.xmlFileName)   {
         wcsncpy(FILE_NAME_CFG, moduleInfo.xmlFileName, wcslen(moduleInfo.xmlFileName));
         FILE_NAME_CFG[wcslen(moduleInfo.xmlFileName)] = 0 ;
      }

      if (wcscmp (moduleInfo.name, L"GM") == 0) {

            wcsncpy(GM_NAME, moduleInfo.name, wcslen(moduleInfo.name));
            GM_NAME[wcslen(moduleInfo.name)] =0;
            if (moduleInfo.entryPoint)             {
               wcsncpy(GM_ENTRY_POINT, moduleInfo.entryPoint, wcslen(moduleInfo.entryPoint));
               GM_ENTRY_POINT[SIZEOF_CHAR(GM_ENTRY_POINT) -1] = 0 ;
            }

            // ToDo: ver si ponemos aqui el path para el events.txt
            // esto es para el multimodulo.
            if (moduleInfo.eventsFilePath)          {
               wcsncpy(PATH_EVENTS_FILE, moduleInfo.eventsFilePath, wcslen(moduleInfo.eventsFilePath));
               PATH_EVENTS_FILE[wcslen(moduleInfo.eventsFilePath)] =0;
            }
      }
      else {

         if (moduleInfo.name) {
            wcsncpy(NAME_MODULE, moduleInfo.name, wcslen(moduleInfo.name));
            NAME_MODULE[wcslen(moduleInfo.name)] =0;
         }

         if (moduleInfo.target)  {
            wcsncpy(TARGET_LOCATION, moduleInfo.target, wcslen(moduleInfo.target));
            TARGET_LOCATION[wcslen(moduleInfo.target)] =0;
         }

         if (moduleInfo.entryPoint)       {
            wcsncpy(ENTRY_POINT_MODULE, moduleInfo.entryPoint, wcslen(moduleInfo.entryPoint));
            ENTRY_POINT_MODULE[wcslen(moduleInfo.entryPoint)] =0;
         }

         if (moduleInfo.eventsFilePath)       {
            wcsncpy(PATH_EVENTS_FILE, moduleInfo.eventsFilePath, wcslen(moduleInfo.eventsFilePath));
            PATH_EVENTS_FILE[wcslen(moduleInfo.eventsFilePath)] =0;
         }
      }
}



// ----------------------------------------------------------------------------
/**
 Extract the params passed to the Function and
 load & execute the library. .
 @param pLine Line read from the config File
 @return  bool true on success, false otherwise
 */
bool SetOptions(char* pLine2) {

   char* pLine = strdup(pLine2);
   if ((pLine == NULL) || (strlen(pLine) <= 0))
      return false;

   char* pParamsStr; // pointer to the params
   bool processLineResult = false;

   // Searching for params label
   if (((pParamsStr = strstr(pLine, PARAMS_LABEL)) != NULL)) {
            /// Setting Params
            pParamsStr = pParamsStr + strlen(PARAMS_LABEL);
            pParamsStr++; // take out first doublequoting
            char* pEndDoubleQuotationMark = strrchr(pParamsStr, '"');
            int size = strlen(pParamsStr) - strlen(pEndDoubleQuotationMark);
            pParamsStr[size] = 0;

            s_params moduleInfo;
               wchar_t* w_params = TAux::STR_TO_WSTR( pParamsStr);
               if ( ExtractParams( w_params, moduleInfo ))  {
                     LoadOptions(moduleInfo);
               }
               if (w_params) {
                   delete [] w_params;
               }
               if( moduleInfo.name)
                  free( moduleInfo.name);
               if( moduleInfo.target)
                  free( moduleInfo.target);
               if( moduleInfo.entryPoint)
                  free( moduleInfo.entryPoint);
               if( moduleInfo.xmlFileName)
                  free( moduleInfo.xmlFileName);
               if (moduleInfo.eventsFilePath)
                  free( moduleInfo.eventsFilePath);

   }
   free( pLine);
   return processLineResult;
} // End processLine
// ----------------------------------------------------------------------------
/**
 Read Config File
 @param   File point to File
 @return  int Number of libraries loaded, -1 otherwise
 */
int readFile(FILE* pFile) {

   // obtain file size:
   fseek(pFile, 0, SEEK_END);
   size_t lSize = ftell(pFile);
   rewind(pFile);

   // File empty
   if (!lSize) {
      fputs("Empty Config File\n", stderr);
      return -1;
   }

   char* buffer;
   // allocate memory to contain the whole file:
   buffer = (char*) malloc(sizeof(char) * lSize + 1);
   if (buffer == NULL) {
      fputs("Memory error in Config File\n", stderr);
      return -1;
   }

   // copy the file into the buffer:
   size_t result = fread(buffer, 1, lSize, pFile);
   if (result != lSize) {
      fputs("Reading error in Config File\n", stderr);
      return -1;
   }

   buffer[lSize] = 0;

   char* pLine;
   char* pSearch;
   char* pRead = strdup(buffer), *pIniRead = pRead;
   boolean ret = true;
   unsigned int sizelbl = strlen(PARAMS_LABEL);
   char *liblbl = new char[sizelbl + 1];

   while (ret) {
      if (((pSearch = strchr(pRead, '\n')) != NULL)) {
         int size = pSearch - pRead;
         pLine = (char *) malloc(size + 1);
         memcpy(pLine, pRead, size);
         pLine[size] = 0;
         pSearch++;
         pRead = pSearch;

      }
      else {

         int size = strlen(pRead);
         pLine = (char *) malloc(size + 1);
         memcpy(pLine, pRead, size);
         pLine[size] = 0;
         ret = false;
      }
      if (pLine) {
         if (strlen(pLine) >= sizelbl) {
            strncpy(liblbl, pLine, sizelbl);
            liblbl[sizelbl] = 0;

            if (strcmp(liblbl, PARAMS_LABEL) == 0) {
               preprocessLine(pLine);
               SetOptions(pLine);
            }
         }
         free(pLine);
      }
      else
         ret = false;
   }

   free(buffer);
   delete[]liblbl;

   if (pRead)
      free(pIniRead);

   return 0;
} // End readFile

///< Struct used for configure the Log System
typedef struct {
   wchar_t ExeName[64];
   ///< Name of Server Application
   bool Enable;
   ///< Enable Log system
   int Port;
   ///< UDP Port
   int Days;
   ///< Days to save logs
   int Level;
   ///< Level the logs
   wchar_t LogFile[MAX_PATH];
   ///< Name of path and logfile
} s_config_launcher;

//
//
bool FetchExeConf(s_config_launcher* info, wchar_t* filename) {

#define MAXTOKENS       128

   wchar_t *tokens[MAXTOKENS];
   int i = 0;

#define MAX_TAB_LEVEL 6

   wchar_t *tokens_LEVEL[MAX_TAB_LEVEL] = {
      L"DEBUG", L"INFO", L"WARN", L"ERROR", L"ALERT", L"OFF"};
   int tokens_LEVEL_VALUE[MAX_TAB_LEVEL] = {0, 1, 2, 3, 4, 5};

   wchar_t *tokens_name[8] = {
      L"exe:", L"days=", L"enable=", L"registersfile=", L"port=", L"level=",
      L"lang=", NULL};
   wchar_t *tokens_val_def[8] = {
      L"Def.exe", L"3", L"on", L"DefLog.log", L"999", L"3", L"en", NULL};

   wchar_t buff[512];
   #define CIPATH_SIZE 64
   #define CI_TOTAL_SIZE ((CIPATH_SIZE*2) + (sizeof(int)*3) + 1)
   memset(info, 0, CI_TOTAL_SIZE);
   memset(buff, 0, sizeof(buff));

   FILE *fd;
   fd = _wfopen(filename, L"r+");
   if (fd == NULL)
      return false;

   wchar_t *p, *b;

   while ((fgetws(buff, sizeof(buff), fd)) != NULL) {

      if (buff[0] == '#')
         continue;
      if (buff[wcslen(buff) - 1] == '\n')
         buff[wcslen(buff) - 1] = 0;
      if ((b = wcsstr(buff, L"exe:")) == NULL)
         continue;

      for ((p = wcstok(b, L" ")); p; (p = wcstok(NULL, L" ")), i++) {
         if (i < MAXTOKENS - 1)
            tokens[i] = p;
      }
      tokens[i] = L'\0';
      break;
   }

   int max = i;
   int len;

   for (i = 0; i < max; i++) {

      if ((b = wcsstr(tokens[i], tokens_name[0])) != NULL) {
         len = wcslen(tokens_name[0]);
         if (wcslen(b + len) < wcslen(tokens[i]))
            wcscpy(info->ExeName, b + len);
         continue;
      }

      if ((b = wcsstr(tokens[i], tokens_name[1])) != NULL) {
         len = wcslen(tokens_name[1]);
         if (wcslen(b + len) < wcslen(tokens[i]))
            info->Days = _wtoi(b + len);
         continue;
      }

      if ((b = wcsstr(tokens[i], tokens_name[2])) != NULL) {
         if (wcsstr(tokens[i], L"on") != NULL)
            info->Enable = true;
         else
            info->Enable = false;

         continue;
      }

      if ((b = wcsstr(tokens[i], tokens_name[3])) != NULL) {
         len = wcslen(tokens_name[3]);
         if (wcslen(b + len) < wcslen(tokens[i])) {
            wcscpy(info->LogFile, b + len);

            wchar_t  *t;
            wchar_t  q[MAX_PATH];
            memset(  q, 0x00, sizeof(q));
            wchar_t  path[MAX_PATH];
            memset(  path, 0x00, sizeof(path));
            bool createPath = false;

            wcscpy(q, b + len);
            t = wcstok(q, L"\\");

            while(t!=NULL)                                                    {
               wcscat (path , t );
               wcscat (path ,L"\\");
               t = wcstok(NULL, L"\\");
               if (!t==NULL) {
                  if (wcsstr (t, L"." )){
                     createPath = true;
                     break;
                  }
               }

            }
            if (createPath)
               _wmkdir(String(path).c_str());
         }
         continue;
      }

      if ((b = wcsstr(tokens[i], tokens_name[4])) != NULL) {
         len = wcslen(tokens_name[4]);
         if (wcslen(b + len) < wcslen(tokens[i]))
            info->Port = _wtoi(b + len);
         continue;
      }
      if ((b = wcsstr(tokens[i], tokens_name[5])) != NULL) {
         len = wcslen(tokens_name[5]);
         if (wcslen(b + len) < wcslen(tokens[i])) {

            for (int z = 0; z < MAX_TAB_LEVEL; z++) {
               if (wcscmp(b + len, tokens_LEVEL[z]) == 0) {
                  info->Level = tokens_LEVEL_VALUE[z];
                  break;
               }

            }

         }

         continue;
      }
      // Setting language
      if ((b = wcsstr(tokens[i], tokens_name[6])) != NULL) {
         len = wcslen(tokens_name[6]);
         if (wcslen(b + len) < wcslen(tokens[i])) {
            lang_code[0] = L'#';
            wcscat(lang_code, b + len);
            lang_code[wcslen(lang_code)] = L'\0';
         }
         continue;

      }
   }

   // Set default value
   if (wcslen(info->ExeName) < 2)
      wcscpy(info->ExeName, tokens_val_def[0]);

   if (info->Port == 0)
      info->Port = _wtoi(tokens_val_def[4]);

   if (wcslen(info->LogFile) < 2)
      wcscpy(info->LogFile, tokens_val_def[3]);

   if (info->Days == 0)
      info->Days = _wtoi(tokens_val_def[1]);

   fclose(fd);

   return true;
}
// ----------------------------------------------------------------------------
/**
 Load the OPC Server donfiguration params
 @param filename file name of the txt file
 @return  bool true on success, false otherwise
 */
bool LoadOPCConfParams(wchar_t* filename) {

   bool foundconf=false;
   wstring line;
   std::wifstream myfile (filename);
   if (myfile.is_open()) {
      while ( getline (myfile,line) ) {
         int pos = line.find(L"opcparams:");
         if( pos  != std::wstring::npos && pos == 0) {
            wstring restline = line.substr(10);        //remove the opcparams: part of the string
//            wchar_t msg[1024];
//            swprintf(msg, L"opc params file: %s", restline.c_str());
//            LOG_DEBUG(msg);
            // extract the tokens and trim them (thats why we dont call AuxString::explodeW)
            wchar_t delim = '|';
            std::wstring buff=L"";
            std::vector<std::wstring> tokens;

            for (int i = 0 ;i< restline.size();i++)
            {
               const wchar_t &n=restline[i];
               if (n != delim)
                  buff += n;
               else
                  if (n == delim && buff != L""){
                     tokens.push_back(AuxStrings::trimW(buff)); buff = L"";
                  }
            }
            if (buff != L"")
               tokens.push_back(AuxStrings::trimW(buff));
            //check if we have 4 tokens and copy them to the global variables
            if( tokens.size() >=4) {
               int i=0;
               wcsncpy(OPC_HOST_NAME, tokens.operator[](i).c_str(), tokens.operator[](i).size());
               PATH_EVENTS_FILE[tokens.operator[](i).size()] =0;
               i++;
               wcsncpy(OPC_URL, tokens.operator[](i).c_str(), tokens.operator[](i).size());
               OPC_URL[tokens.operator[](i).size()] =0;
               i++;
               wcsncpy(OPC_APP_NAME, tokens.operator[](i).c_str(), tokens.operator[](i).size());
               OPC_APP_NAME[tokens.operator[](i).size()] =0;
               i++;
               wcsncpy(OPC_ADDRESSSPACE, tokens.operator[](i).c_str(), tokens.operator[](i).size());
               OPC_ADDRESSSPACE[tokens.operator[](i).size()] =0;
               foundconf = true;
            }
            else{
               LOG_INFO(L"OPC Server Configuration not valid");
            }
         }    //opcparams line
      }   //while
      myfile.close();
   }
   return foundconf;
}



bool LoadFileConfig(wchar_t* filename)                                     {

   memset(  CONFIG_NAME, 0x00, sizeof( CONFIG_NAME));
   wcsncpy( CONFIG_NAME, filename, SIZEOF_CHAR( CONFIG_NAME)-1);

   LogEngineUDP::GetInstance().InitWithFile(filename);

   s_config_launcher    info;
   FetchExeConf( &info, filename);

   memset(UDPSERVERNAME, 0, sizeof(UDPSERVERNAME));
   wcscpy(UDPSERVERNAME, info.ExeName);

   isUDPServerAlive = false;
      if (TAux::FileExists(UDPSERVERNAME)) {
         if (!isRunning(UDPSERVERNAME)) {
            std::wstring str;
// Use this to make ServerUDP read by its own the configuration (changes in ServerUDP have to be made)
//            str = L"START /SEPARATE ";
//            str = str + UDPSERVERNAME;
//            str = str + L" " + filename;

            str = L"START /SEPARATE ";
            str = str + UDPSERVERNAME;
            str = str + L" " + info.ExeName;
            if(info.Enable)
               str = str + L" " + L"1";
            else
               str = str + L" " + L"0";
            wchar_t strAux[128];
            swprintf( strAux, L"%d", info.Port);
            str = str + L" " + strAux;
            swprintf( strAux, L"%d", info.Days);
            str = str + L" " + strAux;
            swprintf( strAux, L"%d", info.Level);
            str = str + L" " + strAux;
            str = str + L" " + info.LogFile;
            _wsystem(str.c_str());
            isUDPServerAlive = true;
         }
      }

   // Setting language
   ACTIVE_LANG = DEFAULT_ACTIVE_LANG; // By default language "English"

   // Checking if language is defined
   if (SetActiveLang(lang_code)) {
      wchar_t msg[1024];
      swprintf(msg, L"Language selected: %s", LANG_CODE[ACTIVE_LANG]);
      LOG_INFO(msg);
   }
   else                                                                       {
      wchar_t msg[1024];
      swprintf(msg, L"Language code not defined. Default language %s will be loaded", LANG_CODE[ACTIVE_LANG]);
      LOG_INFO(msg);
   }
   bool commConfLoaded=false;
   if (!TAux::FileExists(filename)) {
      wchar_t msg[1024];
      swprintf(msg, L"Configuration File %s does no exist.", filename);
      LOG_DEBUG(msg);
   }
   else {
      FILE* pFileConfig = _wfopen(filename, L"rb");
      readFile(pFileConfig);
      fclose(pFileConfig);
      commConfLoaded = true;
   }

   //load OPC conf params
   LoadOPCConfParams(filename);

   return commConfLoaded;
}

// ----------------------------------------------------------------------------
/**
   INIT_AIS initializes from a specified configuration file, and initializes language to specified value
   @param filename is the name of the configuration file ( txt file)
   @param lang_code is the code of the language used in application
*/
void INIT_AIS(wchar_t* filename, int lang_code) {

   LOGGER( L"LyraAIS");

   // 1.- Read params and set active language
   LoadFileConfig(filename);
   Init_Lang( lang_code);
}

//------------------------------------------------------------------------------

/**
   Init_Lang set the application language to specified value
   @param lang_code is the code of the language used in application
*/
void Init_Lang( int lang_code)                                                {

   LOGGER( L"LyraAIS");

   if (SetActiveLang((wchar_t*)LANG_CODE[lang_code]))                      {
      wchar_t msg[1024];
      swprintf(msg, L"Language selected: %s", LANG_CODE[ACTIVE_LANG]);
      LOG_INFO(msg);
   }

   // 2.- With active lang, we look for  $path/lang/ZZZ_en.txtor ZZZ_XY.txt files
   wchar_t path[1024], current_path[1024];
   bool ret = true;
   wcscpy(path, L".\\lang\0");
   bool tableCleared = false;
   if (TAux::FileExists(path)) {
      // --- Load ENGLISH tranlations
      swprintf(current_path, L"%s\\*_en.txt\0", path);
      // 3.- - For each file, content is load into Map.
      TSearchRec m_sr;
      wchar_t currentFileLang[1024];
      if (FindFirst(current_path, faArchive, m_sr) == 0) {
         swprintf(currentFileLang, L"%s\\%s\0", path, m_sr.Name.c_str());
         //initializes table, clearing it previously
         InitLangTable(currentFileLang, true);
         tableCleared = true;
         while (FindNext(m_sr) == 0) {
            swprintf(currentFileLang, L"%s\\%s\0", path, m_sr.Name.c_str());
            InitLangTable(currentFileLang, false);
         }
      }
      else ; // No traslations or files with text found. DISPATCH_INFO(INFO_CODE,);

      // --- Load ACTIVE_LANG tranlations
      if (ACTIVE_LANG != _EN) {
         // ACTIVE_LANG
         wchar_t CODE[3];
         CODE[0] = LANG_CODE[ACTIVE_LANG][1];
         CODE[1] = LANG_CODE[ACTIVE_LANG][2];
         CODE[2] = L'\0';
         swprintf(current_path, L"%s\\*_%s.txt\0", path, CODE);
         // 3.- - For each file, content is load into Map.
         TSearchRec m_sr;
         wchar_t currentFileLang[1024];
         if (FindFirst(current_path, faArchive, m_sr) == 0) {
            swprintf(currentFileLang, L"%s\\%s\0", path, m_sr.Name.c_str());
            //changes tanslations, but doesn't clear table, becuase non existing translation will remain as in default language
            InitLangTable(currentFileLang, !tableCleared);
            while (FindNext(m_sr) == 0) {
               swprintf(currentFileLang, L"%s\\%s\0", path, m_sr.Name.c_str());
               InitLangTable(currentFileLang, false);
            }
         }
         else ; // No traslations or files with text found. DISPATCH_INFO(INFO_CODE,);

      }
   }
   else {
      wchar_t msg[1024];
      swprintf(msg, L"%s Does NOT exist.", current_path);
      LOG_INFO(msg);
      ret = false;
   }
   //
   // }
   // else
   // ret = false;

   if (!ret)
      LOG_WARN(L"Initialization could not be done in a correct way.");
}



void SetTranslationToComponent(TComponent * comp)                                      {

   if (!comp)
		 return;

  if ( dynamic_cast<TLabel*>(comp))                                  {
      TLabel* lab = (TLabel *)comp;
      if (wcslen (lab->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(lab->Name.c_str(), found);
         if ( text && found )
           lab->Caption = text;
      }
      return;
   }
   if ( dynamic_cast<TSpeedButton*>(comp))                             {
      TSpeedButton*   sb   = (TSpeedButton *)comp;
      if (wcslen (sb->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(sb->Name.c_str(), found);
         if ( text && found )
            sb->Caption = text;
      }
      return;
   }

   if ( dynamic_cast<TBitBtn*>(comp))                             {
      TBitBtn*   sb   = (TBitBtn *)comp;
      if (wcslen (sb->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(sb->Name.c_str(), found);
         if ( text && found )
            sb->Caption = text;
      }
      return;
   }

   if( dynamic_cast<TPanel*>(comp))                                           {
      TPanel*   pn   = (TPanel *)comp;
      if (wcslen (pn->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(pn->Name.c_str(), found);
         if ( text && found )
            pn->Caption = text;
      }
      return;
   }

   if( dynamic_cast<TButton*>(comp))                             {
      TButton*   bt   = (TButton *)comp;
      if (wcslen (bt->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(bt->Name.c_str(), found);
         if ( text && found )
            bt->Caption = text;
      }
      return;
   }

   if( dynamic_cast<TCheckBox*>(comp))                                {
      TCheckBox* cb = (TCheckBox*)comp;
      if (wcslen (cb->Name.c_str() ) > 0 ) {
         bool found;

         const wchar_t* text = LoadText(cb->Name.c_str(), found);
         if ( text && found )
            cb->Caption = text;
      }
      return;
   }

   if( dynamic_cast<TGroupBox*>(comp))                                {
      TGroupBox* p = (TGroupBox*)comp;
      if (wcslen (p->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(p->Name.c_str(), found);
         if ( text && found )
            p->Caption = text;
      }
      return;
  }
   if( dynamic_cast<TForm*>(comp))                                {
      TForm* p = (TForm*)comp;
      if (wcslen (p->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(p->Name.c_str(), found);
         if ( text && found )
            p->Caption = text;
      }
      return;
  }
  if( dynamic_cast<TTabSheet*>(comp))                                {
      TTabSheet* p = (TTabSheet*)comp;
      if (wcslen (p->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(p->Name.c_str(), found);
         if ( text && found )
            p->Caption = text;
      }
      return;
  }
         //TODO: ver los menus items.
         // añadir pop-up menus?
  if( dynamic_cast<TMenuItem*>(comp))                                {
      TMenuItem* p = (TMenuItem*)comp;
         if (wcslen (p->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(p->Name.c_str(), found);
         if ( text && found )
            p->Caption = text;
      }
      return;
  }

//LIST VIEW: CONTROL_NAME_COLOLUMN_NAME Convention
    if (dynamic_cast<TListView*>(comp)) {
      TListView* p = (TListView*)comp;
      std:wstring id = p->Name.c_str();
      for (int i = 0; i < p->Columns->Count; i++) {
         TListColumn *c =          p->Column[0];;

         if (wcslen(c->Caption.c_str()) > 0)    {
            id = id + L"_" + c->Caption.c_str();
            const wchar_t* text = LoadText((wchar_t*)id.c_str());
            if (text && (wcscmp(id.c_str(), text) != 0))
               c->Caption = text;
         }
      }
      return;
   }
   if (dynamic_cast<TAdvSmoothPanel*>(comp))                                  {
      TAdvSmoothPanel* p = (TAdvSmoothPanel*)comp;
      if (wcslen (p->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(p->Name.c_str(), found);
         if ( text && found )
            p->Caption->Text = text;
      }
      return;
   }
   if (dynamic_cast<TAdvSmoothLabel*>(comp))                                  {
      TAdvSmoothLabel* p = (TAdvSmoothLabel*)comp;
      if (wcslen (p->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(p->Name.c_str(), found);
         if ( text && found )
            p->Caption->Text = text;
      }
      return;
   }
   if (dynamic_cast<TAdvSmoothButton*>(comp))                                  {
      TAdvSmoothButton* p = (TAdvSmoothButton*)comp;
      if (wcslen (p->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(p->Name.c_str(), found);
         if ( text && found )
            p->Caption = text;
      }
      return;
   }

   if (dynamic_cast<TAdvSmoothTabPage*>(comp))                                  {
      TAdvSmoothTabPage* p = (TAdvSmoothTabPage*)comp;
      if (wcslen (p->Name.c_str() ) > 0 ) {
         bool found;
         const wchar_t* text = LoadText(p->Name.c_str(), found);
         if ( text && found )
            p->Caption = text;
      }
      return;
   }


}

void  LOAD_TRANSLATIONS (TComponent * view)                                 {

   SetTranslationToComponent( view);

   for(int i=0; i<  view->ComponentCount; i++)  {
      TComponent* c = view->Components[i];
      if (c) {
         LOAD_TRANSLATIONS (c );
      }
   }

}



















//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------


