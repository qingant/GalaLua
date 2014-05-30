#include "Clock.hpp"
#include "GLR.hpp"
#include "LuaNode.hpp"
using namespace GLR;

void Clock::Request( lua_State *l, int tick )
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
        ClockEvent cev = {pid, -1};
        ev._Data = std::string((const char*)&cev, sizeof(cev));
        _Timer.SetTimer(ev);
        Runtime::GetInstance().GetBus().IntSuspend(pid);
    }
    else if (type == Clock::DO_TIMEOUT_C)
    {

        int timeval = luaL_checkinteger(l, 3);
        //int tick = luaL_checkinteger(l, 4);

        printf("timeout %d!\n", timeval);
        Galaxy::GalaxyRT::TIMEREVENT ev(timeval);
        ClockEvent cev = {pid, tick};
        ev._Data = std::string((const char*)&cev, sizeof(cev));
        _Timer.SetTimer(ev);
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
    if (pEv->tick == -1)
    {
        Runtime::GetInstance().GetBus().Response(pEv->pid, 0);
    }
    else
    {
        Runtime::GetInstance().GetBus().TimerSignal(pEv->pid, pEv->tick);
    }
}



