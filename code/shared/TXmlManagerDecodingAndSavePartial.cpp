//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TXmlManagerDecodingAndSavePartial.cpp
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

#include "TAux.h"
#include "Base64.h"
#include "TLabels.h"
#include "TXmlManager.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)


//----------------------------------------------------------------------------
/**
   Decode stream and save in partial way.
   @param   stream
*/bool  TXmlManager::SetConfigPartial( LT::TStream* stream)                    {
      return XmlDecodingPartialType( stream);
}

//----------------------------------------------------------------------------
/**
   Decode stream and save in partial way.
*/
bool TXmlManager::XmlDecodingPartialType ( LT::TStream*  ms)                 {

   if (!ms)
      return false;

   wchar_t   buffer   [ BUFFER_SIZE],
         fullpath    [ BUFFER_SIZE],

         buffer2  [ BUFFER2_SIZE ];
   char  nombre   [ BUFFER_SIZE];

   int size;
   bool ret = true;

   // read manager attributes in next line
   bool close     = false;

   while ( ( !close) && ret)                                                  {
      // read token in next line
      if( !ms->ReadLn( buffer, BUFFER_SIZE - 1, size))
         close  =  true;

      //  ---- TDATA MANAGER ----
      if ( wcsncmp( buffer, TDataManager::strBeginSer, wcslen( TDataManager::strBeginSer )-1 )==0 )              {

         //read name, in the following line
         if ( ms->ReadLn( fullpath ,BUFFER_SIZE - 1  , size)==false)
            return false;

         char code[BUFFER_SIZE];
         if (ms->ReadLn(code, BUFFER_SIZE - 1, size) == false) {
            LOG_WARN1(L"Incorrect size code read.");
            return false;
         }

         wchar_t cpy_fullpath[BUFFER_SIZE];
         swprintf(cpy_fullpath,L"%s\0",fullpath);

         bool isCommonParent  = false;
         xmlNode* nodeCommon  = XmlGetNodeByFullPath           ( fullpath, isCommonParent, true );
         bool isTypeParent    = false;
         xmlNode* nodeType    = XmlGetNodeByFullPathTypeConfig ( fullpath, isTypeParent, true );
         xmlNode* newNode;

         if ( !nodeCommon && !nodeType )                             {
            LOG_ERROR2(L"Parent Node does not exist. Node:", fullpath);
            return false;
         }


         // -----------  Node exists in configuration --------------------
         if  ( !isCommonParent  && nodeCommon )                // Node in common configuration
            newNode     =   nodeCommon;
         else  if (  !isTypeParent    && nodeType   )          // Node in type configuration
            newNode     =  nodeType;
         else { // -----------  Node does NO exists, so node must be created ------------
               newNode  = xmlNewNode(NULL,(xmlChar *)LBL_TDATAMANAGER);
               std::list< std::wstring > tokens;
               ExtractTokens( fullpath, tokens, L"\\" );
               if (tokens.size()> 0 )  {
                     fullpath[0] = '\0';
                     swprintf(fullpath,L"%s\0", tokens.back().c_str());
               }
               xmlChar* xmlName    = ConvertInputWC(fullpath);
               xmlNewProp ( newNode, (xmlChar *)LBL_NAME, (xmlChar *) xmlName);
               xmlNewProp ( newNode, (xmlChar *)LBL_CODE, (xmlChar *) code );
               xmlFree(xmlName);

         }

         if ( ms->ReadLn( buffer2 , BUFFER2_SIZE - 1 , size) == false)
            return false;

         if (newNode)                                                         {
            bool isType = false;
            ret         = XmlDecodingFromStream( ms, isType, newNode);

            // --- Only useful in configuration types for reload configuration
            // --- Add previous sibling
            xmlNode* prevOrder      = xmlGetChildNode(newNode, LBL_ORDER);
            xmlNode* prevSibling    = xmlGetChildNode(newNode, LBL_PREVIOUS);
            xmlChar  *xmlOrder      = NULL,
                     *xmlPrevName   = NULL;
            if (prevSibling)
               xmlPrevName = xmlGetProp(prevSibling, LBL_VALUE);

            if (prevOrder)
                xmlOrder = xmlGetProp(prevOrder, LBL_VALUE);

            if (isType)                                                       {

               if (xmlPrevName)                                               {
                  xmlUnlinkNode  ( prevSibling );
                  xmlFreeNode    ( prevSibling );
                  prevSibling = NULL;

                  xmlAttrPtr prevAttr = xmlHasProp(newNode,LBL_PREVIOUS);
                  if (prevAttr)
                     xmlRemoveProp(prevAttr);

                  xmlNewProp(newNode, (xmlChar*)LBL_PREVIOUS, xmlPrevName);
                  xmlFree(xmlPrevName);
               }

               if (xmlOrder)                                                  {
                  xmlUnlinkNode  ( prevOrder );
                  xmlFreeNode    ( prevOrder );
                  prevOrder = NULL;

                  xmlAttrPtr orderAttr = xmlHasProp(newNode,LBL_ORDER);
                  if (orderAttr)
                     xmlRemoveProp(orderAttr);

                  xmlNewProp(newNode, (xmlChar*)LBL_ORDER, (xmlChar*) xmlOrder);
                  xmlFree(xmlOrder);
               }

              // -----------  Node exists in Common configuration --------------------
               if  ( nodeCommon == newNode )                                  {
                  // oldnode must be unlinked from common config.
                  xmlNode* old_parent = nodeCommon->parent;
                  xmlUnlinkNode  ( nodeCommon );
                  // and newNode must be include in activetype tree.
                  if (InsertNodeIntoActiveTypeConfig(newNode, cpy_fullpath))
                     // update rest of type configurations with node.
                     UpdateNodeInTypeConfigList    (newNode, cpy_fullpath);
                  else
                     xmlAddChild(old_parent,nodeCommon);   //restore value.
               }
               else if ( isTypeParent && nodeType )                           {
                  // -----------  Parent node exists in Active Type Configuration. New Node of Type to be included Active Type Configuration ---
                  xmlAddChild(nodeType,newNode);
               }
               else if (isCommonParent && nodeCommon)  //Last case: At Least parent node in common configurationmust exists to be inserted in active type configuration.
                  InsertNodeIntoActiveTypeConfig(newNode, cpy_fullpath);
            }
            else  /*  NO TYPE*/                                               {
                  if (!isTypeParent && nodeType)                          { // -----------  Node exists in type configuration --------------------
                  //  Checking if any parent is TypeConfig.
                  bool isTypeNode = false;
                  xmlNode* nodeToDelete = IsParentType(nodeType,isTypeNode);
                  if (! isTypeNode )                                       {
                     // newNode must be unlinked from type config and be include in common one.
                     // To this purpose, we check previous manager Name and,  if this does not exist, order is checked.
                     // Should manager order is 1, it is inserted as first child.
                     // if not previous manager name or order is found, manager is inserted at the end of the list.
                     if (nodeCommon && isCommonParent)                     {
                        xmlChar* xmlPrevName = xmlGetProp( newNode, LBL_PREVIOUS);
                        xmlNode* siblingNode = NULL;
                        if (xmlPrevName)                                   {
                           siblingNode = xmlGetChildManager(nodeCommon,(char*)xmlPrevName);
                           xmlFree(xmlPrevName);
                        }
                        int order = -1;
                        if (!siblingNode)                                  {
                           xmlChar* xmlOrder    = xmlGetProp( newNode, LBL_ORDER);
                           if (xmlOrder)                                   {
                              char* buffer = ConvertOutput(xmlOrder);
                              xmlFree(xmlOrder);
                              if ( buffer )                                {
                                 sscanf( buffer ,"%d", &order);
                                 free(buffer);
                              }
                           }
                        }

                       // -- Only used for order utilities
                        xmlAttrPtr prevAttr = xmlHasProp(newNode,LBL_PREVIOUS);
                        if (prevAttr)
                           xmlRemoveProp(prevAttr);
                        xmlAttrPtr orderAttr = xmlHasProp(newNode,LBL_ORDER);
                        if (orderAttr)
                           xmlRemoveProp(orderAttr);

                        // Inserting Node into the right order.
                        xmlNode* newNodeToCommon =  xmlCopyNode (newNode,1);
                        bool inserted = true;
                        if (siblingNode)
                           xmlAddNextSibling(siblingNode, newNodeToCommon);
                        else
                           if ((order == 1) && (nodeCommon->children))
                                 xmlAddPrevSibling(nodeCommon->children, newNodeToCommon);
                           else
                              inserted = false;

                        if (!inserted)  // No order or previous name of manager was found. So newnode is inserted at the end of the list.
                           xmlAddChild(nodeCommon,newNodeToCommon);

                        // Tenemos que insertarlo donde debe no al final. ver orden previous & order

                        if (nodeToDelete)                               {
                           xmlUnlinkNode(nodeToDelete);
                           xmlFreeNode(nodeToDelete);
                        }
                        DeleteNodeFromTypeConfigList  (cpy_fullpath);
                        //6/03/2013. En este caso hemos borrado el newNode,
                        //con lo cual el PrevSibling y PrevOrder ya estan borrados. En todo caso habra que borrarlos del nuevo nodo creado en la configuracion comun
                        prevOrder      = xmlGetChildNode(newNodeToCommon, LBL_ORDER);
                        prevSibling    = xmlGetChildNode(newNodeToCommon, LBL_PREVIOUS);

                     }
                  }

               } else if (isCommonParent && nodeCommon)                       { // Node does not exists.
                    xmlAddChild(nodeCommon,newNode);
               }
               else if ( !isCommonParent && nodeCommon) {
                     //node exsits in common configuration
                     //nothing more to do
               }
               else if ( isTypeParent && nodeType)                                                     {
                  //isType == false and its parent is a type node
                  //in this case nodeType is a pointer to its parent
                  xmlAddChild(nodeType,newNode);
               }
            }   //if/else isType

            if ( prevSibling)                                                  {
               xmlUnlinkNode  ( prevSibling );
               xmlFreeNode    ( prevSibling );
            }

            if ( prevOrder)                                                    {
               xmlUnlinkNode  ( prevOrder );
               xmlFreeNode    ( prevOrder );
            }
         }  //if newNode
   }//End if

      //  ---- TDATA ----
      if ( wcsncmp( buffer, TData::strBeginSer, wcslen( TData::strBeginSer)-1 )==0 ){

         //read name, in the following line
         ret = ret && ms->ReadLn( fullpath , BUFFER_SIZE - 1  , size);

         char code[BUFFER_SIZE];
         if (ms->ReadLn(code, BUFFER_SIZE - 1, size) == false) {
            LOG_WARN1(L"Incorrect size code read.");
            return false;
         }

        //read newLIne char
         if ( ms->ReadLn( buffer2, sizeof(buffer2), size) == false)
            return false;

         wchar_t cpy_fullpath[BUFFER_SIZE];
         swprintf(cpy_fullpath,L"%s\0",fullpath);

         std::list< std::wstring > tokens;
         ExtractTokens( fullpath, tokens, L"\\" );
         if (tokens.size()> 0 )  {
            fullpath[0] = '\0';
            swprintf(fullpath,L"%s\0", tokens.back().c_str());
         }

         xmlChar* xmlName    = ConvertInputWC( fullpath);
         xmlNode* newNode  = xmlNewNode( NULL,( xmlChar *)LBL_TDATA);
         xmlNewProp ( newNode, ( xmlChar *)LBL_NAME, ( xmlChar *) xmlName);
         xmlNewProp ( newNode, ( xmlChar *)LBL_CODE, ( xmlChar *) code);
         xmlFree(xmlName);

         ret = ret && xmlName && newNode;
         if ( ret)                                                      {
            bool isTypeData;
            ret = XmlDecodingFromStreamTData(ms,isTypeData,newNode);
            if (ret)
               ret = UpdateTypeInfoNode(newNode, cpy_fullpath, isTypeData);
                    }
      }  // TDataStr

   } //end while
   LOG_DEBUG1( L"XmlDecodingPartialType, before SaveCOnfigTo");
   if (ret)
      ret = SaveConfigTo();

   return ret;
}

