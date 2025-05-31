//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TXmlManagerEncodingToStream.cpp
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
#include "TXmlManager.h"
#pragma package(smart_init)

//----------------------------------------------------------------------------
/**
   Get Configuration from path.
   @param   path
   @param   streamCfg
*/
bool  TXmlManager::GetConfig( wchar_t* path, LT::TStream* streamCfg, unsigned int filterFlags)            {

   bool ret = false;
   xmlDocPtr m_module_Config  = NULL;
      if ( path && XmlGetConfig( path, m_module_Config ))
         ret = XmlEncodingToStream( m_module_Config, streamCfg, filterFlags, false, NULL);
   return ret;
}

//----------------------------------------------------------------------------
/**
   Get Configuration from path.
   @param   path
   @param   streamCfg
*/
bool  TXmlManager::GetConfigXml( wchar_t* path, std::wstring *str, unsigned int filterFlags)            {

   bool ret = false;
   xmlDocPtr m_module_Config  = NULL;
      if ( path && XmlGetConfig( path, m_module_Config ))
         ret = XmlEncodingToStream( m_module_Config, NULL, filterFlags, true, str);
   return ret;
}

//----------------------------------------------------------------------------
/**
   @param   path
   @param   docConfig
*/
bool  TXmlManager::XmlGetConfig( wchar_t* wpath, xmlDocPtr &docConfig)            {

   bool found  = false;
   bool ret    = true;
   char* path  = TAux::WSTR_TO_STR(wpath);

   if (path)                                                               {
      // Building Path Xpression
      char xPathExpr[MAX_XPATH_EXPR] = "//modules/TDataMngr[@name='";
      strcat(xPathExpr,path);
      strcat(xPathExpr,"']");

      // Search XPath
      xmlXPathObjectPtr xPathObj;

      if ( ! XmlGetNodesFromXpath(xPathExpr, xPathObj) )
         ret = false;

      if (ret)                                                             {
         // Selected Nodes
         xmlNodeSetPtr nodes = xPathObj->nodesetval;
         int sizeList = (nodes) ? nodes->nodeNr : 0;
         if (sizeList > 0 )                                                {
            if (nodes->nodeTab[0]->type  == XML_ELEMENT_NODE)              {
               xmlNode* node = nodes->nodeTab[0];
               docConfig   = xmlNewDoc(BAD_CAST "1.0");
               xmlNode* copyNode =  xmlCopyNode (node,1);
               xmlDocSetRootElement(docConfig, copyNode);
               found = true;
            }
         }

         /* Cleanup */
         xmlXPathFreeObject(xPathObj);

      }
      delete [] path;
   }

   return found;
}

/**
   Serialize configuration into stream.
   @param   docCfg
   @param   streamCfg output
*/
bool  TXmlManager::XmlEncodingToStream( xmlDocPtr docCfg, LT::TStream* streamCfg, unsigned int filterFlags, bool formatXml, std::wstring *str )  {
   //we add a header with the name of the type being sent in the configuration stream
   char *typeName = TAux::WSTR_TO_STR(m_activeType);
   char _typeName[TYPE_NAME_SIZE];
   memset( _typeName, 0x00, sizeof( _typeName));
   strncpy( _typeName, typeName, sizeof( _typeName));
   streamCfg->Write( _typeName, sizeof( _typeName));
   delete [] typeName;
   // Get the root element node
   xmlNode *cur_node   = xmlDocGetRootElement( docCfg );

   bool  ret = false;
   if ( cur_node->type == XML_ELEMENT_NODE)                                {
      if ( strcmp( cur_node->name, LBL_TDATAMANAGER ) == 0)                {

            xmlChar* xmlTmpName     = xmlGetProp( cur_node, LBL_NAME);
            wchar_t* name          = ConvertOutputWC(xmlTmpName );
            xmlFree(xmlTmpName);

            xmlChar* xmlTmpCode    = xmlGetProp( cur_node, LBL_CODE);
            TDataManager* dataMngr = new TDataManager( name, (char*)xmlTmpCode, ( TDataManager *)NULL);
            xmlFree(xmlTmpCode);

            // Flags are removed. Default Constructor adds flags not useful for streaming. If flags change (in defineflags.h),
            // these flags overlap the right ones.
            // dataMngr->DelAttrib(cAttribNames[ATTR_FLAGS]);

            // Construct whole tree TDataManager
            ret = XmlEncodingToStreamTDataMngr( cur_node->children, dataMngr);

            if (ret)                                                          {
               // if active config type is not default config
               if ( _wcsicmp(m_activeType, LBL_DEFAULT ) != 0 )               {
                  char* tmp_name = TAux::WSTR_TO_STR(name);
                  xmlNode *activeNode =  XmlGetActiveConfigNode(m_activeType, name);
                  if  ( activeNode && activeNode->children)
                     ret = XmlEncodingToStreamTDataMngr( activeNode->children, dataMngr);
                    if (tmp_name)
                     delete [] tmp_name;
               }

               // Write TDataManager into Stream
               if ( formatXml == false)
                  ret = dataMngr->Write( streamCfg, true, false, false, true , filterFlags, 0xFFFFFFFF, 0x00000000, true);
               else if ( str)                                                 {
                  ret = dataMngr->WriteXml( *str, true, false, false, true , filterFlags );
               }
            }

            if ( name)
               delete [] name;

            delete  dataMngr;
      }
   }

   if ( docCfg)
         xmlFreeDoc( docCfg);

   return ret;
};

