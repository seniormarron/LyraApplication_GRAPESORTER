//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TXmlManagerDecodingAndSave.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma hdrstop

#include "TAux.h"
#include "Base64.h"
#include "TXmlManager.h"
#include "SysUtils.hpp"
#include "TDataMng.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)
// ----------------------------------------------------------------------------
/**
   Set Configuration
   @param   stream  containing
 */
bool TXmlManager::SetConfig(LT::TStream* stream) {
   return XmlDecodingConfig(stream, m_DocPtr);
}



// ----------------------------------------------------------------------------
/**
   Decode stream and save configuration.

   @param ms stream containing configuration
   @param dataSize size of stream
   @param nameMachine name of the machine
   @param isMachineCfg flag to indicate the configuration contained in ms, belongs to nameMachine
 */
bool TXmlManager::XmlDecodingConfig(LT::TStream* ms, xmlDocPtr docPtr) {

   if (!ms)
      return false;

   bool ret;
   wchar_t buffer[BUFFER_SIZE]; // buffer to get lines from stream
   int size;

   // 1. Read the keyword TDataManager
   if (ms->ReadLn(buffer, BUFFER_SIZE - 1, size) == false) {
      LOG_WARN1(L"keyword TDataManager not read.");
      return false;
   }

   if (wcsncmp(buffer, TDataManager::strBeginSer,
       wcslen(TDataManager::strBeginSer) - 1) != 0) {
      LOG_WARN1(L"Tag TDataManager not read.");
      return false;
   }

   // 2. Check that the full name we get from stream matches our name
   if (ms->ReadLn(buffer, BUFFER_SIZE - 1, size) == false) {
      LOG_WARN1(L"Buffer containing fullname not read.");
      return false;
   }

   // Get full name
   wchar_t fullname[BUFFER_SIZE];
   wcsncpy(fullname, buffer, size);
   fullname[size] = L'\0';

   char code[BUFFER_SIZE];
   if (ms->ReadLn(code, BUFFER_SIZE - 1, size) == false) {
      LOG_WARN1(L"Incorrect size code read.");
      return false;
   }

   // read newLIne char
   if (ms->ReadLn(buffer, BUFFER_SIZE - 1, size) == false) {
      LOG_WARN1(L"New line character not read.");
      return false;

   }

   // ---------------------------------------------------------
   // set name & code
   xmlNode* node = xmlNewNode(NULL, (xmlChar*) LBL_TDATAMANAGER);
   xmlChar* xmlName = ConvertInputWC(fullname);
   xmlNewProp(node, (xmlChar *)LBL_NAME, (xmlChar *) xmlName);
   xmlNewProp(node, (xmlChar *)LBL_CODE, (xmlChar *) code);

   // ---------------------------------------------------------
   xmlNode* nodeRootTypeConfig = NULL;
   xmlNode* nodeTypeConfig = NULL;
   if (m_isType) {

      //buscar si el nodo ya existia, para copiar la traduccion

      xmlNode *activeNode =  XmlGetActiveConfigNode( m_activeType, NULL, docPtr);
      if ( activeNode )                                                       {
         nodeRootTypeConfig =  xmlCopyNode (activeNode,2);
         //buscar atribute Name, que es donde estan las traducciones
         xmlNode* nodeAttrib      = xmlGetChildNode( activeNode, "Name");
         if ( nodeAttrib)                                                     {
            xmlNode *attribCopy =  xmlCopyNode (nodeAttrib,2);
            xmlAddChild(nodeRootTypeConfig, attribCopy);
         }

      }
      else                                                                    {



         nodeRootTypeConfig = xmlNewNode(NULL, (xmlChar*) LBL_TYPE_CONFIG);
         char* activetype = TAux::WSTR_TO_STR(m_activeType);
         if (activetype) {
            xmlNewProp(nodeRootTypeConfig, (xmlChar *) LBL_NAME, (xmlChar *) activetype);
            delete[]activetype;
         }
         else
            xmlNewProp(nodeRootTypeConfig, (xmlChar *) LBL_NAME, "NewType");


      }

      nodeTypeConfig = xmlNewNode(NULL, (xmlChar*) LBL_TDATAMANAGER);
      xmlNewProp(nodeTypeConfig, (xmlChar *)LBL_NAME, (xmlChar *) xmlName);
      xmlAddChild(nodeRootTypeConfig, nodeTypeConfig);
   }

   if (xmlName)
      xmlFree(xmlName);

   bool isType;
   ret = XmlDecodingFromStream( ms, isType, node, nodeTypeConfig);
   /* When "XmlDecodingFromStream" method finishes:
    1.-  node contains module configuration with common configuration and,
    2.-  nodeConfigType contains managers and parameters of active type configuration.
    */
   if (ret)                                                             {

      // Create a new documnemt, only for parse purposes --------------
      xmlDocPtr saveDoc = xmlNewDoc(BAD_CAST "1.0");
      xmlDocSetRootElement(saveDoc, node);
      bool tmpValid = ParseConfig(saveDoc);
      if (tmpValid && node )                                         {
         xmlUnlinkNode(node);
         ret =  XmlUpdateConfig(fullname, node, nodeRootTypeConfig, docPtr);
      }
      else                                                           {
         ret = false;
         LOG_WARN3(L"Unable to save configuration from module ", fullname,L". Some nodes are not well formed. Please revise values of Attributes");
//            if ( m_mngr)
//               DISPATCH_EVENT( ERROR_CODE, L"EV_CONF_NOT_SAVED", L"Some nodes are not well formed", m_mngr);

      }
      if ( saveDoc)  {
         xmlFreeDoc( saveDoc);
      }
   }
   else {
      xmlUnlinkNode(node);
      xmlFreeNode(node);
      if (m_isType) {
         xmlUnlinkNode(nodeRootTypeConfig);
         xmlFreeNode(nodeRootTypeConfig);
      }
   }

   return ret;
};

