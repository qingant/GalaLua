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

void GLR::Bus::Return( int pid, int narg,int type,std::map<std::string,int> m)
{
    Process &nd = Runtime::GetInstance().GetProcess(pid);
    if (nd._Status._State != Process::ProcessStatus::GLR_CALL)
    {
        THROW_EXCEPTION_EX("Fatal Error");
    }

    if (type!=LUA_TTABLE)
    {
        nd._Status._NArg = 0;
        nd._Status._State = Process::ProcessStatus::INT_RESP;
        return;
    }

    lua_newtable(nd._Stack);
    for (std::map<std::string,int>::iterator it=m.begin();it!=m.end();++it)
    {
        //printf("%s  %d ((((((%d)))))) \n",__func__,__LINE__,it->second);
        lua_pushnumber(nd._Stack,it->second);
        lua_setfield(nd._Stack,-2,it->first.c_str());
    }

    nd._Status._NArg = narg;
    nd._Status._State = Process::ProcessStatus::INT_RESP;
}

void GLR::Bus::Return( int pid, int narg, ...)
{
    // Return 不需要锁保护，因为GLR_CALL的进程不会被销毁
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

void GLR::Bus::Send( int pid, const std::string &msg, GLRPROTOCOL::MSG_TYPE type)
{
    Process::SendMsgToNode(pid, msg, type);
}

void GLR::Bus::TimerSignal( int pid, int tick )
{
    try
    {
        // 保护代码块，避免返回的进程被销毁 
        Galaxy::GalaxyRT::CRWLockAdapter _RL(GLR::Process::ProcessMapLock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
        Galaxy::GalaxyRT::CLockGuard _(&_RL);


        Process &nd = Runtime::GetInstance().GetProcess(pid);
        GALA_DEBUG("Now Tick: %d, Last Tick: %d\n", nd._Status._Tick, tick);
        if (tick != nd._Status._Tick)
        {
            return;
        }
        Galaxy::GalaxyRT::CLockGuard _ll(&nd._Lock, false);
        Galaxy::GalaxyRT::CLockGuard _Gl(&nd._IntLock, false);
        if (tick != nd._Status._Tick)
        {
            return;
        }
        GALA_ERROR("Pid(%d) Status(%d)", pid, nd._Status._State);
        if (nd._Status._State != Process::ProcessStatus::INT_WAIT &&
            nd._Status._State != Process::ProcessStatus::RECV_WAIT)
        {
            THROW_EXCEPTION_EX("Fatal Error");
        }
        nd._Status._NArg = 0;
        nd._Status._State = Process::ProcessStatus::INT_RESP;  
        nd.StackDump();
        Runtime::GetInstance().GetSchedule().PutTask(nd);
    }
    catch (const Galaxy::GalaxyRT::CException &e)
    {
        return;
    }
}

bool GLR::Bus::IsCanceled( int pid, int tick )
{
    // 保护代码块，避免返回的进程被销毁 
    Galaxy::GalaxyRT::CRWLockAdapter _RL(GLR::Process::ProcessMapLock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
    Galaxy::GalaxyRT::CLockGuard _(&_RL);
    Process &nd = Runtime::GetInstance().GetProcess(pid);
    if (nd._Status._Tick == tick)
    {
        return false;
    }else
    {
        return true;
    }
}

bool Bus::ResponseEx(int pid, int tick, int narg, ...)
{

    try
    {
        // 保护代码块，避免返回的进程被销毁 
        Galaxy::GalaxyRT::CRWLockAdapter _RL(GLR::Process::ProcessMapLock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
        Galaxy::GalaxyRT::CLockGuard _(&_RL);

        Process &nd = Runtime::GetInstance().GetProcess(pid);
        if (tick != nd._Status._Tick)
        {
            return false;
        }
        Galaxy::GalaxyRT::CLockGuard _Gl(&nd._IntLock, false);
        GALA_ERROR("Pid(%d) Status(%d)", pid, nd._Status._State);
        if (nd._Status._State != Process::ProcessStatus::INT_WAIT &&
            nd._Status._State != Process::ProcessStatus::RECV_WAIT)
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
        nd.StackDump();
        Runtime::GetInstance().GetSchedule().PutTask(nd);
        return true;
    }
    catch (const Galaxy::GalaxyRT::CException &e)
    {
    }
    return  false; 
}
void Bus::Response( int pid, int narg, ...)
{


    try
    {
        // 保护代码块，避免返回的进程被销毁 
        Galaxy::GalaxyRT::CRWLockAdapter _RL(GLR::Process::ProcessMapLock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
        Galaxy::GalaxyRT::CLockGuard _(&_RL);

        Process &nd = Runtime::GetInstance().GetProcess(pid);

        Galaxy::GalaxyRT::CLockGuard _Gl(&nd._IntLock);
        if (nd._Status._State != Process::ProcessStatus::INT_WAIT &&
            nd._Status._State != Process::ProcessStatus::RECV_WAIT)
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
        nd.StackDump();
        Runtime::GetInstance().GetSchedule().PutTask(nd);
    }
    catch (const Galaxy::GalaxyRT::CException &e)
    {
        return;
    }

}



void Bus::RegisterDevice( IController *con )
{
    _Devices[con->DeviceId()] = con;
}

void Bus::Interrupt( int device, int tick, lua_State *l )
{
    GALA_DEBUG("Interrupt No (%d)\n", device);
    try
    {

        IController *pDev = _Devices[device];
        if(pDev!=NULL)
        {
            if (pDev->Version() == 1)
            {
                pDev->Request(l);
            }
            else
            {
                pDev->Request(l, tick);
            }
        }else
        {
            THROW_EXCEPTION_EX("Wrong INT id");
        }
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
