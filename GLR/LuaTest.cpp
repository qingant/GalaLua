// LuaTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "GLR.hpp"
#include "Process.hpp"
#include <unistd.h>
using namespace GLR;

Galaxy::GalaxyRT::CProcess _CProcess;

void daemonize(void)  
{  
    pid_t pid, sid;  
    int fd;   

    /* Fork off the parent process */  
    pid = CRT_fork();  
    if (pid < 0)    
    {     
        exit(1);  
    }     

    if (pid > 0)    
    {     
        exit(0); /*Killing the Parent Process*/  
    }     

    /* At this point we are executing as the child process */  

    /* Create a new SID for the child process */  
    sid = setsid();  
    if (sid < 0)    
    {  
        exit(1);  
    }  

    /* Change the current working directory. */  
    if ((chdir("/")) < 0)  
    {  
        exit(1);  
    }  

    fd = CRT_open("/dev/null",O_RDWR, 0);  
    if (fd != -1)  
    {  
        CRT_dup2 (fd,0);  
        CRT_dup2 (fd,1);  
        CRT_dup2 (fd,2);  
    }  
    /*resettign File Creation Mask */  
    CRT_umask(027);  
}  

int main( int argc, char* argv[] )
{
    //LN_ID_TYPE main_node_id = Process::CreateNode();
    //Process &main_node = Process::GetNodeById(main_node_id);
    //main_node.DoString("print(123)");
    //main_node.DoString("print(nid)");
    //main_node.DoString("print(node_spawn)");
    ////main_node.DoString("function run(){node_spawn(\"run\");}");
    //main_node.LoadFile("test.lua");
    //Process::Status();
    ////main_node.DoString("print(main)");
    ////main_node.PushFun("main");
    //try
    //{
    //    main_node.StackDump();
    //    main_node.PushFun("main");
    //    main_node.StackDump();
    //   // main_node.Resume();
    //   // main_node.Resume();
    //    main_node.Start(Schedule::GetInstance());
    //   /* LuaNode &node1 = LuaNode::GetNodeById(1);
    //    node1.StackDump();
    //    node1.Resume();
    //    LuaNode::Status();*/
    //}
    //catch (const std::exception &e)
    //{
    //    printf("Resume Error : %s\n", e.what());
    //}


    _CProcess.Initialize(argc,argv,NULL,"m:?e:d:?h:?p:?c:?D");

    if (_CProcess.ExistOption("D"))
    {
        daemonize();
    }

    std::string host;
    if (_CProcess.ExistOption("h"))
    {
        host = _CProcess.GetOption("h");
    }
    else
    {
        host = "0.0.0.0";
    }
    int port = 0;
    if (_CProcess.ExistOption("p"))
    {
        port = atoi(_CProcess.GetOption("p").c_str());
    }
    std::string path = "./\?.lua;";
    if (_CProcess.ExistOption("d"))
    {
        path += _CProcess.GetOption("d");
    }

    setenv("LUA_PATH", path.c_str(), 1);

    std::string cpath = "./\?.so;";
    if (_CProcess.ExistOption("c"))
    {
        cpath += _CProcess.GetOption("c");
    }

    setenv("LUA_CPATH", cpath.c_str(), 1);

    std::string entry = "main";
    if (_CProcess.ExistOption("e"))
    {
        entry = _CProcess.GetOption("e");
    }

    //GLR::Runtime::Initialize(argv[1], atoi(argv[2]));
    //GLR::Runtime::GetInstance().Entry(argv[3],argv[4]);
    GLR::Runtime::Initialize(host, port);
    if (_CProcess.ExistOption("m"))
    {
        std::string file=_CProcess.GetOption("m");
        GLR::Runtime::GetInstance().Entry(file,entry);
    }
    else
    {
        GLR::Runtime::GetInstance().EntryEx(_CProcess.Arguments[1]);
    }
    //_CProcess.SetArgument("What the hell? Do not work?");
    while (true)
    {
        usleep(1024*1024*1024);
    }
    //printf("test\n");
    //scanf("%d",new int);
    return 0;
}

