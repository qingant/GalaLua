#include "GalaxmlParser.hpp"

CXMLElement::CXMLElement(const string &path)
    :_Doc(NULL),_XpathCtx(NULL),_XpathObj(NULL)
{
    if(path.size() == 0)
    {
        THROW_SYSERROR();
    }
    xmlKeepBlanksDefault(0);
    _Doc = xmlReadFile(path.c_str(),NULL,0);
    if(ISNULL(_Doc))
    {
        THROW_SYSERROR();
    }
    _XpathCtx = xmlXPathNewContext(_Doc);
    if(ISNULL(_XpathCtx))
    {
        THROW_SYSERROR();
    }
}

CXMLElement::~CXMLElement()
{
    xmlXPathFreeObject(_XpathObj);
    xmlXPathFreeContext(_XpathCtx);
    xmlFreeDoc(_Doc);
    xmlCleanupParser();
    xmlMemoryDump();
}

bool CXMLElement::IsChildNode(xmlNodePtr *nodeList,int count,xmlNodePtr node)
{
    if(NULL == nodeList||NULL == node ||count < 0)
    {
        return false ;
    }
    for(int ii = 0 ; ii < count;++ii)
    {
        if(!xmlStrcmp(nodeList[ii]->name,node->name))
        {
            return true;
        }
    }
    return false;
}

xmlNodePtr *CXMLElement::GetNodeListByPath(const string &path,int *count)
{
    if(path.size() == 0)
    {
        return NULL;
    }
    _XpathObj = xmlXPathEvalExpression(BAD_CAST path.c_str(),_XpathCtx);
    if(NULL != _XpathObj)
    {
        if(!xmlXPathNodeSetIsEmpty(_XpathObj->nodesetval))
        {
            *count = _XpathObj->nodesetval->nodeNr;
            return _XpathObj->nodesetval->nodeTab;
        }
    }
    return NULL ;
}

void CXMLElement::FreeXPathObject()
{
    xmlXPathFreeObject(_XpathObj);
    _XpathObj = NULL ;
}

xmlNodePtr CXMLElement::GetRoot()
{
    return xmlDocGetRootElement(_Doc);
}

xmlNodePtr CXMLElement::GetNodeByName(const string & name,xmlNodePtr parent)
{
    if(name.size() == 0 || NULL == parent)
    {
        return NULL;
    }
    xmlNodePtr child = NULL,node = NULL;

    if(!strcmp(name.c_str(),(char *)parent->name))
    {
        return parent;
    }
    child = parent->xmlChildrenNode;
    while(child != NULL)
    {
        node = GetNodeByName(name,child);
        if(NULL != node)
        {
            break;
        }
        child = child->next;
    }
    return node;
}

char *CXMLElement::Property(xmlNodePtr node,const string &key)
{
    if(ISNULL(node)||key.size() == 0)
    {
        return NULL;
    }
    char *attr = (char *)xmlGetProp(node,BAD_CAST key.c_str());
    if(attr == NULL || (attr != NULL && strlen(attr)==0))
    {
        return NULL;
    }
    return attr;
}

char *CXMLElement::Value(xmlNodePtr node)
{
    if(ISNULL(node))
    {
        return NULL;
    }
    char *value = (char *)xmlNodeGetContent(node);
    if(value == NULL || (value != NULL && strlen(value)==0))
    {
        return NULL;
    }
    return value;
}

xmlNodePtr CXMLElement::IsExist(xmlNodePtr parent,const string &name)
{
    xmlNodePtr child = NULL;
    child = parent->children;
    while(!ISNULL(child))
    {
        if(!strcmp((char *)child->name,name.c_str()))
        {
            return child;
        }
        child = child->next;
    }
    return NULL;
}

string CXMLElement::GetNodePath(xmlNodePtr node,bool isEnd)
{
    string path;
    if(NULL == node)
    {
        return path;
    }
    char *_path = (char *)xmlGetNodePath(node);
    if(!ISNULL(_path))
    {
        path += string(_path);
    }
    if(isEnd)
        path[path.size()] = '\0';
    return path;
}

string CXMLElement::GetName(xmlNodePtr node,bool isEnd)
{
    string  name;
    if(NULL == node)
    {
        return name;
    }
    name += (string)((char *)node->name);
    if(isEnd)
        name[name.size()]='\0';
    return name;
}