//----------------------------------------------------------------------------
bool TXmlManager::UpdateTypeInfoNode(xmlNode* newNode, wchar_t* fullpath, bool isType)      {

   // nodeType and nodeCommon should not be freed "in general case".
   // Nodes should only be freed in the following cases:
   // 1.- When is replaced, the old one is released.
   // 2.- Parent of a node unlinked from configuration type, with no more children, must also be realeased.
   // NO MORE CASES, Free valid nodes will cause to be removed from configuration!!.

   bool isCommonParent  = false;
   xmlNode* nodeCommon  = XmlGetNodeByFullPath           ( fullpath, isCommonParent );
   bool isTypeParent    = false;
   xmlNode* nodeType    = XmlGetNodeByFullPathTypeConfig ( fullpath, isTypeParent );

   if ( !nodeCommon && !nodeType )                             {
      xmlFreeNode( newNode); // parent node does not exist.
      LOG_ERROR2(L"Parent Node does not exist. Node: ", fullpath );
      return false;
   }

   if (isType)                                                                {
      // -----------  Node exists in Common configuration --------------------
      if  ( !isCommonParent  && nodeCommon )                                  {              // Node in common configuration
         //4/12/2012 antes de añadir hay que comprobar que no estuviera ya en la configuración de tipo
         bool repetido = false;
         if ( !isTypeParent &&  nodeType)                                      {
            //ya esta en la de tipo. Aqui antes ha pasado algo, ya que no puede estar simultaneamente en la común y en la de tipo
            //al menos si es un TData
            if ( ( strcmp(nodeType->name, LBL_TDATA) == 0 ) &&  ( strcmp(nodeCommon->name, LBL_TDATA) == 0 ))     {
               //esto no debia haber ocurrido: hay un TData de tipo en la configuracion de tipo y en la conf. común
               repetido = true;
               LOG_WARN1( L"TData en tipo y en conf. común");
//               if ( m_mngr)                                                   {
//                  DISPATCH_EVENT( WARN_CODE, L"TData en tipo y en conf. común", NULL, m_mngr);
//               }
               //ToDo: ¿borrar de común?
            }

         }
         if ( !repetido)                                          {


            // and newNode must be include in activetype tree.
            if (InsertNodeIntoActiveTypeConfig(newNode, fullpath)) {
               // oldnode must be unlinked from common config.
               xmlUnlinkNode  ( nodeCommon );
               xmlFreeNode    ( nodeCommon);
               // update rest of type configurations with node.
               UpdateNodeInTypeConfigList    (newNode, fullpath);
            }
            else { // If something occurs when saving in active config ... save in common config.
               xmlReplaceNode ( nodeCommon , newNode);
               xmlFreeNode    ( nodeCommon);
            }
         }
      }
      else                                                        {
          // -----------  Node exists in Active Type Configuration -----------
         if (  !isTypeParent    && nodeType   )                   {
               xmlReplaceNode ( nodeType , newNode);
               xmlFreeNode    ( nodeType);
         }
         else  if ( isTypeParent && nodeType )                    {
                  // -----------  New Node of Type to be included Active Type Configuration ---
                  xmlAddChild(nodeType,newNode);
         }
         else if (isCommonParent && nodeCommon)                   {
               if (!InsertNodeIntoActiveTypeConfig(newNode, fullpath))       {
                  LOG_WARN1(L"Node Not saved!!!.");
               }
         }
      }
   }
   else                                                           {
      if  ( !isCommonParent  && nodeCommon )                      {        // -----------  Node exists in Common configuration --------------------
         xmlReplaceNode ( nodeCommon , newNode);
         xmlFreeNode    ( nodeCommon);
      }
      else  if (!isTypeParent && nodeType)                        {        // -----------  Node exists in type configuration --------------------
            //  Checking if any parent is TypeConfig.
            bool isTypeNode = false;
            xmlNode* nodeToDelete = IsParentType(nodeType,isTypeNode);
            if ( isTypeNode )                                     {
               // aunque el nodo nos llega sin flag de tipo, el nodo ha sido encontrado en la rama de tipo.
               //sin embargo este nodo es exlusivamente de tipo, es decir, no es una simple ruta creada para poder colgar
               //Tdatas de tipo (replica de un manager existente en la configuracion común),
               //sino que, buscando hacia arriba, encontramos algun manager que tiene las características propias
               //de un manager marcado como tipo ( IsParentType ha comprobado que el nodo tiene el code del manager, cosa que no se
               //guarda en los manager que son una simple réplica)
               //Por tanto, no podemos cambiarlo a la rama común, y simplemente lo actualizamos
               xmlReplaceNode ( nodeType , newNode);
               xmlFreeNode    ( nodeType);
            } else                                                {
               // newNode must be unlinked from type config and be include in common one.
               if (nodeCommon && isCommonParent)                  {
                   // 5/12/2012: el que tenemos que desenganchar  es el nodeType , no el newNode
                  //xmlUnlinkNode  ( newNode );
                  xmlUnlinkNode( nodeType);
                  xmlAddChild(nodeCommon,newNode);
                  if (nodeToDelete)                               {
                     xmlUnlinkNode(nodeToDelete);
                     xmlFreeNode(nodeToDelete);
                  }
                  DeleteNodeFromTypeConfigList  (fullpath);
               }
            }
      } else if (nodeCommon && isCommonParent)                       {        // -----------  Node does not exist --------------------
               xmlAddChild(nodeCommon,newNode);
      }
   }

   return true;
}

