#include "Runtime/Exception.hpp"
#include "lua.hpp"

#pragma once

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

extern "C"{
    typedef IController *(*GLR_CTL_INIT_FUNC)(void*);
    typedef void (*GLR_CTL_FINALIZE_FUNC)(void*);
    IController *Initialize(void *);
    void         Finalize(void *);
}
