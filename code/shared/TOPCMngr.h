//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TOPCManager.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TOPCMngrH
#define TOPCMngrH


#include <string>
#include <map>

#include "TData.h"
#include "TModuleMngr.h"

//----------------------------------------------------------------------------

#define  tagGM  L"GM"                         ///< TAG for Global Manager
#define  TIMEOUT_MUTEX        400            ///< Max delay per message.

 //----------------------------------------------------------------------------
//DLL function declarations

typedef  bool   (__cdecl *UPDATE_TDATA)      (void * pNode,const wchar_t* valuetoSet);
typedef  bool   (__cdecl *READ_TDATA)        (void * pNode, wchar_t* readValue_o, unsigned int &valuesize);
typedef  void   (__cdecl *LOG_ERRORDLL)      (const wchar_t* error);
typedef  void   (__cdecl *DISPATCH_EVENT)    (int typecode, const wchar_t* tag, const wchar_t* text);

typedef bool   ( __stdcall *INITSERVER )     ( const wchar_t* hostname,
                                                const wchar_t* url, const wchar_t* appName,
                                                const wchar_t* addressSpace,
                                                UPDATE_TDATA, READ_TDATA, LOG_ERRORDLL, DISPATCH_EVENT);
typedef void   ( __stdcall *CLOSESERVER )    ( );

typedef void * ( __stdcall *CREATEOPCNODE )  ( const wchar_t* name,
                                             const wchar_t* value,
                                             const wchar_t* description,
                                             bool readOnly );

typedef void   ( __stdcall *UPDATEOPCNODE )  ( void * node,
                                             const wchar_t* name,
                                             const wchar_t* value,
                                             const wchar_t* description,
                                             bool readOnly );

typedef void   ( __stdcall *DELETEALLNODES ) ( );

//----------------------------------------------------------------------------

class TOPCInterface;

//----------------------------------------------------------------------------
//********************************  TDatasNodesMap Class  *************************
//----------------------------------------------------------------------------
/**
   @class TDatasNodesMap
   Class with a bidirectional map to get the tdata of a opcnode and viceversa
   Currently implemented as two stdmaps which relates the tdata name (as wstring) and the opc node pointer (as void *)
*/
class TDatasNodesMap
{

private:

   std::map<std::wstring, void *> _tDataNodeMap;        ///< map to get the node * given the tdata name
   std::map<void *, std::wstring> _nodeTDataMap;        ///< map to get tdata name given the node *

public:
   TDatasNodesMap() {}
   ~TDatasNodesMap() {}

   /**
   *  Add a new pair of tdata name and node * in the maps
   *  @param tdatafullname  name of the tdata
   *  @param np opc node pointer
   *  @return true if added and false if exception
   */
   bool AddTDataNode(const wchar_t * tdatafullname, void * np){
      try{
          std::wstring wsname(tdatafullname);
         _tDataNodeMap.insert(std::pair<std::wstring, void *>(wsname, np));
         _nodeTDataMap.insert(std::pair<void *,std::wstring>(np, wsname));
         return true;
      }
      catch (std::exception){
         return false;
      }
   }

   /**
   *  Get the tdata name given the opc node pointer
   *  @param np opc node pointer
   *  @return string name or empty string if exception
   */
   const std::wstring GetTData(void *np) {
      try{
         return _nodeTDataMap.operator [](np);
      }
      catch (std::out_of_range &) { return L""; }
   }

   /**
   *  Get the node pointer given the tdata name
   *  @param np opc node pointer
   *  @return opc node pointer or NULL if exception
   */
   void * GetNode(const wchar_t * tdatafullname) {
      try{
         std::wstring wsname(tdatafullname);
         return _tDataNodeMap.operator [](wsname);
      }
      catch (std::out_of_range & e) { return NULL; }
   }

   /**
   *  Reset the two maps, deleting all tdata names and pointers.
   */
   void Reset(){
      _tDataNodeMap.clear();
      _nodeTDataMap.clear();
   }

};

//----------------------------------------------------------------------------
//********************************  TOPCMngr Class  *************************
//----------------------------------------------------------------------------
/**
   @class TOPCMngr
   Class to load the OPC DLL server and interact with their functions.
*/
class TOPCMngr                                                               {

protected:
   //dll functions pointers
   INITSERVER         m_initServer;                                        ///< init server DLL function
   CLOSESERVER        m_closeServer;                                       ///< close server DLL function
   DELETEALLNODES     m_deleteAllNodes;                                    ///< delete all nodes DLL function
   CREATEOPCNODE      m_createNode;                                        ///< create new opc node DLL function
   UPDATEOPCNODE      m_updateNode;                                        ///< update opc node value DLL function

   TOPCInterface *    m_opcIfc;                                            ///< OPC interface
   TModuleMngr *      m_dataMngr;                                          ///< Module Manager, to search tdatas

   TDatasNodesMap     m_tdatasnodesmap;                                     ///< map to get the corresponding opcnode of a fulltdataname and viceversa.

   bool               m_started;                                           ///< state to identify when the opcserver should be ready (when Start() executes on module)
   int                m_accessLevel;                                       ///< Access level of the OPC Manager, set to advance user
   bool               m_dllLoaded;                                         ///< state to identify if the dll has been loaded.
   HINSTANCE          m_dllhandle;                                         ///< Handle to the dll

   static const int   m_tdatasizeLimit = 1024;                             ///< Limit tdata's size to reject publishing them

public:
   TOPCMngr(TModuleMngr * modmngr);                                         ///< Constructor
   ~TOPCMngr();                                                             ///< Destructor

   void SetOPCInterface(TOPCInterface * opcIFC);                            ///< Sets the OPCInterface

   bool UpdateOPCConfiguration(std::list<TData *> &list);                    ///< resets the opc nodes configuration and creates the new one

   bool UpdateTDataFromNode(void * pNode,const wchar_t* valuetoSet);        ///< Method called by the Dll to update the tdatas value
   bool UpdateNodeFromTData(TData * tdata);                                 ///< Method called by the OPCInterface to update the opc node
   bool readTDataValue(void * pNode, wchar_t* readValue_o, unsigned int &valuesize); ///< Method called by the Dll to read the tdata value of a opcnode

public:

   bool Start();                                                            ///< Starts the opc Server
   bool Close();                                                            ///< Closes the opc Server

protected:
   std::wstring formatTDataDescription(TData *tdata);                       ///< Get a full description of a TData which depends on its type to be used in the description field of their correspondent opc node

   // --inline functions --
public:
   /**
   *  PrintError method to print a LOG_ERROR1 from the dll
   *  @param mes  message to be logged
   */
   void printErrorMessage(const wchar_t * mes){
      LOG_ERROR1(mes);
   }

   /**
   *  m_dataMngr getter
   *  @return m_dataMngr
   */
   TModuleMngr * getDataMngr(){
      return m_dataMngr;
   }

};

#endif




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
