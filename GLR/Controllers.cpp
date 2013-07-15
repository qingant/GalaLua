#include "Controllers.hpp"
#include "GLR.hpp"
#include "LuaNode.hpp"
using namespace GLR;

void Clock::Request( lua_State *l )
{
    int type = luaL_checkinteger(l, 2);
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    //Process &nd = Runtime::GetInstance().GetProcess(pid);
    if (type == GET_TIME)
    {
        //lua_pushinteger(l, _Timer.GetTimeStamp());
        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TNUMBER, _Timer.GetTimeStamp());
    }
    else if (type == Clock::DO_TIMEOUT)
    {
        int timeval = luaL_checkinteger(l, 3);
        Galaxy::GalaxyRT::TIMEREVENT ev(timeval);
        ClockEvent cev = {pid};
        ev._Data = std::string((const char*)&cev, sizeof(cev));
        _Timer.SetTimer(ev);
        Runtime::GetInstance().GetBus().IntSuspend(pid);
    }


}

GLR::Clock::Clock( void )
    :_Timer(Clock::OnTimeOut)
{
    _Thread = new Galaxy::GalaxyRT::CThread(_Timer, 0x1234);
    _Thread->Start();
}

GLR::Clock::~Clock()
{

}

void GLR::Clock::OnTimeOut( const Galaxy::GalaxyRT::TIMEREVENT &ev )
{
    ClockEvent *pEv = (ClockEvent*)ev._Data.c_str();
    //Process &n = Runtime::GetInstance().GetProcess(pEv->pid);
    Runtime::GetInstance().GetBus().Response(pEv->pid, 0);
}



