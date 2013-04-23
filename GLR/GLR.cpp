#include "GLR.hpp"
#include "Schedule.hpp"
#include "Bus.hpp"
#include "LuaNode.hpp"
#include "Controllers.hpp"
#include "SocketController.hpp"
#include "BusController.hpp"
using namespace GLR;

GLR::Runtime *GLR::Runtime::_Instance = NULL;
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


void GLR::Runtime::Initialize()
{
    _Schedule = &Schedule::GetInstance();
    _Bus = &Bus::GetInstance();
    _Bus->RegisterDevice(new Clock());
    _Bus->RegisterDevice(new SocketController());
    _Bus->RegisterDevice(new BusController());
    //_Host = 
    time(&_Stamp);
}

void GLR::Runtime::Initialize( const std::string &host, int port )
{
    _Instance = new Runtime(host, port);
    _Instance->Initialize();
}
Process  & GLR::Runtime::GetProcess( int pid ) const
{
    return Process::GetNodeById(pid);
}

void GLR::Runtime::EntryEx( const std::string &path )
{
    LN_ID_TYPE main_node_id = Process::CreateNode();
    Process &main_node = Process::GetNodeById(main_node_id);
    main_node.LoadFile(path);
    main_node.Start(*_Schedule);

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
