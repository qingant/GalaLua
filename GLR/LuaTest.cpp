// LuaTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "GLR.hpp"
#include "Process.hpp"
using namespace GLR;

Galaxy::GalaxyRT::CProcess _CProcess;

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

    _CProcess.Initialize(argc,argv,NULL,"h:p:f:e:");
    std::string host=_CProcess.GetOption("h");
    int port=atoi(_CProcess.GetOption("p").c_str());
    std::string file=_CProcess.GetOption("f");
    std::string entry=_CProcess.GetOption("e");

    //GLR::Runtime::Initialize(argv[1], atoi(argv[2]));
    //GLR::Runtime::GetInstance().Entry(argv[3],argv[4]);
    GLR::Runtime::Initialize(host, port);
    GLR::Runtime::GetInstance().Entry(file,entry);
    _CProcess.SetArgument("What the hell? Do not work?");
    while (true)
    {
        usleep(1024*1024*1024);
    }
    //printf("test\n");
    //scanf("%d",new int);
	return 0;
}

