//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TTXTEvent.cpp
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

#include "TTXTEvent.h"
#include "Base64.h"
#include "TAux.h"
#include "time.h"

#include <fstream>
#include <iostream>
#include <sysutils.hpp>

#include "dir.h"
#include <process.h>
#include <stdio.h>

// -- Defines

#define MAX_INFO_EVENT  1024                 ///< Maximum size for event text.
#define MAX_SIZE_FILE   15728640             ///< Max size event file (MB).
#define defaultEventFile   L"LTAIS_events_register.txt"     ///< Default filename for events.

//TODO: Se plantea para hacer en un futuro un save asincrono para eventos. Como
// esta en Tprocess y en TXMlmanager, con un thread y una lista.

  // ------------------------------------------------------------------------------

   /**
    Append string into a UTF8 file
    @param fileNmae is the name of the file where to append
    @param buffer is the string to be added
    */
   bool AppendLineToFile(wchar_t* fileName, wchar_t* buffer) {

      if (!buffer)
         return false;

      bool ret = false;
      bool exist     = TAux::FileExists(fileName);

      if (!exist)    {
         char* fname = TAux::WSTR_TO_STR(fileName);
         FILE *file;
         errno_t err = 1;
         if (fname != NULL)   {
            err = fopen_s(&file,fname,"w");
            delete[]fname;
         }
         if( err == 0 )   {
            //Byte Order Mark that identifies this file as a text file containig UTF8
            const BYTE utf8BOM[] = {0xEF, 0xBB, 0xBF};
            fwrite(utf8BOM, sizeof(utf8BOM[0]), _countof(utf8BOM),file);
            fclose(file);
         } else {
//            LOG_ERROR2( L"TXTEvent AppendLineToFile error creating file",fileName);
         }
      }

      wofstream * file = new wofstream(fileName,  ios_base::app | ios_base::in | ios_base::out );
      if (file)         {
//         if (!exist)    {
//               //Byte Order Mark that identifies this file as a text file containig UTF8
//               const BYTE utf8BOM[] = {0xEF, 0xBB, 0xBF};
//               file->write(utf8BOM, sizeof(utf8BOM[0])* _countof(utf8BOM));
//         }
         (*file) << buffer << std::endl;
//         char* buf = TAux::WSTR_TO_STR(buffer);
//         if (buf)       {
//            (*file) << buf << std::endl;
//            delete[]buf;
            ret = true;
//         }
         file->close();
         delete file;
      }
      return ret;
   }



// ----------------------------------------------------------------------------
// ********************************  TTXTEvent Class  *************************
// ----------------------------------------------------------------------------

/**
    @param   filename for events
 */
TTXTEvent::TTXTEvent(wchar_t* path,wchar_t* filename) {

   m_file = NULL;

   unsigned int len  = SIZEOF_CHAR(m_workingPath);

   if (path == NULL || !path[0])                                              {
      /// Getting current path.
      if ((_wgetcwd(m_workingPath, len)) != NULL)
         wcscat(m_workingPath, L"\\");
      else
         swprintf(m_workingPath, L".\\");
   }
   else                                                                       {
      // Defined path in config.txt
      wcscpy (m_workingPath, path);
      wcscat(m_workingPath, L"\\");
      // Create folder if not exist
      _wmkdir (String(m_workingPath).c_str());
   }


   if (filename)
      swprintf(m_FileName, L"%s%s\0", m_workingPath,filename);
   else
      swprintf(m_FileName, L"%s%s\0", m_workingPath,defaultEventFile);

//   if (!TAux::FileExists(m_FileName))
//      CreateHeader(m_FileName);

   //CheckFileSize( m_FileName);

   CheckFileSize();
   m_mutex              = CreateMutex( NULL, false, NULL);

}

// -----------------------------------------------------------------------------
/**
 Destructor
 */
TTXTEvent::~TTXTEvent()                                                       {

   LOG_INFO1( L"Begin TXTEvent destructor");
   // Close of m_file is done is detructor method because of performance reasons.
   if (m_file)                                                                {
      m_file->close();
      delete m_file;
      m_file = NULL;
   }
   LOG_INFO1( L"End TXTEvent destructor");
}

// -----------------------------------------------------------------------------
/**
   Init function to allow calling virtual funcion createHeader
   @return true is size is greater than maximum allowed
 */
void TTXTEvent::Init()                                               {

   if (!TAux::FileExists(m_FileName))
      CreateHeader(m_FileName);
}

// -----------------------------------------------------------------------------
/**
   Check if size of events file is over than 15M, in that case, make a copy.
   @return true is size is greater than maximum allowed
 */