//----------------------------------------------------------------------------
xmlNode* TXmlManager::xmlGetChildManager( xmlNode* node, char* name)             {

   xmlNode *cur_node;
   if (!name)
      return NULL;

   for (cur_node = node->children; cur_node; cur_node = cur_node->next) {
      if (cur_node->type == XML_ELEMENT_NODE && (strcmp(cur_node->name, LBL_TDATAMANAGER) == 0)) {
         xmlChar* attrname = xmlGetProp(cur_node, LBL_NAME);
         if ((attrname) && stricmp(name, attrname) == 0) {
            xmlFree(attrname);
            return cur_node;
         }
         xmlFree(attrname);
      }
   }

   return NULL;
}

//----------------------------------------------------------------------------
xmlNode* TXmlManager::xmlGetChilPrevManager( xmlNode* node, int order)             {

   xmlNode *cur_node;

   if (order <= 0 )
     return NULL;

   int count = 0;
   for (cur_node = node->children; cur_node; cur_node = cur_node->next) {
      if (cur_node->type == XML_ELEMENT_NODE && (strcmp(cur_node->name, LBL_TDATAMANAGER) == 0)) {
         count++;
         if ((order == 1 && count == 1))
           return cur_node;

         if (count == order - 1 )
           return cur_node;
         else
            if (count >=  order)
              break;
      }
   }

   return NULL;
}