// ----------------------------------------------------------------------------
/**
   XmlDecodingFromStream reads information from stream containing a serilizaed structure of nodes,
   and creates the structure from it, adding this structure to node or  typeConfigParentNode ( depending onthe flags of the read node)
   @param   ms is the stream where configuration is written
   @param   isType returns if attrib flags have FLAG_TYPE activated
   @param   node  is the node that is being read , in the common configuration
   @param   typeConfigParentNode is the node that is being read in the active type configuration. NULL is there is no active type configuration
   @return  true if stream could be deserialized correctly
 */
bool TXmlManager::XmlDecodingFromStream(LT::TStream *ms, bool &isType, xmlNode* node, xmlNode* typeConfigParentNode) {

   wchar_t buffer[BUFFER_SIZE], // buffer to get lines from stream
       buffer2[BUFFER2_SIZE];
   char nombre[100];

   int size;
   bool ret;

   int orderManager = 0;  // Only useful for configuration types

   // read manager attributes in next line, and get isType from flag attributes
   isType = false;
   ret = XmlDecodingFromStreamAttrib(ms, isType, node);

   // remove new line char from stream
   if (ret)
      ret = ret && ms->ReadLn(buffer, BUFFER_SIZE - 1, size);

   if (ret == false) {
      LOG_WARN1(L"New Line character NOT removed.");
      return ret;
   }

   bool close = false;
   while ((!close) && ret) {

      // read token in next line
      if (!ms->ReadLn(buffer, sizeof(buffer), size))
         return false;

      // ---- END TDATAMANAGER ----
      if (wcsncmp(buffer, TDataManager::strEndSer, wcslen(TDataManager::strEndSer) - 1) == 0)
         close = true;

      // ---- TDATAMANAGER ----
      if (wcsncmp(buffer, TDataManager::strBeginSer, wcslen(TDataManager::strBeginSer) - 1) == 0) {

         if (ms->ReadLn(buffer, BUFFER_SIZE - 1, size) == false) {
            LOG_WARN1(L"Incorrect size code read.");
            return false;
         }

         // Get full name
         wchar_t fullname[BUFFER_SIZE];
         wcsncpy(fullname, buffer, BUFFER_SIZE - 1);
         fullname[size] = L'\0';
         // r' must be take out to build xpath expression

         char code[BUFFER_SIZE];
         if (ms->ReadLn(code, BUFFER_SIZE - 1, size) == false) {
            LOG_WARN1(L"Incorrect size code read.");
            return false;
         }

         xmlChar* xmlName = ConvertInputWC(fullname);
         if (!xmlName)
            return false;

         //a child manager existes. Create it ( newNode) and add to node childs
         xmlNode* newNode = xmlNewNode(NULL, (xmlChar*) LBL_TDATAMANAGER);
         xmlNewProp(newNode, (xmlChar *) LBL_NAME, (xmlChar *) xmlName); //name
         xmlNewProp(newNode, (xmlChar *)LBL_CODE, (xmlChar *) code);     //code
         xmlAddChild(node, newNode);   //add newNode to node

         //if whe are working with an active type, create manager node and add it to  typeConfigParentNode childs
         xmlNode* newNodeTypeConfig = NULL;
         if (m_isType && typeConfigParentNode) {
            newNodeTypeConfig = xmlNewNode(NULL, (xmlChar*) LBL_TDATAMANAGER);
            xmlNewProp(newNodeTypeConfig, (xmlChar *) LBL_NAME, (xmlChar *) xmlName);
            xmlAddChild(typeConfigParentNode, newNodeTypeConfig);
         }

         xmlFree(xmlName);

         // read newLIne char
         if (ms->ReadLn(buffer, BUFFER_SIZE - 1, size) == false)
            return false;

         // 3. While we don't get the closing keyword  ( \n</TdataMngr>\n ), go on reading
         bool isType;
         // recursive call to this method, with new parameters. So it will read all childs for neNode
         ret = XmlDecodingFromStream(ms, isType, newNode, newNodeTypeConfig);

         if (m_isType && newNodeTypeConfig && typeConfigParentNode) {
            if (isType) {
               //we had read all newNode ( attributes and childs) and added it to node, but, as this node has FLAG_TYPE activated, it must be
               //changed form common node to type node ( unlinked from common node and linked to type node)
               // Unlink node from type config, in this case, we have to save into the tree not only tag_label but all node.
               xmlUnlinkNode(newNodeTypeConfig);
               xmlFreeNode(newNodeTypeConfig);
               newNodeTypeConfig = NULL;

               // we unlik node from common configuration. Later we will add it to type configuration
               xmlUnlinkNode(newNode);

               // --- Only useful in configation types for reload configuration
               // --- Add previous sibling
               xmlNode* previous_sibling =  node->last;
               if (previous_sibling) {
                  xmlChar* xmlPrevName = xmlGetProp(previous_sibling, LBL_NAME);
                  if (xmlPrevName) {
                     xmlNewProp(newNode, (xmlChar*)LBL_PREVIOUS, xmlPrevName);
                     xmlFree(xmlPrevName);
                  }
               }

               // --- Add order in tree
               char order[8];
               itoa((orderManager + 1), order, 10);
               xmlNewProp(newNode, (xmlChar*)LBL_ORDER, (xmlChar*) order);
               // ---

               xmlAddChild(typeConfigParentNode, newNode);
            }

            // if at least one node child is necessary to be include in type_config tree
            // in any other case node is unlinked.
            if (newNodeTypeConfig) {
               bool found = false;
               xmlNode *nodeFound = newNodeTypeConfig->children;
               for (nodeFound = nodeFound; nodeFound;
               nodeFound = newNodeTypeConfig->next) {
                  if (nodeFound->type == XML_ELEMENT_NODE) {
                     found = true;
                     break;
                  }
               }
               if (!found)
               { // Nodes that are not in type_config are delete from tree.
                  xmlUnlinkNode(newNodeTypeConfig);
                  xmlFreeNode(newNodeTypeConfig);
                  newNodeTypeConfig = NULL;
               }
            }
         }
         orderManager++;
      } // end if TDataManger

      // ---- TDATA ----
      if (wcsncmp(buffer, TData::strBeginSer, wcslen(TData::strBeginSer) - 1) == 0) {

         // read name, in the following line
         ret = ret && ms->ReadLn(buffer, BUFFER_SIZE - 1, size);
         // read code in the following line

         char code[BUFFER_SIZE];
         if (ms->ReadLn(code, BUFFER_SIZE - 1, size) == false) {
            LOG_WARN1(L"Incorrect size code read.");
            return false;
         }

         xmlChar* _name = ConvertInputWC(buffer);
         if (!_name) {
            return false;
         }
         //add node to common configuration
         xmlNode* newNode = xmlNewNode(NULL, (xmlChar*)LBL_TDATA);
         xmlNewProp(newNode, (xmlChar *)LBL_NAME, (xmlChar *) _name);
         xmlFree(_name);
         xmlNewProp(newNode, (xmlChar *)LBL_CODE, (xmlChar *) code);
         xmlAddChild(node, newNode);

         // read newLIne char
         if (ms->ReadLn(buffer2, BUFFER2_SIZE - 1, size) == false)
            return false;
         bool isType;
         if (ret)
            ret = XmlDecodingFromStreamTData(ms, isType, newNode);

         if (m_isType && isType && typeConfigParentNode) {
            //unlink node from common configuration and add it to type configuration
            xmlUnlinkNode(newNode);
            xmlAddChild(typeConfigParentNode, newNode);
         }
      } // end if TData

   } // end While

   return ret;
}

