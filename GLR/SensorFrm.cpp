/*
 * SensorFrm.cpp
 *
 *  Created on: May 21, 2013
 *      Author: esuomyekcim
 */
 



#include "stdafx.h"
#include "GLR.hpp"
#include "Process.hpp"
#include <unistd.h>

#include "resource/resource.h"
#include "resource/resxloader.h"

using namespace GLR;



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

    //close all useless fd(maybe invalid)
    for (int i=0;i<fd;++i)
    {
        close(i);
    }
    for (int i=fd+1;i<1024;++i)
    {
        close(i);
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

    Galaxy::GalaxyRT::CProcess _CProcess;
    GLR::Runtime::_pCProcess=&_CProcess;

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
    std::string cpath = "./\?.so;";
    char *cwd = NULL;
    char clib_path[256] = {};

    char lib_path[256] = {};
    cwd = getenv("GDK_HOME");
    if (cwd != NULL)
    {
        snprintf(clib_path, sizeof(clib_path), "%s/lib/lua/?.so;%s/lib/lua/?.so;", cwd, getenv("HOME"));
        snprintf(lib_path, sizeof(lib_path), "%s/lib/lua/?.lua;%s/lib/lua/?.lua;", cwd, getenv("HOME"));
    }

    std::string path = "./\?.lua;";
    path += lib_path;
    if (_CProcess.ExistOption("d"))
    {
        path += _CProcess.GetOption("d");
    }

    setenv("LUA_PATH", path.c_str(), 1);


    cpath += clib_path;
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
    GLR::Runtime::Initialize(host, port, GLR_initializer, &_CProcess);
    if (_CProcess.ExistOption("g"))
    {
        GALA_DEBUG("RUNNING Gar package\n");
        std::string file=_CProcess.GetOption("g");

        std::string module;
        if (_CProcess.ExistOption("m"))
        {
            module=_CProcess.GetOption("m");
        }
        GLR::Runtime::GetInstance().Entry(file,module,entry);
    }
    else if (_CProcess.ExistOption("m"))
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