//----------------------------------------------------------------------------
/**
   Get  xmlNode/s from path
   @param   fullPath  FullPath of the param to be updated.
   @param   manager  if true, we search for a manager, if false for TDATA.
   @return  isParent  False if node has been found, true if node could not be found. Parent, when exists is returned.
   @return  xmlNode   Node to be updated or parent if node does not exists.
*/
xmlNode*  TXmlManager::XmlGetNodeByFullPath(wchar_t * path, bool & isParent, bool manager)  {

   xmlNode* node  = NULL;
   isParent       = false;
   bool ret       = true;
   char nameModule[64];

   char *fullpath = TAux::WSTR_TO_STR(path);

   if ( !fullpath )
         return NULL;

   // Xpath is built
//   char* startNameXPath = "/*[@name='";
//   char* endNameXPath   ="']";

   std::list< std::string > tokens;
   ExtractTokens( fullpath, tokens, "\\" );

   if (tokens.size() == 0 )
      ret = false;

   if (ret) {
      // fullPath will contain name of Tdata/param.
      fullpath[0] = '\0';
      sprintf(fullpath,"%s", tokens.back().c_str());
      sprintf(nameModule,"%s", tokens.front().c_str());
      tokens.pop_front(); // we take out name of module

      bool found        = false;
      int count_parent  = 0;

      while (! found )                                                           {

         if (count_parent > 1)                                                   {
            node = NULL;
            found = false;
            break;
         }
         count_parent++;
         char xPathExpr    [MAX_XPATH_EXPR]  = "//modules/*[@name='";
         // XPath Expression is built
         strcat ( xPathExpr ,nameModule);
         strcat ( xPathExpr ,endNameXPath);

         std::list<std::string>::iterator  i;
         for( i =  tokens.begin(); i != tokens.end(); i++)                       {
            strcat ( xPathExpr, startNameXPath);
            strcat ( xPathExpr, (*i).c_str());
            strcat ( xPathExpr, endNameXPath);
         }

         tokens.pop_back();

         // Search XPath
         xmlXPathObjectPtr xPathObj;

         if ( XmlGetNodesFromXpath( xPathExpr, xPathObj))                        {

            // Selected Nodes
            xmlNodeSetPtr nodes = xPathObj->nodesetval;
            int sizeList = (nodes) ? nodes->nodeNr : 0;
            //4/12/2012   puede que el nodo ya este replicado ( aunque no debería)
//            if ( sizeList == 1)                                                  {
            if ( sizeList >= 1)                                                  {
               if (nodes->nodeTab[0]->type  == XML_ELEMENT_NODE)                 {
                  node = nodes->nodeTab[0];
                  found = true;
                  // This code is only for insert TDataManagers with the same
                  // name as a TData (Both with the same parent).
                  if ( manager) {
                     if ( strcmp( node->name, LBL_TDATA) == 0)             {
                        node = node->parent;
                        count_parent = 2;
                     }
                  }
               }
            }
            if ( sizeList > 1)                                                   {
               LOG_WARN2( L"REPLICATED XML NODE", path);
//               if ( m_mngr)
//                  DISPATCH_EVENT( WARN_CODE, L"EV_REPLICATED_XML_NODE", path, m_mngr);
            }


            /* Cleanup */
            xmlXPathFreeObject(xPathObj);
         }
      }  // end while

      isParent =  (found && count_parent > 1 );
   }
   if (fullpath)
      delete [] fullpath;

   return node;
}