// ----------------------------------------------------------------------------
/**
 Read a TData from TStream

   @param   stream where serialized tdata  is writtem
   @param   isType returns if FLAG_TYPE in flags is activated
   @param   node is the node where we are reading from the stream
   @return  true if stream could be deserialized correctly

 */
bool TXmlManager::XmlDecodingFromStreamTData(LT::TStream *stream, bool &isType, xmlNode* node) {

   int size = 0;

   // Read size
   if (stream->Size() - stream->posRead() >= (int)sizeof(size)) {
      stream->Read((char *)&size, sizeof(size));
   }
   else
      return false;

   // check size
   if ((stream->Size() <= size) || (size < (int) sizeof(sdata)))
      return false;

   // read type
   e_sdata type;
   char _type[16];
   stream->Read((char *)&type, sizeof(type));
   sprintf(_type, "%d", type);
   xmlNewProp(node, LBL_TYPE, _type);

   // set attributes
   bool ret = XmlDecodingFromStreamAttrib(stream, isType, node);

   return ret;
}

// ----------------------------------------------------------------------------
/**
   Deserialize a stream as attribute and converts it into a node.
   @param   stream
   @param   isType returns if FLAG_TYPE in flags is activated
   @param   node  output node.
   @return  true if stream could be deserialized correctly
 */
