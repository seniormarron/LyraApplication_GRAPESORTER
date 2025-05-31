//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TXmlManagerTypeConfig.cpp
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

#include <iostream>
#include <sstream>

#include "TXmlManager.h"
#include "TAux.h"

//---------------------------------------------------------------------------
bool  TXmlManager::SaveActiveTypeConfig(char* name )                           {

   // Load active Type confiration
   char xPathExpr    [MAX_XPATH_EXPR]  = "//";
   strcat(xPathExpr, LBL_TYPE_CONFIG_ACTIVE);

   // Search XPath
   xmlXPathObjectPtr xPathObj;
   bool ret = false;
   if ( !XmlGetNodesFromXpath( xPathExpr, xPathObj) ) {
        // Get the root element node
      xmlNode *cur_node   = xmlDocGetRootElement( this->m_DocPtr );
      xmlNode *new_node   = xmlNewNode( NULL, ( xmlChar *) LBL_TYPE_CONFIG_ACTIVE );
      xmlNewProp ( new_node,(xmlChar *) LBL_NAME, (xmlChar *) name );
       xmlAddChild (cur_node, new_node);
      ret = true;
   }
    else {

  // Selected Nodes
   xmlNodeSetPtr nodes = xPathObj->nodesetval;
   int sizeList = ( nodes) ? nodes->nodeNr : 0;

   if (sizeList > 0)                                           {
      if ( nodes->nodeTab[0]->type  == XML_ELEMENT_NODE)       {
         xmlNode* node     = nodes->nodeTab[0];
         if (node)   {
            xmlSetProp(node,LBL_NAME,name);
            ret = true;
         }
      }
   }
    xmlXPathFreeObject(xPathObj);
   }
   if (ret) {
      m_toSave = true;  //set toSave to check if we need to save the conf when closing persistence
   }
   return ret;

   /* Cleanup */

}
void  TXmlManager::LoadActiveTypeConfig()                                          {

   // Load active Type confiration
   char xPathExpr    [MAX_XPATH_EXPR]  = "//";
   strcat(xPathExpr, LBL_TYPE_CONFIG_ACTIVE);

   // Search XPath
   xmlXPathObjectPtr xPathObj;
   bool ret = true;
   if ( !XmlGetNodesFromXpath( xPathExpr, xPathObj) )
      ret = false;

  wcscpy(m_activeType,LBL_DEFAULT);
   m_isType = false;
   if (!ret)  {
      return;
   }
   // Selected Nodes
   xmlNodeSetPtr nodes = xPathObj->nodesetval;
   int sizeList = ( nodes) ? nodes->nodeNr : 0;

   if (sizeList > 0) {
      if ( nodes->nodeTab[0]->type  == XML_ELEMENT_NODE)      {
         xmlNode* node     = nodes->nodeTab[0];
         if (node)   {
            xmlChar* xmlName = xmlGetProp( node, LBL_NAME);
            wchar_t* name = ConvertOutputWC( xmlName);
            if (name != NULL)                                        {
               //wcscpy(m_activeType,name);  04/12/2012 comento esto porque si no el SetActiveTypeConfig no hace nada, aunque ahora dispara un guardado a disco que se deberia evitar
               //LOG_INFO2 (L"ACTIVE TYPE Configuration: ",m_activeType);
               SetActiveTypeConfig (name, false); // 04/12/2012 false for not saving to disk, because we are loading configuration from disk
               delete [] name;
            }
            xmlFree(xmlName);
         }
      }
   }
   /* Cleanup */
   xmlXPathFreeObject(xPathObj);

}