//----------------------------------------------------------------------------
/**
   Return node/s from path contained in active type.
   @param   fullPath  FullPath of the param to be updated.
   @return  isParent  False if node has been found, true if node could not be found. Parent, when exists is returned.
   @return  xmlNode   Node to be updated or parent if node does not exists.
*/
xmlNode*  TXmlManager::XmlGetNodeByFullPathTypeConfig(wchar_t * path, bool & isParent, bool manager)  {

   xmlNode* node  = NULL;
   isParent       = false;
   bool ret       = true;
   char nameModule[64];

   char *fullpath = TAux::WSTR_TO_STR(path);

   if ( !fullpath )
         return NULL;

   // Xpath is built
//   char* startNameXPath = "/*[@name='";
//   char* endNameXPath   ="']";

   std::list< std::string > tokens;
   ExtractTokens( fullpath, tokens, "\\" );

   if (tokens.size() == 0 )
      ret = false;

   if (ret )         {

      // fullPath will contain name of Tdata/param.
      fullpath[0] = '\0';
      sprintf(fullpath,"%s", tokens.back().c_str());
      sprintf(nameModule,"%s", tokens.front().c_str());
      tokens.pop_front(); // we take out name of module

      bool found        = false;
      int count_parent  = 0;

      while (! found )                                                        {

      if (count_parent > 1)                                                   {
         node = NULL;
         found = false;
         break;
      }
      count_parent++;
      /* Create XPathExpression */
      char xPathExpr[ MAX_XPATH_EXPR ] = "//";
      strcat( xPathExpr, LBL_TYPE_CONFIG);
      strcat( xPathExpr, "[@name='");
      char* activetype = TAux::WSTR_TO_STR(m_activeType);
      strcat( xPathExpr,activetype );
      delete [] activetype;

      strcat ( xPathExpr ,endNameXPath);
      strcat ( xPathExpr ,startNameXPath);
      strcat ( xPathExpr ,nameModule);
      strcat ( xPathExpr ,endNameXPath);

      std::list<std::string>::iterator  i;
      for( i =  tokens.begin(); i != tokens.end(); i++)                       {
         strcat ( xPathExpr, startNameXPath);
         strcat ( xPathExpr, (*i).c_str());
         strcat ( xPathExpr, endNameXPath);
      }

      tokens.pop_back();

      // Search XPath
      xmlXPathObjectPtr xPathObj;

      if ( XmlGetNodesFromXpath( xPathExpr, xPathObj))                        {

         // Selected Nodes
         xmlNodeSetPtr nodes = xPathObj->nodesetval;
         int sizeList = (nodes) ? nodes->nodeNr : 0;
            // 4/12/2012   puede que el nodo ya este replicado ( aunque no debería)
//         if ( sizeList == 1)                                                  {
         if ( sizeList >= 1)                                                  {
            if (nodes->nodeTab[0]->type  == XML_ELEMENT_NODE)                 {
               node = nodes->nodeTab[0];
               found = true;
                  // This code is only for insert TDataManagers with the same
                  // name as a TData (Both with the same parent).
                  if ( manager) {
                     if ( strcmp( node->name, LBL_TDATA) == 0)             {
                        node = node->parent;
                        count_parent = 2;
                     }
                  }
            }
         }
         if ( sizeList > 1)                                                   {
            LOG_WARN1( L"REPLICATED XML NODE");
//            if ( m_mngr)
//               DISPATCH_EVENT( WARN_CODE, L"EV_REPLICATED_XML_NODE", NULL, m_mngr);
         }

         /* Cleanup */
         xmlXPathFreeObject(xPathObj);
      }
      }  // end while

      isParent = (found && count_parent > 1 );
   }

   if (fullpath)
      delete [] fullpath;

   return node;
}