bool TXmlManager::XmlDecodingFromStreamAttrib(LT::TStream *stream,
    bool &isType, xmlNode* node) {

   bool ret = true;
   int pos = stream->posRead(); // save read position
   long int size = 0;
   isType = false;

   // Check size
   char *pRead;
   if ((pRead = stream->GetSafePtr(pos, sizeof(sattribmngr) - 1)) != NULL) {
      size = ((sattribmngr*)(pRead))->m_sizeMngr;

      // be sure that the pointer is valid in the whole range
      if ((pRead = stream->GetSafePtr(pos, size)) == NULL)
         return false;

      char *pEnd = pRead + size; // point to end of sequence of s_attrib's
      pRead += sizeof(size); // point to first s_attrib

      while (ret && pRead < pEnd) {

         sattrib *pAttr = (sattrib*)pRead;
         if (pRead + pAttr->m_size <= pEnd) {
            // bool     modAttr;
            // ---
            if (wcscmp((*pAttr).m_name, cAttribNames[ATTR_FLAGS]) == 0) {
               TAttrib *attr = new TAttrib(*pAttr);
               if (attr) {
                  int val;
                  attr->Get(val);
                  if (val & FLAG_TYPE_CONFIG)
                     isType = true;
               }
               if (attr)
                  delete attr;
            }
            // --

            XmlDecodingFromStreamNewAttrib(*pAttr, node);
            pRead += pAttr->m_size; // move pointer to next attribute
         }
         else {
            ret = false;
            break;
         }
      }
   }
   else
      ret = false;

   // Move read pointer
   if (ret)
      stream->SeekRead(pos + size);

   return ret;
}

// ----------------------------------------------------------------------------
/**
  Deserialize a stream as attribute and converts it into a node.
      @param   _attr is an structure containing attribute information
      @param   parentNode is the node to which childs the new created node will be added
      @return  true if node could be created from structure
 */
