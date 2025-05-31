//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TOPCManager.cpp
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

#include <string>
#include <sstream>

#include "TAux.h"

#include "definesLang.h"
#include "TOPCMngr.h"
#include "TOPCInterface.h"
#include "TEventDispatcher.h"
#include "LTInit.h"

//*****************************************************************************/
/*************************************************************/
//Funtions that will be called from the DLL


TOPCMngr *g_this;       ///< Set a global pointer to the OPC Manager to be able to call the methods.

/**
   update value of the tdata related with the opc node
   @param  pNode opc node * to get the tdata
   @param  valuetoSet value to set in the tdata
   @return true if the operation concluded normally
*/
bool g_updateTDataFromNode(void * pNode,const wchar_t* valuetoSet){

      return g_this->UpdateTDataFromNode(pNode,valuetoSet);
}

/**
   reads the value of a TData correspondent to the opc node
   @param pNode ,opcNode pointer used to identify the Tdata
   @param readValue_o ,wchar_t * to store the requested value
   @param valuesize ,size of the readValue_o array, also used to output the size needed to read the entire value
   @return returns true if the readValue can be performed, false otherwise
*/
bool g_readTDataValue(void * pNode, wchar_t* readValue_o, unsigned int &valuesize){

   if(g_this)
      return g_this->readTDataValue(pNode,readValue_o, valuesize);
   else
      return false;
}
/**
*  PrintError function to print a LOG_ERROR1 from the dll
*  @param error  message to be logged
*/
void g_logErrorFromDll(const wchar_t* error){

   if(g_this)
      g_this->printErrorMessage(error);
}

/**
*  PrintError function to print a LOG_ERROR1 from the dll
*  @param error  message to be logged
*/
void g_eventDispatcherFromDll(int typecode, const wchar_t* tag, const wchar_t* text){

   if(g_this)
      DISPATCH_EVENT(typecode, tag, text, g_this->getDataMngr());
}

//----------------------------------------------------------------------------
/**
   Constructor for Communication Manager
   @param  moduleName   Char pointer with the name of the module that created de TOPCMngr.
   @param  entryPoint   Char pointer with the parameter to create the default TLinkServer.
*/
TOPCMngr::TOPCMngr    (TModuleMngr * modmngr )                                {

   g_this         = this;  //we need a global pointer to this to call methods inside the functions
   m_dataMngr     = modmngr;
   m_opcIfc       = NULL;

   m_started      = false;
   m_accessLevel  = 1;     //advance user after mask applied
   m_dllLoaded    = false;

   m_initServer       = NULL;
   m_closeServer      = NULL;
   m_deleteAllNodes   = NULL;
   m_createNode       = NULL;
   m_updateNode       = NULL;


   if(wcscmp(OPC_HOST_NAME,L"")==0){
      //if we dont have a host name, we consider the opc as not configured, so
      LOG_INFO1(L"The OPC server Configuration is empty. The OPC Server will be disabled");
      return;
   }

   //load DLL and its functions
   m_dllhandle     =  LoadLibrary(L"OPCUADll.dll");
   if ( m_dllhandle == NULL )                                                        {
      LOG_ERROR1(L"Error loading OPCUADll.dll. The OPCServer will be disabled.");
      return;
   }
   else                                                                       {
      m_initServer       = (INITSERVER)      GetProcAddress( m_dllhandle, "InitServer" );
      m_closeServer      = (CLOSESERVER)     GetProcAddress( m_dllhandle, "CloseServer" );
      m_deleteAllNodes   = (DELETEALLNODES)  GetProcAddress( m_dllhandle, "DeleteAllNodes" );
      m_createNode       = (CREATEOPCNODE)   GetProcAddress( m_dllhandle, "CreateOPCObject" );
      m_updateNode       = (UPDATEOPCNODE)   GetProcAddress( m_dllhandle, "UpdateOPCObject" );

      //start the opc server ( done here because Start() is called after the loadConf
      // and therefore its called afer the UpdateConfiguration)
      bool serverinit=m_initServer(OPC_HOST_NAME,
                  OPC_URL,
                  OPC_APP_NAME,
                  OPC_ADDRESSSPACE,
                  &g_updateTDataFromNode, &g_readTDataValue,
                  &g_logErrorFromDll, &g_eventDispatcherFromDll );

      if ( serverinit ) {
           m_dllLoaded    = true;  //set the dll as loaded
      }

   }
}


//----------------------------------------------------------------------------
/**
   Frees up memory
*/
TOPCMngr::~TOPCMngr()                                                       {
   LOG_INFO1(L"TOPCMngr destructor");

   g_this = NULL;
   if(m_closeServer)
      m_closeServer();

   if ( ! FreeLibrary( m_dllhandle) )                                            {
      LOG_ERROR2(L"Dynamic Library could not be unloaded: ", L"OPCUADll.dll");
      Sleep(2000);
   }
   else                                                                    {
      LOG_INFO2(L"Dynamic Library Unloaded: ", L"OPCUADll.dll");
   }

   m_started = false;


}

 //----------------------------------------------------------------------------
