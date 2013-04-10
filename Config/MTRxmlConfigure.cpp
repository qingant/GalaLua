#include "MTRxmlConfigure.hpp"


using namespace std;
using namespace Galaxy::GalaxyRT;

CXmlMonitorConfigure::CXmlMonitorConfigure(CMonitorConfigure *GalaObject)
    :_ConfObject(GalaObject),_Doc(NULL),_XpathCtx(NULL),_XpathObj(NULL)
{
    if(NULL == _ConfObject)
    {
        THROW_SYSERROR();
    }
}
CXmlMonitorConfigure::~CXmlMonitorConfigure()
{
    _ConfObject = NULL;
    if(_XpathObj)
    {
        xmlXPathFreeObject(_XpathObj);
        _XpathObj = NULL;
    }
    if(_XpathCtx)
    {
        xmlXPathFreeContext(_XpathCtx);
        _XpathCtx = NULL;
    }
    if(_Doc)
    {
        xmlFreeDoc(_Doc);
        _Doc = NULL;
        xmlCleanupParser();
        xmlMemoryDump();
    }
}
char *CXmlMonitorConfigure::Property(xmlNodePtr node,const string &key)
{
    if(NULL == node||key.size() == 0)
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
char *CXmlMonitorConfigure::Value(xmlNodePtr node)
{
    if(NULL == node)
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
string CXmlMonitorConfigure::GetValidPath(xmlNodePtr node)
{
    string path;
    char *pre = NULL;
    if(NULL == node)
    {
        return path;
    }
    string nodePath;
    nodePath += (string)(char *)xmlGetNodePath(node);
    char *p = (char *)nodePath.c_str();
    pre = p;
    while(*p)
    {
        if(*p >= '0' && *p <= '9')
        {
            *(p-1) = '\0';
            int num = atoi(p);
            char strNum[256];
            memset(strNum,0x00,sizeof(strNum));
            snprintf(strNum,sizeof(strNum),"%d",num);
            string szTemp = Strchrr(pre,'/');
            char temp[szTemp.size()+1];
            memset(temp,0x00,sizeof(temp));
            memcpy(temp,szTemp.c_str(),szTemp.size()-1);
            path += (string)temp;
            path += string("[");
            path += string(strNum);
            path += string("]");
            p += strlen(strNum);
            pre = p ;
        }
        p++;
    }
    path += string(pre);
    return path;
}
string CXmlMonitorConfigure::Strchrr(const string & str,char ch)
{
    char szTemp[str.size()+1];
    memset(szTemp,0x00,sizeof(szTemp));
    strcpy((char *)szTemp,str.c_str());
    char *p = (char *)&szTemp[str.size()];
    while(strlen(p) != str.size())
    {
        if(*p == ch)
        {
            *p = '\0';
            break;
        }
        p = p-1;
    }
    return (string)szTemp;
}
bool CXmlMonitorConfigure::IsExist(const string &express)
{
    _XpathObj = xmlXPathEvalExpression(BAD_CAST express.c_str(),_XpathCtx);
    if(NULL == _XpathObj)
    {
        THROW_SYSERROR();
    }
    if(!xmlXPathNodeSetIsEmpty(_XpathObj->nodesetval))
    {
        return true;
    }
    return false;
}
xmlNodePtr CXmlMonitorConfigure::xmlXpath(const string &path)
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
            return _XpathObj->nodesetval->nodeTab[0];
        }
    }
    return NULL ;
}
xmlDocPtr CXmlMonitorConfigure::GetDoc()const
{
    return _Doc ;
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE(xmlNodePtr node,const string &path)
{
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "MonitorList");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "MonitorList";
    OutPut_Configure__STRUCTURE_MonitorLists(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/MonitorList";
    childPath1 += "/MonitorList";
    InPut_Configure__STRUCTURE_MonitorLists(childPath0,childPath1);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists(xmlNodePtr node,const string &path)
{
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "Monitor");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Monitor";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        return ;
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Monitor";
    childPath1 += "/Monitor";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor(childPath0,childPath1);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "Base");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Base";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_Base(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "LowLoger");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "LowLoger";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LowLoger(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "AppLoger");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "AppLoger";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "SVC");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "SVC";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "LSRLists");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "LSRList";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRLists(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "CNRLists");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "CNRList";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRLists(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Base";
    childPath1 += "/Base";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/LowLoger";
    childPath1 += "/LowLoger";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LowLoger(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/AppLoger";
    childPath1 += "/AppLoger";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/SVC";
    childPath1 += "/SVC";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/LSRList";
    childPath1 += "/LSRList";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRLists(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/CNRList";
    childPath1 += "/CNRList";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRLists(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_Base(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "Alias");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Alias";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Alias(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "Token");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Token";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Token(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "SVCLimits");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "SVCLimits";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_SVCLimits(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Alias";
    childPath1 += "/Alias";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Alias(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Token";
    childPath1 += "/Token";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Token(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/SVCLimits";
    childPath1 += "/SVCLimits";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_SVCLimits(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Alias(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Alias(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Token(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Token(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_SVCLimits(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_SVCLimits(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LowLoger(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LowLoger(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "Level");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Level";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Level(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "Details");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Details";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Details(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "Size");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Size";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Size(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "Files");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Files";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Files(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "Dir");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Dir";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Dir(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "NameFormat");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "NameFormat";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_NameFormat(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "DirFormat");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "DirFormat";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_DirFormat(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Level";
    childPath1 += "/Level";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Level(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Details";
    childPath1 += "/Details";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Details(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Size";
    childPath1 += "/Size";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Size(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Files";
    childPath1 += "/Files";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Files(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Dir";
    childPath1 += "/Dir";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Dir(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/NameFormat";
    childPath1 += "/NameFormat";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_NameFormat(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/DirFormat";
    childPath1 += "/DirFormat";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_DirFormat(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Level(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Level(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Details(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Details(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Size(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Size(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Files(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Files(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Dir(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Dir(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_NameFormat(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_NameFormat(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_DirFormat(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_DirFormat(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "Threads");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Threads";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_Threads(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "IsFlush");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "IsFlush";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_IsFlush(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "DBConnection");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "DBConnection";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Threads";
    childPath1 += "/Threads";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_Threads(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/IsFlush";
    childPath1 += "/IsFlush";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_IsFlush(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/DBConnection";
    childPath1 += "/DBConnection";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_Threads(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_Threads(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_IsFlush(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_IsFlush(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "_SCROLLTYPE");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "_SCROLLTYPE";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__SCROLLTYPE(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "_UNIONSELECTOR");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "_UNIONSELECTOR";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/_SCROLLTYPE";
    childPath1 += "/_SCROLLTYPE";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__SCROLLTYPE(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/_UNIONSELECTOR";
    childPath1 += "/_UNIONSELECTOR";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__SCROLLTYPE(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__SCROLLTYPE(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    string parentPath = Strchrr(path,'/');
    parentPath += "/_SCROLLTYPE";
    switch(atoi(_ConfObject->Get(parentPath).c_str()))
    {
        case(DBType::ORACLE):
        {
            child = xmlNewNode(NULL,BAD_CAST "ORACLE");
            xmlAddChild(node,child);
            subpath = path;
            subpath += "/";
            subpath += "ORACLE";
            OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE(child,subpath);
            break;
        }
        case(DBType::DB2):
        {
            child = xmlNewNode(NULL,BAD_CAST "DB2");
            xmlAddChild(node,child);
            subpath = path;
            subpath += "/";
            subpath += "DB2";
            OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2(child,subpath);
            break;
        }
        case(DBType::SQLITE):
        {
            child = xmlNewNode(NULL,BAD_CAST "SQLITE");
            xmlAddChild(node,child);
            subpath = path;
            subpath += "/";
            subpath += "SQLITE";
            OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE(child,subpath);
            break;
        }
    }
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    string parentPath = Strchrr(path0,'/');
    parentPath += "/_SCROLLTYPE";
    xmlNodePtr scrollType = xmlXpath(parentPath);
    if(NULL == scrollType)
    {
        THROW_EXCEPTION(1,"the child _SCROLLTYPE of DBConnection  do not exist");
    }
    char *value = Value(scrollType);
    if(NULL == value)
    {
        THROW_EXCEPTION(1,"the child _SCROLLTYPE of DBConnection  content is empty");
    }
    switch(atoi(value))
    {
        case(DBType::ORACLE):
        {
            childPath0 = path0;
            childPath1 = path1;
            childPath0 += "/ORACLE";
            childPath1 += "/ORACLE";
            InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE(childPath0,childPath1);
            break;
        }
        case(DBType::DB2):
        {
            childPath0 = path0;
            childPath1 = path1;
            childPath0 += "/DB2";
            childPath1 += "/DB2";
            InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2(childPath0,childPath1);
            break;
        }
        case(DBType::SQLITE):
        {
            childPath0 = path0;
            childPath1 = path1;
            childPath0 += "/SQLITE";
            childPath1 += "/SQLITE";
            InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE(childPath0,childPath1);
            break;
        }
        default:
        {
            THROW_EXCEPTION(1,"the type of _UNIONSELECTOR's union is undefine");
        }
    }
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "Name");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Name";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "User");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "User";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "Passwd");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Passwd";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Name";
    childPath1 += "/Name";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/User";
    childPath1 += "/User";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Passwd";
    childPath1 += "/Passwd";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "Name");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Name";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "User");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "User";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "Passwd");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Passwd";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Name";
    childPath1 += "/Name";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/User";
    childPath1 += "/User";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Passwd";
    childPath1 += "/Passwd";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "DBFile");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "DBFile";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/DBFile";
    childPath1 += "/DBFile";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRLists(xmlNodePtr node,const string &path)
{
    char idx[256];
    int items = 64;
    for(int ii =0 ; ii < items ; ++ii)
    {
        memset(idx,0x00,sizeof(idx));
        snprintf(idx,sizeof(idx),"%d",ii);
        string szTemp ;
        szTemp += path;
        szTemp += "[";
        szTemp += idx;
        szTemp += "]";
        szTemp += "/";
        szTemp += "Usable";
        bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
        if(!usedEnable)
        {
            return ;
        }
        string npath;
        string itemName;
        npath += path;
        npath += "[";
        npath += idx;
        npath += "]";
        xmlNodePtr child = NULL;
        itemName +="LSRList";
        itemName +=idx;
        child = xmlNewNode(NULL,BAD_CAST itemName.c_str());
        xmlAddChild(node,child);
        OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList(child,npath);
    };
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRLists(const string &path0,const string &path1)
{
    char idx[256];
    string childPath0;
    string childPath1;
    int items = 64;
    for(int ii =0 ; ii < items ; ++ii)
    {
        childPath0 = path0;
        childPath0 += "s";
        memset(idx,0x00,sizeof(idx));
        snprintf(idx,sizeof(idx),"%d",ii);
        childPath0 += "/LSRList";
        childPath0 += idx;
        childPath1 = path1;
        childPath1 += "[";
        childPath1 += idx;
        childPath1 += "]";
        InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList(childPath0,childPath1);
    };
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "LSR");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "LSR";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        return ;
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/LSR";
    childPath1 += "/LSR";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "Alias");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Alias";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Alias(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "ImplDllName");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "ImplDllName";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_ImplDllName(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "Threads");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Threads";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Threads(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "ThreadStackSize");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "ThreadStackSize";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_ThreadStackSize(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "Protocol");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Protocol";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Alias";
    childPath1 += "/Alias";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Alias(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/ImplDllName";
    childPath1 += "/ImplDllName";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_ImplDllName(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Threads";
    childPath1 += "/Threads";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Threads(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/ThreadStackSize";
    childPath1 += "/ThreadStackSize";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_ThreadStackSize(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Protocol";
    childPath1 += "/Protocol";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Alias(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Alias(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_ImplDllName(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_ImplDllName(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Threads(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Threads(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_ThreadStackSize(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_ThreadStackSize(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "_SCROLLTYPE");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "_SCROLLTYPE";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "_UNIONSELECTOR");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "_UNIONSELECTOR";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/_SCROLLTYPE";
    childPath1 += "/_SCROLLTYPE";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/_UNIONSELECTOR";
    childPath1 += "/_UNIONSELECTOR";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    string parentPath = Strchrr(path,'/');
    parentPath += "/_SCROLLTYPE";
    switch(atoi(_ConfObject->Get(parentPath).c_str()))
    {
        case(ListenerType::LSRSTREAM):
        {
            child = xmlNewNode(NULL,BAD_CAST "LSRSTREAM");
            xmlAddChild(node,child);
            subpath = path;
            subpath += "/";
            subpath += "LSRSTREAM";
            OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM(child,subpath);
            break;
        }
    }
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    string parentPath = Strchrr(path0,'/');
    parentPath += "/_SCROLLTYPE";
    xmlNodePtr scrollType = xmlXpath(parentPath);
    if(NULL == scrollType)
    {
        THROW_EXCEPTION(1,"the child _SCROLLTYPE of Protocol  do not exist");
    }
    char *value = Value(scrollType);
    if(NULL == value)
    {
        THROW_EXCEPTION(1,"the child _SCROLLTYPE of Protocol  content is empty");
    }
    switch(atoi(value))
    {
        case(ListenerType::LSRSTREAM):
        {
            childPath0 = path0;
            childPath1 = path1;
            childPath0 += "/LSRSTREAM";
            childPath1 += "/LSRSTREAM";
            InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM(childPath0,childPath1);
            break;
        }
        default:
        {
            THROW_EXCEPTION(1,"the type of _UNIONSELECTOR's union is undefine");
        }
    }
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "Address");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Address";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "RecvTimeOut");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "RecvTimeOut";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "ExchangeTMO");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "ExchangeTMO";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "PTCDllName");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "PTCDllName";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Address";
    childPath1 += "/Address";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/RecvTimeOut";
    childPath1 += "/RecvTimeOut";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/ExchangeTMO";
    childPath1 += "/ExchangeTMO";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/PTCDllName";
    childPath1 += "/PTCDllName";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "_SCROLLTYPE");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "_SCROLLTYPE";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "_UNIONSELECTOR");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "_UNIONSELECTOR";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/_SCROLLTYPE";
    childPath1 += "/_SCROLLTYPE";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/_UNIONSELECTOR";
    childPath1 += "/_UNIONSELECTOR";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    string parentPath = Strchrr(path,'/');
    parentPath += "/_SCROLLTYPE";
    switch(atoi(_ConfObject->Get(parentPath).c_str()))
    {
        case(SocketType::TCP):
        {
            child = xmlNewNode(NULL,BAD_CAST "TCP");
            xmlAddChild(node,child);
            subpath = path;
            subpath += "/";
            subpath += "TCP";
            OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP(child,subpath);
            break;
        }
        case(SocketType::UNIX):
        {
            child = xmlNewNode(NULL,BAD_CAST "UNIX");
            xmlAddChild(node,child);
            subpath = path;
            subpath += "/";
            subpath += "UNIX";
            OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX(child,subpath);
            break;
        }
    }
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    string parentPath = Strchrr(path0,'/');
    parentPath += "/_SCROLLTYPE";
    xmlNodePtr scrollType = xmlXpath(parentPath);
    if(NULL == scrollType)
    {
        THROW_EXCEPTION(1,"the child _SCROLLTYPE of Address  do not exist");
    }
    char *value = Value(scrollType);
    if(NULL == value)
    {
        THROW_EXCEPTION(1,"the child _SCROLLTYPE of Address  content is empty");
    }
    switch(atoi(value))
    {
        case(SocketType::TCP):
        {
            childPath0 = path0;
            childPath1 = path1;
            childPath0 += "/TCP";
            childPath1 += "/TCP";
            InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP(childPath0,childPath1);
            break;
        }
        case(SocketType::UNIX):
        {
            childPath0 = path0;
            childPath1 = path1;
            childPath0 += "/UNIX";
            childPath1 += "/UNIX";
            InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX(childPath0,childPath1);
            break;
        }
        default:
        {
            THROW_EXCEPTION(1,"the type of _UNIONSELECTOR's union is undefine");
        }
    }
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "IP");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "IP";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "Port");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Port";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/IP";
    childPath1 += "/IP";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Port";
    childPath1 += "/Port";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "Token");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Token";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Token";
    childPath1 += "/Token";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRLists(xmlNodePtr node,const string &path)
{
    char idx[256];
    int items = 64;
    for(int ii =0 ; ii < items ; ++ii)
    {
        memset(idx,0x00,sizeof(idx));
        snprintf(idx,sizeof(idx),"%d",ii);
        string szTemp ;
        szTemp += path;
        szTemp += "[";
        szTemp += idx;
        szTemp += "]";
        szTemp += "/";
        szTemp += "Usable";
        bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
        if(!usedEnable)
        {
            return ;
        }
        string npath;
        string itemName;
        npath += path;
        npath += "[";
        npath += idx;
        npath += "]";
        xmlNodePtr child = NULL;
        itemName +="CNRList";
        itemName +=idx;
        child = xmlNewNode(NULL,BAD_CAST itemName.c_str());
        xmlAddChild(node,child);
        OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList(child,npath);
    };
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRLists(const string &path0,const string &path1)
{
    char idx[256];
    string childPath0;
    string childPath1;
    int items = 64;
    for(int ii =0 ; ii < items ; ++ii)
    {
        childPath0 = path0;
        childPath0 += "s";
        memset(idx,0x00,sizeof(idx));
        snprintf(idx,sizeof(idx),"%d",ii);
        childPath0 += "/CNRList";
        childPath0 += idx;
        childPath1 = path1;
        childPath1 += "[";
        childPath1 += idx;
        childPath1 += "]";
        InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList(childPath0,childPath1);
    };
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "CNR");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "CNR";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        return ;
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/CNR";
    childPath1 += "/CNR";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "Alias");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "Alias";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_Alias(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "PlugIn");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "PlugIn";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/Alias";
    childPath1 += "/Alias";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_Alias(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/PlugIn";
    childPath1 += "/PlugIn";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_Alias(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_Alias(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "_SCROLLTYPE");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "_SCROLLTYPE";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(child,subpath);
    child = xmlNewNode(NULL,BAD_CAST "_UNIONSELECTOR");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "_UNIONSELECTOR";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/_SCROLLTYPE";
    childPath1 += "/_SCROLLTYPE";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(childPath0,childPath1);
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/_UNIONSELECTOR";
    childPath1 += "/_UNIONSELECTOR";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    string parentPath = Strchrr(path,'/');
    parentPath += "/_SCROLLTYPE";
    switch(atoi(_ConfObject->Get(parentPath).c_str()))
    {
        case(ConectorType::ECHO):
        {
            child = xmlNewNode(NULL,BAD_CAST "ECHO");
            xmlAddChild(node,child);
            subpath = path;
            subpath += "/";
            subpath += "ECHO";
            OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO(child,subpath);
            break;
        }
        case(ConectorType::TIMEOUT):
        {
            child = xmlNewNode(NULL,BAD_CAST "TIMEOUT");
            xmlAddChild(node,child);
            subpath = path;
            subpath += "/";
            subpath += "TIMEOUT";
            OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT(child,subpath);
            break;
        }
        case(ConectorType::SQUARE):
        {
            child = xmlNewNode(NULL,BAD_CAST "SQUARE");
            xmlAddChild(node,child);
            subpath = path;
            subpath += "/";
            subpath += "SQUARE";
            OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE(child,subpath);
            break;
        }
        case(ConectorType::STATE):
        {
            child = xmlNewNode(NULL,BAD_CAST "STATE");
            xmlAddChild(node,child);
            subpath = path;
            subpath += "/";
            subpath += "STATE";
            OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE(child,subpath);
            break;
        }
    }
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    string parentPath = Strchrr(path0,'/');
    parentPath += "/_SCROLLTYPE";
    xmlNodePtr scrollType = xmlXpath(parentPath);
    if(NULL == scrollType)
    {
        THROW_EXCEPTION(1,"the child _SCROLLTYPE of PlugIn  do not exist");
    }
    char *value = Value(scrollType);
    if(NULL == value)
    {
        THROW_EXCEPTION(1,"the child _SCROLLTYPE of PlugIn  content is empty");
    }
    switch(atoi(value))
    {
        case(ConectorType::ECHO):
        {
            childPath0 = path0;
            childPath1 = path1;
            childPath0 += "/ECHO";
            childPath1 += "/ECHO";
            InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO(childPath0,childPath1);
            break;
        }
        case(ConectorType::TIMEOUT):
        {
            childPath0 = path0;
            childPath1 = path1;
            childPath0 += "/TIMEOUT";
            childPath1 += "/TIMEOUT";
            InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT(childPath0,childPath1);
            break;
        }
        case(ConectorType::SQUARE):
        {
            childPath0 = path0;
            childPath1 = path1;
            childPath0 += "/SQUARE";
            childPath1 += "/SQUARE";
            InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE(childPath0,childPath1);
            break;
        }
        case(ConectorType::STATE):
        {
            childPath0 = path0;
            childPath1 = path1;
            childPath0 += "/STATE";
            childPath1 += "/STATE";
            InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE(childPath0,childPath1);
            break;
        }
        default:
        {
            THROW_EXCEPTION(1,"the type of _UNIONSELECTOR's union is undefine");
        }
    }
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "ImplDllName");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "ImplDllName";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/ImplDllName";
    childPath1 += "/ImplDllName";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "ImplDllName");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "ImplDllName";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/ImplDllName";
    childPath1 += "/ImplDllName";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "ImplDllName");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "ImplDllName";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/ImplDllName";
    childPath1 += "/ImplDllName";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE(xmlNodePtr node,const string &path)
{
    string szTemp ;
    szTemp += path;
    szTemp += "/";
    szTemp += "Usable";
    bool usedEnable = atoi(_ConfObject->Get(szTemp).c_str());
    if(!usedEnable)
    {
        return ;
    }
    string subpath;
    xmlNodePtr child = NULL;
    child = xmlNewNode(NULL,BAD_CAST "ImplDllName");
    xmlAddChild(node,child);
    subpath = path;
    subpath += "/";
    subpath += "ImplDllName";
    OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName(child,subpath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE(const string &path0,const string &path1)
{
    string childPath0;
    string childPath1;
    if(!IsExist(path0))
    {
        THROW_EXCEPTION(1,path0.c_str());
    }
    childPath0 = path0;
    childPath1 = path1;
    childPath0 += "/ImplDllName";
    childPath1 += "/ImplDllName";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName(childPath0,childPath1);
    string usablePath = path1;
    usablePath += "/Usable";
    InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable(usablePath);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable(const string &path){
    _ConfObject->Put(path,"1",true);
}
void CXmlMonitorConfigure::OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName(xmlNodePtr node,const string &path)
{
    xmlNodePtr content = xmlNewText(BAD_CAST _ConfObject->Get(path).c_str());
    xmlAddChild(node,content);
}
void CXmlMonitorConfigure::InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName(const string &path0,const string &path1)
{
    string childPath0 = path0;
    string childPath1 = path1;
    xmlNodePtr node = xmlXpath(childPath0);
    if(NULL != node)
    {
        char *value = Value(node);
        if(NULL != value)
        {
            _ConfObject->Put(childPath1,value);
        }
    }
}
void CXmlMonitorConfigure::WriteConfig2Xml(const string &path)
{
    if(path.size()==0)
    {
        THROW_EXCEPTION(1,"the path must be not empty");
    }
    if(CRT_access(path.c_str(),W_OK|R_OK) == 0)
    {
        THROW_EXCEPTION(1,"the file is exist");
    }
    xmlSaveCtxtPtr ctxt;
    int saveOpts = 0;
    xmlNodePtr root=NULL,node=NULL,child=NULL;
    _Doc = xmlNewDoc (BAD_CAST "1.0");
    if(NULL == _Doc)
    {
        THROW_EXCEPTION(1,"create document failure");
    }
    root = xmlNewNode(NULL,BAD_CAST "Configure");
    if(NULL ==root)
    {
        THROW_EXCEPTION(1,"create node failure");
    }
    xmlDocSetRootElement(_Doc,root);
    child = xmlNewChild(root,NULL,BAD_CAST "_VERSION",BAD_CAST "2.0");
    if(NULL ==child)
    {
        THROW_EXCEPTION(1,"create node failure");
    }
    child = xmlNewNode(NULL,BAD_CAST "_STRUCTURE");
    if(NULL ==child)
    {
        THROW_EXCEPTION(1,"create node failure");
    }
    node = xmlAddChild(root,child);    if(NULL ==node)
    {
        THROW_EXCEPTION(1,"create node failure");
    }
    OutPut_Configure__STRUCTURE(node,"/Configure/_STRUCTURE");
    xmlSaveFormatFileEnc("ll",_Doc,(char *)_Doc->encoding,1);
    saveOpts |= XML_SAVE_FORMAT;
    ctxt = xmlSaveToFilename(path.c_str(),(char *)_Doc->encoding,saveOpts);
    if (ctxt != NULL)
    {
        if (xmlSaveDoc(ctxt, _Doc) < 0)
        {
            THROW_EXCEPTION(1,"failed save to path");
        }
        xmlSaveClose(ctxt);
    }
    else
    {
        THROW_EXCEPTION(1,"failed save to path");
    }
}
void CXmlMonitorConfigure::LoadXmlConfig(const string &path,bool isExtend)
{
    xmlKeepBlanksDefault(0);
    if(!_Doc)
    {
        _Doc = xmlReadFile(path.c_str(),NULL,0);
    }
    if(NULL ==_Doc)
    {
        THROW_EXCEPTION(1,"parser document failure");
    }
    xmlNodePtr node = xmlDocGetRootElement(_Doc);
    if(NULL ==node)
    {
        THROW_EXCEPTION(1,"parser root element failure");
    }
    _XpathCtx = xmlXPathNewContext(_Doc);
    if(NULL == _XpathCtx)
    {
        THROW_SYSERROR();
    }
    if(!isExtend)
    {
        InPut_Configure__STRUCTURE("/Configure/_STRUCTURE","/Configure/_STRUCTURE");
    }
}
