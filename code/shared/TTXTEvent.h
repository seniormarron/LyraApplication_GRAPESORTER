//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TTXTEvent.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef TTXTEventH
#define TTXTEventH

/// Basic include libraries
#include "TEventPersistence.h"
#include <fstream>

// --------------------------------------------------------------------------
/**
 Extract the 3rd token and returns it
 @param  str
 */
wchar_t* ExtractTagCode( wchar_t *str) {

   wchar_t *p = _wcsdup(str); // Put un p the string value
   wchar_t *q = p; // Put in q the p value
   wchar_t *ptoken;
   int cont = 1;
   wchar_t *ret = NULL;
   while ((ptoken = wcstok(q, L"|")) != NULL) {
      if ( cont++ == 3)                     {
         ret = _wcsdup( ptoken);
         free( p);
         return ret;
      }
      q = q + wcslen(ptoken) + 1;
      if (q >= p + wcslen(str))
         break;
   }
   free(p);
   return ret;
}

//----------------------------------------------------------------------------
//********************************  TTXTEvent Class  *************************
//----------------------------------------------------------------------------

/**
   TXmlEvent is used to store & querying XML Event Files
*/
///< Classs for persistent of Events in text format.
class TTXTEvent  : public TEventPersistence                                   {

 protected:

   wofstream      *m_file;                                ///< Pointer to file for persistence.
   wchar_t        m_FileName[MAX_LENGTH_FULL_PATH];                      ///< Configuration File Name.
   HANDLE         m_mutex;

   void CreateHeader(wchar_t* m_FileName);               ///< writes the header of the events file
   static void ExtractTokens(wchar_t *str,           ///< parses the str and extracts the expected tokens
      std::list<std::wstring> &tokens,
      const wchar_t* pattern);


 private:
   wchar_t        m_workingPath[MAX_LENGTH_FULL_PATH];
   FILE           *m_fitx;

 public:

   // -- Constructor & Destructor methods
   TTXTEvent ( wchar_t* path = NULL, wchar_t* filename = NULL); ///< Constructor.
   virtual ~TTXTEvent ();                                ///< Destructor.
   void Init();
   bool CheckFileSize() ;

   virtual bool   ProcMsg( LT::TMessage *inputMsg,
                           LT::TMessage *&responseMsg); ///< Process message.

   virtual bool   Save( wchar_t* text);                  ///< Appends the "text" line to the event.txt file
   virtual void GetSubHeader(wchar_t * line);       ///< Method to get the fields descriptor lines of the header
};

#endif













//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------

