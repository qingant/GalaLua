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

#pragma once

namespace GLR
{
#pragma pack(1)
    struct MSG_HEAD
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
        };
        uint32_t Len;  //Message Length not including this head structure
        unsigned char  Type; //Message Type
        int32_t GPid; //GPid of process who send this message, nagetive if from controller
        MSG_HEAD()
        {
            memset(this, 0, sizeof(*this));
        }
    };
    typedef struct{
        char   Host[20];              // Host
        uint32_t   Port;              // Port
        uint32_t   Gpid;
    }GLR_ADDR;
    struct GLR_BUS_HEAD
    {
        MSG_HEAD Head;
        GLR_ADDR Source;
    };
#pragma pack()
    class IController
    {
    public:
        IController(void);
        virtual void Request(lua_State*){THROW_EXCEPTION_EX("Not Impl");}
        virtual void Request(lua_State*, int){THROW_EXCEPTION_EX("Not Impl");}
        virtual int  Version() const {return 1;}   //1: Basic 2: With tick(Per-Process INT serial) 
        //void Response(int pid, int nargs);
        virtual ~IController(void);
    private:
        static int _DevIDSerial;
        int _DeviceId;
    public:
        int DeviceId() const { return _DeviceId; }
    protected:
        void DeviceId(int val) { _DeviceId = val; }
    };

    class Bus
    {
    public:
        typedef std::vector<IController*> DEVICE_ARRAY;
    private:
        Bus(void);
    public:
        //************************************
        // Method:    GetInstance
        // FullName:  GLR::Bus::GetInstance
        // Access:    public static
        // Returns:   Bus &
        // Qualifier: This method is **NOT** thread-safe
        //************************************
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
        void Return( int pid, int narg,int type,std::map<std::string,int>m);
        void Send(int pid, const std::string&, MSG_HEAD::MSG_TYPE type = MSG_HEAD::APP);
        Galaxy::GalaxyRT::CPthreadMutex &Lock(){return _Lock;}
        ~Bus(void);
    private:
        DEVICE_ARRAY _Devices;
        Galaxy::GalaxyRT::CPthreadMutex _Lock;
        //DEVICE_MAP   _DevMap;
    };
};