//----------------------------------------------------------------------------
/**

*/
bool  TXmlManager::SetActiveTypeConfig( wchar_t* newActiveTypeName, bool saveToDisk)   {

   if  (!newActiveTypeName )     {
      LOG_WARN1(L"typeName NULL. No value could be assinged.");
      return false;
   }

   if ((newActiveTypeName) && (_wcsicmp(newActiveTypeName,m_activeType) == 0 ) )
      return true;

   bool ret = false;
   if (newActiveTypeName)                                                     {

      char* typeName =  TAux::WSTR_TO_STR ( newActiveTypeName);
      if (!typeName)
         return false;
         // 1.- Search in tree if  configuration Type with name typeName exists
         // Search in tree Active Type
        /* Create XPathExpression */
      char* endNameXPath   ="']";
      char xPathExpr    [MAX_XPATH_EXPR]  = "//type_config[@name='";
      strcat( xPathExpr,typeName);
      strcat( xPathExpr,endNameXPath);

         // Search XPath
      xmlXPathObjectPtr xPathObj;
      if ( m_mutexXmlDoc && WaitForSingleObject( m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0 )    {

         if ( !XmlGetNodesFromXpath( xPathExpr, xPathObj) )                   {
            LOG_WARN4 (L"Configuration Type with name ", newActiveTypeName ,L" does not exist. Default configuration will be loaded. Name: ",m_activeType);
         }
         else                                                             {
            /* Cleanup */
            xmlXPathFreeObject(xPathObj);
            if (wcscmp(newActiveTypeName, m_activeType) != 0) {
               m_activeType[0]=0;
               unsigned int size = wcslen(newActiveTypeName);
               if (  ( SIZEOF_CHAR (m_activeType) > size ) && (size > 0 ) )   {
                  wcsncpy(m_activeType,newActiveTypeName, size);
                  m_activeType[size]=L'\0';
                  m_isType = true;
                  if ( SaveActiveTypeConfig(typeName) && saveToDisk)         {
                     SaveConfigTo();
                  }
                  LOG_WARN2 (L"Configuration Type has been set. ACTIVE: ",m_activeType);
                  ret = true;
               }
            }
         }
         ReleaseMutex( m_mutexXmlDoc);
      }
      else
         LOG_INFO2 (L"Configuration Type could not been set as ACTIVE: ", newActiveTypeName);

      delete [] typeName;
   }

 return ret;
}

//----------------------------------------------------------------------------
/**
   Get in activeTypeName current active config.
   return false is no active type was found, true if so.
*/
bool  TXmlManager::GetActiveTypeConfig( wchar_t* activeTypeName, int &size) {

   if (size > (signed)wcslen(m_activeType)) {
      wcsncpy(activeTypeName, m_activeType , wcslen(m_activeType));
      size = wcslen(m_activeType);
      activeTypeName [size] =L'\0';
      return true;
   }
   return false;
}

//----------------------------------------------------------------------------
/**
   Create a new type configuration called newActiveTypeName. A new entry in
   "modules" Node will bre created.
   @param    newActiveTypeName name for the new type config
   return true if type was successfully created, false if some error occurs.
*/
bool  TXmlManager::CreateTypeConfig  ( wchar_t* newActiveTypeName)     {

   if (!newActiveTypeName || ( (newActiveTypeName) && wcslen(newActiveTypeName) == 0 ) )
      return false;

   if ( _wcsicmp(newActiveTypeName, LBL_DEFAULT) == 0 )
      return false;
   // 1.- TypeName is search to check if already exists.
   /* Create XPathExpression */

   char* typeName =  TAux::WSTR_TO_STR ( newActiveTypeName);
   if (!typeName)
      return false;

   char* endNameXPath   ="']";
   char xPathExpr    [MAX_XPATH_EXPR]  = "//type_config[@name='";
   strcat( xPathExpr,typeName);
   strcat( xPathExpr,endNameXPath);

   // Search XPath
   xmlXPathObjectPtr xPathObj;
   bool ret = true;

   if ( m_mutexXmlDoc && WaitForSingleObject( m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0 )    {

      if ( XmlGetNodesFromXpath( xPathExpr, xPathObj) )          {
         LOG_WARN2 (L"Type Configuration already exists!!. Name: ", newActiveTypeName); //, typeName);
         /* Cleanup */
         xmlXPathFreeObject(xPathObj);
         ret = false;
      }

      if (ret)                                                       {
         // 2.- If not, we create a new typeconfig, with typeName.
         xPathExpr[0] = '\0';
         strcat( xPathExpr,"//modules");

         if ( !XmlGetNodesFromXpath( xPathExpr, xPathObj) )          {
            LOG_WARN1(L"Modules Node does NOT exits!!.");
            ret = false;
         }
         if (ret)                                                    {
            // Selected Nodes
            xmlNodeSetPtr nodes = xPathObj->nodesetval;
            int sizeList = ( nodes) ? nodes->nodeNr : 0;

            if (sizeList > 0) {
               if ( nodes->nodeTab[0]->type  == XML_ELEMENT_NODE)    {
                  xmlNode* node     = nodes->nodeTab[0];
                  if (node)   {
                     xmlNode* newNode  = xmlNewNode ( NULL,  (xmlChar *) LBL_TYPE_CONFIG );
                     xmlNewProp ( newNode,(xmlChar *) LBL_NAME, (xmlChar *)typeName);
                     xmlAddChild( node ,  newNode );

                     xmlNode* newNode2  = xmlNewNode ( NULL,  (xmlChar *) LBL_TATTRIB);
                     xmlAddChild(newNode, newNode2);
                     xmlNewProp (newNode2,(xmlChar *) LBL_NAME, (xmlChar *)"Name");

                     std::wstring newValue(LANG_CODE[ACTIVE_LANG]);
                     newValue += L'=';
                     newValue += newActiveTypeName;

                     char* tmp =  TAux::WSTR_TO_STR ( newValue.c_str());
                     if (tmp)        {
                        xmlNewProp ( newNode2,(xmlChar *) LBL_VALUE, (xmlChar *)tmp);
                        delete [] tmp;
                     }
//                     char* tmpNameModule =  TAux::WSTR_TO_STR ( nameModule);
                     xmlNode *activeNode =  XmlGetActiveConfigNode(m_activeType);
                     if ( activeNode )    {
                        xmlNode* copyNode =  xmlCopyNode (activeNode,1);
                     //   xmlSetProp(copyNode,LBL_NAME,tmpNameModule);
                        xmlAddChild( newNode ,  copyNode );

                        wchar_t text[1024];
                        swprintf(text,L"Type Configuration with name - %s- Created. ", newActiveTypeName);
                        LOG_INFO1 (text);

                     }
                    /* else                                                     {
                        xmlNode* nodeAux              = xmlNewNode ( NULL, (xmlChar *) LBL_TDATAMANAGER );
                        xmlNewProp ( nodeAux,(xmlChar *) LBL_NAME, (xmlChar *)tmpNameModule );
                        xmlAddChild( newNode ,  nodeAux );
                     }*/
                //  if (tmpNameModule)
                //     delete []  tmpNameModule;
                  }
               }
            }

            /* Cleanup */
            xmlXPathFreeObject(xPathObj);
            ret = SaveConfigTo();
         }
      }
      ReleaseMutex( m_mutexXmlDoc);
   }

   delete [] typeName;
   return ret;
}

//----------------------------------------------------------------------------
/**
   Delete typeToDelete type config
*/
bool  TXmlManager:: DeleteTypeConfig         ( wchar_t* typeToDelete)         {

   if  (!typeToDelete )                                                       {
      LOG_WARN1(L"TypeName NULL. No type could be deleted.");
      return false;
   }

   if ((typeToDelete) && (_wcsicmp(typeToDelete,m_activeType) == 0 ) )        {
      LOG_WARN1(L"TypeName Currently Active. Type Active could NOT be deleted.");
      return false;
   }
   bool ret = true;
   if (typeToDelete)                                                     {

      char* typeName =  TAux::WSTR_TO_STR ( typeToDelete);
      if (!typeName)
         return false;

       // 1.- Search in tree if  configuration Type with name typeName exists
      /* Create XPathExpression */
      char* endNameXPath   ="']";
      char xPathExpr    [MAX_XPATH_EXPR]  = "//type_config[@name='";
      strcat( xPathExpr,typeName);
      strcat( xPathExpr,endNameXPath);

         // Search XPath
      xmlXPathObjectPtr xPathObj;

      if ( m_mutexXmlDoc && WaitForSingleObject( m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0 )    {
         try                                                                  {

             if ( !XmlGetNodesFromXpath( xPathExpr, xPathObj) )                      {
               LOG_WARN3 (L"Configuration Type with name ", typeToDelete ,L" does not exist. Type could not be deleted.");
               ret = false;
            }

            if (ret)                                                                {

               xmlNodeSetPtr nodes = xPathObj->nodesetval;
               int sizeList = ( nodes) ? nodes->nodeNr : 0;

               if (sizeList > 0)          {
                  if (nodes->nodeTab[0]->type  == XML_ELEMENT_NODE)             {

                     xmlNode* node  = nodes->nodeTab[0];
                     //--
                     // Check if type to delete is the last one in configuration.
                     // in this case, we check if has nodes of type.
                     std::list<wchar_t*> typeList;

                     GetAvailableTypesNames(typeList);

                     if (typeList.size() == 1)
                        if (HasTypeNodes(node))
                           ret = false;

                     std::list<wchar_t*>::iterator i  = typeList.begin();
                      while  (i != typeList.end())       {
                           wchar_t* todelete = (*i);
                           i = typeList.erase(i);
                           free(todelete);
                     }
                     typeList.clear();
                     //--

                     xmlUnlinkNode(node);
                     xmlFreeNode(node);
                     LOG_INFO2 (L"Configuration Type Deleted. Name: ", typeToDelete);
                  }
               }
            ret = SaveConfigTo();
           /* Cleanup */
           //ToDo: Esta linea se comenta porque al borrar dos tipos consecutivos da un error de acces violation y ni si quiera
           // con el codeguard se puede ver el porqué.
//            xmlXPathFreeObject(xPathObj);
            }
      }
      __finally                                                               {
         ReleaseMutex( m_mutexXmlDoc);
      }

   }

   delete [] typeName;
   }

   return ret;
}
//----------------------------------------------------------------------------
/**
   get a list of the types info struct
   returns the name (outter name in xml), translated name (inner name node) and number (parsed from the translated name)
   of every working mode
   @param[out] typeslist  list of types info
   @return true if succed, false otherwise
*/
bool TXmlManager::getTypesInfo(std::list<s_typeInfo>& typeslist, wstring *pFilterNamesWithStart)   {

   bool ret = false;


   bool filtering =  (pFilterNamesWithStart != NULL);

//   locale native("");
//   locale global;

   /* Create XPathExpression */
//   std::stringstream xPathExpr;
   //usamos funciones del siglo XV (wchar * y char*) porque al builder no le apetece funcionar con C++
   char xPathExpr[ MAX_XPATH_EXPR ] = "//";

   xPathExpr[0]=0;
   strcat(xPathExpr,"//");
   strcat(xPathExpr,LBL_TYPE_CONFIG);
//   xPathExpr.str("");
//   xPathExpr<< "//"<< LBL_TYPE_CONFIG;

   // Search XPath
   xmlXPathObjectPtr xPathObj;

   if ( m_mutexXmlDoc && WaitForSingleObject( m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0 )    {

      wstring name=L"",
              translation,
              number = L"";

      if (XmlGetNodesFromXpath((char *) xPathExpr, xPathObj))                         {
         // Selected Nodes
         xmlNodeSetPtr nodes = xPathObj->nodesetval;
         int sizeList = ( nodes) ? nodes->nodeNr : 0;

         for( int i=0;i<sizeList;i++)                                               {
            if (nodes->nodeTab[i]->type  == XML_ELEMENT_NODE)                       {
               xmlNode* node  = nodes->nodeTab[i];
               xmlChar* xmlName = xmlGetProp( node, LBL_NAME);
               wchar_t* tmp = ConvertOutputWC(xmlName);
               if (tmp != NULL)                                                     {
                  name = tmp;
   //               availableTypeList.push_back(_wcsdup(tmp));
                  delete [] tmp;
                  ret = true;
               }
               xmlFree(xmlName);

               if ( filtering )  {
                  if( name.find(*pFilterNamesWithStart) !=  wstring::npos ) {
                     continue;
                  }
               }
               //now get inner name ->translation
               translation = name;  //default translation value is the name of the type
               xmlNode* nodeAttrib      = xmlGetChildNode(node, "Name");
               if (nodeAttrib)   {
                  xmlChar* xmlValue = xmlGetProp(nodeAttrib, LBL_VALUE);
                  if (xmlValue ) {
                     wchar_t* wValue = TAux::STR_TO_WSTR((char*)xmlValue);
                     if (wValue)       {
                        if (GetTranslationName( wValue, wcslen(wValue)))  {
                           translation = wValue;
                           ret = true;
                        }
                        delete [] wValue;
                     }
                     xmlFree(xmlValue);
                  }
               }
               number = L"";
               //now split the translation to get the number
               std::size_t spacepos = translation.find_first_of(L" ");
               if (spacepos!=std::string::npos){
                  wstring subnum=translation.substr(0,spacepos);
                  int numaux;
                  if (swscanf(subnum.c_str(), L"%d", &numaux) == 1)   {
                       number = subnum;
                       translation = translation.substr(spacepos+1);
                  }
               }
               //Add the info to the list
               typeslist.push_back(s_typeInfo(number,name,translation));
            }
         }
         /* Cleanup */
         xmlXPathFreeObject(xPathObj);

      }

      ReleaseMutex( m_mutexXmlDoc);
      typeslist.sort();
   }

   return ret;
}

//----------------------------------------------------------------------------
/**
   @param   machineName
   @param   availableTypeLis
*/
bool  TXmlManager::GetAvailableTypesNames( std::list<wchar_t*>& availableTypeList)  {

    bool ret = false;

   /* Create XPathExpression */
   char xPathExpr[ MAX_XPATH_EXPR ] = "//";
   strcat(xPathExpr, LBL_TYPE_CONFIG);

   // Search XPath
   xmlXPathObjectPtr xPathObj;

   if ( m_mutexXmlDoc && WaitForSingleObject( m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0 )    {


      if (XmlGetNodesFromXpath( xPathExpr, xPathObj))                         {
         // Selected Nodes
      xmlNodeSetPtr nodes = xPathObj->nodesetval;
      int sizeList = ( nodes) ? nodes->nodeNr : 0;

      availableTypeList.clear() ;
      for( int i=0;i<sizeList;i++)                                               {
         if (nodes->nodeTab[i]->type  == XML_ELEMENT_NODE)                       {
            xmlNode* node  = nodes->nodeTab[i];
            xmlChar* xmlName = xmlGetProp( node, LBL_NAME);
            wchar_t* tmp = ConvertOutputWC(xmlName);
            if (tmp != NULL)                                                     {
               availableTypeList.push_back(_wcsdup(tmp));
               delete [] tmp;
               ret = true;
            }
            xmlFree(xmlName);
         }
      }
      /* Cleanup */
      xmlXPathFreeObject(xPathObj);

      }

      ReleaseMutex( m_mutexXmlDoc);
   }



   return ret;
}
//----------------------------------------------------------------------------
/**
   Rename oldTypeName as newTypeName
*/
bool TXmlManager::RenameTypeConfig   ( wchar_t* oldTypeName, wchar_t* newTypeName)                 {

   if (!oldTypeName || !newTypeName)
      return false;

   bool ret = false;
   if ( m_mutexXmlDoc && WaitForSingleObject( m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0 )    {
      xmlNode *node =  XmlGetActiveConfigNode(oldTypeName);
      if ( node )                                                          {
         if ((stricmp(node->name, LBL_TYPE_CONFIG) != 0) && node->parent )
            node = node->parent;
         SetTypeNameTranslations(node, newTypeName);
         ret = SaveConfigTo();
      }
      ReleaseMutex( m_mutexXmlDoc);
   }

   return ret;
}
//----------------------------------------------------------------------------
/**
   Rename oldTypeName as newTypeName
*/
bool TXmlManager::RenameTypeConfig2   ( wchar_t* oldTypeName, wchar_t* newTypeName)                 {

   if (!oldTypeName || !newTypeName)
      return false;

   bool ret = false;
   if ( m_mutexXmlDoc && WaitForSingleObject( m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0 )    {
      xmlNode *node =  XmlGetActiveConfigNode(oldTypeName);
      if ( node )                                                          {
         if ((stricmp(node->name, LBL_TYPE_CONFIG) != 0) && node->parent )
            node = node->parent;
         SetTypeNameTranslations2(node, newTypeName);
         ret = SaveConfigTo();
      }
      ReleaseMutex( m_mutexXmlDoc);
   }

   return ret;
}
/**

*/
bool TXmlManager::isType( wchar_t* type)                                      {
        // 1.- Search in tree if  configuration Type with name typeName exists
      /* Create XPathExpression */


      char* typeName =  TAux::WSTR_TO_STR ( type);

      bool ret = false;
      if ( (!typeName) || (strlen(typeName) ==0) )      {
         //LOG_WARN (L"Not Valid Name." );
         return ret;
      }

      char* endNameXPath   ="']";
      char xPathExpr    [MAX_XPATH_EXPR]  = "//type_config[@name='";
      strcat( xPathExpr,typeName);
      strcat( xPathExpr,endNameXPath);

         // Search XPath
      xmlXPathObjectPtr xPathObj;

      if ( m_mutexXmlDoc && WaitForSingleObject( m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0 )    {

         if ( XmlGetNodesFromXpath( xPathExpr, xPathObj) )
            ret = true;

         ReleaseMutex( m_mutexXmlDoc);
      }

   delete [] typeName;
   return ret;

}

//----------------------------------------------------------------------------
/**
   Save oldTypeName as newTypeName
*/
bool TXmlManager::SaveTypeAs ( wchar_t* oldTypeName, wchar_t* newTypeName)  {

   if (!oldTypeName || !newTypeName)
      return false;

   if (isType(newTypeName))
      return false;

   bool ret = false;
   if ( m_mutexXmlDoc && WaitForSingleObject( m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0 )    {

      xmlNode *node =  XmlGetActiveConfigNode(oldTypeName);

      if ( node )   {
         if ((stricmp(node->name, LBL_TYPE_CONFIG) != 0) && node->parent )
               node = node->parent;

         xmlNode* newNode =  xmlCopyNode (node,1);
         char* newName = TAux::WSTR_TO_STR(newTypeName);
         if (newName) {
            xmlSetProp ( newNode, LBL_NAME, newName);
            if ( xmlAddChild( node->parent,newNode)) {
               SetTypeNameTranslations(newNode, newTypeName);
               ret = SaveConfigTo();
            }
            delete [] newName;
         }
      }
      ReleaseMutex( m_mutexXmlDoc);
   }

   return ret;
}
/**
   Save oldTypeName as newTypeName
*/
bool TXmlManager::SaveTypeAs2 ( wchar_t* oldTypeName, wchar_t* newTypeName)  {

   if (!oldTypeName || !newTypeName)
      return false;

   if (isType(newTypeName))
      return false;

   bool ret = false;
   if ( m_mutexXmlDoc && WaitForSingleObject( m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0 )    {

      xmlNode *node =  XmlGetActiveConfigNode(oldTypeName);

      if ( node )   {
         if ((stricmp(node->name, LBL_TYPE_CONFIG) != 0) && node->parent )
               node = node->parent;

         xmlNode* newNode =  xmlCopyNode (node,1);
         char* newName = TAux::WSTR_TO_STR(newTypeName);
         if (newName) {
            xmlSetProp ( newNode, LBL_NAME, newName);
            if ( xmlAddChild( node->parent,newNode)) {
               SetTypeNameTranslations2(newNode, newTypeName);
               ret = SaveConfigTo();
            }
            delete [] newName;
         }
      }
      ReleaseMutex( m_mutexXmlDoc);
   }

   return ret;
}

//----------------------------------------------------------------------------
/**
   Get tnames of types into List (without translation)
   @param   list of tranlations names in current language.
*/
bool  TXmlManager:: GetNamesTypeNames( std::list<wchar_t*>&  availableTypeList) {
    bool ret = false;

   /* Create XPathExpression */
   char xPathExpr[ MAX_XPATH_EXPR ] = "//";
   strcat(xPathExpr, LBL_TYPE_CONFIG);

   // Search XPath
   xmlXPathObjectPtr xPathObj;

   if ( m_mutexXmlDoc && WaitForSingleObject( m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0 )    {


      if ( XmlGetNodesFromXpath( xPathExpr, xPathObj))                                                                {
         // Selected Nodes
      xmlNodeSetPtr nodes = xPathObj->nodesetval;
      int sizeList = ( nodes) ? nodes->nodeNr : 0;

      availableTypeList.clear() ;
      for( int i=0;i<sizeList;i++)                                               {
         if (nodes->nodeTab[i]->type  == XML_ELEMENT_NODE)                       {
            xmlNode* node  = nodes->nodeTab[i];
            xmlNode* nodeAttrib      = xmlGetChildNode(node, "Name");
            bool found = false;
            if (nodeAttrib)   {
               xmlChar* xmlValue = xmlGetProp(nodeAttrib, LBL_VALUE);
               if (xmlValue ) {
                  wchar_t* wValue = TAux::STR_TO_WSTR((char*)xmlValue);
                  if (wValue)       {
                        availableTypeList.push_back(_wcsdup(wValue));
                        found = true;
                        ret = true;
                     delete [] wValue;
                  }
                  xmlFree(xmlValue);
               }
            }
            if (!found)                                                 {
                xmlChar* xmlName = xmlGetProp( node, LBL_NAME);
               wchar_t* tmp = ConvertOutputWC(xmlName);
               if (tmp != NULL)                                                     {
                  availableTypeList.push_back(_wcsdup(tmp));
                  delete [] tmp;
                  ret = true;
               }
               xmlFree(xmlName);
            }
         }
      }
      /* Cleanup */
      xmlXPathFreeObject(xPathObj);

      }

      ReleaseMutex( m_mutexXmlDoc);
   }
  return ret;
}

///**
//   Get translations names of types into List.
//   @param   list of tranlations names in current language.
//*/
//bool  TXmlManager:: GetTranslationTypeNames( std::list<wchar_t*>&  availableTypeList) {
//    bool ret = false;
//
//   /* Create XPathExpression */
//   char xPathExpr[ MAX_XPATH_EXPR ] = "//";
//   strcat(xPathExpr, LBL_TYPE_CONFIG);
//
//   // Search XPath
//   xmlXPathObjectPtr xPathObj;
//
//   if ( m_mutexXmlDoc && WaitForSingleObject( m_mutexXmlDoc, TIMEOUT_XML_MUTEX) == WAIT_OBJECT_0 )    {
//
//      if (XmlGetNodesFromXpath( xPathExpr, xPathObj))                         {
//         // Selected Nodes
//      xmlNodeSetPtr nodes = xPathObj->nodesetval;
//      int sizeList = ( nodes) ? nodes->nodeNr : 0;
//
//      availableTypeList.clear() ;
//      for( int i=0;i<sizeList;i++)                                               {
//         if (nodes->nodeTab[i]->type  == XML_ELEMENT_NODE)                       {
//            xmlNode* node  = nodes->nodeTab[i];
//            xmlNode* nodeAttrib      = xmlGetChildNode(node, "Name");
//            bool found = false;
//            if (nodeAttrib)   {
//               xmlChar* xmlValue = xmlGetProp(nodeAttrib, LBL_VALUE);
//               if (xmlValue ) {
//                  wchar_t* wValue = TAux::STR_TO_WSTR((char*)xmlValue);
//                  if (wValue)       {
//                     if (GetTranslationName( wValue, wcslen(wValue)))  {
//                        availableTypeList.push_back(_wcsdup(wValue));
//                        found = true;
//                        ret = true;
//                     }
//                     delete [] wValue;
//                  }
//                  xmlFree(xmlValue);
//               }
//            }
//            if (!found)                                                 {
//                xmlChar* xmlName = xmlGetProp( node, LBL_NAME);
//               wchar_t* tmp = ConvertOutputWC(xmlName);
//               if (tmp != NULL)                                                     {
//                  availableTypeList.push_back(_wcsdup(tmp));
//                  delete [] tmp;
//                  ret = true;
//               }
//               xmlFree(xmlName);
//            }
//         }
//      }
//      /* Cleanup */
//      xmlXPathFreeObject(xPathObj);
//
//      }
//
//      ReleaseMutex( m_mutexXmlDoc);
//   }
//  return ret;
//}


//----------------------------------------------------------------------------
/**
   Get node in tree configuration refered to entered type.
   @param aName name of type
   @param mName module Name (optional)
   return xmlNode node containing current type.
*/
xmlNode*  TXmlManager::XmlGetActiveConfigNode(wchar_t* aName, wchar_t* mName, xmlDocPtr docPtr )                 {


   char* activeName = TAux::WSTR_TO_STR(aName);

   if (!activeName)
      return NULL;

   if ( docPtr==NULL)
      docPtr = m_DocPtr;

   xmlNode * node = NULL;

   /* Create XPathExpression */
   char xPathExpr[ MAX_XPATH_EXPR ] = "//";
   strcat( xPathExpr, LBL_TYPE_CONFIG);
   strcat( xPathExpr, "[@name='");
   strcat( xPathExpr, activeName);
   strcat ( xPathExpr ,endNameXPath);

   if (mName)                                                     {
      char* moduleName    = TAux::WSTR_TO_STR(mName);
      strcat ( xPathExpr ,startNameXPath);
      strcat ( xPathExpr ,moduleName);
      strcat ( xPathExpr ,endNameXPath);
      delete [] moduleName;
   }

   // Search XPath
   xmlXPathObjectPtr xPathObj;

   bool ret = true;
   if ( !XmlGetNodesFromXpath( xPathExpr, xPathObj, docPtr) )
      ret = false;

   if (ret)                                                       {
      // Selected Nodes
      xmlNodeSetPtr nodes = xPathObj->nodesetval;
      int sizeList = ( nodes) ? nodes->nodeNr : 0;

      for( int i=0;i<sizeList;i++)
         if (nodes->nodeTab[i]->type  == XML_ELEMENT_NODE)        {
            node  = nodes->nodeTab[i];
            break;
        }

      /* Cleanup */
      xmlXPathFreeObject(xPathObj);
   }
   delete [] activeName;

   return node;

}
//----------------------------------------------------------------------------
/**
   Set name of type into attribute tranlations "name", with the following format
   #EN=EnglishValueName#ES=SpanishValueName
   @param node type node
   @param typeName name to be include into attribute.
*/
void TXmlManager::SetTypeNameTranslations(xmlNode* node, wchar_t* typeName)   {

   xmlNode* nodeAttrib      = xmlGetChildNode(node, "Name");
   bool saved = false;
   if (nodeAttrib)   {
      xmlChar* xmlValue = xmlGetProp(nodeAttrib, LBL_VALUE);
      if (xmlValue ) {
         wchar_t finalValue[4096];
         wchar_t* wValue = TAux::STR_TO_WSTR((char*)xmlValue);
         if (wValue)       {
            // ----------  Translation FOUND -------------------------------------------
            // 1.- Search of translation for active lang. Active lang is contained in ACTIVE_LANG variable.
            wchar_t lang[8];
            swprintf(lang, L"%s=", LANG_CODE[ACTIVE_LANG]);
            wchar_t* transVal = wcsstr( (wchar_t*)wValue, lang ) ;

            // ----------  Translation FOUND -------------------------------------------
            if  (transVal != NULL)                                                     {
               transVal += wcslen(lang);
               std::wstring newValue(typeName);
               wchar_t* endPointer = wcschr( transVal, L'#');
               unsigned int sizePartOld = transVal - (wchar_t*)wValue;
               memcpy(finalValue, wValue, sizePartOld* sizeof(wchar_t));
               finalValue[sizePartOld]= L'\0';
               wcscat(finalValue, newValue.c_str());       // replacing data
               if (endPointer)
                  wcscat(finalValue, endPointer);

               finalValue[wcslen(finalValue)]= L'\0';
            } // TRanslations NOT FOUND is m_data, so the new one is included.
            else                                                                       {
               std::wstring newValue(lang);
               newValue += typeName;
               wcscpy(finalValue, (wchar_t*)wValue);  //old data
               wcscat(finalValue,newValue.c_str());   // adding new data
            }
            char* transl = TAux::WSTR_TO_STR(finalValue);
            if (transl)    {
               xmlSetProp(nodeAttrib, LBL_VALUE, transl);
               saved = true;
               delete [] transl;
            }
            delete [] wValue;
         }
      }
      if (xmlValue)
         xmlFree(xmlValue);
   } else                                                                     {

      xmlNode* newNode  = xmlNewNode ( NULL,  (xmlChar *) LBL_TATTRIB);
      xmlAddChild(node, newNode);
      xmlNewProp (newNode,(xmlChar *) LBL_NAME,    (xmlChar *)"Name");
      xmlNewProp (newNode,(xmlChar *) LBL_VALUE,   (xmlChar *)"");
      SetTypeNameTranslations(node, typeName);

   }
   if (!saved) {
//     LOG_
   }
}
//----------------------------------------------------------------------------
/**
   Set name of type into attribute tranlations "name", with the following format
   #EN=EnglishValueName#ES=SpanishValueName
   @param node type node
   @param typeName name to be include into attribute.
*/
void TXmlManager::SetTypeNameTranslations2(xmlNode* node, wchar_t* typeName)   {

   xmlNode* nodeAttrib      = xmlGetChildNode(node, "Name");
   bool saved = false;
   if (nodeAttrib)                                                            {

      char* str = TAux::WSTR_TO_STR(typeName);
      xmlSetProp( nodeAttrib, LBL_VALUE, str);
      delete [] str;

      xmlChar* xmlValue = xmlGetProp(nodeAttrib, LBL_VALUE);
   } else                                                                     {

      xmlNode* newNode  = xmlNewNode ( NULL,  (xmlChar *) LBL_TATTRIB);
      xmlAddChild(node, newNode);
      xmlNewProp (newNode,(xmlChar *) LBL_NAME,    (xmlChar *)"Name");
      xmlNewProp (newNode,(xmlChar *) LBL_VALUE,   (xmlChar *)"");
      SetTypeNameTranslations2( node, typeName);

   }
   if (!saved) {
//     LOG_
   }
}

//----------------------------------------------------------------------------
/**
   Internal Use. Check if node type_confing has nodes of type
*/
bool TXmlManager::HasTypeNodes(xmlNode* node)                                {

   if (node && node->parent &&
      (strcmp(node->name, LBL_TDATAMANAGER) == 0)  &&
      (strcmp(node->parent->name, LBL_TDATAMANAGER) == 0))
   return true;

   for (xmlNode*  cur_node = node->children; cur_node; cur_node = cur_node->next) {
      if (cur_node->type == XML_ELEMENT_NODE)      {
         if (strcmp(cur_node->name, LBL_TDATAMANAGER) == 0)   {
            return  HasTypeNodes(cur_node);
         }
         else if  (strcmp(cur_node->name, LBL_TDATA) == 0) {
            return  true;
      }
      }
   }
   return false;
}

////----------------------------------------------------------------------------
///**
//   Get node in tree configuration refered to the active type.
//   return xmlNode node containing current type.
//*/
//xmlNode*  TXmlManager::XmlGetActiveNode()                  {
//
//   char* activeName  = TAux::WSTR_TO_STR(m_activeType);
//   xmlNode * node    = NULL;
//
//   /* Create XPathExpression */
//   char* endNameXPath   ="']";
//   char xPathExpr[ MAX_XPATH_EXPR ] = "//";
//
//   strcat( xPathExpr, LBL_TYPE_CONFIG);
//   strcat( xPathExpr, "[@name='");
//   strcat( xPathExpr, activeName);
//   strcat ( xPathExpr ,endNameXPath);
////   strcat ( xPathExpr ,"/");
////   strcat ( xPathExpr ,LBL_TDATAMANAGER);
//
//   // Search XPath
//   xmlXPathObjectPtr xPathObj;
//
//   bool ret = true;
//   if ( !XmlGetNodesFromXpath( xPathExpr, xPathObj) )
//      ret = false;
//
//   if (ret) {
//   // Selected Nodes
//   xmlNodeSetPtr nodes = xPathObj->nodesetval;
//   int sizeList = ( nodes) ? nodes->nodeNr : 0;
//
//   for( int i=0;i<sizeList;i++)
//      if (nodes->nodeTab[i]->type  == XML_ELEMENT_NODE)                       {
//         node  = nodes->nodeTab[i];
//         break;
//     }
//
//   /* Cleanup */
//   xmlXPathFreeObject(xPathObj);
//
//   }
//   delete [] activeName;
//
//   return node;
//
//}

#pragma package(smart_init)





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