/**
   Sets the TOPCInterface to use it when updating tdatas (to avoid refresh)
   @param   opcIfc     TOPCInterface pointer
*/
void TOPCMngr::SetOPCInterface(TOPCInterface * opcIfc)                    {
   m_opcIfc = opcIfc;
}

//----------------------------------------------------------------------------
/**
   Starts the process of the opcMngr
*/
bool TOPCMngr::Start()                                                     {
   //set the started status to true only if the dll is loaded
   m_started = m_dllLoaded;
   return m_started;
}

//----------------------------------------------------------------------------
/**
   Closes the process of the opcMngr
*/
bool TOPCMngr::Close()                                                     {



   return true;
}

//----------------------------------------------------------------------------
/**
   UpdateConfiguration,
   removes previous configurations, then add the new one
   @param list ,list with the new tdatas to publish
*/
bool TOPCMngr::UpdateOPCConfiguration(std::list<TData *> &list)               {

   if(!m_started)      //state checks
      return false;

   //enable opcinterface
   if(m_opcIfc)
      m_opcIfc->SetEnabled(true);

   m_deleteAllNodes();      //deleteAllNodes of previous configurations

   m_tdatasnodesmap.Reset();  //Reset the bidirectional map of tdatas and opcnodes

   wchar_t namebuffer[1024];

   //ToDo check if the tdata values is huge and avoid publishing it
   for(  std::list<TData *>::iterator i = list.begin(); i != list.end();i++){

      //access level check
      unsigned int flags = (*i)->GetFlags();
      int accessLevel = (int)( (flags & FLAG_LEVEL_MASK) >> 14);
      if ( accessLevel > m_accessLevel ) {       //Skip the tdata if the accessLevel of the TData is bigger then the stablished for the opcManager
         LOG_WARN1(L"TData with opc flags is creating an OPC node but the OPC server has not enough access level");
         continue;
      }

      int tdatasize = (*i)->GetValueDataSize();
      if ( tdatasize > TOPCMngr::m_tdatasizeLimit ) {       //Skip the tdata if the size is bigger than the limit
         LOG_WARN1(L"TData with opc flags is creating an OPC node but the size of the value is bigger than allowed");
         continue;
      }

      (*i)->GetFullName(namebuffer,1024);
      unsigned int vsize = 128;
//      wchar_t * desc = new wchar_t[vsize];
      // get description of the tdata
      std::wstring wdesc =formatTDataDescription((*i));

      vsize=128;
      wchar_t * val = new wchar_t[vsize];
      //     get value
      if (! (*i)->AsText( val, vsize)) {
         wchar_t * oldval = val;      //if astext is false, we try with the new suggested size
         delete[] oldval;  //free previous reserved memory
         val = new wchar_t[vsize];
         (*i)->AsText( val, vsize);   //this shouldnt fail
      }
       bool readOnly = ( (*i)->GetFlags() & FLAG_RDONLY ) ;

      //create node from tdata
      void * node = m_createNode( namebuffer, val, wdesc.c_str(), readOnly );
      std::wstring wsname;
      if ( (*i)->GetFullNameAsWString( wsname))                              {
         m_tdatasnodesmap.AddTDataNode(wsname.c_str(),node);    // add tdata-opcnode pair in the map
      }
//      delete[] desc;
      delete[] val;
   }
   return true;
}

//----------------------------------------------------------------------------
/**
   UpdateTDataFromNode,
   updates the value of the tdata correspondent to the node
   @param pNode ,opcNode pointer used to identify the Tdata
   @param valuetoSet , the new value of the tdata
   @return returns true if the setAsText actually worked, false otherwise.
*/
bool TOPCMngr::UpdateTDataFromNode(void * pNode,const wchar_t* valuetoSet){

   const std::wstring wstdatafullname=m_tdatasnodesmap.GetTData(pNode);
   if (wstdatafullname == L"") {
      LOG_WARN1(L"The OPCServer is sending updates of non-existing TData");
      return false;
   }
   if(m_dataMngr){
         TData *td = m_dataMngr->GetDataObject(wstdatafullname.c_str());
         if(td)
            return td->SetAsText(valuetoSet);
   }
   return false;
}

