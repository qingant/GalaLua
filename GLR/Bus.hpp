#include "stdafx.h"
#include "lua.hpp"
#include <vector>
#include <string>

#include <exception>
#include <tr1/unordered_map>
#include "Runtime/Queue.hpp"

#pragma once

namespace GLR
{
#pragma pack(1)
    struct MSG_HEAD
    {
        enum MSG_TYPE
        {
            APP,        //Normal Application Message
            KILL,       //Message To Kill this GLR Process
            CLOSED,     //Resource bind to this GLR Process closed
        };
        uint32_t Len;  //Message Length not including this head structure
        char     Type; //Message Type
        int32_t GPid; //GPid of process who send this message, nagetive if from controller
    };
#pragma pack()
    class IController
    {
    public:
        IController(void);
        virtual void Request(lua_State*) = 0;
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
        typedef std::tr1::unordered_map<std::string, int> DEVICE_MAP;
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
        void Interrupt(int device, lua_State*);
        void IntSuspend(int pid);
        void Response(int pid, int narg, ...);
        //void Response(int pid, int narg){Response(pid,narg,NULL);}
        void Return(int pid, int narg,...);
        void Send(int pid, const std::string&, MSG_HEAD::MSG_TYPE type = MSG_HEAD::APP);
        Galaxy::GalaxyRT::CPthreadMutex &Lock(){return _Lock;}
        ~Bus(void);
    private:
        DEVICE_ARRAY _Devices;
        Galaxy::GalaxyRT::CPthreadMutex _Lock;
        //DEVICE_MAP   _DevMap;
    };
};
