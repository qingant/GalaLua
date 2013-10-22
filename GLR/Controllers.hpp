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

        virtual void Request( lua_State* , int);
        virtual int  Version()const {return 2;}

    private:
        Galaxy::GalaxyRT::CTimer _Timer;
        Galaxy::GalaxyRT::CThread *_Thread;
    };
}
