#ifndef HIDDQSCDXRJMOWFRXSJY_MTRCONFIGDEF_H_P_P
#define HIDDQSCDXRJMOWFRXSJY_MTRCONFIGDEF_H_P_P
#include "cdtype.h"
#include <stddef.h>


typedef struct{
    BYTE   Usable;
    BYTE   Configure[1024];
}GalaGrpCfg,*PGalaGrpCfg;
typedef struct{
    GalaGrpCfg _GrpCfg[1];
}MonitorConfigureSet,*PMonitorConfigureSet;
typedef struct{
    char _MonitorList_Monitor_Base_Usable[1];
    char _MonitorList_Monitor_Base_Alias[32];
    char _MonitorList_Monitor_Base_Token[32];
    char _MonitorList_Monitor_Base_SVCLimits[4];
}Base,*PBase;
typedef struct{
    char _MonitorList_Monitor_AppLoger_Usable[1];
    char _MonitorList_Monitor_AppLoger__SCROLLTYPE[1];
    char _MonitorList_Monitor_AppLoger__UNIONSELECTOR_Usable[1];
    char _MonitorList_Monitor_AppLoger__UNIONSELECTOR_DATEMODE_Usable[1];
    char _MonitorList_Monitor_AppLoger__UNIONSELECTOR_DATEMODE_NamePattern[128];
    char _MonitorList_Monitor_AppLoger__UNIONSELECTOR_APPMODE_Usable[1];
    char _MonitorList_Monitor_AppLoger__UNIONSELECTOR_APPMODE_NamePattern[128];
    char _MonitorList_Monitor_AppLoger__UNIONSELECTOR_CUSTOMMODE_Usable[1];
    char _MonitorList_Monitor_AppLoger__UNIONSELECTOR_CUSTOMMODE_NamePattern[128];
}AppLoger,*PAppLoger;
typedef struct{
    char _MonitorList_Monitor_SVC_Usable[1];
    char _MonitorList_Monitor_SVC_Threads[4];
    char _MonitorList_Monitor_SVC_IsFlush[1];
    char _MonitorList_Monitor_SVC_DBConnection_Usable[1];
    char _MonitorList_Monitor_SVC_DBConnection__SCROLLTYPE[1];
    char _MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_Usable[1];
    char _MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Usable[1];
    char _MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Name[30];
    char _MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_User[30];
    char _MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_ORACLE_Passwd[30];
    char _MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Usable[1];
    char _MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Name[30];
    char _MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_User[30];
    char _MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_DB2_Passwd[30];
    char _MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_Usable[1];
    char _MonitorList_Monitor_SVC_DBConnection__UNIONSELECTOR_SQLITE_DBFile[256];
}SVC,*PSVC;
typedef struct{
    char _MonitorList_Monitor_LSRList_Usable[1];
    char _MonitorList_Monitor_LSRList_LSR_Usable[1];
    char _MonitorList_Monitor_LSRList_LSR_Alias[32];
    char _MonitorList_Monitor_LSRList_LSR_ImplDllName[32];
    char _MonitorList_Monitor_LSRList_LSR_Threads[4];
    char _MonitorList_Monitor_LSRList_LSR_ThreadStackSize[4];
    char _MonitorList_Monitor_LSRList_LSR_Protocol_Usable[1];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__SCROLLTYPE[1];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_Usable[1];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Usable[1];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address_Usable[1];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__SCROLLTYPE[1];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_Usable[1];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Usable[1];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_IP[16];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_TCP_Port[4];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Usable[1];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_Address__UNIONSELECTOR_UNIX_Token[32];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_RecvTimeOut[4];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_ExchangeTMO[4];
    char _MonitorList_Monitor_LSRList_LSR_Protocol__UNIONSELECTOR_LSRSTREAM_PTCDllName[32];
}LSRList,*PLSRList;
typedef struct{
    char _MonitorList_Monitor_CNRList_Usable[1];
    char _MonitorList_Monitor_CNRList_CNR_Usable[1];
    char _MonitorList_Monitor_CNRList_CNR_Alias[32];
    char _MonitorList_Monitor_CNRList_CNR_PlugIn_Usable[1];
    char _MonitorList_Monitor_CNRList_CNR_PlugIn__SCROLLTYPE[1];
    char _MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_Usable[1];
    char _MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_Usable[1];
    char _MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_ECHO_ImplDllName[32];
    char _MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_Usable[1];
    char _MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_TIMEOUT_ImplDllName[32];
    char _MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_Usable[1];
    char _MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_SQUARE_ImplDllName[32];
    char _MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_Usable[1];
    char _MonitorList_Monitor_CNRList_CNR_PlugIn__UNIONSELECTOR_STATE_ImplDllName[32];
}CNRList,*PCNRList;
typedef struct{
    char _MonitorList_Monitor_Usable[1];
    Base _MonitorList_Monitor_Base;
    char _MonitorList_Monitor_LowLoger[1];
    AppLoger _MonitorList_Monitor_AppLoger;
    SVC _MonitorList_Monitor_SVC;
    LSRList _MonitorList_Monitor_LSRList[64];
    CNRList _MonitorList_Monitor_CNRList[64];
}MonitorConfigure,*PMonitorConfigure;
#endif
