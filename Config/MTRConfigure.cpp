#include "MTRConfigure.hpp"


using namespace std;
using namespace Galaxy::GalaxyRT;

CMonitorConfigure::CMonitorConfigure(PBYTE memAddr,int32_t size)
    :_MemoryAddr(memAddr),_Size(size)
{
    if(NULL == _MemoryAddr || _Size < (int32_t)sizeof(MonitorConfigure))
    {
        THROW_SYSERROR();
    }
    InitializeMap();
}
CMonitorConfigure::~CMonitorConfigure()
{
    _MemoryAddr = NULL;
}
void CMonitorConfigure::InitializeMap()
{
    _Mapping["MonitorList_Monitor_Base_Alias"] = 1;
    _Mapping["MonitorList_Monitor_Base_Token"] = 2;
    _Mapping["MonitorList_Monitor_Base_SVCLimits"] = 3;
    _Mapping["MonitorList_Monitor_Base_Usable"] = 4;
    _Mapping["MonitorList_Monitor_LowLoger"] = 5;
    _Mapping["MonitorList_Monitor_AppLoger_Level"] = 6;
    _Mapping["MonitorList_Monitor_AppLoger_Details"] = 7;
    _Mapping["MonitorList_Monitor_AppLoger_Size"] = 8;
    _Mapping["MonitorList_Monitor_AppLoger_Files"] = 9;
    _Mapping["MonitorList_Monitor_AppLoger_Dir"] = 10;
    _Mapping["MonitorList_Monitor_AppLoger_NameFormat"] = 11;
    _Mapping["MonitorList_Monitor_AppLoger_DirFormat"] = 12;
    _Mapping["MonitorList_Monitor_AppLoger_Usable"] = 13;
    _Mapping["MonitorList_Monitor_SVC_Threads"] = 14;
    _Mapping["MonitorList_Monitor_SVC_IsFlush"] = 15;
    _Mapping["MonitorList_Monitor_SVC_DBConnection__SCROLLTYPE"] = 16;
    _Mapping["MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name"] = 17;
    _Mapping["MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User"] = 18;
    _Mapping["MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd"] = 19;
    _Mapping["MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable"] = 20;
    _Mapping["MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name"] = 21;
    _Mapping["MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User"] = 22;
    _Mapping["MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd"] = 23;
    _Mapping["MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable"] = 24;
    _Mapping["MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile"] = 25;
    _Mapping["MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable"] = 26;
    _Mapping["MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable"] = 27;
    _Mapping["MonitorList_Monitor_SVC_DBConnection_Usable"] = 28;
    _Mapping["MonitorList_Monitor_SVC_Usable"] = 29;
    _Mapping["MonitorList_Monitor_LSRList_Usable"] = 30;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Alias"] = 31;
    _Mapping["MonitorList_Monitor_LSRList_LSR_ImplDllName"] = 32;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Threads"] = 33;
    _Mapping["MonitorList_Monitor_LSRList_LSR_ThreadStackSize"] = 34;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__SCROLLTYPE"] = 35;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE"] = 36;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP"] = 37;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port"] = 38;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable"] = 39;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token"] = 40;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable"] = 41;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable"] = 42;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable"] = 43;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut"] = 44;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO"] = 45;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName"] = 46;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable"] = 47;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable"] = 48;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Protocol_Usable"] = 49;
    _Mapping["MonitorList_Monitor_LSRList_LSR_Usable"] = 50;
    _Mapping["MonitorList_Monitor_CNRList_Usable"] = 51;
    _Mapping["MonitorList_Monitor_CNRList_CNR_Alias"] = 52;
    _Mapping["MonitorList_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE"] = 53;
    _Mapping["MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName"] = 54;
    _Mapping["MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable"] = 55;
    _Mapping["MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName"] = 56;
    _Mapping["MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable"] = 57;
    _Mapping["MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName"] = 58;
    _Mapping["MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable"] = 59;
    _Mapping["MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName"] = 60;
    _Mapping["MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable"] = 61;
    _Mapping["MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable"] = 62;
    _Mapping["MonitorList_Monitor_CNRList_CNR_PlugIn_Usable"] = 63;
    _Mapping["MonitorList_Monitor_CNRList_CNR_Usable"] = 64;
    _Mapping["MonitorList_Monitor_Usable"] = 65;
}
bool CMonitorConfigure::Isdidgit(const string &str)
{
    unsigned int ii = 0;
    for(ii = 0 ; ii < str.size();ii++)
    {
        if(!isdigit(str[ii]))
        {
            return false;
        }
    }
    return true;
}
bool CMonitorConfigure::IsIPAddr(const string &ip)
{
    if(ip.size()==0)
    {
        return false;
    }
    struct in_addr addr;
    if(inet_pton (AF_INET,ip.c_str(),&addr) == 0)
    {
        if(inet_pton (AF_INET6,ip.c_str(),&addr) == 0)
        {
            return false;
        }
    }
    return true;
}
bool CMonitorConfigure::IsPath(const string &path)
{
    if(path.size()==0)
    {
        return false;
    }
    char *p = NULL ,*pre = NULL ;
    int space = 0;
    p = (char *)path.c_str();
    while(*p && (p - path.c_str()) != (int)path.size())
    {
        if(*p != ' ')
        {
            pre = p;
            space = (int)(p - path.c_str());
            break;
        }
        p++;
    }
    while(*pre && (pre - p) != (int)(path.size()-space))
    {
        if(*pre == ' ')
        {
            return false;
        }
        pre++;
    }
    return true;
}
void CMonitorConfigure::PathAnalyze(const string &path,string &name,vector<string> &param)
{
    if(path.size()==0)
    {
        return ;
    }
    char tmp[path.size()+1];
    memset(tmp,0x00,sizeof(tmp));
    memcpy((char *)tmp,path.c_str(),path.size());
    char *p = NULL;
    char *pre = NULL;
    pre =(char *)tmp;
    p = (char *)tmp;
    int count = 0;
    while(1)
    {
        p = strchr(p,'[');
        if(NULL != p)
        {
            *p = '\0';
            name += pre;
            pre = p+1;
            p = strchr(pre,']');
            if(NULL != p)
            {
                *p = '\0';
                if(!Isdidgit(pre))
                {
                    THROW_EXCEPTION(1,"the idex of path is not numeric");
                }
                param.push_back(pre);
                pre = p+1;
            }
        }
        else
        {
            name += pre;
            break;
        }
        p++;
        count++;
    }
}
string CMonitorConfigure::GetEntryName(const string &path)
{
    string fun;
    char *pre=NULL,*tail = NULL ;
    char tmp[path.size()+1];
    if(path.size()!=0)
    {
        memset(tmp,0x00,sizeof(tmp));
        memcpy((char *)tmp,path.c_str(),path.size());
        pre = (char *)tmp;
        tail = (char *)tmp+1;
        while(*tail && (tail - path.c_str()) != (int)path.size())
        {
            if(*tail == '/')
            {
                if(!strncmp(pre+1,"_STRUCTURE",tail-pre-1))
                {
                    fun += (++tail);
                    break;
                }
                pre += (tail - pre);
                tail++;
            }
            else
            {
                tail++;
            }
        }
    }
    return fun;
}
string CMonitorConfigure::Entry2Method(const string &path)
{
    char tmp[path.size()+1];
    memset(tmp,0x00,sizeof(tmp));
    memcpy(tmp,path.c_str(),path.size());
    char *p = (char *)tmp;
    while(*p)
    {
        if(*p == '/')
        {
            *p = '_';
        }
        p++;
    }
    return (char *)tmp;
}
int32_t CMonitorConfigure::Atoi(const string &value)
{
    return (int32_t)atoi(value.c_str());
}
char *CMonitorConfigure::GetNodeName(char *path)
{
    if(NULL == path||path[0]!='/')
    {
        THROW_EXCEPTION(1,"the param is empty");
    }
    char *p = (char *)path;
    char *pre = p;
    while(*p)
    {
        pre = p;
        p = strchr(p,'/');
        if(NULL == p)
        {
            break;
        }
        p++;
    }
    *(pre-1) = '\0';
    return pre;
}
PMonitorConfigure CMonitorConfigure::get_MonitorConfigure()
{
    return (PMonitorConfigure)_MemoryAddr;
}
PBase CMonitorConfigure::get_MonitorList_Monitor_Base()
{
    return (PBase)&get_MonitorConfigure()->_MonitorList_Monitor_Base;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_Base_Alias()
{
    return (PBYTE)&get_MonitorList_Monitor_Base()->_MonitorList_Monitor_Base_Alias;
}
void CMonitorConfigure::set_MonitorList_Monitor_Base_Alias(PBYTE Alias)
{
    if(NULL == Alias || strlen((char *)Alias)>32)
    {
        THROW_EXCEPTION(1,"the size of Alias is larger than the max of 32");
    }
    memcpy((char *)get_MonitorList_Monitor_Base()->_MonitorList_Monitor_Base_Alias,(char *)Alias,32);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_Base_Token()
{
    return (PBYTE)&get_MonitorList_Monitor_Base()->_MonitorList_Monitor_Base_Token;
}
void CMonitorConfigure::set_MonitorList_Monitor_Base_Token(PBYTE Token)
{
    if(NULL == Token || strlen((char *)Token)>32)
    {
        THROW_EXCEPTION(1,"the size of Token is larger than the max of 32");
    }
    if(!IsPath((char *)Token))
    {
        THROW_EXCEPTION(1,"the Token is not path format");
    }
    memcpy((char *)get_MonitorList_Monitor_Base()->_MonitorList_Monitor_Base_Token,(char *)Token,32);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_Base_SVCLimits()
{
    return (PBYTE)&get_MonitorList_Monitor_Base()->_MonitorList_Monitor_Base_SVCLimits;
}
void CMonitorConfigure::set_MonitorList_Monitor_Base_SVCLimits(int32_t SVCLimits)
{
    *(int32_t *)get_MonitorList_Monitor_Base()->_MonitorList_Monitor_Base_SVCLimits = SVCLimits;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_Base_Usable()
{
    return (PBYTE)&get_MonitorList_Monitor_Base()->_MonitorList_Monitor_Base_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_Base_Usable(bool Enable,bool isLoad)
{
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_Base()->_MonitorList_Monitor_Base_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LowLoger()
{
    return (PBYTE)&get_MonitorConfigure()->_MonitorList_Monitor_LowLoger;
}
void CMonitorConfigure::set_MonitorList_Monitor_LowLoger(LevelType::LevelType LowLoger)
{
    *get_MonitorConfigure()->_MonitorList_Monitor_LowLoger = LowLoger;
}
PAppLoger CMonitorConfigure::get_MonitorList_Monitor_AppLoger()
{
    return (PAppLoger)&get_MonitorConfigure()->_MonitorList_Monitor_AppLoger;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_AppLoger_Level()
{
    return (PBYTE)&get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_Level;
}
void CMonitorConfigure::set_MonitorList_Monitor_AppLoger_Level(LevelType::LevelType Level)
{
    *get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_Level = Level;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_AppLoger_Details()
{
    return (PBYTE)&get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_Details;
}
void CMonitorConfigure::set_MonitorList_Monitor_AppLoger_Details(Switch::Switch Details)
{
    *get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_Details = Details;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_AppLoger_Size()
{
    return (PBYTE)&get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_Size;
}
void CMonitorConfigure::set_MonitorList_Monitor_AppLoger_Size(int32_t Size)
{
    *(int32_t *)get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_Size = Size;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_AppLoger_Files()
{
    return (PBYTE)&get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_Files;
}
void CMonitorConfigure::set_MonitorList_Monitor_AppLoger_Files(int32_t Files)
{
    *(int32_t *)get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_Files = Files;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_AppLoger_Dir()
{
    return (PBYTE)&get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_Dir;
}
void CMonitorConfigure::set_MonitorList_Monitor_AppLoger_Dir(PBYTE Dir)
{
    if(NULL == Dir || strlen((char *)Dir)>128)
    {
        THROW_EXCEPTION(1,"the size of Dir is larger than the max of 128");
    }
    memcpy((char *)get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_Dir,(char *)Dir,128);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_AppLoger_NameFormat()
{
    return (PBYTE)&get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_NameFormat;
}
void CMonitorConfigure::set_MonitorList_Monitor_AppLoger_NameFormat(AppLogNameType::AppLogNameType NameFormat)
{
    *get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_NameFormat = NameFormat;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_AppLoger_DirFormat()
{
    return (PBYTE)&get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_DirFormat;
}
void CMonitorConfigure::set_MonitorList_Monitor_AppLoger_DirFormat(AppLogDirType::AppLogDirType DirFormat)
{
    *get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_DirFormat = DirFormat;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_AppLoger_Usable()
{
    return (PBYTE)&get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_AppLoger_Usable(bool Enable,bool isLoad)
{
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_AppLoger()->_MonitorList_Monitor_AppLoger_Usable = Enable;
}
PSVC CMonitorConfigure::get_MonitorList_Monitor_SVC()
{
    return (PSVC)&get_MonitorConfigure()->_MonitorList_Monitor_SVC;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_Threads()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_Threads;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_Threads(int32_t Threads)
{
    *(int32_t *)get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_Threads = Threads;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_IsFlush()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_IsFlush;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_IsFlush(bool IsFlush)
{
    *get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_IsFlush = IsFlush;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection__SCROLLTYPE()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__SCROLLTYPE;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection__SCROLLTYPE(DBType::DBType _SCROLLTYPE)
{
    *get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__SCROLLTYPE = _SCROLLTYPE;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name(PBYTE Name)
{
    if(NULL == Name || strlen((char *)Name)>30)
    {
        THROW_EXCEPTION(1,"the size of Name is larger than the max of 30");
    }
    memcpy((char *)get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name,(char *)Name,30);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User(PBYTE User)
{
    if(NULL == User || strlen((char *)User)>30)
    {
        THROW_EXCEPTION(1,"the size of User is larger than the max of 30");
    }
    memcpy((char *)get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User,(char *)User,30);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd(PBYTE Passwd)
{
    if(NULL == Passwd || strlen((char *)Passwd)>30)
    {
        THROW_EXCEPTION(1,"the size of Passwd is larger than the max of 30");
    }
    memcpy((char *)get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd,(char *)Passwd,30);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable(bool Enable,bool isLoad)
{
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name(PBYTE Name)
{
    if(NULL == Name || strlen((char *)Name)>30)
    {
        THROW_EXCEPTION(1,"the size of Name is larger than the max of 30");
    }
    memcpy((char *)get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name,(char *)Name,30);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User(PBYTE User)
{
    if(NULL == User || strlen((char *)User)>30)
    {
        THROW_EXCEPTION(1,"the size of User is larger than the max of 30");
    }
    memcpy((char *)get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User,(char *)User,30);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd(PBYTE Passwd)
{
    if(NULL == Passwd || strlen((char *)Passwd)>30)
    {
        THROW_EXCEPTION(1,"the size of Passwd is larger than the max of 30");
    }
    memcpy((char *)get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd,(char *)Passwd,30);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable(bool Enable,bool isLoad)
{
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile(PBYTE DBFile)
{
    if(NULL == DBFile || strlen((char *)DBFile)>256)
    {
        THROW_EXCEPTION(1,"the size of DBFile is larger than the max of 256");
    }
    if(!IsPath((char *)DBFile))
    {
        THROW_EXCEPTION(1,"the DBFile is not path format");
    }
    memcpy((char *)get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile,(char *)DBFile,256);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable(bool Enable,bool isLoad)
{
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable(bool Enable,bool isLoad)
{
    switch(*get_MonitorList_Monitor_SVC_DBConnection__SCROLLTYPE())
    {
        case(DBType::ORACLE):
        {
            bool value = *(bool *)get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable();
            if(!value)
            {
                Enable = false;
            }
            break;
        }
        case(DBType::DB2):
        {
            bool value = *(bool *)get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable();
            if(!value)
            {
                Enable = false;
            }
            break;
        }
        case(DBType::SQLITE):
        {
            bool value = *(bool *)get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable();
            if(!value)
            {
                Enable = false;
            }
            break;
        }
    }
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_DBConnection_Usable()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_DBConnection_Usable(bool Enable,bool isLoad)
{
    bool value0 = *(bool *)get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable();
    if(!Enable || !value0)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_DBConnection_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_SVC_Usable()
{
    return (PBYTE)&get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_SVC_Usable(bool Enable,bool isLoad)
{
    bool value0 = *(bool *)get_MonitorList_Monitor_SVC_DBConnection_Usable();
    if(!Enable || !value0)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_SVC()->_MonitorList_Monitor_SVC_Usable = Enable;
}
PLSRList CMonitorConfigure::get_MonitorList_Monitor_LSRList(int32_t LSRListIdx)
{
    return (PLSRList)&get_MonitorConfigure()->_MonitorList_Monitor_LSRList[LSRListIdx];
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_Usable(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_Usable(int32_t LSRListIdx,bool Enable,bool isLoad)
{
    bool value0 = *(bool *)get_MonitorList_Monitor_LSRList_LSR_Usable(LSRListIdx);
    if(!Enable || !value0)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Alias(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Alias;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Alias(int32_t LSRListIdx,PBYTE Alias)
{
    if(NULL == Alias || strlen((char *)Alias)>32)
    {
        THROW_EXCEPTION(1,"the size of Alias is larger than the max of 32");
    }
    memcpy((char *)get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Alias,(char *)Alias,32);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_ImplDllName(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_ImplDllName;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_ImplDllName(int32_t LSRListIdx,PBYTE ImplDllName)
{
    if(NULL == ImplDllName || strlen((char *)ImplDllName)>32)
    {
        THROW_EXCEPTION(1,"the size of ImplDllName is larger than the max of 32");
    }
    memcpy((char *)get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_ImplDllName,(char *)ImplDllName,32);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Threads(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Threads;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Threads(int32_t LSRListIdx,int32_t Threads)
{
    *(int32_t *)get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Threads = Threads;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_ThreadStackSize(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_ThreadStackSize;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_ThreadStackSize(int32_t LSRListIdx,int32_t ThreadStackSize)
{
    *(int32_t *)get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_ThreadStackSize = ThreadStackSize;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__SCROLLTYPE;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(int32_t LSRListIdx,ListenerType::ListenerType _SCROLLTYPE)
{
    *get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__SCROLLTYPE = _SCROLLTYPE;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(int32_t LSRListIdx,SocketType::SocketType _SCROLLTYPE)
{
    *get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE = _SCROLLTYPE;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP(int32_t LSRListIdx,PBYTE IP)
{
    if(NULL == IP || strlen((char *)IP)>16)
    {
        THROW_EXCEPTION(1,"the size of IP is larger than the max of 16");
    }
    if(!IsIPAddr((char *)IP))
    {
        THROW_EXCEPTION(1,"the IP is not IP format");
    }
    memcpy((char *)get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP,(char *)IP,16);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port(int32_t LSRListIdx,int32_t Port)
{
    *(int32_t *)get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port = Port;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable(int32_t LSRListIdx,bool Enable,bool isLoad)
{
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token(int32_t LSRListIdx,PBYTE Token)
{
    if(NULL == Token || strlen((char *)Token)>32)
    {
        THROW_EXCEPTION(1,"the size of Token is larger than the max of 32");
    }
    memcpy((char *)get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token,(char *)Token,32);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable(int32_t LSRListIdx,bool Enable,bool isLoad)
{
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable(int32_t LSRListIdx,bool Enable,bool isLoad)
{
    switch(*get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(LSRListIdx))
    {
        case(SocketType::TCP):
        {
            bool value = *(bool *)get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable(LSRListIdx);
            if(!value)
            {
                Enable = false;
            }
            break;
        }
        case(SocketType::UNIX):
        {
            bool value = *(bool *)get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable(LSRListIdx);
            if(!value)
            {
                Enable = false;
            }
            break;
        }
    }
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable(int32_t LSRListIdx,bool Enable,bool isLoad)
{
    bool value0 = *(bool *)get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable(LSRListIdx);
    if(!Enable || !value0)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut(int32_t LSRListIdx,int32_t RecvTimeOut)
{
    *(int32_t *)get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut = RecvTimeOut;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO(int32_t LSRListIdx,int32_t ExchangeTMO)
{
    *(int32_t *)get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO = ExchangeTMO;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName(int32_t LSRListIdx,PBYTE PTCDllName)
{
    if(NULL == PTCDllName || strlen((char *)PTCDllName)>32)
    {
        THROW_EXCEPTION(1,"the size of PTCDllName is larger than the max of 32");
    }
    memcpy((char *)get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName,(char *)PTCDllName,32);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable(int32_t LSRListIdx,bool Enable,bool isLoad)
{
    bool value0 = *(bool *)get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable(LSRListIdx);
    if(!Enable || !value0)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable(int32_t LSRListIdx,bool Enable,bool isLoad)
{
    switch(*get_MonitorList_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(LSRListIdx))
    {
        case(ListenerType::LSRSTREAM):
        {
            bool value = *(bool *)get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable(LSRListIdx);
            if(!value)
            {
                Enable = false;
            }
            break;
        }
    }
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Protocol_Usable(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Protocol_Usable(int32_t LSRListIdx,bool Enable,bool isLoad)
{
    bool value0 = *(bool *)get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable(LSRListIdx);
    if(!Enable || !value0)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Protocol_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_LSRList_LSR_Usable(int32_t LSRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_LSRList_LSR_Usable(int32_t LSRListIdx,bool Enable,bool isLoad)
{
    bool value0 = *(bool *)get_MonitorList_Monitor_LSRList_LSR_Protocol_Usable(LSRListIdx);
    if(!Enable || !value0)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_LSRList(LSRListIdx)->_MonitorList_Monitor_LSRList_LSR_Usable = Enable;
}
PCNRList CMonitorConfigure::get_MonitorList_Monitor_CNRList(int32_t CNRListIdx)
{
    return (PCNRList)&get_MonitorConfigure()->_MonitorList_Monitor_CNRList[CNRListIdx];
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_Usable(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_Usable(int32_t CNRListIdx,bool Enable,bool isLoad)
{
    bool value0 = *(bool *)get_MonitorList_Monitor_CNRList_CNR_Usable(CNRListIdx);
    if(!Enable || !value0)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_Alias(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_Alias;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_Alias(int32_t CNRListIdx,PBYTE Alias)
{
    if(NULL == Alias || strlen((char *)Alias)>32)
    {
        THROW_EXCEPTION(1,"the size of Alias is larger than the max of 32");
    }
    memcpy((char *)get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_Alias,(char *)Alias,32);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(int32_t CNRListIdx,ConectorType::ConectorType _SCROLLTYPE)
{
    *get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE = _SCROLLTYPE;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName(int32_t CNRListIdx,PBYTE ImplDllName)
{
    if(NULL == ImplDllName || strlen((char *)ImplDllName)>32)
    {
        THROW_EXCEPTION(1,"the size of ImplDllName is larger than the max of 32");
    }
    memcpy((char *)get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName,(char *)ImplDllName,32);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable(int32_t CNRListIdx,bool Enable,bool isLoad)
{
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName(int32_t CNRListIdx,PBYTE ImplDllName)
{
    if(NULL == ImplDllName || strlen((char *)ImplDllName)>32)
    {
        THROW_EXCEPTION(1,"the size of ImplDllName is larger than the max of 32");
    }
    memcpy((char *)get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName,(char *)ImplDllName,32);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable(int32_t CNRListIdx,bool Enable,bool isLoad)
{
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName(int32_t CNRListIdx,PBYTE ImplDllName)
{
    if(NULL == ImplDllName || strlen((char *)ImplDllName)>32)
    {
        THROW_EXCEPTION(1,"the size of ImplDllName is larger than the max of 32");
    }
    memcpy((char *)get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName,(char *)ImplDllName,32);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable(int32_t CNRListIdx,bool Enable,bool isLoad)
{
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName(int32_t CNRListIdx,PBYTE ImplDllName)
{
    if(NULL == ImplDllName || strlen((char *)ImplDllName)>32)
    {
        THROW_EXCEPTION(1,"the size of ImplDllName is larger than the max of 32");
    }
    memcpy((char *)get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName,(char *)ImplDllName,32);
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable(int32_t CNRListIdx,bool Enable,bool isLoad)
{
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable(int32_t CNRListIdx,bool Enable,bool isLoad)
{
    switch(*get_MonitorList_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(CNRListIdx))
    {
        case(ConectorType::ECHO):
        {
            bool value = *(bool *)get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable(CNRListIdx);
            if(!value)
            {
                Enable = false;
            }
            break;
        }
        case(ConectorType::TIMEOUT):
        {
            bool value = *(bool *)get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable(CNRListIdx);
            if(!value)
            {
                Enable = false;
            }
            break;
        }
        case(ConectorType::SQUARE):
        {
            bool value = *(bool *)get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable(CNRListIdx);
            if(!value)
            {
                Enable = false;
            }
            break;
        }
        case(ConectorType::STATE):
        {
            bool value = *(bool *)get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable(CNRListIdx);
            if(!value)
            {
                Enable = false;
            }
            break;
        }
    }
    if(!Enable)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_PlugIn_Usable(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_PlugIn_Usable(int32_t CNRListIdx,bool Enable,bool isLoad)
{
    bool value0 = *(bool *)get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable(CNRListIdx);
    if(!Enable || !value0)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_PlugIn_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_CNRList_CNR_Usable(int32_t CNRListIdx)
{
    return (PBYTE)&get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_CNRList_CNR_Usable(int32_t CNRListIdx,bool Enable,bool isLoad)
{
    bool value0 = *(bool *)get_MonitorList_Monitor_CNRList_CNR_PlugIn_Usable(CNRListIdx);
    if(!Enable || !value0)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorList_Monitor_CNRList(CNRListIdx)->_MonitorList_Monitor_CNRList_CNR_Usable = Enable;
}
PBYTE CMonitorConfigure::get_MonitorList_Monitor_Usable()
{
    return (PBYTE)&get_MonitorConfigure()->_MonitorList_Monitor_Usable;
}
void CMonitorConfigure::set_MonitorList_Monitor_Usable(bool Enable,bool isLoad)
{
    bool value0 = *(bool *)get_MonitorList_Monitor_Base_Usable();
    bool value1 = *(bool *)get_MonitorList_Monitor_AppLoger_Usable();
    bool value2 = *(bool *)get_MonitorList_Monitor_SVC_Usable();
    for(int ii =0 ; ii < 64; ++ii)
    {
        bool value3 = *(bool *)get_MonitorList_Monitor_LSRList_Usable(ii);
        if(!value3)
        {
            Enable = false;
            break;
        }
    }
    for(int ii =0 ; ii < 64; ++ii)
    {
        bool value3 = *(bool *)get_MonitorList_Monitor_CNRList_Usable(ii);
        if(!value3)
        {
            Enable = false;
            break;
        }
    }
    if(!Enable || !value0 || !value1 || !value2)
    {
        Enable = false ;
    }
    if(isLoad)
    {
        Enable = true;
    }
    *get_MonitorConfigure()->_MonitorList_Monitor_Usable = Enable;
}
string CMonitorConfigure::Get(const string &path)
{
    return Get_ex(path).second;
}
std::pair<std::string,std::string> CMonitorConfigure::Get_ex(const string &path)
{
    if(path.size()==0)
    {
        THROW_EXCEPTION(1,"the path is empty,pleas check the path!");
    }
    string name;
    std::pair<std::string,std::string> szValue;
    vector<string> param;
    PathAnalyze(path,name,param);
    string str = Entry2Method(GetEntryName(name));
    if(str.size()==0)
    {
        THROW_EXCEPTION(1,"do not match function for ""+path+""");
    }
    PBYTE ret = NULL;
    switch(_Mapping[str.c_str()])
    {
        case(1):
        {
            ret = get_MonitorList_Monitor_Base_Alias();
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(2):
        {
            ret = get_MonitorList_Monitor_Base_Token();
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(3):
        {
            ret = get_MonitorList_Monitor_Base_SVCLimits();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*(int32_t *)ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "int32_t";
            break;
        }
        case(4):
        {
            ret = get_MonitorList_Monitor_Base_Usable();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(5):
        {
            ret = get_MonitorList_Monitor_LowLoger();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "LevelType::LevelType";
            break;
        }
        case(6):
        {
            ret = get_MonitorList_Monitor_AppLoger_Level();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "LevelType::LevelType";
            break;
        }
        case(7):
        {
            ret = get_MonitorList_Monitor_AppLoger_Details();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "Switch::Switch";
            break;
        }
        case(8):
        {
            ret = get_MonitorList_Monitor_AppLoger_Size();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*(int32_t *)ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "int32_t";
            break;
        }
        case(9):
        {
            ret = get_MonitorList_Monitor_AppLoger_Files();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*(int32_t *)ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "int32_t";
            break;
        }
        case(10):
        {
            ret = get_MonitorList_Monitor_AppLoger_Dir();
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(11):
        {
            ret = get_MonitorList_Monitor_AppLoger_NameFormat();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "AppLogNameType::AppLogNameType";
            break;
        }
        case(12):
        {
            ret = get_MonitorList_Monitor_AppLoger_DirFormat();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "AppLogDirType::AppLogDirType";
            break;
        }
        case(13):
        {
            ret = get_MonitorList_Monitor_AppLoger_Usable();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(14):
        {
            ret = get_MonitorList_Monitor_SVC_Threads();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*(int32_t *)ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "int32_t";
            break;
        }
        case(15):
        {
            ret = get_MonitorList_Monitor_SVC_IsFlush();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(16):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection__SCROLLTYPE();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "DBType::DBType";
            break;
        }
        case(17):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name();
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(18):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User();
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(19):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd();
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(20):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(21):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name();
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(22):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User();
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(23):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd();
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(24):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(25):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile();
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(26):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(27):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(28):
        {
            ret = get_MonitorList_Monitor_SVC_DBConnection_Usable();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(29):
        {
            ret = get_MonitorList_Monitor_SVC_Usable();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(30):
        {
            ret = get_MonitorList_Monitor_LSRList_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(31):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Alias(Atoi(param[0]));
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(32):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_ImplDllName(Atoi(param[0]));
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(33):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Threads(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*(int32_t *)ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "int32_t";
            break;
        }
        case(34):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_ThreadStackSize(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*(int32_t *)ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "int32_t";
            break;
        }
        case(35):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "ListenerType::ListenerType";
            break;
        }
        case(36):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "SocketType::SocketType";
            break;
        }
        case(37):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP(Atoi(param[0]));
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(38):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*(int32_t *)ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "int32_t";
            break;
        }
        case(39):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(40):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token(Atoi(param[0]));
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(41):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(42):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(43):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(44):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*(int32_t *)ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "int32_t";
            break;
        }
        case(45):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*(int32_t *)ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "int32_t";
            break;
        }
        case(46):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName(Atoi(param[0]));
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(47):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(48):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(49):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Protocol_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(50):
        {
            ret = get_MonitorList_Monitor_LSRList_LSR_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(51):
        {
            ret = get_MonitorList_Monitor_CNRList_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(52):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_Alias(Atoi(param[0]));
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(53):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "ConectorType::ConectorType";
            break;
        }
        case(54):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName(Atoi(param[0]));
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(55):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(56):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName(Atoi(param[0]));
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(57):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(58):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName(Atoi(param[0]));
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(59):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(60):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName(Atoi(param[0]));
            char szValueTemp[strlen((char *)ret)+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            memcpy((char *)szValueTemp,(char *)ret,strlen((char *)ret));
            szValue.second = (string)szValueTemp;
            szValue.first = "string";
            break;
        }
        case(61):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(62):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(63):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_PlugIn_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(64):
        {
            ret = get_MonitorList_Monitor_CNRList_CNR_Usable(Atoi(param[0]));
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        case(65):
        {
            ret = get_MonitorList_Monitor_Usable();
            char szValueTemp[32+1];
            memset(szValueTemp,0x00,sizeof(szValueTemp));
            snprintf(szValueTemp,sizeof(szValueTemp),"%d",*ret);
            szValue.second = (string)szValueTemp;
            szValue.first = "bool";
            break;
        }
        default:
        {
            THROW_EXCEPTION(1,"do not match function for ""+path+""");
        }
    }
    return szValue;
}
void CMonitorConfigure::Put(const string &path,char *value,bool isLoad)
{
    if(path.size()==0|| NULL ==value)
    {
        THROW_EXCEPTION(1,"the path is empty,or value is empty!");
    }
    string name;
    vector<string> param;
    PathAnalyze(path,name,param);
    string str = Entry2Method(GetEntryName(name));
    if(str.size()==0)
    {
        THROW_EXCEPTION(1,"do not match function for ""+path+""");
    }
    switch(_Mapping[str.c_str()])
    {
        case(1):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_Base_Alias(_type);
            break;
        }
        case(2):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_Base_Token(_type);
            break;
        }
        case(3):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            int32_t _type = (int32_t)atoi((char *)szValue);
            set_MonitorList_Monitor_Base_SVCLimits(_type);
            break;
        }
        case(4):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_Base_Usable(_type,isLoad);
            break;
        }
        case(5):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            LevelType::LevelType _type = (LevelType::LevelType)atoi((char *)szValue);
            set_MonitorList_Monitor_LowLoger(_type);
            break;
        }
        case(6):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            LevelType::LevelType _type = (LevelType::LevelType)atoi((char *)szValue);
            set_MonitorList_Monitor_AppLoger_Level(_type);
            break;
        }
        case(7):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            Switch::Switch _type = (Switch::Switch)atoi((char *)szValue);
            set_MonitorList_Monitor_AppLoger_Details(_type);
            break;
        }
        case(8):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            int32_t _type = (int32_t)atoi((char *)szValue);
            set_MonitorList_Monitor_AppLoger_Size(_type);
            break;
        }
        case(9):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            int32_t _type = (int32_t)atoi((char *)szValue);
            set_MonitorList_Monitor_AppLoger_Files(_type);
            break;
        }
        case(10):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_AppLoger_Dir(_type);
            break;
        }
        case(11):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            AppLogNameType::AppLogNameType _type = (AppLogNameType::AppLogNameType)atoi((char *)szValue);
            set_MonitorList_Monitor_AppLoger_NameFormat(_type);
            break;
        }
        case(12):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            AppLogDirType::AppLogDirType _type = (AppLogDirType::AppLogDirType)atoi((char *)szValue);
            set_MonitorList_Monitor_AppLoger_DirFormat(_type);
            break;
        }
        case(13):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_AppLoger_Usable(_type,isLoad);
            break;
        }
        case(14):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            int32_t _type = (int32_t)atoi((char *)szValue);
            set_MonitorList_Monitor_SVC_Threads(_type);
            break;
        }
        case(15):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_SVC_IsFlush(_type);
            break;
        }
        case(16):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            DBType::DBType _type = (DBType::DBType)atoi((char *)szValue);
            set_MonitorList_Monitor_SVC_DBConnection__SCROLLTYPE(_type);
            break;
        }
        case(17):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name(_type);
            break;
        }
        case(18):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User(_type);
            break;
        }
        case(19):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd(_type);
            break;
        }
        case(20):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable(_type,isLoad);
            break;
        }
        case(21):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name(_type);
            break;
        }
        case(22):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User(_type);
            break;
        }
        case(23):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd(_type);
            break;
        }
        case(24):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable(_type,isLoad);
            break;
        }
        case(25):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile(_type);
            break;
        }
        case(26):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable(_type,isLoad);
            break;
        }
        case(27):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable(_type,isLoad);
            break;
        }
        case(28):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_SVC_DBConnection_Usable(_type,isLoad);
            break;
        }
        case(29):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_SVC_Usable(_type,isLoad);
            break;
        }
        case(30):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(31):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_LSRList_LSR_Alias(Atoi(param[0]),_type);
            break;
        }
        case(32):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_LSRList_LSR_ImplDllName(Atoi(param[0]),_type);
            break;
        }
        case(33):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            int32_t _type = (int32_t)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Threads(Atoi(param[0]),_type);
            break;
        }
        case(34):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            int32_t _type = (int32_t)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_ThreadStackSize(Atoi(param[0]),_type);
            break;
        }
        case(35):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            ListenerType::ListenerType _type = (ListenerType::ListenerType)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(Atoi(param[0]),_type);
            break;
        }
        case(36):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            SocketType::SocketType _type = (SocketType::SocketType)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(Atoi(param[0]),_type);
            break;
        }
        case(37):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP(Atoi(param[0]),_type);
            break;
        }
        case(38):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            int32_t _type = (int32_t)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port(Atoi(param[0]),_type);
            break;
        }
        case(39):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(40):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token(Atoi(param[0]),_type);
            break;
        }
        case(41):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(42):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(43):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(44):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            int32_t _type = (int32_t)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut(Atoi(param[0]),_type);
            break;
        }
        case(45):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            int32_t _type = (int32_t)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO(Atoi(param[0]),_type);
            break;
        }
        case(46):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName(Atoi(param[0]),_type);
            break;
        }
        case(47):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(48):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(49):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Protocol_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(50):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_LSRList_LSR_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(51):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_CNRList_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(52):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_CNRList_CNR_Alias(Atoi(param[0]),_type);
            break;
        }
        case(53):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            ConectorType::ConectorType _type = (ConectorType::ConectorType)atoi((char *)szValue);
            set_MonitorList_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(Atoi(param[0]),_type);
            break;
        }
        case(54):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName(Atoi(param[0]),_type);
            break;
        }
        case(55):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(56):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName(Atoi(param[0]),_type);
            break;
        }
        case(57):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(58):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName(Atoi(param[0]),_type);
            break;
        }
        case(59):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(60):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            PBYTE _type = (PBYTE)szValue;
            set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName(Atoi(param[0]),_type);
            break;
        }
        case(61):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(62):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(63):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_CNRList_CNR_PlugIn_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(64):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_CNRList_CNR_Usable(Atoi(param[0]),_type,isLoad);
            break;
        }
        case(65):
        {
            int szLen = strlen((char *)value);
            char szValue[szLen+1];
            memset(szValue,0x00,sizeof(szValue));
            memcpy((char *)szValue,(char *)value,szLen);
            bool _type = (bool)atoi((char *)szValue);
            set_MonitorList_Monitor_Usable(_type,isLoad);
            break;
        }
        default:
        {
            THROW_EXCEPTION(1,"do not match function for ""+path+""");
        }
    }
}
