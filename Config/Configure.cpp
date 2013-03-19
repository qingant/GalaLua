#include "HeadDesp.hpp"
#include "Configure.hpp"

using namespace std;
using namespace Galaxy::GalaxyRT;

Configure::Configure(const std::string &Path,int prot)
    :_Validication(false),_Path(Path),_SHMObject(NULL)
{
    if(IsExistPath(_Path))
    {
        _Validication = OpenFileAndMapping(Path,prot);
    }
    else
    {
        THROW_EXCEPTION(1, "The Directory of Configure File Not Exists ");
    }
}

Configure::~Configure()
{

}

bool Configure::IsExistPath(const std::string &Path)
{
    INT ii = 0 ;
    if(Path.size()==0)
    {
        return false;
    }
    if(ISNULL(CRT_strstr(Path.c_str(),"/")))
    {
        return false;
    }
    else
    {
        char tmp[1024];
        CRT_memset(&tmp,0x00,sizeof(tmp));
        CRT_memcpy(tmp,Path.c_str(),Path.size());
        ii = Path.size();
        while(ii && tmp[ii -1] != '/') ii--;
        if(ii == 0)
        {
            return false;
        }
        char *p = (char *)tmp;
        p[ii -1] = '\0';
        if(CRT_access(p,R_OK|W_OK) == -1)
        {
            return false;
        }
    }
    return true ;
}

