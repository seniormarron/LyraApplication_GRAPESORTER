//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TXmlManager.cpp
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
#pragma package(smart_init)

// -- Libxml --
#include "TAux.h"
#include "Base64.h"

#include <iostream>
#include <sstream>

#include <process.h>
#include "TXmlManager.h"
#include "definesModulesModes.h"
#include "SysUtils.hpp"             // For FileExist
#include "THardlockChecker.h"
#include "dir.h"
#include "TShowMsgForm.h"
#include "TLoadCfgDlg.h"
#include "threadsDefs.h"

const char *TXmlManager::startNameXPath = "/*[@name='";
const char* TXmlManager::endNameXPath = "']";

// ----------------------------------------------------------------------------
// ********************************  TXmlManager Class  ***********************
// ----------------------------------------------------------------------------

/**
    Construct an instance of TXMLManager
    @param   srcPathFileName  file o path of configuration file
    @param   asynchronousSave  flag to indicate if persistence is  asynchronous (true by default).
 */
TXmlManager::TXmlManager(wchar_t* srcPathFileName, bool asynchronousSave, TDataManager *mn) {

   m_mngr               = mn;
   m_asynchronousSave   = asynchronousSave;
   m_thread             = NULL;
   m_stopEvent          = NULL;
   m_XmlFileName[0]     = L'\0';
   m_mutexXmlDoc        = NULL;
   m_listBox            = new TSafeList<LT::TMessage*> ( srcPathFileName);
   m_bakIndex           = 0;


   /// Getting current path.
   unsigned int len  = SIZEOF_CHAR(m_workingPath);

   if ((_wgetcwd(m_workingPath, len)) != NULL)
      wcscat(m_workingPath, L"\\");
   else
      swprintf(m_workingPath, L".\\");


   if (!DirectoryExists(m_workingPath))
      LOG_ERROR1(L"Working Directory does not exist. ");

   m_DocPtr = NULL;
   m_asynchronousSave = false;
   if (Init( asynchronousSave, srcPathFileName) != 0)                         {
      m_DocPtr = 0;
      LOG_WARN1(L"XmlManager couldn't be initialized");

   }
}

// ----------------------------------------------------------------------------
/**
 Destroy object TXMLManager
 */
TXmlManager::~TXmlManager()                                                   {
   ClosePersistence();

   xmlCleanupParser();

   if (m_listBox)  {
      delete m_listBox;
      m_listBox = NULL;
   }


}

/**
   @param [out] asynchronousSaveVal is the value of asynchronousSave before setting it to false
*/
void TXmlManager::ClosePersistence( )                                         {


   if (!StopThread())   {
      LOG_ERROR1(L"Thread for persistence NOT stopped.");
   }

   //check if we need to save the configuration
   if ( m_toSave ) {
      LOG_ERROR1(L"Saving on ClosePersistence");
      if (!SaveConfigTo()) {
         LOG_ERROR1(L"Configuration could not be saved");
      }
   }

   // Close mutex
   if (m_mutexXmlDoc) {
      ReleaseMutex(m_mutexXmlDoc);
      CloseHandle(m_mutexXmlDoc);
      m_mutexXmlDoc = NULL;
   }

   // Free de document
   if (m_DocPtr) {
      xmlFreeDoc(m_DocPtr);
      m_DocPtr = NULL;
   }
   // Free the variables that may have been allocated by the parser
//   xmlCleanupParser();       //vic: muevo esto al destructor porque en la documentación pone que no se debe usar si se va a usar la libreria de nuevo

   xmlMemoryDump();

}

