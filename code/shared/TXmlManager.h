//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TXmlManager.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TXmlManagerH
#define TXmlManagerH

// LibXML
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

// Basic include libraries
#include "TLabels.h"
#include "TPersistence.h"
#include "TDataMngControl.h"
#include "TEventDispatcher.h"

// -- Defines
#define MAX_XPATH_EXPR     1024

#define TIMEOUT_XML_MUTEX  250000
#define MAX_BAK            5

//struct to get types info

/**
   @struct s_typeInfo
   Working mode / type info.
*/
struct s_typeInfo                                                         {
   std::wstring   m_number;                  ///< number of the type (optional)
   std::wstring   m_name;                    ///< internal name, ID
   std::wstring   m_translation;             ///< name to show
   /**
   Constructor
   */
   s_typeInfo()       {
   };

   /**
   Constructor
   */
   s_typeInfo(wstring number, wstring name, wstring translation)       {
      m_number = number;
      m_name = name;
      m_translation = translation;
   };

   /**
      Operator <
      lists of this structure will be ordered taking into account m_translation field
   */
   bool operator<(const s_typeInfo &ti)
   {
      int num   = _wtoi(m_number.c_str());
      int numTi = _wtoi(ti.m_number.c_str()) ;

      if(num - numTi)                                                         {
         return num < numTi;
      }
      else                                                                    {
         return m_translation < ti.m_translation;
      }
   }
};


// Structs used in import/export working modes

/**
   @struct s_idXmlNode
   used to store a pair of id, xmlNode
*/
struct s_idXmlNode                                                         {
   int          m_id;                          ///< id of the product name.
   xmlNodePtr   m_pxmlNode;                    ///< xmlNode pointer

   /**
   Constructor
   */
   s_idXmlNode()       {
   };

   /**
   Constructor
   */
   s_idXmlNode(int id, xmlNodePtr xmlnode)       {
      m_id = id;
      m_pxmlNode = xmlnode;
   };
};

/**
   @struct s_NonAddedWorkingModes
   used to store the loaded document, and a list of non loaded working modes and their names.
*/
struct s_NonAddedWorkingModes                                                {
   xmlDocPtr               m_importedDoc;        ///< imported xmlDoc
   std::list<xmlNodePtr>   m_listExistingNodes;  ///< xmlNode pointer list of existing types
   std::list<xmlNodePtr>   m_listNewNodes;       ///< xmlNode pointer list of new types
   std::list<std::wstring> m_listNames;          ///< names of NonAdded types
   std::wstring            m_outMessage;         ///< output message
   bool                    m_result;             ///< to store closures "return" values
   bool                    m_xmlException;        ///< true when the xml library failed
   /**
   Constructor
   */
   s_NonAddedWorkingModes()       {
   };

   ~s_NonAddedWorkingModes()       {
//      xmlFreeDoc(m_importedDoc);      //commented, now doing manually
   };
};




// ----------------------------------------------------------------------------
// ********************************  TXmlManager Class  ***********************
// ----------------------------------------------------------------------------
 class TDataManager;


/// This class provides management of XML documents
/**
 Implementation of TXmlManager is divided into the following files:

 TXmlManager.cpp                       : General methods for the class. Current File.
 TXmlManagerDecodingAndSaver.cpp       : Methods for decoding stream and save configuration into xml file.
 TXmlManagerDecodingAndSavePartial.cpp : Methods for decoding stream and save a chunk of configuration into xml file.
 TXmlManagerEncondingToStream.cpp      : Methods for reading a configuration from xml tree and encoding to a stream.
 TXmlManagerTypeConfig.cpp             : Methods for management of configuration for machines.
 */
class TXmlManager : public TPersistence                                       {

  protected:
   // -- Member Variables ---
   TDataManager   *m_mngr;                                           ///< for dispatching events in local persistence
   xmlDocPtr   m_DocPtr;                                             ///< Pointer to XML Configuration Tree.
   HANDLE      m_mutexXmlDoc;                                        ///< Mutex to sinchronyze the access to the xml doc.
   wchar_t     m_XmlFileName[MAX_LENGTH_FULL_PATH];                  ///< Configuration File Name.
   wchar_t     m_activeType[MAX_LENGTH_FULL_PATH];                   ///< Current Active Type
   bool        m_isType;                                             ///< Flag to indicate if there is a Active Type Currently
   wchar_t     m_workingPath[MAX_LENGTH_FULL_PATH];                  ///< Current Working path

   static const char *startNameXPath;                                ///< Tags for construct xPath Expression
   static const char *endNameXPath;                                  ///< Tags for construct xPath Expression

