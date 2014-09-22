#include "Bus.hpp"
#include "Runtime/Timer.hpp"
#include "Runtime/Event.hpp"
#pragma once
namespace GLR
{

    class Clock : public IController
    {
    public:
        enum REQTYPE
        {
            GET_TIME = 0,
            DO_TIMEOUT = 1,
            DO_TIMEOUT_C = 2,
            DO_TIMEOUT_SIGNAL = 3,
        };
        struct ClockEvent
        {
            int pid;
            int tick;
        };
    public:
        static void OnTimeOut(const Galaxy::GalaxyRT::TIMEREVENT &);
        Clock(void);

        ~Clock();
        using IController::Request;
        virtual void Request( lua_State* , int);
        virtual int  Version()const {return 2;}
    private:
        inline void SetTimer(lua_State *l, ClockEvent &cev);

    private:
        Galaxy::GalaxyRT::CTimer _Timer;
        Galaxy::GalaxyRT::CThread *_Thread;
    };
}
