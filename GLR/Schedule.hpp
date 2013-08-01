#include "stdafx.h"
#include "lua.hpp"
#include <vector>

#include <string>
#ifdef _AIX
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif
#include "Runtime/Queue.hpp"
#include "Runtime/Thread.hpp"

#pragma once
namespace GLR
{

    class Process;
    typedef Galaxy::GalaxyRT::_TypeDef_FIFO<Process*>::CFIFOQueue LN_SCHE_QUEUE_TYPE;
    class Schedule;

    class Processor : public Galaxy::GalaxyRT::CRunnable
    {
    public:
        struct ProcessorStatus
        {
            enum STATE
            {
                RUNNING,              // binded to a GLR Process and running
                WAITING,              // wait for a GLR Process being excutable
                UNBIND,               // newly created
                BINDED,               // bind to a native thread

            };
            STATE _State;              // Current State
            int   _GLRProcessId;       // Binded GLR Process ID
            time_t _LastOp;            // Last Operation Timestamp
            time_t _Start;
            int   _ScheCount;          // Schedule Operation Counting
        };
        Processor(Schedule&);
        using Galaxy::GalaxyRT::CRunnable::Run;
        virtual void* Run(const Galaxy::GalaxyRT::CThread &);

        ~Processor();

        virtual void Clean( const Galaxy::GalaxyRT::CThread & );

    private:
        Schedule &_Sche;
        ProcessorStatus _Status;
    };

    class Schedule
    {
    private:
        Schedule(int threads);
    public:
        friend class Processor;
        static Schedule &GetInstance(int threads = 8);
        //void CoreThread();
        void PutTask(Process &node);
        void Stop();
    public:
        ~Schedule(void);
    private:
        std::vector<std::pair<Processor*, Galaxy::GalaxyRT::CThread*> >  _Cores;
        LN_SCHE_QUEUE_TYPE      _ScheQueue;
        bool                    _isLiving;

    };

};