bool TXmlManager::XmlDecodingFromStreamNewAttrib(const sattrib &_attr, xmlNode* parentNode) {

   wchar_t name[ATTR_TAM_NAME];

   swprintf(name, L"%s\0", _attr.m_name);
   e_sattrib type = _attr.m_type;
   unsigned int m_dataSize = _attr.DataSize();
   char _type[16];
   sprintf(_type, "%d", type);

//   if (wcscmp(name, L"Flags") == 0)  // This is only to remove flags when flags are changed in defineflags.h file.
//      return true;

   TAttrib * newAttrib = new TAttrib(_attr);

   xmlNode* newNode = xmlNewNode(NULL, LBL_TATTRIB);
   xmlChar* xmlName = ConvertInputWC(name);
   if (!xmlName) {
      return false;  //ToDo liberar memoria newNode
   }
   xmlNewProp(newNode, LBL_NAME, (xmlChar*) xmlName);
   xmlNewProp(newNode, LBL_TYPE, _type);

   switch (type) {
   case TYPE_BIN: {
         char* valBin = (char *)malloc(m_dataSize);
         newAttrib->Get((void*)valBin, m_dataSize);
         std::string buffer =
             base64_encode(reinterpret_cast< const unsigned char*>(valBin),
             m_dataSize);
         if (valBin)
            free(valBin);

         xmlChar* newValBin = ConvertInput(buffer.c_str());
         xmlNewProp(newNode, LBL_VALUE, newValBin);
         buffer.clear();
         xmlFree(newValBin);

      } break;
   default: {
         if (type == TYPE_WCHAR) {
            wchar_t* val = new wchar_t[(m_dataSize+1) / 2 + 1];
            if (val) {

               newAttrib->Get((void*)val, m_dataSize);
               xmlChar* xmlVal = ConvertInputWC(val);
               if (xmlVal) {
                  xmlNewProp(newNode, LBL_VALUE, BAD_CAST xmlVal);
                  xmlFree(xmlVal);
               }
               delete [] val;
            }
         }
         else {
            wchar_t val[250];
            unsigned int size = sizeof(val);
            newAttrib->Get(val, size);
            xmlChar* xmlVal = ConvertInputWC(val);
            if (xmlVal) {
               xmlNewProp(newNode, LBL_VALUE, BAD_CAST xmlVal);
               xmlFree(xmlVal);
            }
         }
      } break;
   }

   XmlDeleteAttribByName(parentNode, xmlName);
   xmlAddChild(parentNode, newNode);

   if (xmlName) {
      xmlFree(xmlName);
   }
   if (newAttrib)
      delete newAttrib;

   return true;
}

// -----------------------------------------------------------------------------
/**
 Search for an attribute named "name" in the children list of a node. If it is
 found, node is removed.

 @param node is the node where to look the children
 @param name is the required name
 */
void TXmlManager::XmlDeleteAttribByName(xmlNode* node, xmlChar* name) {

   if (!node || !name)
      return;

   xmlNode *cur_node;

   for (cur_node = node->children; cur_node; cur_node = cur_node->next) {
      if (cur_node->type == XML_ELEMENT_NODE && (strcmp(cur_node->name,
          LBL_TATTRIB) == 0)) {
         xmlChar* oldName = xmlGetProp(cur_node, LBL_NAME);
         if ((oldName) && stricmp(name, oldName) == 0) {
            xmlUnlinkNode(cur_node);
            xmlFreeNode(cur_node);   //ToDo liberar memoria
            break;
         }
         xmlFree(oldName);
      }
   }
}

// ----------------------------------------------------------------------------
/**
   Update configuration
   @param   path
   @param   node
   @param   typeConfigNode
 */