// ----------------------------------------------------------------------------
/**
   @see @TPersistence
*/
bool TXmlManager::ResetAndLoadConfiguration(wchar_t *newfileName)             {

   if (!newfileName)
      return false;
    bool asynchronousSave = m_asynchronousSave;
    ClosePersistence();

   if (newfileName)                                                        {
      wchar_t fileName [MAX_LENGTH_FULL_PATH];
      if (IsRelativePath(newfileName))
         swprintf(fileName, L"%s%s\0", m_workingPath,newfileName);
      else                                                                 {
         wcscpy(fileName,newfileName);
         fileName[wcslen(newfileName)] = 0;;
      }

      if (!FileExists(m_XmlFileName)) {
         (CopyFile((LPCTSTR) fileName, (LPCTSTR)m_XmlFileName, false) != 0);
      }
      else {

     // 3.- Rename configuration file with new file
      wchar_t fileold[1024];
      swprintf(fileold,L"%s.old",m_XmlFileName);
      bool ret = (CopyFile((LPCTSTR) m_XmlFileName, (LPCTSTR)fileold, false) != 0);
      if (ret) {
         ret = TAux::RemoveFile(m_XmlFileName);
         if (ret)
            ret = ret && (CopyFile((LPCTSTR) fileName, (LPCTSTR)m_XmlFileName, false) != 0);
      }
      if (!ret)  // restore if file not copied.
         (CopyFile((LPCTSTR) fileold, (LPCTSTR)m_XmlFileName, false) != 0);

      TAux::RemoveFile(fileold);
      }

      m_thread = NULL;
      m_stopEvent = NULL;

      if (Init(asynchronousSave, NULL) != 0) {
         m_DocPtr = 0;
         LOG_WARN1(L"XmlManager couldnt be initialized.");
         return false;
      }
      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------
/**
 Initilize xml parser and launches thread for ansynchornous save, when required
 @param srcPath Xml File Name
 @param showMessageFlag Flag to indicate Error Messages to be showed to the user.
                        If false, Error Messages must be only showed in the Logger Application.
 */
int TXmlManager::Init( bool asynchronousSave, wchar_t *srcPath, bool showMessagesFlag)                {
     /*
    * this initialize the library and check potential ABI mismatches
    * between the version it was compiled for and the actual shared
    * library used.
    */

   xmlInitParser();

   LIBXML_TEST_VERSION

   if (srcPath) {
      m_XmlFileName[0] = L'\0';
      swprintf(m_XmlFileName, L"%s%s\0", m_workingPath,srcPath);
   }

   //Add bak names in the list
   m_bakIndex = 0;

   wchar_t fileBakName[MAX_LENGTH_FULL_PATH];
   swprintf(fileBakName, L"%s.bak", m_XmlFileName);
   for (int i = 0; i < MAX_BAK; i++)                                          {
      swprintf( m_fileBakName_list[i], L"%s%d", fileBakName, i);
   }

   //obtener el m_bakIndex por el cual proseguir, que sera el más reciente
   FILETIME    ftWriteEarlier;
   int indexWriteEarlier = -1;
   bool exists[MAX_BAK];
   for ( int i = 0; i < MAX_BAK; i++)                                         {

      FILETIME    ftCreate, ftAccess, ftWrite;
      if (TAux::FileExists( m_fileBakName_list[i]))                           {

         HANDLE hFile = INVALID_HANDLE_VALUE;

         hFile = CreateFile( m_fileBakName_list[i], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
         if ( !hFile)                                                {
            exists[i] = false;
            continue;
         }
         // Retrieve the file times for the file.
         GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
         CloseHandle( hFile);
         exists[i] = true;

         if ( indexWriteEarlier == -1 || CompareFileTime ( &ftWrite, &ftWriteEarlier) == -1)    {
            indexWriteEarlier = i;
            ftWriteEarlier = ftWrite;
         }

      }
   }
   //si no hemos encontrado ninguno, empezar por el 0
   if (indexWriteEarlier==-1) {
      indexWriteEarlier=0;
   }
   else                                                                       {
      //si hemos encontrado alguno, comprobar si el anterior en orden existe. Si no existe, usar el anterior en orden ( sucesivamente)
      m_bakIndex = max( indexWriteEarlier, 0);
      while (true)                                                            {

         indexWriteEarlier -=1;
         if (indexWriteEarlier<0) {
            indexWriteEarlier=MAX_BAK-1;
         }
         if (!exists[indexWriteEarlier]) {
            m_bakIndex = indexWriteEarlier;
         }
         else
            break;
      }
   }

   // if Xml File does not exists, we restore backup file.
   if (!FileExists(m_XmlFileName))                                            {
//      wchar_t msg[128];
//      swprintf(msg,L"%s %s", LoadText(L"XML_BACKUP_NOTICE"), m_XmlFileName);
//      if (showMessagesFlag)
//         ShowMsg( msg, L"WARNING", MY_MB_OK);
//
//      LOG_INFO2(msg, L"XML FILE WARNING");
//      if (!RestoreBakFile(false))                                             {
         // CreateDefaultFileXml();
         m_DocPtr = NULL;
//         LOG_WARN1(L"Could not restore backup file");

         if (!RestoreOthersBakFiles(showMessagesFlag))                        {
            // CreateDefaultFileXml();
            m_DocPtr = NULL;
            LOG_WARN1(L"Could not restore backup file");
            //ask the user to select a xml file
            wchar_t *fileName = SelectXmlFile();
            if ( fileName) {
               memset( m_XmlFileName, 0x00, sizeof(m_XmlFileName));
               wcsncpy( m_XmlFileName,  fileName, MAX_LENGTH_FULL_PATH-1);
               free( fileName);

            }
         }
//      }
   }
   //try to load restored or selected file
   if ( FileExists(m_XmlFileName))                                            {

      /* Load XML document  with config data */
      if (!LoadConfigFrom(m_XmlFileName)) {
         LOG_WARN1(L"Configuration Not Loaded.");


         // Free de document
         if (m_DocPtr) {
            m_DocPtr = NULL;
            xmlFreeDoc(m_DocPtr);
         }

         // Free the variables that may have been allocated by the parser
         xmlCleanupParser();
         xmlMemoryDump();

         //launch thread anyway, if required
         if ( asynchronousSave)
            LaunchThread();

         return -1;
      }
   }
   // Mutex to synchronize access
   if (m_DocPtr) {
      // TODO: use xmlMutex instead see more details
      m_mutexXmlDoc = CreateMutex(NULL, false, NULL);
      if (!m_mutexXmlDoc) {
         LOG_WARN1(L"Mutex for accesing XML Document not created!!. ");

      }

      LoadActiveTypeConfig();
   }
   if ( asynchronousSave)
      LaunchThread();


   return 0;
}

// ------------------------------------------------------------------------------
/**
 Funtion Tthread for the process
 */
unsigned WINAPI TXmlManager::ThreadFuncTXmlManager(LPVOID o) {

   TXmlManager *owner   = (TXmlManager*)o;
   HANDLE eventHandles[2];

   // LOG purposes
   LOGGER(L"Persistence");

   // Thread functionlity starts
   eventHandles[0] = owner->m_stopEvent;
   eventHandles[1] = owner->m_semaphore;

   int nEvents = 2;
   DWORD res;

   while ((res = WaitForMultipleObjects(nEvents, eventHandles, false, 500))   != WAIT_OBJECT_0) {
      int lastCode = 0;
      try                                                                     {
         #ifndef _DEBUG
         try                                                                  {
         #endif

            switch (res) {
            case WAIT_OBJECT_0 + 1:                                           {
               while (owner->GetList()->GetSize () > 0 )                      {
                  LT::TMessage *msg = NULL;
                  owner->GetList()->Pop(msg);
                  if  ((msg) && (msg->GetTStream() != NULL) )                 {
                      switch (msg->Type()) {
                      case MSG_SAVE_CFG: // Save all configuration from Module
                         lastCode = 1;

                         owner->SaveMsgCfg(msg);
                         break;
                      case MSG_SAVE_PARTIAL_CFG: // Save Partial Configuration
                         lastCode = 2;
                         owner->SavePartialMsgCfg(msg);
                         break;
                      case MSG_SET_TYPE_CONFIG_ACTIVE:
                         lastCode = 3;

                         owner->SetActiveTypeMsg(msg);
                         break;
                      case MSG_TYPE_CONFIG_NEW:
                         lastCode = 4;

                         owner->CreateTypeMsg(msg);
                         break;
                      case MSG_TYPE_CONFIG_DELETE:
                         lastCode = 5;

                         owner->DeleteTypeMsg(msg);
                         break;

                     case  MSG_SAVE_CFG_TO:
                     case  MSG_TYPE_CONFIG_RENAME:
                     case  MSG_TYPE_CONFIG_SAVE_AS:
                         lastCode = 6;

                        owner->ExecuteOtherMsg(msg);
                        break;

                      default:
                         break;
                      }
                     if (msg) {
                        delete msg;
                        msg = NULL;
                     }
                  }
                  else                                                        {
                      // couldn't get mutex of list
                        ReleaseSemaphore(eventHandles[1], 1, NULL);
                      // increment again counter, because WaitForMultiple... decremented it
                      // but we couldn't do a pop ( because of mutex)
                  }
               } // while
               break;
               } // case WAIT_OBJECT_0  + 1
            case WAIT_TIMEOUT:
               break;
            case WAIT_ABANDONED_0:
            default:
               break;
            } // switch

         #ifndef _DEBUG
      }
      __except(EXCEPTION_EXECUTE_HANDLER)                                     {
         // Hardware Exceptions
         unsigned long code = GetExceptionCode();
         ExceptionHandler( code, owner->GetMngr(), L"xmlMngr");
         wchar_t str[100];
         swprintf( str, L"SetConfiguration: lastCode = %d", lastCode);
         LOG_ERROR( str);

      }
      #endif
   }
   catch ( std::exception &ex)                                             {
      //C++ standard exceptions
      STD_EXCEPTION_HANDLER_2( L"Excep in xmlMngr", owner->GetMngr());
   }
   catch ( Exception &e)                                                   {
      //Builder C++ exceptions
      BUILDER_EXCEPTION_HANDLER_2( L"Excep in xmlMngr", owner->GetMngr());
   }
   catch (...)                                                             {
      DEFAULT_EXCEPTION_HANDLER_2( L"Excep in xmlMngr", owner->GetMngr());
   }


   } // while

   return 0;
}



// ------------------------------------------------------------------------------
/**
 Launch thread of the current process for asynchronous saving of configuration
 @return true if thread was sucessfully created, false otherwise
 */
bool TXmlManager::LaunchThread()                                              {


   if (!m_DocPtr)
      return false;

   if (m_thread)
      return true;
   else                                                                       {

      // Check states
      m_stopEvent = CreateEvent(NULL, true, false, NULL);
      m_semaphore = m_listBox->GetSemaphore();
      // authomatically reset event
      ResetEvent(m_stopEvent);
      unsigned idx;
      m_thread = (HANDLE)_beginthreadex(NULL,
          // address of thread security attributes
          (DWORD)0x10000, // initial thread stack size, in bytes
          ThreadFuncTXmlManager, // address of thread function
          this, // argument for new thread
          0, // creation flags
          &idx); // address of returned thread identifier
      SetThreadName ( idx,"XMLManagerT");

      // Prioridad
//      SetPriorityClass(m_thread, BELOW_NORMAL_PRIORITY_CLASS);
      SetThreadPriority(m_thread, BELOW_NORMAL_PRIORITY_CLASS);
      if ( m_thread)                                                          {
         m_asynchronousSave = true;
         return true;
      }
      else                                                                    {
         m_asynchronousSave = false;
         return true;
      }
   }

}

// ------------------------------------------------------------------------------
/**
 Stop Thread of the process
 @return true if the thread of process and all their child processes stopped sucessfully, false otherwise.
 */
bool TXmlManager::StopThread()                                                {

   if (m_thread) {

      SetEvent(m_stopEvent);
      LOG_DEBUG1(L"Persistence Stop thread event signaled");

      WaitForSingleObject(m_thread, INFINITE);
      LOG_DEBUG1(L"Persistence Thread Stopped");

      if (m_thread) {
         CloseHandle(m_thread);
         m_thread = NULL;
      }

      if (m_stopEvent) {
         ResetEvent(m_stopEvent);
         CloseHandle(m_stopEvent);
         m_stopEvent = NULL;
      }
      m_asynchronousSave = false;
      LT::TMessage * msg = NULL;
      while (m_listBox->Pop(msg))   {
         LOG_WARN1(L"Message discarded, thread stopped: ");
         LT::TMessage *resp = NULL;
         if (!ProcMsg(msg, resp))
            if (msg)   {
               delete msg;
               msg = NULL;
            }

      }

   }
   return true;
}

// ----------------------------------------------------------------------------
/**
 Restore Backup File
 @param showMessageFlag Flag to indicate Error Messages to be showed to the user.
If false, Error Messages must be only showed in the Logger Application.
 */
bool TXmlManager::RestoreBakFile(bool showMessagesFlag)                       {

   wchar_t *cpXmlFileName = _wcsdup(m_XmlFileName);
   bool ret = false;
   wchar_t bakFileName[1024];

   swprintf(bakFileName, L"%s.bak", cpXmlFileName);

   // If backup file exists
   if (TAux::FileExists(bakFileName))                                         {
      TAux::RemoveFile(cpXmlFileName);
      // Copy from bak file
      ret = (CopyFile((LPCTSTR) bakFileName, (LPCTSTR)cpXmlFileName,
          false) != 0);
      if (ret) {
         // Parse Xml file
         char* xml_FILE_NAME = TAux::WSTR_TO_STR(m_XmlFileName);
         if (xml_FILE_NAME) {
            if( m_DocPtr)                                                     {
               xmlFreeDoc(m_DocPtr);
               m_DocPtr = NULL;
            }
            m_DocPtr = xmlReadFile(xml_FILE_NAME,ENCODING_UTF_8,XML_PARSE_HUGE);  //vicmod added hugefiles support
//            m_DocPtr = xmlParseFile(xml_FILE_NAME);
            if (m_DocPtr == NULL) {
               wchar_t msg[128];
               swprintf(msg, L"%s : %s", LoadText(L"XML_BACKUP_ERROR"), m_XmlFileName);
               if (showMessagesFlag)
                  ShowMsg( msg, L"ERROR", MY_MB_OK);
               LOG_INFO2(msg, L"XML FILE WARNING");
               ret   = false;
            }
            else                                                              {
               ret = true;
               LOG_INFO1(L"XML FILE RESTORED OK");
            }
            delete[]xml_FILE_NAME;
         }
      }
      else {
         LOG_WARN1(L"Unable to copy the configuration .bak file");
      }
   }
   // Backup file does not exist
   else                                                                       {
      wchar_t msg[128];
      swprintf(msg, L"%s %s", LoadText(L"XML_BACKUP_NOTEXIST"), m_XmlFileName);
      if (showMessagesFlag)
         ShowMsg( msg,L"ERROR", MY_MB_OK);
      LOG_INFO2(msg, L"XML FILE WARNING");
   }

   if (cpXmlFileName)
      free(cpXmlFileName);
   return ret;
}
//-----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
/**
 Restore Backup File
 tries to parse bak files, orderly, by date
 @param showMessageFlag Flag to indicate Error Messages to be showed to the user.
 If false, Error Messages must be only showed in the Logger Application.
 */
bool TXmlManager::RestoreOthersBakFiles(bool showMessagesFlag)                {

   bool ret = false;

   //se deben de ordenar los bak, por orden de fecha de modificación, por lo que
   //se creará un buffer temporal, reordenando los ficheros
   FILETIME          ftCreate, ftAccess, ftWrite;
   HANDLE            hFile = INVALID_HANDLE_VALUE;

   struct s_bakInfo                                                            {
      wchar_t fileName[MAX_LENGTH_FULL_PATH];
      FILETIME time;
      s_bakInfo()                                                             {
         memset( fileName, 0x00, sizeof( fileName));
         time.dwLowDateTime = 0;
         time.dwHighDateTime = 0;
      }
   };

   s_bakInfo bakInfo[MAX_BAK];

   //se rellenan los valores en la structra y después será ordenada por fecha de modificación
   for ( int i = 0; i < MAX_BAK; i++)                                         {

      if (TAux::FileExists( m_fileBakName_list[i]))                           {

         hFile = INVALID_HANDLE_VALUE;

         hFile = CreateFile( m_fileBakName_list[i], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
         if ( hFile==NULL)
             continue;

         // Retrieve the file times for the file.
         GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
         CloseHandle( hFile);
         bakInfo[i].time = ftWrite;    //fecha ultima modificacion
         wcscpy( bakInfo[i].fileName, m_fileBakName_list[i]);
      }
   }
   // Ordenamiento por burbuja invertida , no tenemos en cuenta si existen o no, ya lo comprobaremos despues
   int j;
   FILETIME auxTIME;
   wchar_t auxName[MAX_LENGTH_FULL_PATH];
   int i = 1;
   bool ord = false;
   //en el indice mas alto se quedara el fichero mas temprano ( mas antiguo)
   while(!ord)                                                                {
      ord=true;
      for( j = 0; j < MAX_BAK-i; j++)                                         {

         if ( ( bakInfo[j].time.dwLowDateTime == 0  && bakInfo[j].time.dwHighDateTime== 0 ) || CompareFileTime ( &bakInfo[j].time, &bakInfo[j+1].time) == -1)    {

            // Intercambiar los elementos
            auxTIME                 = bakInfo[j].time;
            wcscpy( auxName, bakInfo[j].fileName);
            bakInfo[j].time         = bakInfo[j+1].time;
            wcscpy( bakInfo[j].fileName, bakInfo[j+1].fileName);
            bakInfo[j+1].time       = auxTIME;
            wcscpy( bakInfo[j+1].fileName, auxName);
            ord                     = false;// Indicador de vector ordenado
         }
      }
      i++;
   }

   //recorremos del mas reciente al mas antiguo
   for (int i = 0; i < MAX_BAK; i++)                                          {

      wchar_t *cpXmlFileName = _wcsdup(m_XmlFileName);

      // If backup file exists
      if ( TAux::FileExists( bakInfo[i].fileName))                            {
         TAux::RemoveFile(cpXmlFileName);
         // Copy from bak file
         ret = (CopyFile((LPCTSTR) bakInfo[i].fileName, (LPCTSTR)cpXmlFileName,
             false) != 0);
         if (ret)                                                             {
            // Parse Xml file
            char* xml_FILE_NAME = TAux::WSTR_TO_STR(m_XmlFileName);
            if (xml_FILE_NAME) {
               if( m_DocPtr)                                                  {
                  xmlFreeDoc(m_DocPtr);
                  m_DocPtr = NULL;
               }
               m_DocPtr =xmlReadFile(xml_FILE_NAME,ENCODING_UTF_8,XML_PARSE_HUGE);  //vicmod added hugefiles support
//               m_DocPtr = xmlParseFile(xml_FILE_NAME);
               if (m_DocPtr == NULL)                                          {
                  //no borramos el fichero en disco, puede ser util tenerlo para poderlo revisar
                  wchar_t msg[128];
                  swprintf(msg, L"%s : %s", LoadText(L"XML_BACKUP_ERROR"), m_XmlFileName);
                  LOG_INFO2(msg, L"XML FILE WARNING");
                  ret   = false;
               }
               else                                                           {
                  ret = true;
                  wchar_t txt[512];
                  swprintf( txt, L"XML BAK FILE RESTORED OK. Attempt: %d", i + 1);
                  LOG_INFO1(txt);
                  DISPATCH_EVENT( INFO_CODE, L"EV_BAK_RESTORED", txt, m_mngr);
               }
               delete[]xml_FILE_NAME;
            }
         }
         else                                                                 {
            LOG_WARN1(L"Unable to copy the configuration .bak file");
         }
      }

      if (cpXmlFileName)
         free(cpXmlFileName);

      if (ret)
         return true;
   }

   if (showMessagesFlag)                                                      {
      wchar_t msg[128];
      swprintf(msg, L"%s : %s", LoadText(L"XML_BACKUP_ERROR"), m_XmlFileName);
      if (showMessagesFlag)
         ShowMsg( msg, L"ERROR", MY_MB_OK);
   }

   return ret;
}

// ----------------------------------------------------------------------------

bool TXmlManager::RestoreSosFile(bool showMessagesFlag) {

   wchar_t *cpXmlFileName = _wcsdup(m_XmlFileName);
   bool ret = false;
   wchar_t bakFileName[1024];

   swprintf(bakFileName, L"%s.sos", cpXmlFileName);

   // If backup file exists
   if (TAux::FileExists(bakFileName)) {
      TAux::RemoveFile(cpXmlFileName);
      // Copy from bak file
      ret = (CopyFile((LPCTSTR) bakFileName, (LPCTSTR)cpXmlFileName,
          false) != 0);
      if (ret) {
         // Parse Xml file
         char* xml_FILE_NAME = TAux::WSTR_TO_STR(m_XmlFileName);
         if (xml_FILE_NAME) {
            if( m_DocPtr)                                                     {
               xmlFreeDoc(m_DocPtr);
               m_DocPtr = NULL;
            }
            m_DocPtr =xmlReadFile(xml_FILE_NAME,ENCODING_UTF_8,XML_PARSE_HUGE);  //vicmod added hugefiles support
//            m_DocPtr = xmlParseFile(xml_FILE_NAME);
            if (m_DocPtr == NULL) {
               wchar_t msg[128];
               swprintf(msg, L"%s : %s", L"Error: Fichero Seguridad no se pudo cargar", m_XmlFileName);
               if (showMessagesFlag)
                  ShowMsg( msg, L"ERROR", MY_MB_OK);
               FILE *file = fopen( "FALLOS.TXT", "a+");
               if( file)                                                      {
                  fprintf( file, "ERROR_CODE, XML SECURITY FILE WARNING %s\n", FormatDateTime( FormatSettings.ShortDateFormat+String(" ")+FormatSettings.LongTimeFormat, Now()));
                  fclose(file);
               }
               LOG_WARN1( LoadText(L"XML SECURITY FILE WARNING"));
//               if( m_mngr)                                                    {
//                  DISPATCH_EVENT( ERROR_CODE, L"XML SECURITY FILE WARNING", L"XML SECURITY FILE WARNING", m_mngr);
//               }
               ret = false;
            }
            else                                                              {
               ret = true;
               FILE *file = fopen( "FALLOS.TXT", "a+");
               if( file)                                                      {
                  fprintf( file, "XML SECURITY FILE RESTORED OK %s\n", FormatDateTime( FormatSettings.ShortDateFormat+String(" ")+FormatSettings.LongTimeFormat, Now()));
                  fclose(file);
               }
               LOG_INFO1( LoadText(L"XML SECURITY FILE RESTORED OK"));

//               if( m_mngr)
//                  DISPATCH_EVENT( INFO_CODE, L"XML SECURITY FILE RESTORED OK", L"XML SECURITY FILE RESTORED OK", NULL);
            }
            delete[]xml_FILE_NAME;
         }
      }
      else {
         LOG_WARN1(L"Unable to copy the configuration .bak file");
      }
   }
   // Backup file does not exist
   else {
      wchar_t msg[128];
      swprintf(msg, L"%s %s", L"Fichero Seguridad NO existe", m_XmlFileName);
      if (showMessagesFlag)
         ShowMsg( msg,L"ERROR", MY_MB_OK);

      FILE *file = fopen( "FALLOS.TXT", "a+");
      if( file)                                                      {
         fprintf( file, "NO XML SECURITY FILE EXIST %s\n", FormatDateTime( FormatSettings.ShortDateFormat+String(" ")+FormatSettings.LongTimeFormat, Now()));
         fclose(file);
      }
      LOG_WARN1( LoadText(L"NO XML SECURITY FILE EXIST"));
//      if( m_mngr)
//         DISPATCH_EVENT( ERROR_CODE, L"NO XML SECURITY FILE EXIST", L"NO XML SECURITY FILE EXIST", m_mngr);
   }

   if (cpXmlFileName)
      free(cpXmlFileName);
   return ret;
}
//-----------------------------------------------------------------------------
/**
   Load Other Xml File

   Shows the loadCfgForm, for load a configuration
*/
bool TXmlManager::LoadOtherXmlByForm()                                        {

   wchar_t* fileName = TXmlManager::SelectXmlFile();

   if ( fileName && CheckFile( fileName))                                     {
      if ( ResetAndLoadConfiguration( fileName))                              {
         free( fileName);
         return true;
      }
      free( fileName);

   }

   return false;

}
//------------------------------------------------------------------------------

/**
   SelectXmlFile
   opens a dialog form asking for an xml file to be selected
   memory returned must be freed using free
   @return string with selected file name, containging folder name
*/
wchar_t  *TXmlManager::SelectXmlFile()                                        {

   wchar_t* fileName = NULL;
   //Show LoadConfiguration form
   TLoadCfgDlg *dlg = new TLoadCfgDlg(Application, this, false);
   if (dlg)                                                                   {
      while (dlg->ShowModal() == mrOk)                                        {
         int index = dlg->cfglistbox->ItemIndex;
         if (index == -1)
            ShowMsg(L"Config not selected", L"WARNING",MY_MB_OK);
         else                                                                 {
            UnicodeString strToLoad = dlg->cfglistbox->Items->operator[](index);
            std::wstring tmp;
            if (dlg->localcb->Checked)
               tmp = std::wstring(dlg->LocalDir->Caption.c_str());
            else if ((dlg->externalcb->Enabled) && (dlg->externalcb->Checked))
               tmp = std::wstring(dlg->ExternalDir->Caption.c_str());

            tmp = tmp + L"\\" + strToLoad.c_str();
            fileName = _wcsdup(tmp.c_str());
            break;
         }
      }
      /// while
      delete dlg;
   }
   return fileName;
}

// ----------------------------------------------------------------------------

/**
   Create default configuration file
 */
void TXmlManager::CreateDefaultConfigFile() {

   // Create a new documnemt
   m_DocPtr = xmlNewDoc(BAD_CAST "1.0");

   xmlNode* node = xmlNewNode(NULL, (xmlChar*) LBL_LYRAAIS);
   xmlDocSetRootElement(m_DocPtr, node);
}

// ----------------------------------------------------------------------------
/**
 Create default file Xml
 @param   srcPath  Xml file name
 */
void TXmlManager::CreateDefaultFileXml(wchar_t* srcPath) {

   // Create a new documnemt
   m_DocPtr = xmlNewDoc(BAD_CAST "1.0");

   xmlNode* node = xmlNewNode(NULL, (xmlChar*) LBL_LYRAAIS);
   xmlDocSetRootElement(m_DocPtr, node);

   if (srcPath != NULL) {
      m_XmlFileName[0] = '\0';
      swprintf(m_XmlFileName, L"%s%s\0", m_workingPath,srcPath);
   }
   else {
      SYSTEMTIME lt;
      GetLocalTime(&lt);
      wchar_t t_now[32];
      swprintf(t_now, L"%02d%02d%02d_%02d%02d%02d%02d", lt.wDay, lt.wMonth,lt.wYear, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
      swprintf(m_XmlFileName, L"%spersistence_%s.xml", m_workingPath, t_now);
   }
}

// ----------------------------------------------------------------------------
/**
 Create bak file
 @param Suffix to be added to the filename (default current timestamp )
 @return True is backup file is created
 */
bool TXmlManager::CreateBakFile(wchar_t* suffix) {

   bool in_created = false;

   if (suffix == NULL) {
      SYSTEMTIME lt;
      GetLocalTime(&lt);
      suffix = new wchar_t[32];
      swprintf(suffix, L"%02d%02d%02d_%02d%02d%02d%02d", lt.wDay, lt.wMonth,lt.wYear, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
      in_created = true;
   }

   wchar_t old_bak_event_file[256], nametmp[256];

   // Size of the Xml file name
   unsigned int nbytes = wcslen(m_XmlFileName) - wcslen(L".xml");
   wcsncpy(nametmp, m_XmlFileName, nbytes);
   nametmp[nbytes] = L'\0';

   swprintf(old_bak_event_file, L"%s%s_%s.xml", m_workingPath, nametmp, suffix);
   CopyFile((LPCTSTR)m_XmlFileName, (LPCTSTR)old_bak_event_file, false);

   if (in_created)
      delete[]suffix;

   // Remove file and create new one.
   xmlFreeDoc(m_DocPtr);

   bool ret = TAux::RemoveFile(m_XmlFileName);
   if (ret) {
      wchar_t* tmp = _wcsdup(m_XmlFileName);
      if (tmp) {
         CreateDefaultFileXml(tmp);
         free(tmp);
         return true;
      }
   }
   return false;
}

// ----------------------------------------------------------------------------
/**
 ConvertInput:
 @in: string in a given encoding
 @encoding: the encoding used

 Converts @in into UTF-8 for processing with libxml2 APIs

 Returns the converted UTF-8 string, or NULL in case of error.
 */
xmlChar * TXmlManager::ConvertInput(const char *in, const char* encoding) {

   xmlChar *out;
   int ret, size, out_size, temp;
   xmlCharEncodingHandlerPtr handler;

   if (in == 0)
      return 0;

   handler = xmlFindCharEncodingHandler(encoding);

   if (!handler) {
      wchar_t txt[256];
      swprintf(txt, L"ConvertInput: no encoding handler found for '%s'\n",
          encoding ? encoding : "");
      LOG_DEBUG1(txt);
      return 0;
   }

   size = (int) strlen(in) + 1;
   out_size = size * 2 - 1;
   out = (unsigned char *) xmlMalloc((size_t) (out_size + 1));  // 9/5/2013 añado 1 porque luego se esta haciendo out[out_size]=0

   if (out != 0) {
      temp = size - 1;
      ret = handler->input(out, &out_size, (const xmlChar*) in, &temp);
      if ((ret < 0) || (temp - size + 1)) {
         if (ret < 0)                                                         {
            LOG_DEBUG1(L"ConvertInput: conversion wasn't successful.");
         }
         else {
            wchar_t txt[256];
            swprintf(txt,
                L"ConvertInput: conversion wasn't successful. converted: %i octets.",
                temp);
            LOG_DEBUG1(txt);
         }
         xmlFree(out);
         out = NULL; //9/5/2013
      }
      else
         out[out_size] = 0; /* null terminating out */
   }
   else
      LOG_DEBUG1(L"ConvertInput: no mem");

   return out ? out : NULL;
}

// ----------------------------------------------------------------------------
/**
 ConverOutput:
 @in: xmkChar string in a given encoding
 @encoding: the encoding used

 Converts @in into char* for processing

 Returns the converted UTF-8 string, or NULL in case of error.
 */
char* TXmlManager::ConvertOutput(xmlChar *in, const char* encoding) {

   char* out;
   int ret, size, out_size, temp;

   xmlCharEncodingHandlerPtr handler;

   if (in == 0)
      return 0;

   handler = xmlFindCharEncodingHandler(encoding);
   if (!handler) {
      wchar_t msg[512];
      swprintf(msg, L"ConvertOutput: no encoding handler found for '%s'",
          encoding ? encoding : "");
      LOG_DEBUG1(msg);
      return 0;
   }
   size = (int) strlen(in) + 1;
   out_size = size * 2 - 1;
   out = (unsigned char *) malloc((size_t) (out_size + 1));   // 9/5/2013 añado 1 porque luego se esta haciendo out[out_size]=0

   if (out != 0) {
      temp = size - 1;
      ret = handler->output(out, &out_size, (const xmlChar*) in, &temp);
      if ((ret < 0) || (temp - size + 1)) {
         if (ret < 0) {
            LOG_DEBUG1(L"ConvertInput: conversion wasn't successful.");
         }
         else {
            wchar_t msg[256];
            swprintf(msg,
                L"ConvertInput: conversion wasn't successful. converted: %i octets.",
                temp);
            LOG_DEBUG1(msg);
         }
         free(out);
         out = NULL;
      }
      else {
         out[out_size] = 0; // null terminating out
      }
   }
   else
      LOG_DEBUG1(L"ConvertInput: no mem .");
   return out ? out : NULL;
}

// ----------------------------------------------------------------------------

/**
 ConverOutput:
 @in: xmkChar string in a given encoding
 @encoding: the encoding used

 Converts @in into char* for processing

 Returns the converted UTF-8 string, or NULL in case of error.
 */
xmlChar *TXmlManager::ConvertInputWC(const wchar_t *in, const char* encoding) {

   // return ConvertInput ((char*)in, encoding);
   char* c_in = TAux::WSTR_TO_STR(in);
   if (c_in) {
      xmlChar* xmlOutValue = ConvertInput(c_in, encoding);
      delete[]c_in;
      return xmlOutValue;
   }
   return NULL;
}

// ----------------------------------------------------------------------------

/**
 ConverOutput:
 @in: xmkChar string in a given encoding
 @encoding: the encoding used

 Converts @in into char* for processing

 Returns the converted UTF-8 string, or NULL in case of error.
 */
wchar_t* TXmlManager::ConvertOutputWC(xmlChar *in, const char* encoding) {

   char* out = ConvertOutput(in, encoding);
   if (out) {
      // wchar_t* outVal = (wchar_t*) out;
      wchar_t* outVal = TAux::STR_TO_WSTR(out);
      free(out);
      return outVal;
   }
   return NULL;
}

// ----------------------------------------------------------------------------
/**
 Get Node to be updated, If not exists, parent node is returned.
 @param ms stream containing configuration
 */
void TXmlManager::ExtractTokens(wchar_t *str, std::list<std::wstring> &tokens,
    wchar_t* regex) {

   wchar_t *p = _wcsdup(str); // Put un p the string value
   wchar_t *q = p; // Put in q the p value
   wchar_t *ptoken;

   while ((ptoken = wcstok(q, regex)) != NULL) {
      tokens.push_back(std::wstring(ptoken));
      q = q + wcslen(ptoken) + 1;
      if (q >= p + wcslen(str))
         break;
   }
   free(p);
}

// ----------------------------------------------------------------------------

/**
 Get Node to be updated, If not exists, parent node is returned.
 @param ms stream containing configuration
 */
void TXmlManager::ExtractTokens(char *str, std::list<std::string> &tokens,
    char* regex) {

   char *p = strdup(str);
   char *q = p;
   char *ptoken;

   while ((ptoken = strtok(q, regex)) != NULL) {

      tokens.push_back(std::string(ptoken));

      q = q + strlen(ptoken) + 1;
      if (q >= p + strlen(str))
         break;
   }
   free(p);
}

// ----------------------------------------------------------------------------
/**
   Return into xPathObj parameter, nodes found in xPathExpr expression.
   @param   xPathExpr xPath expression.
   @param   xPathObj nodes found matching xPathExpr, NULL otherwise
   @param   docPtr is a pointer to the document the document
 */
bool TXmlManager::XmlGetNodesFromXpath(char* xPathExpr,
    xmlXPathObjectPtr & xPathObj, xmlDocPtr docPtr ) {

   if ( docPtr == NULL)
      docPtr = m_DocPtr;

   xmlXPathContextPtr xPathCtx;
   bool ret = false;

   /* Create xpath evaluation context */
   xPathCtx = xmlXPathNewContext( docPtr);
   if (xPathCtx == NULL) {
      LOG_DEBUG1(L"Error: unable to create new XPath context");
      return false;
   }

   xmlChar* ConvertedxPathExpr = ConvertInput(xPathExpr);

   if (ConvertedxPathExpr) {

      /* Evaluate xpath expression */
      xPathObj = xmlXPathEvalExpression(ConvertedxPathExpr, xPathCtx);
      if (xPathObj == NULL) {
         wchar_t txt[512];
         swprintf(txt, L"Error: unable to evaluate xpath expression %s",
             xPathExpr);
         LOG_DEBUG1(txt);
         xmlXPathFreeContext(xPathCtx);
         xmlFree(ConvertedxPathExpr);
         return false;
      }

      ret = (xPathObj->nodesetval) ? (xPathObj->nodesetval->nodeNr > 0) : false;

      /* Cleanup */
      if (!ret)
         xmlXPathFreeObject(xPathObj);

      xmlFree(ConvertedxPathExpr);
      xmlXPathFreeContext(xPathCtx);
   }
   return ret;
}

// ----------------------------------------------------------------------------
///**
//   Return into xPathObj parameter, nodes found in xPathExpr expression.
//   @param docPtr is pointer to a document
//   @param   xPathExpr xPath expression.
//   @param   xPathObj nodes found matching xPathExpr, NULL otherwise
// */
//bool TXmlManager::XmlGetNodesFromXpath( xmlDocPtr   docPtr, char* xPathExpr,
//    xmlXPathObjectPtr & xPathObj) {
//
//   xmlXPathContextPtr xPathCtx;
//   bool ret = false;
//
//   /* Create xpath evaluation context */
//   xPathCtx = xmlXPathNewContext(docPtr);
//   if (xPathCtx == NULL) {
//      LOG_DEBUG1(L"Error: unable to create new XPath context");
//      return false;
//   }
//
//   xmlChar* ConvertedxPathExpr = ConvertInput(xPathExpr);
//
//   if (ConvertedxPathExpr) {
//
//      /* Evaluate xpath expression */
//      xPathObj = xmlXPathEvalExpression(ConvertedxPathExpr, xPathCtx);
//      if (xPathObj == NULL) {
//         wchar_t txt[512];
//         swprintf(txt, L"Error: unable to evaluate xpath expression %s",
//             xPathExpr);
//         LOG_DEBUG1(txt);
//         xmlXPathFreeContext(xPathCtx);
//         xmlFree(ConvertedxPathExpr);
//         return false;
//      }
//
//      ret = (xPathObj->nodesetval) ? (xPathObj->nodesetval->nodeNr > 0) : false;
//
//      /* Cleanup */
//      if (!ret)
//         xmlXPathFreeObject(xPathObj);
//
//      xmlFree(ConvertedxPathExpr);
//      xmlXPathFreeContext(xPathCtx);
//   }
//   return ret;
//}

// ----------------------------------------------------------------------------

/**
 precondition: Init already executed
 returns value of node machineCode in the xml file
 */
char *TXmlManager::GetFileCode() {

   /* Create XPathExpression */
   char xPathExpr[MAX_XPATH_EXPR] = "//xmlCode";

   // Search XPath
   xmlXPathObjectPtr xPathObj;

   if (!XmlGetNodesFromXpath(xPathExpr, xPathObj))
      return NULL;

   // Selected Nodes
   xmlNodeSetPtr nodes = xPathObj->nodesetval;
   int sizeList = (nodes) ? nodes->nodeNr : 0;

   for (int i = 0; i < sizeList; i++) {
      if (nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
         xmlNode* node = nodes->nodeTab[i];
         xmlChar* value = xmlGetProp(node, "name");

         if (value != NULL) {
            char* code = strdup(value);
            xmlFree(value);
            return code;
         }
      }
   }
   /* Cleanup */
   xmlXPathFreeObject(xPathObj);
   return NULL;
}

//------------------------------------------------------------------------------

/**
 precondition: Init already executed
 returns value of node machineCode in the xml file
 */
char *TXmlManager::GetFileCode( xmlDocPtr  docPtr)                            {

   /* Create XPathExpression */
   char xPathExpr[MAX_XPATH_EXPR] = "//xmlCode";

   // Search XPath
   xmlXPathObjectPtr xPathObj;

   if (!XmlGetNodesFromXpath( xPathExpr, xPathObj, docPtr))
      return NULL;

   // Selected Nodes
   xmlNodeSetPtr nodes = xPathObj->nodesetval;
   int sizeList = (nodes) ? nodes->nodeNr : 0;

   for (int i = 0; i < sizeList; i++) {
      if (nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
         xmlNode* node = nodes->nodeTab[i];
         xmlChar* value = xmlGetProp(node, "name");

         if (value != NULL) {
            char* code = strdup(value);
            xmlFree(value);
            return code;
         }
      }
   }
   /* Cleanup */
   xmlXPathFreeObject(xPathObj);
   return NULL;
}

// ----------------------------------------------------------------------------
/**
 Check if Config is Loaded
 */
bool TXmlManager::ConfigLoaded() {
   if (m_DocPtr != 0)
      return true;
   return false;
}

// ----------------------------------------------------------------------------
/**
   Load configuration from fileNamePath.
   @param   fileNamePath
   @return bool true if configuration from fileNamePath has been succesfully loaded, false otherwise.
 */
bool TXmlManager::MyLoadConfigFrom(wchar_t* fileNamePath)                     {
   //ToDo:  parece que por un lado se le pasa el fileNamePath a CheckFile, pero no a RestoreBakFile, que utiliza una variable interna ??
   if (!CheckFile(fileNamePath))                                              {
     if (!RestoreBakFile(false))                                              {
         if (!RestoreOthersBakFiles( true))                                   {

            if (!RestoreSosFile(true))                                        {

               if( m_DocPtr)                                                  {
                  xmlFreeDoc(m_DocPtr);
                  m_DocPtr = NULL;
               }
               LOG_WARN1(L"Could not restore backup file");

               //Load other configuration with form
               if (!LoadOtherXmlByForm())                                     {
                  LOG_WARN1(L"Could not load file");
                  return false;
               }
               else
                  LOG_INFO1(L"Configuration has been loaded");
            }
         }
      }
   }

   char* xml_tmp = TAux::WSTR_TO_STR(m_XmlFileName);
   if (xml_tmp) {
      if( m_DocPtr)                                                           {
         xmlFreeDoc(m_DocPtr);
         m_DocPtr = NULL;
      }
      m_DocPtr =xmlReadFile(xml_tmp,ENCODING_UTF_8,XML_PARSE_HUGE);  //vicmod added hugefiles support
//      m_DocPtr = xmlParseFile(xml_tmp);
      delete[]xml_tmp;
   }
//
   if (m_DocPtr == NULL)   {
      LOG_WARN1(L"Could not restore backup file");
      return false;
   }
//      wchar_t msg[128];
//      swprintf(msg, L"Error: unable to parse file %s. Backup File will be loaded instead.",m_XmlFileName);
//      LOG_INFO2(msg, L"XML FILE WARNING");
//      if (!RestoreBakFile(true)) {
//         // CreateDefaultConfigFile();
//         m_DocPtr = NULL;
//         LOG_WARN1(L"Could not restore backup file");
//         return false;
//      }
//   }

   return true;
}



// ----------------------------------------------------------------------------
/**
   Parse configuration file
   @return true if parsed, false if not.

 */
bool TXmlManager::ParseFileError () {

   // Una forma de hacerlo
   bool ret = false;
   xmlDocPtr   docPtr = NULL;
   char* xml_tmp = TAux::WSTR_TO_STR(m_XmlFileName);
   if (xml_tmp)                                                               {

      docPtr =xmlReadFile(xml_tmp,ENCODING_UTF_8,XML_PARSE_HUGE);  //vicmod added hugefiles support
//      docPtr = xmlParseFile(xml_tmp);
      if (docPtr)                                                             {
         xmlFreeDoc(docPtr);
         ret =  true;
      }
      delete[]xml_tmp;
   }
   return ret;
}

// ----------------------------------------------------------------------------
/**
   Parse configuration tree loaded in memory.
   @return true if parsed, false if not.
 */
bool TXmlManager::ParseConfig(xmlDocPtr doc )                                 {

   if (doc == NULL)
      doc = m_DocPtr;

   bool ret = false;


   xmlChar *xmlbuff;
   xmlDoc *xmldoc;
   int buffersize;

//   xmlSaveFileEnc("parseaux.xml",doc,ENCODING_UTF_8);     //To check fail of libxml in a future
//   return CheckFile(L"parseaux.xml");

//   LOG_INFO1(L"StartParsing generate string");
   xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize,0);  //DUMPS MEMORY TO xmlBuff variable
   xmlErrorPtr error = xmlGetLastError();
   if(error) {
      wchar_t buffer[512]={L""};
      wchar_t * werror=TAux::STR_TO_WSTR(error->message);
      wcscat(buffer, L"Error parsing configuration document: ");
      wcscat(buffer,werror);
      LOG_WARN1(buffer);
      delete [] werror;
   }

//   wchar_t buffer[1024];
//   swprintf(buffer, L"Error parsing configuration document: %s\0", TAux::STR_TO_WSTR(error->message));
//   LOG_INFO1(buffer);
   if (xmlbuff)   {
      xmldoc = xmlParseDoc (xmlbuff);
      error = xmlGetLastError();
      if(error) {
         wchar_t buffer[512]={L""};
         wchar_t * werror=TAux::STR_TO_WSTR(error->message);
         wcscat(buffer, L"Error parsing configuration document: ");
         wcscat(buffer,werror);
         LOG_WARN1(buffer);
         delete [] werror;
      }
      if  (xmldoc)                            {
         ret = true;
         xmlFreeDoc(xmldoc);
//         LOG_INFO1(L"Parsed sccesfully");
      }
//      else LOG_INFO1(L"Parsed failed");
      xmlFree(xmlbuff);
   }


   if ( ret)                                                                  {
      //check schema
      ret = is_valid( doc, "LyraTechs.xsd");
   }

   return ret;
}


// ----------------------------------------------------------------------------

bool TXmlManager::SaveConfigTo( wchar_t* srcPath )                     {
   return SaveSpecifiedConfigTo( srcPath, m_DocPtr);
}
// ----------------------------------------------------------------------------
/**
 SaveSpecifiedConfigTo
 @param   fileNamePath   New File Name where configuration will be save, NULL value will save configuration into the current file.
 @param   parse If true check the config
 */
bool TXmlManager::SaveSpecifiedConfigTo( wchar_t* srcPath, xmlDocPtr docPtr)  {

   if (docPtr == NULL)                                                        {
      LOG_ERROR1(L"NULL Pointer to XML Document.");
      return false;
   }

   //vic: deleted because is so time consuming and we will perform  after a parse of the saved file
//   if (!ParseConfig( docPtr))                                                 {
//      LOG_ERROR1(L"Error in parsing");
//      return false;
//   }


   bool ret = false;
   if (srcPath == NULL)                                                       { // General Case.
//      wchar_t fileBakName[MAX_LENGTH_FULL_PATH];
//      swprintf(fileBakName, L"%s.bak", m_XmlFileName);

//      // sec file, only to be created when none exception has ocorred up to the moment
//      wchar_t fileSecName[MAX_LENGTH_FULL_PATH];
//      swprintf(fileSecName, L"%s.sec", m_XmlFileName);

      wchar_t* tmpfileNamePath =  new wchar_t[wcslen(m_XmlFileName) + wcslen(L".tmp") + 1];
      if (tmpfileNamePath==NULL) {
         LOG_WARN2(L"Error getting memory", m_XmlFileName);
         return false;
      }
      swprintf(tmpfileNamePath, L"%s.tmp\0", m_XmlFileName);
#ifndef _DEBUG
      try {
#endif
        // 1.- Create a  temporal bak file
         if (_wrename(m_XmlFileName, tmpfileNamePath) != 0)                   {
            LOG_DEBUG1(L"Error renaming file");
         }

         char* xml_tmp = TAux::WSTR_TO_STR(m_XmlFileName);
         if (xml_tmp) {
//               LOG_INFO1(L"T/ -  -  -  -  TXmlManager::SaveSpecifiedConfigTo saving file");
            if (docPtr && xmlSaveFormatFileEnc(xml_tmp,docPtr, ENCODING_UTF_8, 0) > 0){        //vic changed format to 0

//                LOG_INFO1(L"T/ -  -  -  -  TXmlManager::SaveSpecifiedConfigTo parsing file");
               LOG_DEBUG1( L"SaveSpecifiedConfigTo, after xmlSaveFormatFileEnc");
               xmlDocPtr checkDocPtr =xmlReadFile(xml_tmp,ENCODING_UTF_8,XML_PARSE_HUGE);
               if(checkDocPtr)   {
                  LOG_DEBUG1( L"SaveSpecifiedConfigTo, after xmlParseFile");
//                  LOG_INFO1(L"T/ -  -  -  -  TXmlManager::SaveSpecifiedConfigTo parsed file");
                  ret = true;
                  m_toSave = false;       //as we saved the conf, we set toSave to false
                  xmlFreeDoc( checkDocPtr);
               }
               else{
                  LOG_ERROR1(L"Error in parsing saved conf file");
                  ret = false;
                  xmlErrorPtr error = xmlGetLastError();
                  if(error) {
                     wchar_t buffer[512]={L""};
                     wchar_t * werror=TAux::STR_TO_WSTR(error->message);
                     wcscat(buffer, L"Error parsing configuration document: ");
                     wcscat(buffer,werror);
                     LOG_WARN1(buffer);
                     delete [] werror;
                  }
               }
            }
            ret = true;
            delete[]xml_tmp;
         }
         else                                                                 {
            LOG_WARN2(L"Error getting memory", m_XmlFileName);
            return false;
         }
#ifndef _DEBUG
      } __except(EXCEPTION_EXECUTE_HANDLER)                             {

         // Restoring ...
         //05/12/2012 este rename estaba al reves
         //if (_wrename(m_XmlFileName, tmpfileNamePath) != 0) {
         if (_wrename( tmpfileNamePath, m_XmlFileName) != 0) {
            LOG_WARN2(L"File Not restored. Name: ", tmpfileNamePath);
//            // intentamos restaurar el temporal contra el .bak
//            if (_wrename( tmpfileNamePath, fileBakName) != 0)
//               LOG_WARN2(L"File Not restored. Name: ", fileBakName);
            //intentamos restaurar el temporal contra el .bak
            if (_wrename( tmpfileNamePath, m_fileBakName_list[m_bakIndex]) != 0)
               LOG_WARN2(L"File Not restored. Name: ", m_fileBakName_list[m_bakIndex]);
         }

         if (tmpfileNamePath != NULL)
            delete[]tmpfileNamePath;
         tmpfileNamePath = NULL;
         return false;

      }
#endif
      // Save OK
      if (ret)                                                                {
//         LOG_INFO1(L"T/ -  -  -  -  TXmlManager::SaveSpecifiedConfigTo removing temp");
         bool val = true;
         if (TAux::FileExists(m_fileBakName_list[m_bakIndex]))                                   {
            if (_wremove(m_fileBakName_list[m_bakIndex]) != 0)                                   {
               LOG_WARN1(L"Error removing file");
               val = false;
            }
         }
         LOG_DEBUG1( L"SaveSpecifiedConfigTo, after _wremove bak file");

         // Rename
         if ( val)                                                            {

//

//            //se va a hacer una lista de .baks, teniendo como máximo el define MAX_BAK.
//            //Se tiene un index, para así guardar cada vez uno
//            if ( CopyFile((LPCTSTR) tmpfileNamePath, (LPCTSTR)m_fileBakName_list[m_bakIndex], false) == 0) {
//               LOG_WARN2(L"Error creting a .bak array", m_fileBakName_list[m_bakIndex]);
//            }

            if (_wrename(tmpfileNamePath, m_fileBakName_list[m_bakIndex]) != 0)
               LOG_WARN2(L"Error renaming tmp file to ",  m_fileBakName_list[m_bakIndex]);

//
//            //seguridad: volver a parsear el fichero
//            char* xml_tmp = TAux::WSTR_TO_STR(tmpfileNamePath);
//            if (xml_tmp)                                                      {
//               xmlDocPtr checkDocPtr =xmlReadFile(xml_tmp,ENCODING_UTF_8,XML_PARSE_HUGE);  //vicmod added hugefiles support
//               //xmlDocPtr checkDocPtr = xmlParseFile(xml_tmp);
//               if (checkDocPtr)                                               {
//                  if (_wrename(tmpfileNamePath, m_fileBakName_list[m_bakIndex]) != 0)            {
//                     LOG_WARN1(L"Error renaming file");
//                  }
//                  xmlFreeDoc( checkDocPtr);
//               }
//               else                                                           {
//                  LOG_WARN2(L"Error getting memory for parsing tmp file", tmpfileNamePath);
//               }
//            }
            //se aumenta el indice
            if (++m_bakIndex >= MAX_BAK)
               m_bakIndex = 0;

         }
      }
      // Save No-OK
      else                                                                    {
         if (_wremove(m_XmlFileName) != 0)                                    {
            LOG_WARN2(L"Error removing file: " ,m_XmlFileName );

         }
         if (_wrename(tmpfileNamePath, m_XmlFileName) != 0)                   {
            LOG_WARN2(L"File Not restored. Name: ", m_XmlFileName);

         }
      }

      if (tmpfileNamePath != NULL)
         delete[]tmpfileNamePath;
      tmpfileNamePath = NULL;
      LOG_DEBUG1( L"After delete tmpfileNamePath");

   }
   else    {//Used by interface. Writes configuration into an specific file name.                                                                 {
      wchar_t  newFileNamePath[MAX_LENGTH_FULL_PATH];
      if (IsRelativePath(srcPath))
         swprintf(newFileNamePath, L"%s%s\0", m_workingPath,srcPath);
      else  {
         wcscpy(newFileNamePath,srcPath);
         newFileNamePath[wcslen(srcPath)] = 0;;
      }

      wchar_t* tmpfileNamePath;
      if (wcsstr(newFileNamePath, L".xml")) {
         tmpfileNamePath = new wchar_t[wcslen(newFileNamePath) + 1];
         swprintf(tmpfileNamePath, L"%s", newFileNamePath);
      }
      else {
         tmpfileNamePath =
             new wchar_t[wcslen(newFileNamePath) + wcslen(L".xml") + 1];
         swprintf(tmpfileNamePath, L"%s.xml\0", newFileNamePath);
      }

      LOG_DEBUG1( L"SaveSpecifiedConfigTo, before  xmlSaveFormatFileEnc");
      char* xml_tmp = TAux::WSTR_TO_STR(tmpfileNamePath);

      if (xml_tmp) {

         if ( docPtr && xmlSaveFormatFileEnc(xml_tmp, docPtr, ENCODING_UTF_8, 0) < 0) {          //vic changed format to 0
            LOG_WARN2(L"Configuration could not be save into file. Name:", tmpfileNamePath);
            ret = false; // Not Save
         }
         else   {
            ret = true;
            LOG_DEBUG1(L"SaveSpecifiedConfigTo saved set mtosave to false");
            m_toSave = false;       //as we saved the conf, we set toSave to false
            xmlMemoryDump();
         }

         delete[]xml_tmp;
      }
      LOG_DEBUG1( L"SaveSpecifiedConfigTo, after  xmlSaveFormatFileEnc");

      if (tmpfileNamePath)
         delete[]tmpfileNamePath;
   }

   xmlMemoryDump();
   LOG_DEBUG1( L"After xmlMemoryDump");
   return ret;
}

// ----------------------------------------------------------------------------
/**
 Process messages
 @param   msg   Message
 */

bool TXmlManager::SaveMsgCfg(LT::TMessage *msg) {

   bool ret = true;

   if (m_mutexXmlDoc && WaitForSingleObject(m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0)    {
   try         {

      if (!SetConfig(msg->GetTStream()))   {
         ret = false;
      }
   }
   __finally   {
         ReleaseMutex(m_mutexXmlDoc);
      }
   }

   wchar_t sourceModuleName[addressSize];
   msg->GetOrg(sourceModuleName, sizeof(sourceModuleName));

   if (ret)                                                                  {
      LOG_INFO2(L"Configuration has been saved. Module: ", sourceModuleName);
   }
   else
      LOG_INFO2(L"Configuration could not be saved. Module: ",
       sourceModuleName);

   return ret;
}

// ----------------------------------------------------------------------------
/**
 Process messages
 @param   msg   Message
 */

bool TXmlManager::SavePartialMsgCfg(LT::TMessage *msg) {

   bool ret = true;
   wchar_t sourceModuleName[addressSize];
   msg->GetOrg(sourceModuleName, sizeof(sourceModuleName));


   LOG_DEBUG1(L"Partial configuration begins");
   if (m_mutexXmlDoc && WaitForSingleObject(m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0) {
      try {

         LOG_DEBUG1(L"Partial configuration got mutex");
         if (!SetConfigPartial(msg->GetTStream()))
            ret = false;
      }
      __finally {
         ReleaseMutex(m_mutexXmlDoc);
      }
   }

   if (ret){
      LOG_INFO2(L"Partial configuration has been saved. Module: ", sourceModuleName);
   }
   else
      LOG_INFO2(L"Partial configuration could not be saved. Module: ", sourceModuleName);

   return ret;
}

// ----------------------------------------------------------------------------
/**
   Process messages.
 */
bool TXmlManager::SetActiveTypeMsg(LT::TMessage *msg) {

   wchar_t *newActiveConfigName = new wchar_t[msg->DataSize() + 1];
   if (!newActiveConfigName) {
      LOG_ERROR1(L"Can't alocate memory");
      return false;
   }

   newActiveConfigName[0] = L'\0';
   wmemcpy(newActiveConfigName, (wchar_t*)msg->DataAsRaw(), msg->DataSize());
   newActiveConfigName[msg->DataSize()] = L'\0';

   bool ret = SetActiveTypeConfig(newActiveConfigName); // Mutex inside method

   if (newActiveConfigName)
      delete[]newActiveConfigName;

   return ret;
}

// ----------------------------------------------------------------------------
/**
   Process messages.
 */
bool TXmlManager::CreateTypeMsg(LT::TMessage *msg) {

   wchar_t sourceModuleName[addressSize];
   msg->GetOrg(sourceModuleName, sizeof(sourceModuleName));

   wchar_t *newActiveConfigName = new wchar_t[msg->DataSize() + 1];
   if (!newActiveConfigName) {
      LOG_ERROR1(L"Can't alocate memory");
      return false;
   }

   newActiveConfigName[0] = L'\0';
   wmemcpy(newActiveConfigName, (wchar_t*)msg->DataAsRaw(), msg->DataSize());
   newActiveConfigName[msg->DataSize()] = L'\0';

   bool ret = CreateTypeConfig(newActiveConfigName);      // Mutex inside.

   if (newActiveConfigName)
      delete[]newActiveConfigName;
   return ret;
}

// ----------------------------------------------------------------------------
/**
   Process message.
 */
bool TXmlManager::DeleteTypeMsg(LT::TMessage *msg) {

   wchar_t *deleteActiveConfigName = new wchar_t[msg->DataSize() + 1];
   if (!deleteActiveConfigName) {
      LOG_ERROR1(L"Can't alocate memory");
      return false;
   }

   deleteActiveConfigName[0] = L'\0';
   wmemcpy(deleteActiveConfigName, (wchar_t*)msg->DataAsRaw(), msg->DataSize());
   deleteActiveConfigName[msg->DataSize()] = L'\0';

   bool ret = DeleteTypeConfig(deleteActiveConfigName); // Mutex inside

   if (deleteActiveConfigName)
      delete[]deleteActiveConfigName;

   return ret;

}
// ----------------------------------------------------------------------------
/**
   Check if filepath exists,  is relative or absolute.
 */
bool TXmlManager::CheckFile(wchar_t *checkFileName)                           {

   if (checkFileName == NULL)
      return false;

   xmlDocPtr   docPtr = NULL;
   wchar_t fileName[1024];

   if (IsRelativePath(checkFileName))
      swprintf(fileName, L"%s%s\0", m_workingPath,checkFileName);
   else  {
      wcscpy(fileName,checkFileName);
      fileName[wcslen(checkFileName)] = 0;;
   }

   if (!FileExists(fileName))                                                 {
      wchar_t msg[1024];
      swprintf(msg, L"%s %s", LoadText(L"XML_NOTEXIST"), fileName);
      ShowMsg( msg,L"ERROR", MY_MB_OK);
      LOG_WARN2(msg, L"XML FILE WARNING");

      return false;
   }
   else {
      char* xml_tmp = TAux::WSTR_TO_STR(fileName);
      if (xml_tmp) {
         docPtr = xmlReadFile(xml_tmp,ENCODING_UTF_8,XML_PARSE_HUGE);  //vicmod added hugefiles support
//         docPtr = xmlParseFile(xml_tmp);
         delete[]xml_tmp;
      }
      if (docPtr) {

         bool ret = is_valid( docPtr, "LyraTechs.xsd");

         xmlFreeDoc(docPtr);
         if ( !ret)                                                        {
            wchar_t msg[1024];
            swprintf(msg, L"%s %s", LoadText(L"XML_INVALID"), fileName);
            ShowMsg( msg,L"ERROR", MY_MB_OK);
            LOG_WARN2(msg, L"XML FILE WARNING");
         }
         return ret;
      }
   }

   return false;
}



//-----------------------------------------------------------------------------

bool TXmlManager::ValidateMachineCodeInFile( wchar_t* fileNamePath)           {

   bool ret = false;
   if (fileNamePath == NULL)
      return false;

   xmlDocPtr   docPtr = NULL;
   wchar_t fileName[1024];

   if (IsRelativePath(fileNamePath))
      swprintf(fileName, L"%s%s\0", m_workingPath,fileNamePath);
   else  {
      wcscpy(fileName,fileNamePath);
      fileName[wcslen(fileNamePath)] = 0;
   }

   if (!FileExists(fileName))                                                 {
      return false;
   }
   else {
      char* xml_tmp = TAux::WSTR_TO_STR(fileName);
      if (xml_tmp) {
         docPtr = xmlReadFile(xml_tmp,ENCODING_UTF_8,XML_PARSE_HUGE);  //vicmod added hugefiles support
//         docPtr = xmlParseFile(xml_tmp);
         delete[]xml_tmp;
      }
      if (docPtr) {
         char *machineCode = TXmlManager::GetFileCode( docPtr);
         ret =  THardlockChecker::ValidateMachineCode( machineCode);
         xmlFreeDoc(docPtr);
         free( machineCode);
         return ret;
      }
   }

   return false;


}



bool TXmlManager::SaveConfigToMsg(LT::TMessage *msg)                         {

   bool ret = false;
   //en la primera linea llega el nombre del fichero
   wchar_t newName[MAX_PATH+1];
   int size;
   LT::TStream *stream = msg->GetTStream();
   if ( stream == NULL)
      return false;

   stream->ReadLn( newName, MAX_PATH+1, size);
   if ( newName[0]==0) {
      LOG_ERROR1(L"Can't alocate memory");
      return false;
   }


   if (m_mutexXmlDoc && WaitForSingleObject(m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0)    {

      //duplicar el árbol XML
      xmlDocPtr   docPtr =  xmlCopyDoc( m_DocPtr, 1);
      try         {
         ret = XmlDecodingConfig( stream, docPtr);

         wchar_t sourceModuleName[addressSize];
         msg->GetOrg(sourceModuleName, sizeof(sourceModuleName));

         if (ret){
            LOG_INFO2(L"Configuration has been saved. Module: ", sourceModuleName);
         }
         else
            LOG_INFO2(L"Configuration could not be saved. Module: ",
             sourceModuleName);



         //actualizar el arbol ( copia) con el contenido del stream
         ret =    ret && SaveSpecifiedConfigTo( newName, docPtr);
         xmlFreeDoc( docPtr);
      }
      __finally   {
         ReleaseMutex(m_mutexXmlDoc);
      }
   }


   return ret;
}

// ----------------------------------------------------------------------------
/**
   Process message.
 */
bool TXmlManager::RenameTypeConfigMsg(LT::TMessage *msg)                     {

   bool ret = false;
    wchar_t *newName = new wchar_t[msg->DataSize() + 1];
   if (!newName) {
      LOG_ERROR1(L"Can't alocate memory");
      return false;
   }

   newName[0] = L'\0';
   wmemcpy(newName, (wchar_t*)msg->DataAsRaw(), msg->DataSize());
   newName[msg->DataSize()] = L'\0';

   ret = RenameTypeConfig(m_activeType, newName);

   delete [] newName; //vic: añado esto que creo que debe estar
   return ret;
}


// ----------------------------------------------------------------------------
/**
   Process message.
 */
bool TXmlManager::SaveTypeAsMsg(LT::TMessage *msg)                     {

   bool ret = false;
    wchar_t *newName = new wchar_t[msg->DataSize() + 1];
   if (!newName) {
      LOG_ERROR1(L"Can't alocate memory");
      return false;
   }

   newName[0] = L'\0';
   wmemcpy(newName, (wchar_t*)msg->DataAsRaw(), msg->DataSize());
   newName[msg->DataSize()] = L'\0';

   ret =    SaveTypeAs(m_activeType,newName) ;

   delete [] newName; //vic: añado esto que creo que debe estar
   return ret;
}

// ----------------------------------------------------------------------------
/**
   Process message.
 */
bool TXmlManager::ExecuteOtherMsg( LT::TMessage *&msg)                       {

   bool proc = false;
   switch (msg->Type())                                                       {
      case MSG_SAVE_CFG_TO:                                                   {
         proc = SaveConfigToMsg(msg);
         if (msg)                                                             {
            delete msg;
            msg   = NULL;
         }
         return proc;
      }

   case  MSG_TYPE_CONFIG_RENAME:
         proc = RenameTypeConfigMsg(msg);                                     {
         if (msg)                                                             {
            delete msg;
            msg   = NULL;
         }
         return proc;
      }

   case  MSG_TYPE_CONFIG_SAVE_AS:
         proc = SaveTypeAsMsg(msg);                                           {
         if (msg)                                                             {
            delete msg;
            msg   = NULL;
         }
         return proc;
      }
   default:
      break;
   }

   return false;
}


//
// ----------------------------------------------------------------------------
/**
 Process messages
 @param   msg   Message
 @param forceSynchronously forces the message to be processed synchronously, even if m_asynchronousSave is set to true
         Not applies to all messages.
 */
bool TXmlManager::ProcMsg(LT::TMessage *msg, LT::TMessage *&outputMsg) {

   if (!m_DocPtr)
      return false;

   bool proc = true;

//    vicente: he quitado esta condicion para poder obtener la configuración desde
//           las comuniaciones sin tener que enviar un mensaje al GM
//   // If destination of message is not GM, return false
//   if (!msg->IsDest(tagGM)) {
//      LOG_INFO1(L"Destination of message is not GM");
//      return false;
//   }

   wchar_t sourceModuleName[addressSize];
   msg->GetOrg(sourceModuleName, sizeof(sourceModuleName));

   if (!m_mutexXmlDoc) {
      LOG_WARN1(L"Mutex is NULL");
      return false;
   }

   switch (msg->Type()) {
      // ---------------------------------------------------------------------
   case MSG_GET_CFG_EMBEDDED:
   case MSG_GET_CFG: { // Get Configuration
         bool ret = false;
         e_typeMsg type = msg->Type()== MSG_GET_CFG_EMBEDDED? MSG_CFG_EMBEDDED : MSG_CFG;
         LT::TMessage *responseMsg =  new LT::TMessage( type, tagGM, sourceModuleName, PRIOR_NORMAL, "", 0);

         if (m_mutexXmlDoc && WaitForSingleObject(m_mutexXmlDoc, TIMEOUT_XML_MUTEX)  == WAIT_OBJECT_0) {
            try                                                               {
               LOG_INFO1(L"ProcMsg: MSG_GET_CFG");
               //codigo DE PRUEBA para pruebas conexion movil
               //si nos llega solicitud desde modulo MOBILE, entonces filtraremos por flags, y solicitaremos
               //la configuracion del modulo  (  habra que ver que hacer con el multimodulo, o con otros nombres de modulo )
               wchar_t mName[addressSize];
               unsigned int   filterFlags = FLAG_SAVE | FLAG_SAVE_NO_VALUE;
               bool           formatXml   = false;

//               if ( msg->Type() == MSG_GET_MOBILE_CFG )                       {
//                  filterFlags = FLAG_MOBILE;
//                  formatXml = true;
//               }
               if (GetConfig( sourceModuleName, responseMsg->GetTStream(), filterFlags))    {

                  responseMsg->WriteSize();
                  outputMsg = responseMsg;
                  LOG_INFO3(L"Configuration for module ", sourceModuleName, L" has been sent. ");
                  ret = true;
               }
               else                                                           {
                  LT::TMessage *noResponseMsg = new LT::TMessage(MSG_NO_CFG, tagGM, sourceModuleName,PRIOR_NORMAL, "", 0);
                  outputMsg = noResponseMsg;
                  LOG_ERROR3(L"There is no configuration for Module ", sourceModuleName, L". Default configuration will be loaded.");
               }
            }
            __finally                                                         {
               ReleaseMutex(m_mutexXmlDoc);
            }
         }
         else
            LOG_INFO1(L"Mutex for xml configuration file not set.");

         if (!ret)
            if (responseMsg)  {
               delete responseMsg;
               responseMsg  = NULL;
            }

         return ret;
      }
      // ---------------------------------------------------------------------
   case MSG_SAVE_CFG: { // Save all configuration from Module
         if ( m_thread && m_asynchronousSave)                                 {
            m_listBox->Push(msg);
         }
         else {
            proc = SaveMsgCfg(msg);
            if (msg) {  // if no thread is activated, msg must be deleted
               delete msg;
               msg = NULL;
            }
         }
         outputMsg = NULL;
         return proc;
      }
      // ---------------------------------------------------------------------
   case MSG_SAVE_PARTIAL_CFG: { // Save Partial Configuration
         LOG_DEBUG1(L"Partial msg arrived to xml manager");
         if (m_thread && m_asynchronousSave)
            m_listBox->Push(msg);
         else  {
            proc = SavePartialMsgCfg(msg);
            if (msg) {  // if no thread is activated, msg must be deleted
               delete msg;
               msg = NULL;
            }
         }

         outputMsg = NULL;
         return proc;
      }
      // ---------------------------------------------------------------------
   case MSG_SET_TYPE_CONFIG_ACTIVE: { // Ser Active a type configuration

         if (m_thread && m_asynchronousSave)
            m_listBox->Push(msg);
         else                       {
            proc = SetActiveTypeMsg(msg);
            if (msg) {  // if no thread is activated, msg must be deleted
               delete msg;
               msg = NULL;
            }
         }
         outputMsg = NULL;

      } break;
      // ---------------------------------------------------------------------
   case MSG_TYPE_CONFIG_NEW: { // Create a new type configuration

         if (m_thread && m_asynchronousSave)
            m_listBox->Push(msg);
         else                       {
            proc = CreateTypeMsg(msg);
            if (msg) {  // if no thread is activated, msg must be deleted
               delete msg;
               msg = NULL;
            }
         }

         outputMsg = NULL;
      } break;
      // ---------------------------------------------------------------------
   case MSG_TYPE_CONFIG_DELETE: {
         if (m_thread && m_asynchronousSave)
            m_listBox->Push(msg);
         else                       {
            proc = DeleteTypeMsg(msg);
            if (msg) {  // if no thread is activated, msg must be deleted
               delete msg;
               msg = NULL;
            }
         }

         outputMsg = NULL;

      } break;

   // ---------------------------------------------------------------------
   case  MSG_SAVE_CFG_TO:
   case  MSG_TYPE_CONFIG_RENAME:
   case  MSG_TYPE_CONFIG_SAVE_AS:
         if (m_thread && m_asynchronousSave)
            m_listBox->Push(msg);
         else                       {
            proc = ExecuteOtherMsg(msg);
            if (msg) {  // if no thread is activated, msg must be deleted
               delete msg;
               msg = NULL;
            }
         }

         outputMsg = NULL;
      break;

   // ---------------------------------------------------------------------
   case  MSG_GET_TYPE_CONFIG_ACTIVE:                                          {

      // Control Mutex is not necessary.
      if (wcslen(m_activeType) > 0) {
         LT::TMessage *responseMsg =  new LT::TMessage(
                                       MSG_TYPE_CONFIG_ACTIVE,
                                       tagGM,
                                       sourceModuleName,
                                       PRIOR_NORMAL,
                                       (const char*)m_activeType,
                                       (wcslen(m_activeType) + 1)*sizeof(wchar_t));
         responseMsg->WriteSize();
         outputMsg =  responseMsg;
      }
   } break;

   // ---------------------------------------------------------------------
   case  MSG_TYPE_CONFIG_GET_AVAILABLES:                                      {

      std::list<wchar_t*>typeList;

      GetAvailableTypesNames(typeList);       // Control Mutex has been added into this function

      wchar_t inList[4096] = L"";
      if (typeList.size() > 0)                                                {
         std::list<wchar_t*>::iterator i  = typeList.begin();
         while ( i != typeList.end() )             {
            wcscat(inList, (*i));
            wcscat(inList, L";");
            //vic added a i++ to avoid infinite loop
            i++;
         }
      }

      LT::TMessage *responseMsg =  new LT::TMessage(
                                       MSG_TYPE_CONFIG_AVAILABLES,
                                       tagGM,
                                       sourceModuleName,
                                       PRIOR_NORMAL,
                                       (const char*)inList,
                                       (wcslen(inList) + 1)*sizeof(wchar_t));
         responseMsg->WriteSize();
         outputMsg =  responseMsg;


      std::list<wchar_t*>::iterator i = typeList.begin();
      while ( i != typeList.end() )            {
         wchar_t* todelete = (*i);
         i  = typeList.erase (i);
         free (todelete);
      }
   }
   break;

//   // ---------------------------------------------------------------------
//   case  MSG_TYPE_CONFIG_GET_TRANSLATIONS_AVAILABLES:                         {
//
//      std::list<wchar_t*>typeList;
//      GetTranslationTypeNames(typeList);        // Control Mutex has been added into this function
//
//      wchar_t inList[4096] = L"";
//      if (typeList.size() > 0)  {
//         std::list<wchar_t*>::iterator i  = typeList.begin();
//         while ( i != typeList.end() )             {
//            wcscat(inList, (*i));
//            wcscat(inList, L";");
//         }
//      }
//
//      LT::TMessage *responseMsg =  new LT::TMessage(
//                                       MSG_TYPE_CONFIG_TRANSLATIONS_AVAILABLES,
//                                       tagGM,
//                                       sourceModuleName,
//                                       PRIOR_NORMAL,
//                                       (const char*)inList,
//                                       (wcslen(inList) + 1)*sizeof(wchar_t));
//         responseMsg->WriteSize();
//         outputMsg =  responseMsg;
//
//
//      std::list<wchar_t*>::iterator i = typeList.begin();
//      while ( i != typeList.end() )            {
//         wchar_t* todelete = (*i);
//         i  = typeList.erase (i);
//         free (todelete);
//      }
//   }  break;

   default:
      break;
   } // end switch
   return proc;
}



// ----------------------------------------------------------------------------
/**
 CleanUnusedTypeNodeFromFile
 For debugging and cleaning configuration purposes.
 Loads a file, deletes disabled nodes from ContaminBridge, StructureBridge and ClassifierGroup->BridgeClass of all typenodes
 Produces memory problems, DONT USE IT IN PRODUCTION RELEASES
 @param   fileName   path with a full config file
 */
bool TXmlManager::CleanUnusedTypeNodeFromFile( wchar_t* fileName) {

   //usamos funciones del siglo XV (wchar * y char*) porque al builder no le apetece funcionar con C++

   wchar_t wsstream[512]={L""};
   char sstream[MAX_XPATH_EXPR]={""};

//   std::stringstream sstream;
//   std::wstringstream wsstream;

   int unused=0;
   //load file into xmlDoc
   xmlDocPtr workingXmlDoc = NULL;
   char* filename = TAux::WSTR_TO_STR(fileName);
   if (filename) {
      workingXmlDoc = xmlReadFile(filename,ENCODING_UTF_8,XML_PARSE_HUGE);  //vicmod added hugefiles support
//         delete[]filename;
   }
   if (workingXmlDoc == NULL)   {
      LOG_ERROR1(L"Unable to load types configuration file");
      return false;
   }
   sstream[0]=0;
   strcat(sstream,"/lyratechs/modules/type_config/TDataMngr[@name='lyratechs']/TDataMngr[@name='ContaminBridge']/TDataMngr/TData[@name='Enabled']/TAttrib[@name='Value' and @value='0']");
//      sstream.str("");
//      sstream << "/lyratechs/modules/type_config/TDataMngr[@name='lyratechs']/TDataMngr[@name='ContaminBridge']/TDataMngr/TData[@name='Enabled']/TAttrib[@name='Value' and @value='0']";

   //delete unused contaminBridge
   xmlXPathObjectPtr xPathObj;
   if ( XmlGetNodesFromXpath( sstream, xPathObj, workingXmlDoc) ) {
      // Selected Nodes
      xmlNodeSetPtr nodes = xPathObj->nodesetval;
      int sizeList = ( nodes) ? nodes->nodeNr : 0;
      for( int ii=0;ii<sizeList;ii++)                          {
            xmlNode* node  = nodes->nodeTab[ii];
            xmlNode* TdataMngrCont  =  node->parent->parent;
            xmlUnlinkNode(TdataMngrCont);
            //xmlFreeNode(TdataMngrCont);   //if we release here, the xmlXPathFreeObject fails
            unused++;
      }
      xmlXPathFreeObject(xPathObj);
   }
   sstream[0]=0;
   strcat(sstream,"/lyratechs/modules/type_config/TDataMngr[@name='lyratechs']/TDataMngr[@name='StructureBridge']/TDataMngr/TData[@name='Enabled']/TAttrib[@name='Value' and @value='0']");

//      sstream.str("");
   //delete unused StructureBridge
   if ( XmlGetNodesFromXpath( sstream, xPathObj, workingXmlDoc) ) {
      // Selected Nodes
      xmlNodeSetPtr nodes = xPathObj->nodesetval;
      int sizeList = ( nodes) ? nodes->nodeNr : 0;
      for( int ii=0;ii<sizeList;ii++)                          {
            xmlNode* node  = nodes->nodeTab[ii];
            xmlNode* TdataMngrCont  =  node->parent->parent;
            xmlUnlinkNode(TdataMngrCont);
            //xmlFreeNode(TdataMngrCont);    //if we release here, the xmlXPathFreeObject fails
            unused++;
      }
      xmlXPathFreeObject(xPathObj);
   }
   sstream[0]=0;
   strcat(sstream,"/lyratechs/modules/type_config/TDataMngr[@name='lyratechs']/TDataMngr[@name='ClassifierGroup']/TDataMngr[@name='BridgeClass']/TDataMngr/TData[@name='Enabled']/TAttrib[@name='Value' and @value='0']");

//   sstream.str("");
   //delete unused ClassifierGroup -> BridgeClass
   if ( XmlGetNodesFromXpath( sstream, xPathObj, workingXmlDoc) ) {
      // Selected Nodes
      xmlNodeSetPtr nodes = xPathObj->nodesetval;
      int sizeList = ( nodes) ? nodes->nodeNr : 0;
      for( int ii=0;ii<sizeList;ii++)                          {
            xmlNode* node  = nodes->nodeTab[ii];
            xmlNode* TdataMngrCont  =  node->parent->parent;
            xmlUnlinkNode(TdataMngrCont);
            //xmlFreeNode(TdataMngrCont);    //if we release here, the xmlXPathFreeObject fails
            unused++;
      }
      xmlXPathFreeObject(xPathObj);
   }
//   wsstream[0]=0;
   wsprintf(wsstream,L"CleanUnused. Removed %d nodes.",unused);

   LOG_INFO1(wsstream);

//   sstream[0]=0;
   sprintf(sstream,"%s_clean.xml",filename);

   xmlSaveFileEnc( sstream,workingXmlDoc,ENCODING_UTF_8);


   if (filename) {
      delete[]filename;
   }

   xmlFreeDoc(workingXmlDoc);
   return true;
}




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