//----------------------------------------------------------------------------
/**
   @param   a_node
   @param   dataMngr
*/
bool TXmlManager::XmlEncodingToStreamTDataMngr( xmlNode * a_node, TNode* parentNode)     {

   bool ret          = true;
   xmlNode *cur_node;

   for ( cur_node = a_node; cur_node; cur_node = cur_node->next)              {
      if ( cur_node->type == XML_ELEMENT_NODE )                               {

         xmlChar* xmlName = xmlGetProp( cur_node, LBL_NAME);
         wchar_t* name = ConvertOutputWC( xmlName);
         xmlFree(xmlName);

         if ( name != NULL)                                                   {
            // -- Getting DATAMANAGER ---
            if ( strcmp( cur_node->name, LBL_TDATAMANAGER ) == 0)             {
               TDataManager* parent =  dynamic_cast<TDataManager*>(parentNode);
               if (!parent)                                                   {
                  LOG_INFO1(L"Parent is not a manager");
                  if ( name)
                     delete [] name;
                  continue;
               }

               xmlChar* xmlCode = xmlGetProp( cur_node, LBL_CODE);

               TDataManager   *newDataMngr = NULL;
               std::list<TDataManager *>::iterator it    = parent->DataManagerBegin();
               std::list<TDataManager *>::iterator itEnd = parent->DataManagerEnd();

               while ( it != itEnd)                                           {
                  if ( ( (*it)->GetName() ) && ( wcscmp((*it)->GetName(), name  )==0) && wcslen( (*it)->GetName()) > 0 ) {
                     //we found it. Point at it with
                     newDataMngr     = *it;
                     break;
                  }
                  it++;
               }

               if (newDataMngr == NULL && xmlCode)    {
                  xmlChar* xmlOrder    = xmlGetProp( cur_node, LBL_ORDER);
                  xmlChar* xmlPrevName = xmlGetProp( cur_node, LBL_PREVIOUS);
                  if (xmlOrder && xmlPrevName)        {

                     newDataMngr       =  new TDataManager (name, (char*)xmlCode);
                     // Flags are removed. Default Constructor adds flags not useful for streaming. If flags change (in defineflags.h),
                     // these flags overlap the right ones.
                     //newDataMngr->DelAttrib(cAttribNames[ATTR_FLAGS]);

                     wchar_t* prevName = ConvertOutputWC( xmlPrevName);
                     ((TDataManager*)parent)->Add(newDataMngr, atoi(xmlOrder), prevName);

                     xmlFree(xmlOrder);
                     xmlFree(xmlPrevName);
                     if (prevName)
                        delete [] prevName;
                  }
                   else       {
                     newDataMngr =  new TDataManager (name, (char*)xmlCode, (TDataManager*)parent);
                     // Flags are removed. Default Constructor adds flags not useful for streaming. If flags change (in defineflags.h),
                     // these flags overlap the right ones.
                     // newDataMngr->DelAttrib(cAttribNames[ATTR_FLAGS]);
                  }
               }

               if ( newDataMngr )
                  XmlEncodingToStreamTDataMngr( cur_node->children, newDataMngr);
            }
            // -- Getting TDATA  ---
            else if ( strcmp( cur_node->name, LBL_TDATA) == 0)                {
               TDataManager* parent =  dynamic_cast<TDataManager*>(parentNode);
               if (!parent)                                                   {
                  LOG_INFO1(L"Parent is not a manager");
                  if ( name)
                     delete [] name;
                  continue;
               }

               xmlChar* xmlType = xmlGetProp( cur_node, LBL_TYPE);
               xmlChar* xmlCode = xmlGetProp( cur_node, LBL_CODE);

               TData *newTData = NULL;
               std::list<TData *>::iterator itdata    = parent->DataBegin();
               std::list<TData *>::iterator itdataEnd = parent->DataEnd();

               while ( itdata != itdataEnd)                                   {
                  if ( ( (*itdata)->GetName()) && ( wcscmp((*itdata)->GetName() , name)==0)) {
                     newTData      = *itdata;
                     break;
                  }
                  itdata++;
               }

               if  ( newTData == NULL && xmlCode && xmlType)                  {
                  newTData = new TData(  ( TDataManager*)parent , name, (char*)xmlCode,
                                                   ( e_sdata)  atoi( (char*)xmlType) );
                  // Flags are removed. Default Constructor adds flags not useful for streaming. If flags change (in defineflags.h),
                  // these flags overlap the right ones.
                  // newTData->DelAttrib(cAttribNames[ATTR_FLAGS]);
               }

               xmlFree(xmlCode);
               xmlFree(xmlType);

               if ( newTData )
                     XmlEncodingToStreamTDataMngr( cur_node->children, newTData);

            }
            // -- Getting TATTRIB  ---
            else if( strcmp( cur_node->name, LBL_TATTRIB ) == 0)
               SetAttributeValueByType ( parentNode , cur_node, name);

            if ( name)
               delete [] name;
         }
         else {
            ret = false;
            LOG_WARN1(L" NAME NULL. No Name in Tree Node");
         }
      }
   } // for

   return ret;
}