//----------------------------------------------------------------------------
/**
   UpdateNodeFromTData,
   updates the value of the node correspondent to the tdata
   @param tdata ,tdata pointer to be changed
   @return returns true if the setAsText actually worked, false otherwise.
*/
bool TOPCMngr::UpdateNodeFromTData(TData * tdata){

   if(!m_started || tdata == NULL)      //state and tdata checks
      return false;

   //access level check
   unsigned int flags = tdata->GetFlags();
   int accessLevel = (int)( (flags & FLAG_LEVEL_MASK) >> 14);
   if ( accessLevel > m_accessLevel ) {       //Skip the tdata if the accessLevel of the TData is bigger then the stablished for the opcManager
//      LOG_WARN1(L"TData with opc flags is sending update but the OPC server has not enough access level");
      return false;
   }
   //get the name and find their opc node pointer
   wchar_t namebuffer[1024];
   tdata->GetFullName(namebuffer,1024);
   void * node = m_tdatasnodesmap.GetNode(namebuffer);
   if (node == NULL) {
//      LOG_WARN1(L"TData with opc flags sent update to non-existing OPC node");
      return false;
   }
   unsigned int vsize = 128;
//   wchar_t * desc = new wchar_t[vsize];
   // get description of the tdata
   std::wstring wdesc =formatTDataDescription(tdata);

   wchar_t * val = new wchar_t[vsize];
   // get value of the tdata
   if (! tdata->AsText( val, vsize)) //if astext is false, we try with the new suggested size
   {
      wchar_t * oldval = val;
      delete[] oldval;  //free previous reserved memory
      val = new wchar_t[vsize];
      tdata->AsText( val, vsize);   //this shouldnt fail
   }
   bool readOnly = ( tdata->GetFlags() & FLAG_RDONLY ) ;   // get readonly flag

   //create node from tdata
   m_updateNode( node, namebuffer, val, wdesc.c_str(), readOnly );
//   delete[] desc;
   delete[] val;
   return true;
}

//----------------------------------------------------------------------------
/**
   readTDataValue,
   reads the value of a TData correspondent to the opc node
   @param pNode ,opcNode pointer used to identify the Tdata
   @param readValue_o ,wchar_t * to store the requested value
   @param valuesize ,size of the readValue_o array, also used to output the size needed to read the entire value
   @return returns true if the readValue can be performed, false otherwise
*/
bool TOPCMngr::readTDataValue(void * pNode, wchar_t* readValue_o, unsigned int &valuesize){
   //get tdataname from map
   const std::wstring wstdatafullname=m_tdatasnodesmap.GetTData(pNode);
   if (wstdatafullname == L"") {
      LOG_WARN1(L"The OPCServer is trying to read a non-existing TData");
      return false;
   }
   if(m_dataMngr){
         TData *td = m_dataMngr->GetDataObject(wstdatafullname.c_str());
         if(td){
            return td->AsText( readValue_o, valuesize);
         }
   }
   return false;

}

//----------------------------------------------------------------------------
/**
   Get a full description of a TData which depends on its type to be used
   in the description field of their correspondent opc node
   @param tdata tdata to get the description
   @return wstring with the full description
*/
std::wstring TOPCMngr::formatTDataDescription(TData *tdatasrc){
   std::wstringstream ret;
   ret.str(L""); //initialize the steam
   bool firstuse=true;
   for (int i = 0; i < tdatasrc->Count(); i++)   {
      TAttrib *t = ((TAttribMngr *)tdatasrc)->operator[](i);
      std::wstring atname = t->GetName();
      if( atname == cAttribNames[ATTR_DESCRIPTION]
            || atname == cAttribNames[ATTR_MAX]
            || atname == cAttribNames[ATTR_MIN]
            || atname == cAttribNames[ATTR_LIST]){

         if (t->GetType() == e_sattrib::TYPE_INT){
            int val;
            tdatasrc->GetAttribValue(t->GetName(), val);
            if(!firstuse)
               ret <<" || ";
            ret << atname.c_str() << ":" << val ;
            firstuse=false;
         }
         if (t->GetType() == e_sattrib::TYPE_FLOAT){
            float val;
            tdatasrc->GetAttribValue(t->GetName(), val);
            if(!firstuse)
               ret <<" || ";
            ret << atname.c_str() << ":" << val;
            firstuse=false;
         }
         if (t->GetType() == e_sattrib::TYPE_WCHAR){
            unsigned int valsize = 128;
            wchar_t * val = new wchar_t[valsize];

            if (! tdatasrc->GetAttribValue(t->GetName(), val, valsize)) {
               //if astext is false, we try with the new suggested size
               wchar_t * oldval = val;
               delete[] oldval;  //free previous reserved memory
               val = new wchar_t[valsize];
               tdatasrc->GetAttribValue(t->GetName(), val, valsize);
            }
            if(!firstuse)
               ret <<" || ";
            ret << atname.c_str() << ":" << val;
            firstuse=false;
            delete[] val;
         }
         if (t->GetType() == e_sattrib::TYPE_CHAR){
            unsigned int valsize = 128;
            char * val = new char[valsize];

            if (! tdatasrc->GetAttribValue(t->GetName(), val, valsize))  {
               //if astext is false, we try with the new suggested size
               char * oldval = val;
               delete[] oldval;  //free previous reserved memory
               val = new char[valsize];
               tdatasrc->GetAttribValue(t->GetName(), val, valsize);
            }
            if(!firstuse)
               ret <<" || ";
            ret << atname.c_str() << ":" << AuxStrings::utf8_to_utf16(val);
            firstuse=false;
            delete[] val;
         }
      }
   }   //end for attributes

   //add notify warning
   if(!( tdatasrc->GetFlags() & FLAG_NOTIFY))
      ret << L" || Not notifying changes, use polling instead subscription to get real TData values.";

   return ret.str();
}




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------



