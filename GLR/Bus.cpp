#include "Bus.hpp"
#include "LuaNode.hpp"
#include "Schedule.hpp"
#include "GLR.hpp"
using namespace GLR;

Bus::Bus(void)
    :_Devices(32)
{

}


Bus::~Bus(void)
{
}

Bus & GLR::Bus::GetInstance()
{
    static Bus _Instance;
    return _Instance;
}

void GLR::Bus::IntSuspend( int pid )
{
    Process &nd = Runtime::GetInstance().GetProcess(pid);
    nd._Status._State = Process::ProcessStatus::INT_WAIT;
}

void GLR::Bus::Return( int pid, int narg, ...)
{
    Process &nd = Runtime::GetInstance().GetProcess(pid);
    if (nd._Status._State != Process::ProcessStatus::GLR_CALL)
    {
        THROW_EXCEPTION_EX("Fatal Error");
    }
    int type = 0;
    const char *str = NULL;
    int number = 0;
    va_list argp;
    va_start(argp, narg);

    for (int i = 0; i!=narg; ++i)
    {
        type = va_arg(argp, int);
        switch (type)
        {
        case LUA_TSTRING:
            str = va_arg(argp, const char*);
            number = va_arg(argp, int);
            lua_pushlstring(nd._Stack, str, number);
            break;
        case LUA_TNUMBER:
            number = va_arg(argp, int);
            lua_pushinteger(nd._Stack, number);
            break;
        case LUA_TNIL:
            lua_pushnil(nd._Stack);
            break;
        case LUA_TBOOLEAN:
            number = va_arg(argp, int);
            lua_pushboolean(nd._Stack, number);
            break;
        default:
            break;
        }
    }

    va_end(argp);
    nd._Status._NArg = narg;
    nd._Status._State = Process::ProcessStatus::INT_RESP;
}

void GLR::Bus::Send( int pid, const std::string &msg, MSG_HEAD::MSG_TYPE type)
{
    Process::SendMsgToNode(pid, msg, type);
}

void Bus::Response( int pid, int narg, ...)
{

    Process &nd = Runtime::GetInstance().GetProcess(pid);
    Galaxy::GalaxyRT::CLockGuard _Gl(&nd._IntLock);

    GALA_ERROR("Pid(%d) Status(%d)", pid, nd._Status._State);
    if (nd._Status._State != Process::ProcessStatus::INT_WAIT)
    {
        THROW_EXCEPTION_EX("Fatal Error");
    }
    int type = 0;
    const char *str = NULL;
    int number = 0;
    va_list argp;
    va_start(argp, narg);

    for (int i = 0; i!=narg; ++i)
    {
        type = va_arg(argp, int);
        switch (type)
        {
        case LUA_TSTRING:
            str = va_arg(argp, const char*);
            number = va_arg(argp, int);
            if (number == 0)
            {
                lua_pushlstring(nd._Stack, str, strlen(str));
            }
            else
            {
                lua_pushlstring(nd._Stack, str, number);
            }

            break;
        case LUA_TNUMBER:
            number = va_arg(argp, int);
            lua_pushinteger(nd._Stack, number);
            break;
        case LUA_TNIL:
            lua_pushnil(nd._Stack);
            break;
        case LUA_TBOOLEAN:
            number = va_arg(argp, int);
            lua_pushboolean(nd._Stack, number);
            break;
        default:
            break;
        }
    }

    va_end(argp);

    nd._Status._NArg = narg;
    nd._Status._State = Process::ProcessStatus::INT_RESP;  
    Runtime::GetInstance().GetSchedule().PutTask(nd);

}



void Bus::RegisterDevice( IController *con )
{
    _Devices[con->DeviceId()] = con;
}

void Bus::Interrupt( int device, lua_State *l )
{
    printf("Interrupt No (%d)\n", device);
    try
    {
        _Devices[device]->Request(l);
    }
    catch (Galaxy::GalaxyRT::CException &e)
    {
        GALA_DEBUG("Bus Error (%s)", e.what());
        lua_getglobal(l,"__id__");
        int pid = luaL_checkinteger(l,-1);
        const char *err = e.what();
        Return(pid, 2, LUA_TNIL, LUA_TSTRING, err, strlen(err));
    }
    //return lua_yield(l);
}

int IController::_DevIDSerial = 0;
IController::IController( void )
    :_DeviceId(_DevIDSerial++)
{

}


IController::~IController( void )
{

}
