//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TPersistence.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef TPersistenceH
#define TPersistenceH

#include "TStream.h"
#include "TMessage.h"
#include "tdataMng.h"
#include "tcomm.h"
#define TYPE_NAME_SIZE 64 //size of field in CFG message containing name of type being sent

//----------------------------------------------------------------------------
//********************************  TPersistence Class  **********************
//----------------------------------------------------------------------------
/// Class for Persistence Configuration Data
class TPersistence                                                            {

 protected:

   bool m_machineCodeValidated;                                        ///< Machine code Checking

   bool  LoadConfigFrom( wchar_t* fileNamePath);                       ///< Checks hardlock from configuration.

   // -- Virtual Methods for inherit
   //-------------------------------------------------------------------------
   /**
      Return Active Configuration into straemCfg variable.
      @param   path        name of module or path of configuration to be returned.
      @param   streamCfg   stream where configuration will be returned.
      @param   filterFlags will be used for filtering TDatas when serializing
      @return  bool  true on success, false otherwise

   */
   virtual bool GetConfig( wchar_t* path, LT::TStream* streamCfg, unsigned int filterFlags = FLAG_SAVE | FLAG_SAVE_NO_VALUE) = 0;

   /**
      Return Active Configuration into wstring variable ( unsing an xml format)
      @param   path        name of module or path of configuration to be returned.
      @param   str        string where configuration will be returned.
      @param   filterFlags will be used for filtering TDatas when serializing
      @return  bool  true on success, false otherwise

   */
public:   //ToDo: prueba moviles
   virtual bool GetConfigXml( wchar_t* path, std::wstring *str, unsigned int filterFlags = FLAG_SAVE | FLAG_SAVE_NO_VALUE) = 0;
protected:

   //-------------------------------------------------------------------------
   /**
      Save Configuration contained in stream into file.
      @param   streamCfg   stream containing configuration to be saved.
      @return  bool  true on success, false otherwise
   */
   virtual bool SetConfig( LT::TStream* stream) = 0;

   // ----------------------------------------------------------------------------
   /**
      Load configuration from fileNamePath.
      @param   fileNamePath
      @return bool true if configuration from fileNamePath has been succesfully loaded, false otherwise.
    */
   virtual bool MyLoadConfigFrom( wchar_t *fileNamePath) = 0;

   virtual char *GetFileCode() = 0;                                    ///<To check code with hardlock allowed codes

 public:

   TPersistence()                                                             {
      m_machineCodeValidated = false;
   };
   virtual ~TPersistence()          {};

   //-------------------------------------------------------------------------
   /**
      Check if config is loaded
      @return  bool  true on success, false otherwise
   */
   virtual bool ConfigLoaded() = 0;

   /**
      Reset old configuration and load from fileNamePath
      Used for interface purporses.
      @param fileNamepath filename to be loaded
   */
   virtual bool  ResetAndLoadConfiguration( wchar_t* fileNamePath) = 0;

   //-------------------------------------------------------------------------
   /**
      Check integrity of file
      @return  bool  true on success, false otherwise
   */
   virtual bool CheckFile(wchar_t * fileName) = 0;

   //-------------------------------------------------------------------------
   /**
      Save Config to file
      @param   fileNamePath
      @return  bool  true on success, false otherwise
   */
   virtual bool SaveConfigTo( wchar_t* fileNamePath) = 0;

   //-------------------------------------------------------------------------
   /**
      Get Available Config types (list of) from configuration.
      @param   availableTypeList
      @return  bool  true on success, false otherwise
   */
   virtual bool GetAvailableTypesNames( std::list<wchar_t*>&  availableTypeList) =0 ;
   //-------------------------------------------------------------------------
//   /**
//      Get Tranlations (list of) of Available Config types from configuration.
//      @param   availableTypeList
//      @return  bool  true on success, false otherwise
//   */
//   virtual bool GetTranslationTypeNames( std::list<wchar_t*>&  availableTypeList) =0 ;
   /**
      Get Tranlations (list of) of Available Config types from configuration.
      @param   GetNamesTypeNames
      @return  bool  true on success, false otherwise
   */
   virtual bool GetNamesTypeNames( std::list<wchar_t*>&  availableTypeList) =0 ;
   //-------------------------------------------------------------------------
   /**
      Get Active Config type name
      @param   activeTypeName
      @param   size
      @return  bool  true on success, false otherwise
   */
   virtual bool GetActiveTypeConfig( wchar_t* activeTypeName, int &size) = 0;

   //-------------------------------------------------------------------------
   /**
      Set Active Config Type
      @param   activeTypeName
      @return  bool  true on success, false otherwise
   */
   virtual bool SetActiveTypeConfig    ( wchar_t* typeName, bool saveToDisk = true) = 0;

   //-------------------------------------------------------------------------
   /**
      Create new configuration type.
      @param   activeTypeName
      @return  bool  true on success, false otherwise
   */
   virtual bool CreateTypeConfig  ( wchar_t* typeName) = 0;

   //-------------------------------------------------------------------------
   /**
      Rename an existent type
      @param   old type name, new type name
      @return  bool  true on success, false otherwise
   */
   virtual bool RenameTypeConfig  ( wchar_t* oldTypeName, wchar_t* newTypeName) = 0;
   virtual bool RenameTypeConfig2  ( wchar_t* oldTypeName, wchar_t* newTypeName) = 0;

   //-------------------------------------------------------------------------
   /**
      Save current Active Type as the NewName entered.
      @param   old type name, new type name
      @return  bool  true on success, false otherwise
   */
   virtual bool SaveTypeAs( wchar_t* oldTypeName, wchar_t* newTypeName) = 0;
   virtual bool SaveTypeAs2( wchar_t* oldTypeName, wchar_t* newTypeName) = 0;


  //-------------------------------------------------------------------------
   /**
      Delete a configuration type.
      @param   activeTypeName   name of type to be deleted
      @return  bool  true on success, false otherwise
   */
   virtual bool DeleteTypeConfig  ( wchar_t* typeName) = 0;

   //-------------------------------------------------------------------------
   /**
      General Method for processing messages.
      @param inputMsg Message received.
      @param responseMsg Ouptut Message to be returned (when necessary).
      @return true when message was sucessfully processed, false otherwise.
   */
   virtual bool ProcMsg( LT::TMessage *inputMsg, LT::TMessage *&responseMsg) = 0;

//   virtual bool  GetCfgMachine(wchar_t* name, TDataManager* mngr) = 0;     ///< Return machine configuration. Currently not used.

   //-------------------------------------------------------------------------
   /**
      Return  Code Machine Checking.
   */
   bool GetMachineCodeValidated()                                             {
      return m_machineCodeValidated;
   }

   virtual bool ValidateMachineCodeInFile( wchar_t* fileNamePath) = 0;
};

#endif




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