bool Configure::OpenFileAndMapping(const std::string &Path,int prot)
{
    PGalaHeadDesp    pFileHead;
    BYTE                szTemp[sizeof(*pFileHead)+2];
    pFileHead = (PGalaHeadDesp)szTemp;
    if (CRT_access(Path.c_str(), R_OK|W_OK) == -1)
    {
        if (prot & PROT_WRITE)
        {
            CFile   _File(Path.c_str(),O_CREAT|O_RDWR,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
            return false ;
        }
        else
        {
            THROW_EXCEPTION(1, "Configure File Not Exists , PROT is ONLY READ");
        }
    }
    CFile   _File(Path.c_str(),O_SYNC|O_RDWR,S_IRUSR);
    INT _Length = _File.Read(szTemp,sizeof(*pFileHead));
    if(_Length!=sizeof(*pFileHead))
    {
        return false;
    }

    if(pFileHead->Magic[0]!='G')
    {
        return false;
    }
    if((pFileHead->Magic[1]!='0') && (pFileHead->Magic[1]!='1'))
    {
        return false;
    }
    if(pFileHead->Magic[2]!='V')
    {
        return false;
    }
    if(pFileHead->Magic[3]!='1')
    {
        return false;
    }
    if(pFileHead->Coding!=1)
    {
        return false;
    }
    //校验长度和CRC32
    _SHMObject.reset(new CFileMapping(&_File,pFileHead->Size, prot));
    if(ISNULL(_SHMObject.get()))
    {
        return false;
    }
    return true;
}

PVOID Configure::GetSHMEntry()
{
    if(ISNULL(_SHMObject.get()))
    {
        return NULL;
    }
    return _SHMObject->Get();
}

GalaConfigureGRP::GalaConfigureGRP(const std::string &path,int prot)
    :Configure(path,prot)
{
    if(!_Validication)
    {
        Initialize();
    }
    _ConfObject.reset(new CMonitorConfigure(((PBYTE)GetSHMEntry()+sizeof(GalaHeadDesp)),sizeof(MonitorConfigure)));
    if(NULL == _ConfObject.get())
    {
        THROW_EXCEPTION(1, "Object is invalid");
    }
}

GalaConfigureGRP::~GalaConfigureGRP()
{

}

void GalaConfigureGRP::Initialize()
{
    CFile _file(_Path.c_str(),O_SYNC|O_RDWR,S_IRUSR);
    unsigned int MapSize = sizeof(MonitorConfigure)+sizeof(GalaHeadDesp);
    this->_SHMObject.reset(new CFileMapping(&_file,MapSize, PROT_READ|PROT_WRITE));
    PBYTE   _Entry = (PBYTE)GetSHMEntry();
    PGalaHeadDesp pHead = (PGalaHeadDesp)_Entry;
    pHead->Magic[0] = 'G';
    pHead->Magic[1] = '1';
    pHead->Magic[2] = 'V';
    pHead->Magic[3] = '1';
    pHead->WRKStatus = USABLE_DISABLED;
    pHead->Coding = 1;
    pHead->Size = _SHMObject->GetSize();
    pHead->UsedLength = MapSize;
    CRT_memset(_Entry+sizeof(GalaHeadDesp),0x00,sizeof(MonitorConfigure));
    pHead->MappingKey = 0xABCD;
    pHead->Stamp = CRT_time(NULL);
}

CMonitorConfigure *GalaConfigureGRP::GetObject()
{
    return _ConfObject.get();
}

ConfigurePlt::ConfigurePlt(const std::string &path,int prot)
    :Configure(path,prot),_PltFile(path),_Prot(prot),GroupId(-1),_GRP(NULL)
{
    if(!_Validication)
    {
        Initialize();
    }
}


ConfigurePlt::~ConfigurePlt()
{

}

void ConfigurePlt::Initialize()
{
    CFile _file(_Path.c_str(),O_SYNC|O_RDWR,S_IRUSR);
    unsigned int MapSize = sizeof(MonitorConfigureSet)+sizeof(GalaHeadDesp);
    this->_SHMObject.reset(new CFileMapping(&_file,MapSize, PROT_READ|PROT_WRITE));
    PBYTE   _Entry = (PBYTE)GetSHMEntry();
    PGalaHeadDesp pHead = (PGalaHeadDesp)_Entry;
    pHead->Magic[0] = 'G';
    pHead->Magic[1] = '1';
    pHead->Magic[2] = 'V';
    pHead->Magic[3] = '1';
    pHead->WRKStatus = USABLE_DISABLED;
    pHead->Coding = 1;
    pHead->Size = _SHMObject->GetSize();
    pHead->UsedLength = MapSize;//_Size;
    CRT_memset(_Entry+sizeof(GalaHeadDesp),0x00,sizeof(MonitorConfigureSet));
    pHead->MappingKey = 0xABCD;
    pHead->Stamp = CRT_time(NULL);

    PMonitorConfigureSet pSet = GetWorkGrpSet();
    for (int idx = 0; idx != MONITOR_MAX; ++idx)
    {
        pSet->_GrpCfg[idx].Usable = USABLE_ENABLED;
        CRT_snprintf((PSTR)pSet->_GrpCfg[idx].Configure, 1024, "%s%04d.conf","monitor",idx);
    }
}

PMonitorConfigureSet ConfigurePlt::GetWorkGrpSet()
{
    PBYTE   _Entry = (PBYTE)GetSHMEntry();
    return (PMonitorConfigureSet)(_Entry+sizeof(GalaHeadDesp));
}

int ConfigurePlt::GetWRKGRPStatus(int idx)
{
    if(idx < 0 || idx > MONITOR_MAX)
        return -1;
    int length = sizeof(MonitorConfigure)+sizeof(GalaHeadDesp);
    CHAR TempMem[length+1];
    PGalaGrpCfg pCFG = GetGrpCfg(idx);
    char szTemp[1024+1];
    memset(szTemp,0x00,sizeof(szTemp));
    char szTempPath[1024];
    CRT_memset(szTempPath,0x00,sizeof(szTempPath));
    CRT_strcpy(szTempPath,_PltFile.c_str());
    char *pszTemp = CRT_strrchr(szTempPath,'/');
    *pszTemp = '\0';
    CRT_snprintf(szTemp,sizeof(szTemp),"%s/%s",szTempPath,pCFG->Configure);
    if (CRT_access((const char *)szTemp, R_OK) == -1)
    {
        assert(USABLE_DISABLED == 0);
        return USABLE_DISABLED;
    }
    CFile _file((CPSTR)szTemp,O_SYNC|O_RDONLY,S_IRUSR|S_IWUSR);
    memset((char *)TempMem,0x00,sizeof(TempMem));
    _file.Read(TempMem,length);
    PMonitorConfigure pEntry = (PMonitorConfigure)(TempMem+sizeof(GalaHeadDesp));
    return *pEntry->_MonitorList_Monitor_Usable;
}

PGalaGrpCfg ConfigurePlt::GetGrpCfg(INT idx)
{
    PMonitorConfigureSet pSet = GetWorkGrpSet();
    return &pSet->_GrpCfg[idx];
}

void ConfigurePlt::Put(const string &path,char *value)
{
    string szPath = WRKGRPReady(path);
    if(!_GRP->GetObject())
    {
        THROW_EXCEPTION(1,"the object invalid");
    }
    _GRP->GetObject()->Put(szPath,value);
}

string ConfigurePlt::Get(const string &path)
{
    string szPath = WRKGRPReady(path);
    if(!_GRP->GetObject())
    {
        THROW_EXCEPTION(1,"the object invalid");
    }
    return _GRP->GetObject()->Get(szPath);
}

PBYTE ConfigurePlt::GetLowerLeavel(int idx)
{
    if(idx < 0 || idx > MONITOR_MAX)
    {
        THROW_EXCEPTION(1,"the idx invalid");
    }
    if(!_GRP.get())
    {
        GroupId = idx;
        char szTemp[1024];
        PGalaGrpCfg pCFG = GetGrpCfg(idx);
        if(CRT_strlen((char *)pCFG->Configure) == 0)
        {
            char szMsg[32];
            memset(szMsg,0x00,sizeof(szMsg));
            CRT_snprintf(szMsg,sizeof(szMsg),"%d",idx);
            THROW_EXCEPTION(1,"the Configure's file of Group"+string(szMsg)+" not exist");
        }
        CRT_memset(szTemp,0x00,sizeof(szTemp));
        char szTempPath[1024];
        CRT_memset(szTempPath,0x00,sizeof(szTempPath));
        CRT_strcpy(szTempPath,_PltFile.c_str());
        char *pszTemp = CRT_strrchr(szTempPath,'/');
        *pszTemp = '\0';
        CRT_snprintf(szTemp,sizeof(szTemp),"%s/%s",szTempPath,pCFG->Configure);
        _GRP.reset(new GalaConfigureGRP(szTemp,_Prot));
    }
    return _GRP->GetObject()->get_MonitorList_Monitor_LowLoger();
}

std::pair<std::string,std::string> ConfigurePlt::Get_ex(const string &path)
{
	string szPath = WRKGRPReady(path);
    if(!_GRP->GetObject())
    {
        THROW_EXCEPTION(1,"the object invalid");
    }
    return _GRP->GetObject()->Get_ex(szPath);
}

string ConfigurePlt::WRKGRPReady(const string &path)
{
    char szTemp[path.size()+1];
    CRT_memset(szTemp,0x00,sizeof(szTemp));
    CRT_strncpy((char *)szTemp,path.c_str(),path.size());
    string szPath;
    char *p = strchr((char *)szTemp,'[');
    if(NULL == p)
    {
        THROW_EXCEPTION(1,"do not exist \'[\' symbols");
    }
    *p = '\0';
    szPath += szTemp;
    int idx = atoi(++p);
    p = strchr(p++,']');
    if(NULL == p)
    {
        THROW_EXCEPTION(1,"do not match \']\' symbols");
    }
    szPath += (p+1);
    if((idx != GroupId) || (_GRP.get() == NULL))
    {
        GroupId = idx;
        PGalaGrpCfg pCFG = GetGrpCfg(idx);
        if(CRT_strlen((char *)pCFG->Configure) == 0)
        {
            char szMsg[32];
            memset(szMsg,0x00,sizeof(szMsg));
            CRT_snprintf(szMsg,sizeof(szMsg),"%d",idx);
            THROW_EXCEPTION(1,"the Configure's file of Group"+string(szMsg)+" not exist");
        }
        CRT_memset(szTemp,0x00,sizeof(szTemp));
        char szTempPath[1024];
        CRT_memset(szTempPath,0x00,sizeof(szTempPath));
        CRT_strcpy(szTempPath,_PltFile.c_str());
        char *pszTemp = CRT_strrchr(szTempPath,'/');
        *pszTemp = '\0';
        CRT_snprintf(szTemp,sizeof(szTemp),"%s/%s",szTempPath,pCFG->Configure);
        _GRP.reset(new GalaConfigureGRP(szTemp,_Prot));
    }
    if(!_GRP->GetObject())
    {
        THROW_EXCEPTION(1,"the object invalid");
    }
    return szPath;
}

map<xmlDocPtr,xmlNodePtr> ConfigurePlt::NodeEntry()
{
	xmlDocPtr doc = NULL;
    xmlNodePtr root=NULL,node=NULL,child=NULL;
    doc = xmlNewDoc (BAD_CAST "1.0");
    if(NULL ==doc)
    {
        THROW_EXCEPTION(1,"create document failure");
    }
    root = xmlNewNode(NULL,BAD_CAST "Configure");
    if(NULL ==root)
    {
        THROW_EXCEPTION(1,"create node failure");
    }
    xmlDocSetRootElement(doc,root);
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
    node = xmlAddChild(root,child);
    if(NULL ==node)
    {
        THROW_EXCEPTION(1,"create node failure");
    }
    child = xmlNewChild(node,NULL,BAD_CAST "MonitorSet",NULL);
    if(NULL ==child)
    {
        THROW_EXCEPTION(1,"create node failure");
    }
    map<xmlDocPtr,xmlNodePtr> ret;
    ret[doc] = child;
    return ret;
}

void ConfigurePlt::Save(const std::string &path)
{
    if(path.size() == 0)
    {
    	THROW_EXCEPTION(1,"xml save path is empty");
    }
    if(IsExistPath(path))
    {
    	map<xmlDocPtr,xmlNodePtr> items = NodeEntry();
    	if(items.size() == 0)
    	{
    		THROW_EXCEPTION(1, "save xml file failure");
    	}
    	map<xmlDocPtr,xmlNodePtr>::iterator iter;
    	xmlDocPtr doc = NULL ;
    	xmlNodePtr node = NULL ;
    	xmlSaveCtxtPtr ctxt = NULL;
    	int saveOpts = 0;
    	for(iter = items.begin();iter != items.end() ;++iter)
    	{
    		doc = iter->first;
    		node = iter->second;
    	}
    	if(!doc || !node)
    	{
    		THROW_EXCEPTION(1, "save xml file failure");
    	}
    	for(int ii = 0 ; ii != MONITOR_MAX; ++ii)
    	{
    		char szTemp[1024+1];
    		PGalaGrpCfg pCFG = GetGrpCfg(ii);
    		if(GetWRKGRPStatus(ii) == 0)
    		{
    			continue;
    		}
        	CRT_memset(szTemp,0x00,sizeof(szTemp));
        	char szTempPath[1024];
            CRT_memset(szTempPath,0x00,sizeof(szTempPath));
            CRT_strcpy(szTempPath,_PltFile.c_str());
            char *pszTemp = CRT_strrchr(szTempPath,'/');
            *pszTemp = '\0';
        	CRT_snprintf(szTemp,sizeof(szTemp),"%s/%s",szTempPath,pCFG->Configure);
        	GalaConfigureGRP GRP(szTemp,PROT_READ);
        	char szNum[64+1];
    		memset(szNum,0x00,sizeof(szNum));
    		snprintf(szNum,sizeof(szNum),"%d",ii);
    		string nodeName;
    		nodeName += "Monitor";
    		nodeName += szNum;
        	xmlNodePtr child = xmlNewChild(node,NULL,BAD_CAST nodeName.c_str(),NULL);
    		if(NULL ==child)
    		{
    		    THROW_EXCEPTION(1,"create node failure");
    		}
    		xmlNewProp(child, BAD_CAST "Id", BAD_CAST szNum);
    		CXmlMonitorConfigure xmlGRP(GRP.GetObject());
    		xmlGRP.OutPut_Configure__STRUCTURE_MonitorLists_Monitor(child,"/Configure/_STRUCTURE/MonitorList/Monitor");
    	}
    	xmlSaveFormatFileEnc("ll",doc,(char *)doc->encoding,1);
    	saveOpts |= XML_SAVE_FORMAT;
    	ctxt = xmlSaveToFilename(path.c_str(),(char *)doc->encoding,saveOpts);
    	if (ctxt != NULL)
    	{
    	    if (xmlSaveDoc(ctxt, doc) < 0)
    	    {
    	        THROW_EXCEPTION(1,"failed save to path");
    	    }
    	    xmlSaveClose(ctxt);
    	}
    	else
    	{
    	    THROW_EXCEPTION(1,"failed save to path");
    	}
    	xmlFreeDoc(doc);
    	xmlCleanupParser();
    }
    else
    {
    	THROW_EXCEPTION(1, "The Directory of xml save path Not Exists ");
    }
}

bool ConfigurePlt::isMatch(int id ,xmlNodePtr node)
{
	if(id < 0 || id > MONITOR_MAX || !node)
	{
		THROW_EXCEPTION(1,"the param invalid");
	}
	xmlNodePtr child = node->children;
	bool isExist = false;
	while(NULL != child)
	{
		if(XML_ELEMENT_NODE != child->type)
		{
			child = child->next;
			continue;
		}
		char *attr = (char *)xmlGetProp(child,BAD_CAST "Id");
    	if(attr && id == atoi(attr))
    	{
    	    isExist = true;
    	    break;
    	}
		child = child->next;
	}
	return isExist;
}

void ConfigurePlt::Load(const std::string &path)
{
	if(path.size() == 0)
    {
    	THROW_EXCEPTION(1,"xml save path is empty");
    }
    if(IsExistPath(path))
    {
    	for(int ii = 0 ; ii != MONITOR_MAX; ++ii)
    	{
    		char szTemp[1024+1];
    		PGalaGrpCfg pCFG = GetGrpCfg(ii);
    		if(strlen((char *)pCFG->Configure) == 0)
    		{
    			continue;
    		}
        	CRT_memset(szTemp,0x00,sizeof(szTemp));
        	char szTempPath[1024];
            CRT_memset(szTempPath,0x00,sizeof(szTempPath));
            CRT_strcpy(szTempPath,_PltFile.c_str());
            char *pszTemp = CRT_strrchr(szTempPath,'/');
            *pszTemp = '\0';
        	CRT_snprintf(szTemp,sizeof(szTemp),"%s/%s",szTempPath,pCFG->Configure);
        	if(CRT_access(szTemp,0) == 0)
            {
                CRT_unlink(szTemp);
            }
        	GalaConfigureGRP GRP(szTemp,PROT_READ|PROT_WRITE);
    		CXmlMonitorConfigure xmlGRP(GRP.GetObject());
    		xmlGRP.LoadXmlConfig(path,true);
    		xmlNodePtr node = xmlGRP.xmlXpath("/Configure/_STRUCTURE/MonitorSet");
    		if(!node)
    		{
    			THROW_EXCEPTION(1, "load xml failure");
    		}
    		if(isMatch(ii,node))
    		{
    			char szNum[64+1];
    			memset(szNum,0x00,sizeof(szNum));
    			snprintf(szNum,sizeof(szNum),"%d",ii);
    			string xmlPath;
    			xmlPath += "/Configure/_STRUCTURE/MonitorList/Monitor";
    			xmlPath += szNum;
    			xmlGRP.InPut_Configure__STRUCTURE_MonitorLists_Monitor(xmlPath,"/Configure/_STRUCTURE/MonitorList/Monitor");
    		}
    	}
    }
    else
    {
    	THROW_EXCEPTION(1, "The Directory of xml save path Not Exists ");
    }
}

void ConfigurePlt::LoadGRP(const std::string &path,int idx)
{
	if(path.size() == 0 || idx <0 || idx > MONITOR_MAX)
    {
    	THROW_EXCEPTION(1,"monitor xml save path is empty");
    }
    char szTemp[1024+1];
    PGalaGrpCfg pCFG = GetGrpCfg(idx);
    if(strlen((char *)pCFG->Configure) == 0)
    {
    	THROW_EXCEPTION(1, "The Monitor Configure file  Not Exists ");
    }
    CRT_memset(szTemp,0x00,sizeof(szTemp));
    char szTempPath[1024];
    CRT_memset(szTempPath,0x00,sizeof(szTempPath));
    CRT_strcpy(szTempPath,_PltFile.c_str());
    char *pszTemp = CRT_strrchr(szTempPath,'/');
    *pszTemp = '\0';
    CRT_snprintf(szTemp,sizeof(szTemp),"%s/%s",szTempPath,pCFG->Configure);
    if(CRT_access(szTemp,0) == 0)
    {
        CRT_unlink(szTemp);
    }
    GalaConfigureGRP GRP(szTemp,PROT_READ|PROT_WRITE);
    CXmlMonitorConfigure xmlGRP(GRP.GetObject());
    xmlGRP.LoadXmlConfig(path);
}

void ConfigurePlt::SaveGRP(const std::string &path,int idx)
{
	if(path.size() == 0 || idx <0 || idx > MONITOR_MAX)
    {
    	THROW_EXCEPTION(1,"monitor xml save path is empty");
    }
    char szTemp[1024+1];
    PGalaGrpCfg pCFG = GetGrpCfg(idx);
    if(strlen((char *)pCFG->Configure) == 0)
    {
    	THROW_EXCEPTION(1, "The Monitor Configure file  Not Exists ");
    }
    CRT_memset(szTemp,0x00,sizeof(szTemp));
    char szTempPath[1024];
    CRT_memset(szTempPath,0x00,sizeof(szTempPath));
    CRT_strcpy(szTempPath,_PltFile.c_str());
    char *pszTemp = CRT_strrchr(szTempPath,'/');
    *pszTemp = '\0';
    CRT_snprintf(szTemp,sizeof(szTemp),"%s/%s",szTempPath,pCFG->Configure);
    GalaConfigureGRP GRP(szTemp,PROT_READ);
    CXmlMonitorConfigure xmlGRP(GRP.GetObject());
    xmlGRP.WriteConfig2Xml(path);
}