bool TTXTEvent::CheckFileSize()                                               {

   bool ret = false;


   struct _stat stFileInfo;
   //ToDo: reescribo el calculo de tamaño, porque abriendo el fichero y posicionandose
   //al final, con algunos ficheros tarda mucho, independientemente del tamaño
   if ( TAux::FileInformation(m_FileName, &stFileInfo))                       {
      ret = stFileInfo.st_size > MAX_SIZE_FILE;
       if (ret) {
         if (m_file) {
            m_file->close();
            delete m_file;
            m_file = NULL;
         }

         SYSTEMTIME lt ;
         LT::Time::LocalTimeFromTimeStamp( LT::Time::GetTime(), &lt);
         wchar_t fdate[1100];
         swprintf(fdate, L"%s_%02d%02d%02d_%02d%02d%02d.txt",m_FileName,lt.wYear,lt.wMonth,lt.wDay,lt.wHour, lt.wMinute,lt.wSecond);
       //  swprintf(fdate, L"%s_copy.txt",file,lt.wYear,lt.wMonth,lt.wDay,lt.wHour, lt.wMinute,lt.wSecond);
         if (!MoveFile(m_FileName,fdate))
            return false;
      }

   }

   return ret;
}

// ------------------------------------------------------------------------------
/**
 Save the event.txt m_file
 @param   text
 */
bool TTXTEvent::Save(wchar_t* text) {

   wchar_t timestamp[32] = L"";
   wchar_t level[32] = L"";
   wchar_t tag_code[32] = L"";
   wchar_t moduleName[32] = L"";
   wchar_t description[256] = L"";

   const int tsWidth = 24;
   const int levelWidth = 4;
   const int tgWidth = 30;
   const int modWidth = 6;
   const int desWidth = 255;

   std::list<std::wstring>textEventTokens;

   // Extract Events from text.
   ExtractTokens(text, textEventTokens, L"|");

   if (textEventTokens.size() > 3) {

      // Value of the TData is constructed.
      std::list<std::wstring>::iterator i;

      int j = 1;
      for (i = textEventTokens.begin(); i != textEventTokens.end(); i++) {
         switch (j) {
         case 1:
            wcsncpy( timestamp, (*i).c_str(), tsWidth);
            timestamp[tsWidth]=0;
            break;
         case 2:
            wcsncpy( level, (*i).c_str(), levelWidth);
            level[levelWidth]=0;
            break;
         case 3:
            wcsncpy( tag_code, (*i).c_str(), tgWidth);
            tag_code[tgWidth] = 0;
            break;
         case 4:
            wcsncpy( moduleName, (*i).c_str(), modWidth);
            moduleName[modWidth]=0;
            break;
         case 5:
            wcsncpy( description, (*i).c_str(), desWidth);
            description[desWidth]=0;
            break;
         }
         j++;
      }
   }

   textEventTokens.clear();

   // taking off  white spaces
   int i = 0;
   wchar_t * code = tag_code;
   while (code[i]== L' ')
      code++;

   wchar_t tmpEvent[512];
   wchar_t format[100];
   swprintf( format, L"%%-%ds\| %%%ds    \| %%-%ds \| %%%ds \| %%s",
            tsWidth, levelWidth, tgWidth, modWidth, desWidth);

   //swprintf(tmpEvent, L"%-24s\| %2s    \| %-30s \| %6s \| %s ", timestamp,level, tag_code, moduleName, description);
   swprintf(tmpEvent, format, timestamp,level, code, moduleName, description);


   // Write on m_file
//
//   CheckFileSize();
//   if (!TAux::FileExists(m_FileName))
//      CreateHeader(m_FileName);
//
//   if (!m_file)
//      m_file = new fstream(m_FileName,  ios_base::app | ios_base::in | ios_base::out );
//
//   if (m_file) {
      if ( WaitForSingleObject( m_mutex, 10) == WAIT_OBJECT_0)                   {
         try {

            CheckFileSize();
            if (!TAux::FileExists(m_FileName))
               CreateHeader(m_FileName);

            if (!m_file)
               m_file = new wofstream(m_FileName,  ios_base::app | ios_base::in | ios_base::out );

            if (m_file) {

                    (*m_file) << tmpEvent << std::endl;
//                  char* buf = TAux::WSTR_TO_STR(tmpEvent);
//                  if (buf != NULL) {
//                     (*m_file) << buf << std::endl;
//                     delete[]buf;
//                  }
            }
         }
         __finally   {
            ReleaseMutex( m_mutex);
         }
//
      }



//   }

//   char *fname = TAux::WSTR_TO_STR(m_FileName);
//   if (fname) {
////      if ( WaitForSingleObject( m_mutex, 10) == WAIT_OBJECT_0)                   {
////         try {
//
//            m_fitx = fopen( fname, "a");
//
//            if (m_fitx) {
//               char* buf = TAux::WSTR_TO_STR(tmpEvent);
//               if (buf != NULL) {
//
//                  fprintf( m_fitx, "%s\n", buf);
//                  delete [] buf;
//               }
//            fclose( m_fitx);
//            m_fitx = NULL;
//            }
//            delete [] fname;
////         }
////         __finally   {
////             ReleaseMutex( m_mutex);
////         }
////      }
//   }

   return true;
}