   wchar_t     m_fileBakName_list[MAX_BAK][MAX_LENGTH_FULL_PATH];    ///< List of xml .baks
   int         m_bakIndex;                                           ///< Index for create .bak

   TSafeList<LT::TMessage*> *m_listBox;                             ///< Link List to objects and process.
   bool m_asynchronousSave;                                          ///< true if asincronous processing messages for persistence, false otherwise.
   HANDLE m_thread;                                                  ///< Handle for the thread of process
   HANDLE m_stopEvent;                                               ///< Flag  event for stop the thread
   HANDLE m_semaphore;                                               ///< Flag  event to indicate a push into list.

   bool        m_toSave;                                             ///< Flag to set if the configuration need to be saved when destroying the persistence

   // -- Methods and Functions ---
   static unsigned WINAPI ThreadFuncTXmlManager(LPVOID owner);       ///< Thread Function for the process
   bool LaunchThread( );                    ///< Launch Thread Function
   bool StopThread( );                            ///< Stop Thread

   //--------------------------- UTILS ----------------------------------------
   // -- Initialize Libxml
   virtual int Init(bool asynchronousSave, wchar_t *srcPath, bool showMessagesFlag = true);///< Init Libxml libraries
   bool ParseConfig (xmlDocPtr doc = NULL);
   bool ParseFileError ();

   // -- bak files
   bool RestoreBakFile(bool showMessagesFlag = true);                ///< RestoreBakFile
   bool RestoreOthersBakFiles(bool showMessagesFlag = true);         ///< RestoreOthersBakFiles
   bool LoadOtherXmlByForm();                                        ///< Load other xml file by form.
   wchar_t  *SelectXmlFile();                                        ///< opens a dialog form asking for an xml file to be selected

   bool RestoreSosFile(bool showMessagesFlag = true);                ///< RestoreSosFile

   void CreateDefaultConfigFile();                                   ///< Create a new XML configuration file in case the default file is missing.
   void CreateDefaultFileXml(wchar_t* srcPath = NULL);               ///< Create default file if none exists
   bool CreateBakFile(wchar_t* suffix = NULL);                       ///< Create .bak file
   void ClosePersistence();

   // For processing messages depending on type message.
   bool ExecuteOtherMsg    (LT::TMessage *&msg);
   bool SaveMsgCfg         (LT::TMessage *msg);
   bool SavePartialMsgCfg  (LT::TMessage *msg);
   bool SetActiveTypeMsg   (LT::TMessage *msg);
   bool CreateTypeMsg      (LT::TMessage *msg);
   bool DeleteTypeMsg      (LT::TMessage *msg);
   bool SaveConfigToMsg    (LT::TMessage *msg);
   bool RenameTypeConfigMsg(LT::TMessage *msg);
   bool SaveTypeAsMsg      (LT::TMessage *msg);

   /**
      Return list of message box for processing
   */
   TSafeList<LT::TMessage*> * GetList() {  return m_listBox; }

   virtual char *GetFileCode();                                                        ///< see @TPersistence
   virtual bool MyLoadConfigFrom(wchar_t* fileNamePath);                               ///< see @TPersistence

   // -- Conversions for attribues from different encodings
   xmlChar* ConvertInputWC (const wchar_t *in,  const char* encoding =ENCODING_UTF_8); ///< Converts @in into UTF-8 for processing with libxml2 APIs
   xmlChar* ConvertInput   (const char *in,     const char* encoding = ENCODING_UTF_8);///< Converts @in into UTF-8 for processing with libxml2 APIs
   char*    ConvertOutput  (xmlChar *in,        const char* encoding = ENCODING_UTF_8);///< Converts @in into char* for processing
   wchar_t* ConvertOutputWC(xmlChar *in,        const char* encoding = ENCODING_UTF_8);///< Converts @in into char* for processing

   void ExtractTokens(wchar_t *str, std::list<std::wstring> &tokens, wchar_t* regex);   ///< Utils for extract tockens.
   void ExtractTokens(char *str, std::list<std::string> &tokens, char* regex);          ///< Utils for extract tockens.

   virtual bool GetConfig( wchar_t* path, LT::TStream* streamCfg, unsigned int filterFlags = FLAG_SAVE | FLAG_SAVE_NO_VALUE);                     ///< see @TPersistence
   virtual bool GetConfigXml( wchar_t* path, std::wstring* str, unsigned int filterFlags = FLAG_SAVE | FLAG_SAVE_NO_VALUE);                     ///< see @TPersistence
   virtual bool SetConfig( LT::TStream* stream);                                       ///< see @TPersistence
   virtual bool SetConfigPartial( LT::TStream* stream);                                ///< see @TPersistence

