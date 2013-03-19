    #ifndef XPKLORELLNMPAPQFWKHO_MTRCONFIGURE_H_P_P
    #define XPKLORELLNMPAPQFWKHO_MTRCONFIGURE_H_P_P
    #include <map>
    #include <string>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <iconv.h>
    #include <libxml/xmlsave.h>
    #include <libxml/xpathInternals.h>
    #include <libxml/parser.h>
    #include <libxml/tree.h>
    #include <libxml/xpath.h>
    #include "Runtime/Exception.hpp"
    #include "MTRConfigDef.hpp"


    #define CNRLIST 64
    #define LSRLIST 64
    #define MONITOR_MAX 1


    namespace DBType
    {
        enum DBType
        {
            NONETYPE = 0,
            ORACLE = 1,
            DB2 = 2,
            AGENT = 3,
            SQLITE = 4,
        };
    };
    namespace ListenerType
    {
        enum ListenerType
        {
            LSRSTREAM = 0,
            FTP = 1,
            TASK = 2,
            MQ = 3,
        };
    };
    namespace SocketType
    {
        enum SocketType
        {
            TCP = 0,
            UNIX = 1,
        };
    };
    namespace LogType
    {
        enum LogType
        {
            DATEMODE = 0,
            APPMODE = 1,
            CUSTOMMODE = 2,
        };
    };
    namespace ConectorType
    {
        enum ConectorType
        {
            ECHO = 0,
            TIMEOUT = 1,
            SQUARE = 2,
            STATE = 3,
        };
    };
    namespace LowType
    {
        enum LowType
        {
            DEBUG = 0,
            INFO = 1,
            WARN = 2,
            ERROR = 3,
            FATAL = 4,
        };
    };
    class CMonitorConfigure:public Galaxy::GalaxyRT::CNonCopyAble
    {
    public:
        CMonitorConfigure(PBYTE memAddr,int32_t size);
        ~CMonitorConfigure();
        std::string Get(const std::string &path);
        std::pair<std::string,std::string> Get_ex(const std::string &path);
        void Put(const std::string &path,char *value,bool isLoad = false);
        PMonitorConfigure get_MonitorConfigure();
        PBase get_MonitorList_Monitor_Base();
        PBYTE get_MonitorList_Monitor_Base_Alias();
        void set_MonitorList_Monitor_Base_Alias(PBYTE Alias);
        PBYTE get_MonitorList_Monitor_Base_Token();
        void set_MonitorList_Monitor_Base_Token(PBYTE Token);
        PBYTE get_MonitorList_Monitor_Base_SVCLimits();
        void set_MonitorList_Monitor_Base_SVCLimits(int32_t SVCLimits);
        PBYTE get_MonitorList_Monitor_Base_Usable();
        void set_MonitorList_Monitor_Base_Usable(bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_LowLoger();
        void set_MonitorList_Monitor_LowLoger(LowType::LowType LowLoger);
        PAppLoger get_MonitorList_Monitor_AppLoger();
        PBYTE get_MonitorList_Monitor_AppLoger__SCROLLTYPE();
        void set_MonitorList_Monitor_AppLoger__SCROLLTYPE(LogType::LogType _SCROLLTYPE);
        PBYTE get_MonitorList_Monitor_AppLoger__UNIONSELECTOR_DATEMODE_NamePattern();
        void set_MonitorList_Monitor_AppLoger__UNIONSELECTOR_DATEMODE_NamePattern(PBYTE NamePattern);
        PBYTE get_MonitorList_Monitor_AppLoger__UNIONSELECTOR_DATEMODE_Usable();
        void set_MonitorList_Monitor_AppLoger__UNIONSELECTOR_DATEMODE_Usable(bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_AppLoger__UNIONSELECTOR_APPMODE_NamePattern();
        void set_MonitorList_Monitor_AppLoger__UNIONSELECTOR_APPMODE_NamePattern(PBYTE NamePattern);
        PBYTE get_MonitorList_Monitor_AppLoger__UNIONSELECTOR_APPMODE_Usable();
        void set_MonitorList_Monitor_AppLoger__UNIONSELECTOR_APPMODE_Usable(bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_AppLoger__UNIONSELECTOR_CUSTOMMODE_NamePattern();
        void set_MonitorList_Monitor_AppLoger__UNIONSELECTOR_CUSTOMMODE_NamePattern(PBYTE NamePattern);
        PBYTE get_MonitorList_Monitor_AppLoger__UNIONSELECTOR_CUSTOMMODE_Usable();
        void set_MonitorList_Monitor_AppLoger__UNIONSELECTOR_CUSTOMMODE_Usable(bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_AppLoger__UNIONSELECTOR_Usable();
        void set_MonitorList_Monitor_AppLoger__UNIONSELECTOR_Usable(bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_AppLoger_Usable();
        void set_MonitorList_Monitor_AppLoger_Usable(bool Enable,bool isLoad = false);
        PSVC get_MonitorList_Monitor_SVC();
        PBYTE get_MonitorList_Monitor_SVC_Threads();
        void set_MonitorList_Monitor_SVC_Threads(int32_t Threads);
        PBYTE get_MonitorList_Monitor_SVC_IsFlush();
        void set_MonitorList_Monitor_SVC_IsFlush(bool IsFlush);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection__SCROLLTYPE();
        void set_MonitorList_Monitor_SVC_DBConnection__SCROLLTYPE(DBType::DBType _SCROLLTYPE);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name();
        void set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name(PBYTE Name);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User();
        void set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User(PBYTE User);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd();
        void set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd(PBYTE Passwd);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable();
        void set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable(bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name();
        void set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name(PBYTE Name);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User();
        void set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User(PBYTE User);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd();
        void set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd(PBYTE Passwd);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable();
        void set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable(bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile();
        void set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile(PBYTE DBFile);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable();
        void set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable(bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable();
        void set_MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable(bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_SVC_DBConnection_Usable();
        void set_MonitorList_Monitor_SVC_DBConnection_Usable(bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_SVC_Usable();
        void set_MonitorList_Monitor_SVC_Usable(bool Enable,bool isLoad = false);
        PLSRList get_MonitorList_Monitor_LSRList(int32_t LSRListIdx);
        PBYTE get_MonitorList_Monitor_LSRList_Usable(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_Usable(int32_t LSRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Alias(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Alias(int32_t LSRListIdx,PBYTE Alias);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_ImplDllName(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_ImplDllName(int32_t LSRListIdx,PBYTE ImplDllName);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Threads(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Threads(int32_t LSRListIdx,int32_t Threads);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_ThreadStackSize(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_ThreadStackSize(int32_t LSRListIdx,int32_t ThreadStackSize);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(int32_t LSRListIdx,ListenerType::ListenerType _SCROLLTYPE);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(int32_t LSRListIdx,SocketType::SocketType _SCROLLTYPE);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP(int32_t LSRListIdx,PBYTE IP);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port(int32_t LSRListIdx,int32_t Port);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable(int32_t LSRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token(int32_t LSRListIdx,PBYTE Token);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable(int32_t LSRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable(int32_t LSRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable(int32_t LSRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut(int32_t LSRListIdx,int32_t RecvTimeOut);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO(int32_t LSRListIdx,int32_t ExchangeTMO);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName(int32_t LSRListIdx,PBYTE PTCDllName);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable(int32_t LSRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable(int32_t LSRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Protocol_Usable(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Protocol_Usable(int32_t LSRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_LSRList_LSR_Usable(int32_t LSRListIdx);
        void set_MonitorList_Monitor_LSRList_LSR_Usable(int32_t LSRListIdx,bool Enable,bool isLoad = false);
        PCNRList get_MonitorList_Monitor_CNRList(int32_t CNRListIdx);
        PBYTE get_MonitorList_Monitor_CNRList_Usable(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_Usable(int32_t CNRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_Alias(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_Alias(int32_t CNRListIdx,PBYTE Alias);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(int32_t CNRListIdx,ConectorType::ConectorType _SCROLLTYPE);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName(int32_t CNRListIdx,PBYTE ImplDllName);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable(int32_t CNRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName(int32_t CNRListIdx,PBYTE ImplDllName);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable(int32_t CNRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName(int32_t CNRListIdx,PBYTE ImplDllName);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable(int32_t CNRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName(int32_t CNRListIdx,PBYTE ImplDllName);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable(int32_t CNRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable(int32_t CNRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_PlugIn_Usable(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_PlugIn_Usable(int32_t CNRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_CNRList_CNR_Usable(int32_t CNRListIdx);
        void set_MonitorList_Monitor_CNRList_CNR_Usable(int32_t CNRListIdx,bool Enable,bool isLoad = false);
        PBYTE get_MonitorList_Monitor_Usable();
        void set_MonitorList_Monitor_Usable(bool Enable,bool isLoad = false);
    protected:
        bool IsIPAddr(const std::string &);
        bool IsPath(const std::string &);
        bool Isdidgit(const std::string &);
        void PathAnalyze(const std::string &,std::string &,std::vector<std::string> &);
        std::string GetEntryName(const std::string &);
        std::string Entry2Method(const std::string &);
        int32_t Atoi(const std::string &);
        char *GetNodeName(char *);
        void InitializeMap();
    private:
        PBYTE _MemoryAddr;
        int32_t _Size;
        std::map<std::string,int> _Mapping;

    };
#endif