bool TXmlManager::XmlUpdateConfig(wchar_t* path1, xmlNode* node, xmlNode* typeConfigNode, xmlDocPtr docPtr) {

if ( docPtr ==NULL)
   docPtr = m_DocPtr;

//si dejamos que el try/catch los camufle, podra llegar al fichero de configuracion y al .bak
#ifndef _DEBUG

try                                                                           {

try                                                                           {
#endif
   if (node) {

      /* Create XPathExpression */
      char* path = TAux::WSTR_TO_STR(path1);
      if (!path)
         return false;

      bool ret = true;
      char xPathExpr[MAX_XPATH_EXPR];
      char* endNameXPath = "']";
      xPathExpr[0] = '\0';
      strcat(xPathExpr, "//modules/TDataMngr[@name='");
      strcat(xPathExpr, path);
      strcat(xPathExpr, endNameXPath);

      // Search XPath
      xmlXPathObjectPtr xPathObj;

      if (!XmlGetNodesFromXpath(xPathExpr, xPathObj, docPtr))
         ret = false;

      if (ret) {
         xmlNodeSetPtr nodes = xPathObj->nodesetval;

         int sizeList = (nodes) ? nodes->nodeNr : 0;
         if (sizeList > 0) {
            if (nodes->nodeTab[0]->type == XML_ELEMENT_NODE) {
               xmlNode* config_node = nodes->nodeTab[0];
               xmlReplaceNode(config_node, node);
               xmlFreeNode(config_node);
            }
         }
         // Cleanup
//         xmlXPathFreeObject(xPathObj);

         if (typeConfigNode) {
            /** Type Config */
            if (_wcsicmp(m_activeType, LBL_DEFAULT) != 0) {
               xPathExpr[0] = '\0';
               strcat(xPathExpr, "//type_config[@name='");
               char* activeType = TAux::WSTR_TO_STR(m_activeType);
               strcat(xPathExpr, activeType);
               delete[]activeType;
               strcat(xPathExpr, endNameXPath);

               if (!XmlGetNodesFromXpath(xPathExpr, xPathObj)) {
                  LOG_WARN2(L"Active Config Type NOT FOUND. Name: ",
                      m_activeType);
                  xmlFreeNode(typeConfigNode);
               }
               else if (xPathObj) {
                  nodes = xPathObj->nodesetval;
                  sizeList = (nodes) ? nodes->nodeNr : 0;
                  if (sizeList == 1) {
                     if (nodes->nodeTab[0]->type == XML_ELEMENT_NODE) {
                        xmlNode* type_config_node = nodes->nodeTab[0];
                        xmlReplaceNode(type_config_node, typeConfigNode);
                     }
                  }
                  // Cleanup
                  xmlXPathFreeObject(xPathObj);
               }
            }
            else
               xmlFreeNode(typeConfigNode);
         }

         ret = SaveConfigTo();
      }
      delete[]path;
      return ret;
   }


   #ifndef _DEBUG
   } //inner try
         __except(EXCEPTION_EXECUTE_HANDLER)                              {
            // Hardware Exceptions
            unsigned long code = GetExceptionCode();
            Exception_Handler( code,  L"XmlUpdateConfig");
            if ( m_mngr)                                                      {
//               DISPATCH_EVENT( WARN_CODE, L"EV_XML_EXCEPTION", L"Exception in XmlUpdateConfig", m_mngr);
               LOG_WARN2(L"XML_EXCEPTION", L"Exception in XmlUpdateConfig");
            }
//            m_allowSecFile = false;
         }

   }  //outer try
   catch ( std::exception &ex)                                                {
         //C++ standard exceptions
         STD_EXCEPTION_HANDLER_AUX( L"Exception in XmlUpdateConfig");
         if ( m_mngr)                                                         {
//            DISPATCH_EVENT( WARN_CODE, L"EV_XML_EXCEPTION", L"Exception in XmlUpdateConfig", m_mngr);
            LOG_WARN2(L"XML_EXCEPTION", L"Exception in XmlUpdateConfig");
         }
//         m_allowSecFile = false;
   }
   catch ( Exception &e)                                                      {
         //Builder C++ exceptions
         BUILDER_EXCEPTION_HANDLER_AUX( L"Exception in XmlUpdateConfig ");
         if ( m_mngr)                                                         {
//            DISPATCH_EVENT( WARN_CODE, L"EV_XML_EXCEPTION", L"Exception in XmlUpdateConfig", m_mngr);
            LOG_WARN2(L"XML_EXCEPTION", L"Exception in XmlUpdateConfig");
         }
//         m_allowSecFile = false;

   }
   catch (...)                                                                {
         DEFAULT_EXCEPTION_HANDLER_AUX( L"Exception in XmlUpdateConfig ");
         if ( m_mngr)                                                         {
//            DISPATCH_EVENT( WARN_CODE, L"EV_XML_EXCEPTION", L"Exception in XmlUpdateConfig", m_mngr);
            LOG_WARN2(L"XML_EXCEPTION", L"Exception in XmlUpdateConfig");
         }
//         m_allowSecFile = false;

   }
   #endif

   return false;
}




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
