#ifndef _GLR_BUS_HPP
#define _GLR_BUS_HPP

#include "stdafx.h"
#include "lua.hpp"
#include <vector>
#include <string>
#include <map>

#include <exception>
#ifdef _AIX
#include <unordered_map>
#endif
#include "Runtime/Queue.hpp"
#include "ControllerInterface.hpp"
#pragma once

namespace GLR
{
#pragma pack(1)

typedef struct
{
    uint32_t 			_Length; 		// length(body)
    char 				_Magic[2]; 		// 'G','L'
    unsigned char 		_Version; 	    // 1,2,3
    unsigned char 		_Type; 			// message type
    uint32_t 			_Stamp; 		// time stamp
    uint32_t			_Checksum;		// crc32 or md5
}PRTC_HEADER;

typedef struct
{
    uint32_t			_FromGpid;		//前一字节为域ID，后3字节为设备ID
    uint32_t			_CurrNodeGpid;	//当前节点ID
    uint32_t			_ToGpid;		//最终节点
}RTU_HEADER;

typedef struct
{
    uint32_t			_CorrId;		//会话号
    uint32_t			_MsgId;			//自产生的MsgId，只有From
}MSG_HEADER;

typedef union
{
    struct
    {
        unsigned char		_Host[24];		//域HOST
        uint32_t			_Port;			//端口
    }_V2;
    struct
    {
        uint32_t           _Host;
        uint32_t           _Port;
    }_V3;
}HOST_HEADER;




struct GLRPROTOCOL
{
    enum MSG_TYPE
    {
        REGISTER = 0,
        REGISTER_OK = 1,
        REGISTER_FAIL = 2,
        APP = 3,        //Normal Application Message
        KILL = 4,       //Message To Kill this GLR Process
        CLOSED = 5,     //Resource bind to this GLR Process closed
        EXIT = 6,       //Process Exit Message
        ACK = 7,
        IOCP = 8,       //IO notify or IOCP
    };
    PRTC_HEADER			_Protocol;
    RTU_HEADER			_Route;
    MSG_HEADER			_Msg;
    HOST_HEADER			_Host;
    unsigned char 		_Content[0];
    GLRPROTOCOL(){
        memset(this, 0, sizeof(*this));
    }
};
#ifdef _AIX
#pragma pack(pop)
#else
#pragma pack()
#endif

class Bus
{
public:
    typedef std::vector<IController*> DEVICE_ARRAY;
private:
    Bus(void);
public:
    static Bus &GetInstance();
    void RegisterDevice(IController*);
    void Interrupt(int device, int tick, lua_State*);
    void IntSuspend(int pid);
    void Response(int pid, int narg, ...);
    bool ResponseEx(int pid, int tick, int narg, ...);
    void TimerSignal(int pid, int tick);
    bool IsCanceled(int pid, int tick);
    //void Response(int pid, int narg){Response(pid,narg,NULL);}
    void Return(int pid, int narg,...);
    void Return( int pid,int narg, GLRPROTOCOL *header);
    void Return( int pid, int narg,int type,std::map<std::string,int>m);
    void Send(int pid, const std::string&, GLRPROTOCOL::MSG_TYPE type = GLRPROTOCOL::APP);
    Galaxy::GalaxyRT::CPthreadMutex &Lock(){return _Lock;}
    ~Bus(void);
private:
    DEVICE_ARRAY _Devices;
    Galaxy::GalaxyRT::CPthreadMutex _Lock;
    //DEVICE_MAP   _DevMap;
};
};
#endif // _GLR_BUS_HPP
