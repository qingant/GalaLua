#include "GLR.hpp"
#include "Schedule.hpp"
#include "Bus.hpp"
#include "LuaNode.hpp"
#include "Controllers/Clock.hpp"
#include "Controllers/SocketController.hpp"
#include "Controllers/BusController.hpp"

using namespace GLR;

GLR::Runtime *GLR::Runtime::_Instance = NULL;

GLR::Runtime::GLR_INITIALIZER GLR::Runtime::_Initializer = NULL;
Galaxy::GalaxyRT::CProcess  *GLR::Runtime::_pCProcess= NULL;

GLR::Runtime::Runtime(const std::string &host, int port)
    :_Host(host), _NodeId(port)
{
    //Initialize();
}

GLR::Runtime::~Runtime()
{

}

Runtime & GLR::Runtime::GetInstance()
{
    if (_Instance == NULL)
    {
        THROW_EXCEPTION_EX("Not Initialized");
    }
    else
    {
        return *_Instance;
    }
}

int GLR::Runtime::GetThreads()
{
    const char *threads;
    if (_pCProcess->ExistOption("T"))
    {
        threads=_pCProcess->GetOption("T").c_str();
    }
    else
    {
        threads=getenv("GDK_THREADS");
    }

    if (threads!=NULL)
    {
        return strtol(threads, NULL, 0);
    }
    return -1;
}

void GLR::Runtime::Initialize()
{
    _Schedule = &Schedule::GetInstance(GetThreads());
    
    _Bus = &Bus::GetInstance();
    _Bus->RegisterDevice(new Clock());
    _Bus->RegisterDevice(new SocketController());
    _Bus->RegisterDevice(new BusController());
    time(&_Stamp);
}

void GLR::Runtime::Initialize( const std::string &host, int port, GLR::Runtime::GLR_INITIALIZER func, Galaxy::GalaxyRT::CProcess *_p)
{
    _Initializer = func;
    _pCProcess = _p;
    _Instance = new Runtime(host, port);
    _Instance->Initialize();
}
Process  & GLR::Runtime::GetProcess( int pid ) const
{
    return Process::GetNodeById(pid);
}

void GLR::Runtime::EntryEx( const std::string &path ,GLR::Runtime::LoadEntryFrom  type)
{
    LN_ID_TYPE main_node_id = Process::CreateNode();
    Process &main_node = Process::GetNodeById(main_node_id);
    if (type==GLR::Runtime::L_FILE)
    {
        main_node.LoadFile(path);
    }
    else if (type==GLR::Runtime::L_STRING)
    {
        main_node.LoadString(path);
    }
    main_node.Start(*_Schedule);

}

void GLR::Runtime::_ElegantExit()
{
    GetSchedule().Stop();
}

void GLR::Runtime::ElegantExit()
{
    Galaxy::GalaxyRT::CRunnable &r = *(new Exiter());
    (new Galaxy::GalaxyRT::CThread(r, 0))->Start();
}

void Runtime::Entry( const std::string &path, const std::string &entry )
{
    LN_ID_TYPE main_node_id = Process::CreateNode();
    Process &main_node = Process::GetNodeById(main_node_id);
    /* main_node.LoadFile(path);
    main_node.PushFun(entry);*/
    //lua_getglobal(main_node._Stack, "glr.spawn");
    main_node.Entry(path, entry);

    main_node.Start(*_Schedule);
}

void* GLR::Exiter::Run( const Galaxy::GalaxyRT::CThread & )
{
    GLR::Runtime::GetInstance()._ElegantExit();
    kill(getpid(), SIGINT);
    return NULL;
}