   // --  Utils Tree
   bool XmlGetNodesFromXpath(char* xpathExpr, xmlXPathObjectPtr & xPathObj, xmlDocPtr docPtr = NULL);           ///< Get Nodes that match an XPath Expression.
//   bool XmlGetNodesFromXpath( xmlDocPtr   docPtr, char* xPathExpr, xmlXPathObjectPtr & xPathObj);
   char *GetFileCode( xmlDocPtr  docPtr);
   void XmlDeleteAttribByName(xmlNode* node, xmlChar* name);                           ///< Search for an attribute for remove it.
   xmlNode* XmlGetActiveConfigNode(wchar_t* activeName, wchar_t* moduleName = NULL, xmlDocPtr docPtr = NULL);   ///<Get node in tree configuration refered to entered type.

   //--------------------------------------------------------------------------
   //---------------------- METHODS IMPLEMENTED IN ADDED FILES ----------------
   //--------------------------------------------------------------------------

   /* --  Methods in  TXmlManagerEncondingToStream.cpp : Used by "GetConfig" to get configuration from XML document  */
   bool XmlEncodingToStream(xmlDocPtr docCfg, LT::TStream* streamCfg, unsigned int filterFlags = FLAG_SAVE | FLAG_SAVE_NO_VALUE,  bool formatXml = false, std::wstring *str=NULL);             ///< Encode the configuration module from DocTree to TStream (From XML to TStream)
   bool XmlEncodingToStreamTDataMngr(xmlNode * a_node, TNode* dataMngr );            ///< Encode a manager into a stream.
   bool XmlGetConfig(wchar_t* path, xmlDocPtr& docConfig);                          ///< Same as GetConfig.
   void SetAttributeValueByType(TNode* node, xmlNode* xmlCurrentNode,wchar_t* name);///< Set/Create an attribute for a node.

   /* Methods in TXmlManagerDecodingAndSaver.cpp. Used by "SetConfig" to save configuration into XML config file  */
   bool XmlDecodingConfig(LT::TStream* ms, xmlDocPtr docPtr);
   bool XmlDecodingFromStream(LT::TStream* stream, bool &isType, xmlNode*, xmlNode* typeConfigNode = NULL);
   bool XmlDecodingFromStreamAttrib(LT::TStream* stream, bool &isType, xmlNode*);
   bool XmlDecodingFromStreamTData(LT::TStream* stream, bool &isType,xmlNode*);
   bool XmlUpdateConfig(wchar_t* path, xmlNode* node,xmlNode* typeConfigNode = NULL, xmlDocPtr docPtr = NULL);
   bool XmlDecodingFromStreamNewAttrib(const sattrib &_attr, xmlNode* node);
   xmlNode* xmlGetChildNode(xmlNode* node, char* name);
   xmlNode* xmlGetChildManager(xmlNode* node, char* name);
   xmlNode* xmlGetChilPrevManager(xmlNode* node, int order);

   /* Methods in TXmlManagerDecodingAndSaverPartial.cpp. Used by "SetConfig" to save partial configuration into XML config file.>  */
   bool XmlDecodingPartial(LT::TStream* ms);
   bool XmlDecodingPartialType(LT::TStream* ms);
   xmlNode* XmlGetNodeByFullPathTypeConfig(wchar_t * fullpath, bool & isParent,bool manager = false);
   xmlNode* XmlGetNodeByFullPath(wchar_t * fullpath, bool & isParent, bool manager = false);///< Get Node to be updated, If not exists, parent node is returned.

    /* Methods in TXmlManagerTypeconfig.cpp. Used for managing configuration types. */
   bool InsertNodeIntoActiveTypeConfig(xmlNode* newNode, wchar_t* fullpath,wchar_t* typeName = NULL);
   bool UpdateNodeInTypeConfigList(xmlNode* oldNode, wchar_t* fullpath);
   bool DeleteNodeFromTypeConfigList(wchar_t* fullpath);
   bool UpdateTypeInfoNode(xmlNode* newNode, wchar_t* fullpath, bool);
   xmlNode* IsParentType(xmlNode* node, bool &isTypeNode);
   bool SaveActiveTypeConfig(char* name);
   bool HasTypeNodes(xmlNode* node);
   void SetTypeNameTranslations(xmlNode* node, wchar_t* typenName);
   void SetTypeNameTranslations2(xmlNode* node, wchar_t* typenName);
   void LoadActiveTypeConfig();
   bool isType( wchar_t* typeName);

  public:

   // -- Constructor & Destructor methods
   TXmlManager(wchar_t* srcPath, bool asynchronousSave = true, TDataManager *mn = NULL);      ///< Constructor
   virtual ~TXmlManager();                                                             ///< Destructor

