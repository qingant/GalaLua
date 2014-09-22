#include "Clock.hpp"
#include "GLR.hpp"
#include "LuaNode.hpp"
using namespace GLR;

void Clock::Request( lua_State *l, int tick )
{
    int type = luaL_checkinteger(l, 2);
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    ClockEvent cev = {pid, tick};
    //Process &nd = Runtime::GetInstance().GetProcess(pid);
    switch (type)
    {
    case GET_TIME:
        {
            Galaxy::GalaxyRT::TPType now=_Timer.GetTimeStamp();
            Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNUMBER,now.tv_sec, LUA_TNUMBER ,  now.tv_usec/1000);
        }
        break;
    case DO_TIMEOUT:
        cev.tick=-1;
        SetTimer(l,cev);
        Runtime::GetInstance().GetBus().IntSuspend(pid);
        break;
    case Clock::DO_TIMEOUT_C:
        SetTimer(l,cev);
        break;
    case Clock::DO_TIMEOUT_SIGNAL:
        cev.tick=-2;
        SetTimer(l,cev);
        break;
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
    else if (pEv->tick==-2)
    {
        //TODO: 1.增加消息内容 2.考虑gpid复用的问题
        Process::SendTimeoutMsg(pEv->pid);
    }
    else
    {
        Runtime::GetInstance().GetBus().TimerSignal(pEv->pid, pEv->tick);
    }
}

inline void Clock::SetTimer(lua_State *l, ClockEvent &cev)
{
    //XXX: timeval单位为毫秒
    int timeval = luaL_checkinteger(l, 3);

    int sec=timeval/1000;
    int msec=timeval%1000;

    Galaxy::GalaxyRT::TIMEREVENT ev(sec, msec*1000);

    ev._Data = std::string((const char*)&cev, sizeof(cev));
    _Timer.SetTimer(ev);
}