//xmlNode* TXmlManager::IsParentType(xmlNode* node, bool &isTypeNode)   {
//
//   xmlNode* nodeToDelete = NULL;
//   isTypeNode = false;
//   xmlNode* parent = node; //->parent;
//   while ( parent && parent->parent && stricmp( parent->parent->name, LBL_TDATAMANAGER ) == 0)         {
//      xmlChar* code = xmlGetProp(parent, LBL_CODE);
//      if ( code ) {
//         isTypeNode     = true;
//         nodeToDelete   = NULL;
//         break;
//      }
//      else {
//         nodeToDelete = parent;
//         parent = parent->parent;
//      }
//      xmlFree(code);
//   }
//   return nodeToDelete;
//
//}

/**
   A node exists in type configuration, we have to decide if remove it from type_config
   or not. To this purpose, we check if some parent has nodes of type.
   If no node is found, node is set to be deleted.
   @param
   @param isTypeNode
*/
xmlNode* TXmlManager::IsParentType(xmlNode* node, bool &isTypeNode)   {

   xmlNode* nodeToDelete   = node;
   isTypeNode              = false;
   xmlNode* parent         = node->parent;
   while ( parent && stricmp( parent->name, LBL_TDATAMANAGER ) == 0)         {
      xmlChar* code = xmlGetProp(parent, LBL_CODE);
      if ( code ) {
         isTypeNode     = true;
         nodeToDelete   = NULL;
         xmlFree(code);
         break;
      }
      else                                                                   {
         xmlNode *cur_node;
         int count = 0;
         for (cur_node = parent->children; cur_node; cur_node = cur_node->next)  {
            if ( cur_node->type == XML_ELEMENT_NODE &&
               ((strcmp(cur_node->name, LBL_TDATAMANAGER) == 0) || (strcmp(cur_node->name, LBL_TDATA) == 0)) )  {
                  count++;
                  if (count == 2)   // There are at lease 2 branches, only remove current branche.
                     return nodeToDelete;
               }
         }
      }
      nodeToDelete = parent;
      parent = parent->parent;
   } // WHILE

   return nodeToDelete;
}

//----------------------------------------------------------------------------
/**
   Delete node from path contained in active type.
*/
bool      TXmlManager::DeleteNodeFromTypeConfigList     (wchar_t* path)   {

   char nameModule[64];
   bool ret = true;
   char *fullpath = TAux::WSTR_TO_STR(path);

   if ( !fullpath )
         return NULL;


   // Xpath is built
//   char* startNameXPath = "/*[@name='";
//   char* endNameXPath   ="']";

   std::list< std::string > tokens;
   ExtractTokens( fullpath, tokens, "\\" );

   if (tokens.size() == 0 )
      ret = false;

   if (ret ) {

   // fullPath will contain name of Tdata/param.
   fullpath[0] = '\0';
   sprintf(fullpath,"%s", tokens.back().c_str());
   sprintf(nameModule,"%s", tokens.front().c_str());
   tokens.pop_front(); // we take out name of module

         /* Create XPathExpression */
      char xPathExpr[ MAX_XPATH_EXPR ] = "//";
      strcat( xPathExpr, LBL_TYPE_CONFIG);
      strcat( xPathExpr, "/");
      strcat ( xPathExpr ,startNameXPath);
      strcat ( xPathExpr ,nameModule);
      strcat ( xPathExpr ,endNameXPath);

      std::list<std::string>::iterator  i;
      for( i =  tokens.begin(); i != tokens.end(); i++)                       {
         strcat ( xPathExpr, startNameXPath);
         strcat ( xPathExpr, (*i).c_str());
         strcat ( xPathExpr, endNameXPath);
      }

      tokens.pop_back();

      // Search XPath
      xmlXPathObjectPtr xPathObj;

      if ( XmlGetNodesFromXpath( xPathExpr, xPathObj))         {
         // Selected Nodes
         xmlNodeSetPtr nodes = xPathObj->nodesetval;

         int sizeList = ( nodes) ? nodes->nodeNr : 0;
         for( int i=0;i<sizeList;i++)                          {
            if (nodes->nodeTab[i]->type  == XML_ELEMENT_NODE)  {
               xmlNode* node  = nodes->nodeTab[i];
               bool isTypeNode = false;
               xmlNode* nodeToDelete = IsParentType(node,isTypeNode);
               if (!isTypeNode)       {
                  xmlUnlinkNode(node);
                  xmlFreeNode(node);
                  if (nodeToDelete && (nodeToDelete != node)) {
                     xmlUnlinkNode(nodeToDelete);
                     xmlFreeNode(nodeToDelete);

                  }
               }
            }
         }
         /* Cleanup */
         xmlXPathFreeObject(xPathObj);
      }
   }
   if (fullpath)
      delete [] fullpath;
   return ret;

}