//----------------------------------------------------------------------------
/**
   Set a xmlNode to TNode by type.
   @param   node
   @param   name
   @param   type
   @param   buffer
*/
void TXmlManager::SetAttributeValueByType( TNode* node, xmlNode* cur_node, wchar_t* name)      {

//
//   if (wcscmp(name, L"Flags") == 0)  // This is only to remove flags when flags are changed in defineflags.h file.
//      return;

   xmlChar* _type  = xmlGetProp( cur_node, LBL_TYPE);
   e_sattrib type = (e_sattrib)atoi( (char*)_type );
   xmlFree(_type);

   switch(type)                                                   {
      //----------------------------------------------------------------------------
      case TYPE_BIN:                                              {
         xmlChar* _value     = xmlGetProp( cur_node, LBL_VALUE);
         std::string encoded((char*)_value);
         std::string value   = base64_decode(encoded);
         node->SetAttribValue( name ,  (void*)value.c_str(), value.size());
         xmlFree(_value);
      }
      break;
      //----------------------------------------------------------------------------
      case TYPE_WCHAR:                                          {
         xmlChar* xmlValue = xmlGetProp( cur_node, LBL_VALUE);
         wchar_t* buffer = ConvertOutputWC(xmlValue );
         xmlFree(xmlValue);
         if (buffer) {
               node->SetAttribValue( name, buffer);
            delete [] buffer;
         }
      }
       break;
      //----------------------------------------------------------------------------
      case TYPE_CHAR:
      case TYPE_INT:
      case TYPE_FLOAT:                                                 {
         xmlChar* xmlValue = xmlGetProp( cur_node, LBL_VALUE);
         char* buffer = ConvertOutput(xmlValue);
         xmlFree(xmlValue);
         if ( buffer )   {
            if  ( type ==  TYPE_INT )        {
               int val;
               if ( !sscanf( buffer ,"%d", &val) )        {
                  LOG_WARN1(L"Integer value not read.");
                 return;
               }
               node->SetAttribValue( name, val);
            } else if ( type ==  TYPE_FLOAT )                     {
               float val;
               if (! sscanf(buffer, "%f",  &val) )       {
                  LOG_WARN1(L"Float value not read.");
                 return;
               }
               node->SetAttribValue( name, val);
            } else if ( type == TYPE_CHAR)
                  node->SetAttribValue( name, buffer);

         free(buffer);
         }
      } break;
      //----------------------------------------------------------------------------
      default: break;
      }
}




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------

