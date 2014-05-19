#include "Runtime/Exception.hpp"
#include "lua.hpp"

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
    IController *Initialize(void *);
    void         Finalize(void *);
}
