    #ifndef BUQCLJJIVSWMDKQTBXIX_MTRXMLCONFIGURE_H_P_P
    #define BUQCLJJIVSWMDKQTBXIX_MTRXMLCONFIGURE_H_P_P
    #include <map>
    #include <string>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <libxml/parser.h>
    #include <libxml/tree.h>
    #include <libxml/xpath.h>
    #include "Runtime/Exception.hpp"
    #include "MTRConfigure.hpp"


    class CXmlMonitorConfigure:public Galaxy::GalaxyRT::CNonCopyAble
    {
    public:
        CXmlMonitorConfigure(CMonitorConfigure *GalaObject);
        ~CXmlMonitorConfigure();
        void LoadXmlConfig(const std::string &,bool isExtend = false);
        void WriteConfig2Xml(const std::string &);
        xmlDocPtr GetDoc()const;
        char *Property(xmlNodePtr node,const std::string &key);
        char *Value(xmlNodePtr node);
        std::string GetValidPath(xmlNodePtr node);
        std::string Strchrr(const std::string & str,char ch);
        bool IsExist(const std::string &express);
        xmlNodePtr xmlXpath(const std::string &express);
        void OutPut_Configure__STRUCTURE(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_Base(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Alias(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Alias(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Token(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_Token(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_SVCLimits(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_Base_SVCLimits(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LowLoger(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LowLoger(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Level(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Level(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Details(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Details(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Size(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Size(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Files(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Files(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Dir(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_Dir(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_NameFormat(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_NameFormat(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_DirFormat(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_AppLoger_DirFormat(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_Threads(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_Threads(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_IsFlush(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_IsFlush(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__SCROLLTYPE(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__SCROLLTYPE(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRLists(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRLists(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Alias(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Alias(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_ImplDllName(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_ImplDllName(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Threads(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Threads(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_ThreadStackSize(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_ThreadStackSize(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__SCROLLTYPE(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRLists(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRLists(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_Alias(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_Alias(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName(const std::string &path0,const std::string &path1);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE(const std::string &path0,const std::string &path1);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable(const std::string &path);
        void OutPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName(xmlNodePtr node,const std::string &path);
        void InPut_Configure__STRUCTURE_MonitorLists_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName(const std::string &path0,const std::string &path1);
    private:
        CMonitorConfigure         *_ConfObject;
        xmlDocPtr _Doc;
        xmlXPathContextPtr _XpathCtx;
        xmlXPathObjectPtr _XpathObj;

    };
#endif
