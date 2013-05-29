#ifndef GALA_CONFIG_XML_ELEMENT_ANALYZE_H_P_P
#define GALA_CONFIG_XML_ELEMENT_ANALYZE_H_P_P

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <string>
#include <vector>
#include <map>
#include <iconv.h>
#include "cstub.h"
#include "Exception.hpp"
#include "File.hpp"

using namespace std;
using namespace Galaxy::GalaxyRT;

class CXMLElement:public CNonCopyAble
{
public:
    CXMLElement(const string &);
    ~CXMLElement();
public:
    xmlNodePtr GetRoot();
    xmlNodePtr GetNodeByName(const string &,xmlNodePtr);
    char *Property(xmlNodePtr,const string &);
    string Property_ex(xmlNodePtr,const string &);
    char *Value(xmlNodePtr);
    xmlNodePtr IsExist(xmlNodePtr,const string &);
    string GetNodePath(xmlNodePtr,bool isEnd=true);
    string GetMethodName(xmlNodePtr);
    string GetName(xmlNodePtr,bool isEnd=true);
    string GetNodeName(xmlNodePtr,bool isEnd=true);
    int GetChildCount(xmlNodePtr);
    string GetChildNameByIndex(xmlNodePtr,int);
    xmlNodePtr GetChildNodeByIndex(xmlNodePtr,int);
    xmlNodePtr GetStructure();
    string GetTitle(xmlNodePtr);
    vector<string> GetUnionName(xmlNodePtr);
    xmlNodePtr *GetNodeListByPath(const string &,int *);
    bool IsChildNode(xmlNodePtr *,int,xmlNodePtr);
    void FreeXPathObject();
    int GetArrayNodeChildCount(xmlNodePtr);
    int ConvertEnc(char *instr,const char *from = "UTF-8", const char *to = "GBK");
    string GetAliasPath(xmlNodePtr);
    bool IsReadonly(xmlNodePtr);
    bool IsArrayStruct(xmlNodePtr);
    string NodeToString(const string &);
private:
    xmlDocPtr _Doc;
    xmlXPathContextPtr _XpathCtx;
    xmlXPathObjectPtr  _XpathObj;
};


#endif