// ------------------------------------------------------------------------------
/**
      General Method for processing messages.
      @param inputMsg Message received.
      @param responseMsg Ouptut Message to be returned (when necessary).
      @return true when message was sucessfully processed, false otherwise.
*/
bool TTXTEvent::ProcMsg(LT::TMessage *inputMsg, LT::TMessage *&responseMsg) {

   bool ret = false;
   switch (inputMsg->Type())                                                  {
   case MSG_LT_EVENT: {
         //Event message
         if (!inputMsg->IsDest(tagGM))
            return false;
         char buffer[1024];
         int size;
         // Set read pointer to the beginning, just in case somebody has already processed message.
         (inputMsg->GetTStream())->SeekRead( inputMsg->Size() - inputMsg->DataSize());
         if ((inputMsg->GetTStream())->Read(buffer, inputMsg->DataSize())  == false) {
            LOG_WARN1(L" Event could not be read. ");
            return false;
         }
         wchar_t* newBuffer = (wchar_t*)buffer;
         if (newBuffer)
            ret = Save(newBuffer); // Extract and Save Event
      } break;
   case MSG_LT_TXT_TO_FILE:                                                  {
         //DispatchToFile message
         if (!inputMsg->IsDest(tagGM))
            return false;
         // Set read pointer to the beginning, just in case somebody has already processed message.
         (inputMsg->GetTStream())->SeekRead( inputMsg->Size() - inputMsg->DataSize());
         //Reads first line, which contains the name of the file where to save the rest of the message
         LT::TStream *str = inputMsg->GetTStream();
         wchar_t fileName[128];
         int size;
         if ( str->ReadLn( fileName, SIZEOF_CHAR( fileName), size))           {
            //obtener puntero seguro a resto del mensaje
            wchar_t *content = (wchar_t*)str->GetSafePtr( str->posRead(), str->Size() - str->posRead());
            //guardamos en el fichero indicado
            wofstream *f = new wofstream( fileName,  ios_base::app | ios_base::out );

            if (f) {
               try {
//                  char* buf = TAux::WSTR_TO_STR(content);
//                  if (buf != NULL) {
                    (*f) << content << std::endl;
//                     delete[]buf;
//                  }
               }
               __finally   {
                  f->close();
                  delete f;
               }

            }
         }
         ret = true;
      } break;
   default:
      break;
   } // switch

   return ret;
}



// --------------------------------------------------------------------------
/**
 Write the header for event.txt m_file
 @param      wchar_t m_FileName
 */
void TTXTEvent::CreateHeader(wchar_t* m_FileName) {

   const int size = 16;
   wchar_t tag_level[size] = L"Level";
//   wchar_t tag_type[size] = L"Type";
   wchar_t tag_code[size] = L"Tag Code";
   wchar_t tag_module[size] = L"Module";
//   wchar_t tag_desc[size] = L"Description";

   AppendLineToFile(m_FileName,
       L"#############################################################################################################");
   AppendLineToFile(m_FileName,
       L"##                            LyRa Techs Artificial Intelligence Software                                  ##");
   AppendLineToFile(m_FileName,
       L"##                                           REGISTERS                                                     ##");
   AppendLineToFile(m_FileName,
       L"#############################################################################################################");
   AppendLineToFile(m_FileName,
       L"-------------------------------------------------------------------------------------------------------------");

   wchar_t line[300];
//   swprintf(line,
//       L"Timestamp               \|   %2s \| %-30s \| %-5s \| %5s",
//       tag_level, tag_code, tag_module, tag_desc);

   GetSubHeader(line)  ;
   AppendLineToFile(m_FileName, line);
   AppendLineToFile(m_FileName,
       L"-------------------------------------------------------------------------------------------------------------\n\n");
}

// --------------------------------------------------------------------------
/**
 Extract tockens from a string and put them into tokens list
 @param  str
 @param  tokens
 @param  pattern
 */
 void TTXTEvent::ExtractTokens(wchar_t *str, std::list<std::wstring> &tokens,
    const wchar_t* pattern) {

   if (!pattern) {
      LOG_WARN(L"No pattern found extracting tokens. ");
      return;
   }
   wchar_t *p = _wcsdup(str); // Put un p the string value
   wchar_t *q = p; // Put in q the p value
   wchar_t *ptoken;

   while ((ptoken = wcstok(q, pattern)) != NULL) {

      tokens.push_back(std::wstring(ptoken));
      q = q + wcslen(ptoken) + 1;
      if (q >= p + wcslen(str))
         break;
   }
   free(p);
}


void TTXTEvent::GetSubHeader(wchar_t * line){

     const int size = 16;
   wchar_t tag_level[size] = L"Level";
//   wchar_t tag_type[size] = L"Type";
   wchar_t tag_code[size] = L"Tag Code";
   wchar_t tag_module[size] = L"Module";
   wchar_t tag_desc[size] = L"Description";

//   wchar_t line[300];
   swprintf(line,
       L"Timestamp               \|   %2s \| %-30s \| %-5s \| %5s",
       tag_level, tag_code, tag_module, tag_desc);

}






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------


