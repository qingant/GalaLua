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
            GET_TIME,
            DO_TIMEOUT,
        };
        struct ClockEvent
        {
            int pid;
        };
    public:
        static void OnTimeOut(const Galaxy::GalaxyRT::TIMEREVENT &);
        Clock(void);

        ~Clock();

        //************************************
        // Method:    Request
        // FullName:  Clock::Request
        // Access:    virtual public 
        // Returns:   void
        // Qualifier: 
        // Parameter: lua_State * 
        //************************************
        virtual void Request( lua_State* );

    private:
        Galaxy::GalaxyRT::CTimer _Timer;
        Galaxy::GalaxyRT::CThread *_Thread;
    };
}