//----------------------------------------------------------------------------
/**
   Update node in active type config
*/
bool TXmlManager::UpdateNodeInTypeConfigList(xmlNode * oldNode, wchar_t* fullpath) {


   bool ret = true;

   /* Create XPathExpression */
   char xPathExpr[ MAX_XPATH_EXPR ] = "//";
   strcat(xPathExpr,LBL_TYPE_CONFIG);

   // Search XPath
   xmlXPathObjectPtr xPathObj;

   if ( !XmlGetNodesFromXpath( xPathExpr, xPathObj) )
      return false;

   // Selected Nodes
   xmlNodeSetPtr nodes = xPathObj->nodesetval;
   int sizeList = ( nodes) ? nodes->nodeNr : 0;

   for( int i=0;i<sizeList;i++)                                               {
      if (nodes->nodeTab[i]->type  == XML_ELEMENT_NODE)                       {
         xmlNode* node  = nodes->nodeTab[i];
         xmlChar* xmlName = xmlGetProp(node,LBL_NAME );
         wchar_t* value = ConvertOutputWC(xmlName);
         if (value ) {
            if   (_wcsicmp (value ,m_activeType ) != 0)     {
               xmlNode* cpyNode = xmlCopyNode (oldNode, 1);
               InsertNodeIntoActiveTypeConfig(cpyNode, fullpath, value);
            }
            delete [] value;
         }
         xmlFree(xmlName);
      }
   }

   /* Cleanup */
   xmlXPathFreeObject(xPathObj);

   return ret;
}