   virtual bool ProcMsg(LT::TMessage *msg, LT::TMessage *&responseMsg);              ///< see @TPersistence
   virtual bool ConfigLoaded();                                                        ///< see @TPersistence

   // -- setting configuration for type configuration
   virtual bool CreateTypeConfig      (wchar_t* typeName);                             ///< see @TPersistence
   virtual bool DeleteTypeConfig      (wchar_t* typeName);                             ///< see @TPersistence
   virtual bool SetActiveTypeConfig   (wchar_t* typeName, bool saveToDisk = true);           ///< see @TPersistence

//   virtual bool GetCfgMachine(wchar_t* name, TDataManager* mngr);                      ///< see @TPersistence. Note: Not used.
   virtual bool ResetAndLoadConfiguration   (wchar_t *fileName);                       ///< see @TPersistence

   virtual  bool SaveConfigTo        ( wchar_t* fileNamePath = NULL);               ///< see @TPersistence
   bool        SaveSpecifiedConfigTo ( wchar_t* fileNamePath, xmlDocPtr docPtr);
   virtual  bool GetActiveTypeConfig ( wchar_t* activeTypeName, int &size);           ///< see @TPersistence
   virtual  bool RenameTypeConfig    ( wchar_t* oldTypeName, wchar_t* newTypeName);   ///< see @TPersistence
   virtual  bool RenameTypeConfig2   ( wchar_t* oldTypeName, wchar_t* newTypeName);   ///< see @TPersistence
   virtual  bool SaveTypeAs          ( wchar_t* oldTypeName, wchar_t* newTypeName);   ///< see @TPersistence
   virtual  bool SaveTypeAs2         ( wchar_t* oldTypeName, wchar_t* newTypeName);   ///< see @TPersistence
   virtual  bool CheckFile           ( wchar_t * fileName);                           ///< see @TPersistence
   virtual  bool GetAvailableTypesNames(std::list<wchar_t*>& availableTypeList);       ///< see @TPersistence
//   virtual  bool GetTranslationTypeNames(std::list<wchar_t*>& availableTypeList);      ///< see @TPersistence
   virtual  bool GetNamesTypeNames( std::list<wchar_t*>&  availableTypeList)  ;        ///< see @TPersistence
   bool          getTypesInfo(std::list<s_typeInfo>& typeslist, wstring *filterNamesWithStart = NULL); ///< get a list of the types info struct

   bool CleanUnusedTypeNodeFromFile( wchar_t* fileNamePath);


   virtual  bool ValidateMachineCodeInFile( wchar_t* fileNamePath);

   /* Methods in TXmlManagerImportExportWorkingModels.cpp. */
   bool SaveTypeConfigsInFile(std::list<wchar_t *> &typeConfigsNames,const wchar_t * fileName);    ///< save all needed information of the selected typeconfigs into the fileName file
   bool LoadTypeConfigsFromFile (const wchar_t * fileName, s_NonAddedWorkingModes & nonAddedModes);  ///< load all non existing working modes
   bool ReplaceTypeConfigs (s_NonAddedWorkingModes & nonAddedModes);                                 ///< replaces all working modes in nonAddedModes structure
   ///< load a file withh typeconfigs and products and merge them with the current xmlDoc tree

   TDataManager *GetMngr()                                                    {
      return m_mngr;
   }


};


int is_valid(const xmlDocPtr doc, const char *schema_filename)
{
   return 1;
   /*
    xmlDocPtr schema_doc = xmlReadFile(schema_filename, NULL, XML_PARSE_NONET);
    if (schema_doc == NULL) {
        // the schema cannot be loaded or is not well-formed
        return -1;
    }
    xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc);
    if (parser_ctxt == NULL) {
        // unable to create a parser context for the schema
        xmlFreeDoc(schema_doc);
        return -2;
    }
    xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
    if (schema == NULL) {
        // the schema itself is not valid
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        return -3;
    }
    xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
    if (valid_ctxt == NULL) {
        // unable to create a validation context for the schema
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        return -4;
    }
    int is_valid = (xmlSchemaValidateDoc(valid_ctxt, doc) == 0);
//    if ( is_valid == 0)                                                      {
//      xmlSchemaValidityErrorFunc err;
//      xmlSchemaValidityWarningFunc warn;
//      int ve = xmlSchemaGetValidErrors( valid_ctxt , &err , &warn, NULL);
//      if ( err)                                                              {
//         char message[200];
//         err( NULL, message);
//         MessageBeep( -1);
//      }
//    }
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    // force the return value to be non-negative on success
    return is_valid ? 1 : 0;
   */
}

#endif





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