string CXMLElement::GetNodeName(xmlNodePtr node,bool isEnd)
{
    string name;
    if(NULL == node)
    {
        return name;
    }
    char *attr = Property(node,"name");
    if(NULL==attr)
    {
        name += (string)((char *)node->name);
    }
    else
    {
        name += string(attr);
    }
    if(isEnd)
        name[name.size()] = '\0';
    return name;
}

int CXMLElement::GetChildCount(xmlNodePtr parent)
{
    int iCount = 0 ;
    if(ISNULL(parent))
    {
        return -1;
    }
    xmlNodePtr child =  parent->children;
    while(!ISNULL(child))
    {
          if(child->type == XML_ELEMENT_NODE)
          {
            iCount++;
        }
        child = child->next;
    }
    return iCount;
}

string CXMLElement::GetChildNameByIndex(xmlNodePtr node,int idx)
{
    if(ISNULL(node)||idx < 0)
    {
        THROW_EXCEPTION(1,"the function 's param of GetChildNameByIndex invalid");
    }
    xmlNodePtr child = node->children;
    int i = 0;
    char *pName = NULL;
    string key ;
    char szTemp[4096];
    memset(szTemp,0x00,sizeof(szTemp));
    while(!ISNULL(child))
    {
        char *attr = Property(child,"type");
        if((!ISNULL(attr) && strcmp("invalid",attr))||ISNULL(attr))
        {
            if(i==idx)
            {
                pName = Property(child,"title");
                strcpy((char *)szTemp,pName);
                break;
            }
            i++;
        }
        child = child->next;
    }
    if(ConvertEnc((char *)szTemp) != 0)
    {
        THROW_EXCEPTION(1,"Convert Encoding failure");
    }
    key = (string)szTemp;
    key[strlen(szTemp)] = '\0';
    return key;
}

string CXMLElement::Property_ex(xmlNodePtr node,const string &key)
{
    string value;
    if(NULL == node || key.size() == 0)
    {
        return value;
    }
    char *pName = NULL;
    char szTemp[4096];
    memset(szTemp,0x00,sizeof(szTemp));
    char *attr = Property(node,"type");
    if((NULL != attr && strcmp("invalid",attr))|| NULL == attr)
    {
        pName = Property(node,key.c_str());
        strcpy((char *)szTemp,pName);
    }
    if(ConvertEnc((char *)szTemp) != 0)
    {
        THROW_EXCEPTION(1,"Convert Encoding failure");
    }
    value = (string)szTemp;
    value[strlen(szTemp)] = '\0';
    return value;
}

vector<string> CXMLElement::GetUnionName(xmlNodePtr node)
{
    vector<string> name;
    if(NULL == node )
    {
        THROW_EXCEPTION(1,"the node invalid");
    }
    assert(node!=NULL);
    xmlNodePtr type_node = GetNodeByName("type", node);
    assert(type_node != NULL);
    const char *type_name = (const char*)xmlNodeGetContent(type_node);
    assert(type_name != NULL);

    
    xmlNodePtr root = GetRoot();
    xmlNodePtr typedefs = GetNodeByName("_TYPEDEFINE", root);
    assert(typedefs != NULL);
    xmlNodePtr type_def = GetNodeByName(type_name, typedefs);
    xmlNodePtr child = type_def->children;
    while (NULL != child)
    {
        if(XML_ELEMENT_NODE == child->type)
        {
            name.push_back((char *)child->name);  
        }
        child = child->next;
    }
    return name;
    
    // xmlNodePtr parent = node->parent;
    // xmlNodePtr unionNode = IsExist(parent,"_UNIONSELECTOR");
    // if(!unionNode)
    // {
    //     THROW_EXCEPTION(1,"the node of "+string((char *)parent->name)+" not exist the node of _UNIONSELECTOR");
    // }
    // xmlNodePtr child = unionNode->children;
    // while(NULL != child)
    // {
    //     if(XML_ELEMENT_NODE == child->type)
    //     {
    //         char *attr = Property(child,"meta");
    //         if(!strcmp("array",attr)||!strcmp("struct",attr)
    //             ||!strcmp("union",attr))
    //         {
    //             name.push_back((char *)child->name);
    //         }
    //     }
    //     child = child->next;
    // }
    // return name;
}