//----------------------------------------------------------------------------
/**
   Insert node into typeName configuration.
*/
bool TXmlManager::InsertNodeIntoActiveTypeConfig(xmlNode * newNode, wchar_t* path, wchar_t* typeName) {


   xmlNode* node  = NULL;
   char nameModule[64];
   bool isManager = (stricmp( newNode->name, LBL_TDATAMANAGER)==0);
   bool ret  = true;
   // Xpath is built
//   char* startNameXPath = "/*[@name='";
//   char* endNameXPath   ="']";

   char *fullpath = TAux::WSTR_TO_STR(path);

   char fullpath2 [500];
   sprintf(fullpath2,"%s\0",fullpath);
   std::list< std::string > tokens;
   ExtractTokens( fullpath2, tokens, "\\" );

   if (tokens.size() >  0 )               {

      // fullPath will contain name of Tdata/param.
      fullpath2[0] = '\0';
      sprintf(fullpath2,"%s", tokens.back().c_str());
      sprintf(nameModule,"%s", tokens.front().c_str());
      tokens.pop_front(); // we take out name of module

      /* Create XPathExpression */
      char xPathExpr[ MAX_XPATH_EXPR ] = "//";
      strcat( xPathExpr, LBL_TYPE_CONFIG);
      strcat( xPathExpr, "[@name='");
      if (typeName) {
         char* tName = TAux::WSTR_TO_STR(typeName);
         strcat( xPathExpr, tName);
         delete [] tName;
      }
      else {
         char* tName = TAux::WSTR_TO_STR(m_activeType);
         strcat( xPathExpr,tName );
         delete [] tName;
      }

      strcat ( xPathExpr ,endNameXPath);
      strcat ( xPathExpr ,startNameXPath);
      strcat ( xPathExpr ,nameModule);
      strcat ( xPathExpr ,endNameXPath);

      bool found        = false;

      // Search XPath
      xmlXPathObjectPtr xPathObj;

      if ( XmlGetNodesFromXpath( xPathExpr, xPathObj))                        {

         // Selected Nodes
         xmlNodeSetPtr nodes = xPathObj->nodesetval;
         int sizeList = (nodes) ? nodes->nodeNr : 0;
         if ( sizeList == 1)                                                  {
            if (nodes->nodeTab[0]->type  == XML_ELEMENT_NODE)                 {
               node = nodes->nodeTab[0];
               found = true;
            }
         }
         /* Cleanup */
         xmlXPathFreeObject(xPathObj);
      }

      if (!node)     {
         // 07/01/2013 buscar el nodo raiz en la configuracion comun y copiarlo a la configuracion de tipo
         //LOG_WARN1(L"Active Type Config Not found");
         //ret = false;

         strcpy( xPathExpr ,"//");
         strcat( xPathExpr, LBL_MODULES_GROUP_NAME);
         strcat ( xPathExpr ,startNameXPath);
         strcat ( xPathExpr ,nameModule);
         strcat ( xPathExpr ,endNameXPath);


         if ( XmlGetNodesFromXpath( xPathExpr, xPathObj))                        {


            xmlNodeSetPtr nodes = xPathObj->nodesetval;
            int sizeList = (nodes) ? nodes->nodeNr : 0;
            if ( sizeList == 1)                                                  {
               if (nodes->nodeTab[0]->type  == XML_ELEMENT_NODE)                 {
                  node = nodes->nodeTab[0];
               }
            }
            /* Cleanup */
            xmlXPathFreeObject(xPathObj);
            //añadir el nodo al no de tipo
            //buscamos el nodo de tipo

            strcpy( xPathExpr ,"//");
            strcat( xPathExpr, LBL_TYPE_CONFIG);
            strcat( xPathExpr, "[@name='");
            if (typeName) {
               char* tName = TAux::WSTR_TO_STR(typeName);
               strcat( xPathExpr, tName);
               delete [] tName;
            }
            else {
               char* tName = TAux::WSTR_TO_STR(m_activeType);
               strcat( xPathExpr,tName );
               delete [] tName;
            }
            strcat ( xPathExpr ,endNameXPath);

            if ( XmlGetNodesFromXpath( xPathExpr, xPathObj))                   {
               xmlNodeSetPtr nodes = xPathObj->nodesetval;
               int sizeList = (nodes) ? nodes->nodeNr : 0;
               if ( sizeList == 1)                                                  {
                  if (nodes->nodeTab[0]->type  == XML_ELEMENT_NODE)                 {
                     xmlNode *configNode = nodes->nodeTab[0];
                     //hacer una copia del nodo anteriormente encontrado
                     xmlNode *newNode  = xmlNewNode(NULL,(xmlChar *)LBL_TDATAMANAGER);
                     xmlNewProp ( newNode, (xmlChar *)LBL_NAME, (xmlChar *) nameModule);
                     //añadirlo al node configNode
                     xmlAddChild( configNode, newNode);
                     node = newNode; //node apunta ahora al nodo nuevo, añadido a la configuracion de tipo
                     found = true;
                  }
               }
            }
            else                                                              {
               LOG_WARN1(L"Active Type Config Not found");
               ret = false;
            }
         }
         else                                                              {
            LOG_WARN1(L"Active Type Config Not found");
            ret = false;
         }


   }
   if (ret && found && node)                                   {
      xmlNode *cur_node = node->children;
      while ((cur_node != NULL) &&  (tokens.size() != 0))      {
         if ( cur_node->type == XML_ELEMENT_NODE  &&  ( strcmp(cur_node->name, LBL_TDATAMANAGER) == 0)  ) {
            xmlChar* name = xmlGetProp ( cur_node, LBL_NAME);
            if ( ( name ) && stricmp( name, tokens.front().c_str()) == 0 )                   {
               tokens.pop_front();
               node     = cur_node;
               cur_node = cur_node->children;
            }
            else
               cur_node = cur_node->next;
            xmlFree(name);

         }
         else
          cur_node =  cur_node->next;
      }

      xmlNode * newNode2;
      // 6/02/2013
      //si estamos añadiendo un manager, y hemos encontrado toda la ruta en la configuracion de tipo, eso quiere decir que ya existía
      //seguramente porque algun TData contenido en él estaba marcado con el flag de tipo
      //por tanto no lo debemos añadir, sino reemplazr

      if ( ( isManager && tokens.empty() ) )                                  {

         //hacer una copia del nodo a agregar, porque ñe vamos a añadir nodos ya existentes antes de reemplazar
         xmlNode *newNodeExtended = xmlCopyNode( newNode, 1); //copia recursiva
         //añadimos los nodos que ya existian en la configuracion de tipo
         for ( cur_node = node->children; cur_node; cur_node = cur_node->next) {
            if ( cur_node->type == XML_ELEMENT_NODE )                          {

               xmlChar* xmlName = xmlGetProp( cur_node, LBL_NAME);
               wchar_t* name = ConvertOutputWC( xmlName);
               xmlFree(xmlName);

               if ( name != NULL)                                             {

                  //solamente queremos copiar los hijos de tipo TData o Manager, no los atributos del manager

                  if ( (strcmp( cur_node->name, LBL_TDATAMANAGER ) == 0) ||  (strcmp( cur_node->name, LBL_TDATA ) == 0))     {
                     //copiar nodo hijo de node a newNode
                     //añadir copia a la lista
                     xmlAddChild( newNodeExtended, xmlCopyNode( cur_node, 1));
                  }
               }
            }
         }

         //reemplazar newNode
         xmlNode *oriNode = xmlReplaceNode( node, newNodeExtended); //reemplazamos y nos quedamos con copia
         xmlUnlinkNode( oriNode);
         xmlFreeNode( oriNode);
         //ToDo: aunque hemos hecho una copia de newNode para añadir nodos, no podemos borrr el original, porque a veces se vuelve a utilizar
         //en el bucle de actualizacion de configuraciones
      }
      else                                                                    {
         tokens.pop_back(); //we take out, name of tdamanager. Dont needed.
         if (tokens.size() > 0) {
            std::list<std::string>::iterator  i;
            newNode2 = node;
            for( i =  tokens.begin(); i != tokens.end(); i++)                 {
               xmlNode* tmpNode = xmlNewNode(NULL,(xmlChar *)LBL_TDATAMANAGER);
               xmlNewProp ( tmpNode, (xmlChar *)LBL_NAME, (xmlChar *) (*i).c_str());
               xmlAddChild(newNode2,tmpNode);
               newNode2 = tmpNode;
            }
            node = newNode2;
         }
         xmlAddChild(node, newNode);
      }
   }

   }
   if (fullpath)
       delete [] fullpath;

   return ret;
}

//----------------------------------------------------------------------------
xmlNode* TXmlManager::xmlGetChildNode( xmlNode* node, char* name)             {

   xmlNode *cur_node;

   for (cur_node = node->children; cur_node; cur_node = cur_node->next) {
      if (cur_node->type == XML_ELEMENT_NODE && (strcmp(cur_node->name, LBL_TATTRIB) == 0)) {
         xmlChar* attrname = xmlGetProp(cur_node, LBL_NAME);
         if ((attrname) && stricmp(name, attrname) == 0) {
            xmlFree(attrname);
            return cur_node;
         }
         xmlFree(attrname);
      }
   }

   return NULL;
}








//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------



