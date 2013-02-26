#include "Schedule.hpp"
#include "LuaNode.hpp"


using namespace GLR;

Schedule::~Schedule()
{
    _ScheQueue.Close();
    for (typeof(_Cores.begin()) it = _Cores.begin(); it != _Cores.end(); ++it)
    {
        it->second->Join();
    }

}



Schedule & Schedule::GetInstance( int threads /*= 4*/ )
{
    static Schedule schedule(threads);
    return schedule;
}

Schedule::Schedule( int threads )
{
    for (int i = 0; i != threads; ++i)
    {
        //std::thread *t = new std::thread([&](){this->CoreThread();});
        Processor *p = new Processor(*this);
        Galaxy::GalaxyRT::CThread *t = new Galaxy::GalaxyRT::CThread(*p, i);
        t->Start();
        //t->detach();
        _Cores.push_back(std::make_pair(p, t));
        //_Cores.push_back(t);
    }
}

void Schedule::CoreThread()
{
    while (true)
    {
        
        Process *node = NULL;
        
        try
        {
	        node = _ScheQueue.Get();
        }   
        catch (const std::exception &e)
        {
            //printf("%s\n", e.what());
            return;
        }
        
        node->Resume();
        if (node->State() == Process::ProcessStatus::YIELD)
        {
            _ScheQueue.Put(node);

        }
    }

}

void Schedule::PutTask( Process &node )
{
    _ScheQueue.Put(&node);
}

Processor::Processor(Schedule &sche):_Sche(sche)
{
    _Status._State = ProcessorStatus::UNBIND;
    _Status._GLRProcessId = -1;
    _Status._LastOp = 0;
    _Status._ScheCount = 0;
}

void* Processor::Run( const Galaxy::GalaxyRT::CThread &thread)
{
    assert(&thread);
    _Status._State = ProcessorStatus::BINDED;
    time(&_Status._Start);

    while (true)
    {
        
        Process *node = NULL;
        
        try
        {
            _Status._State = ProcessorStatus::WAITING;
            time(&_Status._LastOp);
	        node = _Sche._ScheQueue.Get();

            // Set Status
            _Status._State = ProcessorStatus::RUNNING;
            ++_Status._ScheCount;
            time(&_Status._LastOp);
            _Status._GLRProcessId = node->Id();
        }   
        catch (const std::exception &e)
        {
            // TODO: better error handling
            printf("%s\n", e.what());
            return NULL;
        }
        
        node->Resume();
        if (node->State() == Process::ProcessStatus::YIELD)
        {
            _Sche._ScheQueue.Put(node);

        }
    }
    return NULL;
}

Processor::~Processor()
{

}

void Processor::Clean( const Galaxy::GalaxyRT::CThread & thread)
{
    assert(&thread);
}