string CXMLElement::GetTitle(xmlNodePtr node)
{
    if(ISNULL(node))
    {
        THROW_EXCEPTION(1,"the function 's param of GetChildNameByIndex invalid");
    }
    char *pName = NULL;
    string key ;
    char szTemp[4096];
    memset(szTemp,0x00,sizeof(szTemp));
    char *attr = Property(node,"type");
    if((!ISNULL(attr) && strcmp("invalid",attr))||ISNULL(attr))
    {
        pName = Property(node,"title");
        if (pName == NULL)
        {
            return std::string((const char*)node->name);
        }
        strcpy((char *)szTemp,pName);
    }
    if(ConvertEnc((char *)szTemp) != 0)
    {
        THROW_EXCEPTION(1,"Convert Encoding failure");
    }
    key = (string)szTemp;
    key[strlen(szTemp)] = '\0';
    return key;
}

xmlNodePtr CXMLElement::GetChildNodeByIndex(xmlNodePtr parent,int idx)
{
    if(ISNULL(parent)||idx < 0)
    {
        THROW_EXCEPTION(1,"the function 's param of GetChildNodeByIndex invalid");
    }
    xmlNodePtr child = NULL;
    child = parent->children;
    int iCount = 0 ;
    while(NULL != child)
    {
          if(child->type == XML_ELEMENT_NODE)
          {
              char *attr = Property(child,"type");
              if((!ISNULL(attr) && strcmp("invalid",attr))||ISNULL(attr))
              {
                  iCount++;
              }
          }
          if(iCount == (idx+1))
          {
              break;
          }
        child = child->next;
    }
    return child;
}

xmlNodePtr CXMLElement::GetStructure()
{
    return GetNodeByName("_STRUCTURE",GetRoot());
}

int CXMLElement::GetArrayNodeChildCount(xmlNodePtr node)
{
    if(ISNULL(node))
    {
        THROW_EXCEPTION(1,"the node is NULL");
    }
    char *items = Property(node,"items");
    if(!ISNULL(items))
    {
        return atoi(items);
    }
    return 1;
}

int CXMLElement::ConvertEnc(char *instr,const char *from,const char *to)
{
    char buf[4096];
    char rcvBuf[4096];
    char *in = NULL, *rcv = NULL;
    strcpy((char *)buf, instr);
    memset(rcvBuf, 0x00, sizeof(rcvBuf));
    iconv_t c_pt;
    if ((c_pt = iconv_open(to, from)) == (iconv_t)-1)
    {
        return -1;
    }
    iconv(c_pt, NULL, NULL, NULL, NULL);
    rcv = (char *)rcvBuf;
    in  = (char *)buf;
    size_t lenin = strlen(instr)+1;
    size_t lenout = 4096;
    
    if((int)(iconv(c_pt, &in, (size_t *)&lenin, &rcv, (size_t *)&lenout)) == -1)
    {
        return -2;
    }
    strcpy(instr, (char *)rcvBuf);
    iconv_close(c_pt);
    
    return 0;
}

string CXMLElement::GetAliasPath(xmlNodePtr node)
{
    if(!node)
    {
        THROW_EXCEPTION(1,"the node is NULL");
    }
    return Property_ex(node,"AliasPath");
}

bool CXMLElement::IsReadonly(xmlNodePtr node)
{
    if(!node)
    {
        THROW_EXCEPTION(1,"the node is NULL");
    }
    bool isRW = false;
    char *attr = Property(node,"rw");
    if(attr && !strcmp("readonly",attr))
    {
        isRW = true;
    }
    return isRW;
}

bool CXMLElement::IsArrayStruct(xmlNodePtr node)
{
    if(!node)
    {
        THROW_EXCEPTION(1,"the node is NULL");
    }
    bool isArrayStruct = false ;
    char *attr = Property(node,"arraystruct");
    if(attr && !strcmp("true",attr))
    {
        isArrayStruct = true ;
    }
    return isArrayStruct ;
}

string CXMLElement::NodeToString(const string &path)
{
    int iCount;
    string content;
    xmlNodePtr *nodeList = GetNodeListByPath(path,&iCount);
    if(!nodeList)
    {
        THROW_EXCEPTION(1,"the "+path+" not exist");
    }
    if(iCount != 1)
    {
        THROW_EXCEPTION(1,"the "+path+" Multiple definitions");
    }
    xmlNodePtr node = nodeList[0];
    xmlBufferPtr nodeBuffer = xmlBufferCreate();
    if(!nodeBuffer)
    {
        THROW_EXCEPTION(1,"the memory alloc failure");
    }
    if (xmlNodeDump(nodeBuffer,_Doc, node, 0, 1) == -1)
    {
        THROW_EXCEPTION(1,"the memory dump failure");
    }
    content = string((char *)nodeBuffer->content);
    xmlBufferFree(nodeBuffer);

    return content;
}

