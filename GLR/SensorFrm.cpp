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

#include <string>
#include <sysexits.h>
#include "SigleInstantiation.hpp"

#include "resource/resource.h"
#include "resource/resxloader.h"

#include <sys/wait.h>

using namespace GLR;

void start(void)
{
again:
    int pid=fork();
    if (pid<0){
        throw "fork error ..\n";
    }
    else if (pid>0){
        while (true){
            int ret=waitpid(pid,NULL,0);
            if (ret<0)
            {
                throw "waitpid error ...\n";
            }
            //TODO: wait some seconds before start agent??
            goto again;
        }
    }

}

void daemonize(int argc,char **argv)  
{  
    pid_t pid, sid;  
    int fd;   

    /* Fork off the parent process */  
    pid = CRT_fork();  
    if (pid < 0){     
        exit(1);  
    }
    else if (pid > 0){
        exit(0);
    }
    else{
        /* Create a new SID for the child process */  
        sid = setsid();  
        if (sid < 0){  
            exit(1);
        }

        //close all useless fd(maybe invalid)
        for (int i=0;i<1024;++i){
            close(i);
        }

        fd = CRT_open("/dev/null",O_RDWR, 0);
        if (fd != -1){  
            CRT_dup2(fd,0);
            CRT_dup2(fd,1);
            CRT_dup2(fd,2);
        }

        /*resettign File Creation Mask */  
        CRT_umask(027);
    }

    start();
}


int main( int argc, char* argv[] )
{
    try
    {
        Galaxy::GalaxyRT::CProcess _CProcess;
        GLR::Runtime::_pCProcess=&_CProcess;

        _CProcess.Initialize(argc,argv,NULL,"m:?e:d:?h:?p:?c:?DSw:");
        if (_CProcess.ExistOption("v"))
        {
            std::cerr<<"Build: "<<__DATE__<<" "<<__TIME__<<std::endl;
            exit(EXIT_SUCCESS);
        }
/**
 * 处理pidfile（开始）
 */
        std::string pidpathname;
        if (_CProcess.ExistOption("w"))
        {
            pidpathname = _CProcess.GetOption("w");
            if (pidpathname.length() == 0)
            {
                std::cerr<<"Option 'w' require parameter."<<std::endl;
                exit(EX_USAGE);
            }
            pidpathname += std::string("/glr_sl.pid");
        }
        else
        {
            std::cerr<<"No 'w' option, "
                    "PLEASE ENTER THE CURRENT WORKING DIRECTORY!"<<std::endl;
            exit(EX_USAGE);
        }
/**
 * 处理pidfile（结束）
 */
        if (_CProcess.ExistOption("S"))
        {
            const std::string pidstring = _CProcess.GetOption("S");
            const char * const cpidstring = pidstring.c_str();
            char *endstr;
            errno = 0;
            const pid_t pid = (pid_t) strtoul(cpidstring, &endstr, 10);
            DaemonProcTerm(pidpathname.c_str(), pid);
        }

        if (_CProcess.ExistOption("D"))
        {
            daemonize(argc,argv);
        }

        SigleInstantiation sigledaemon(std::string("glr_sl"), pidpathname);

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
        char *cwd = NULL;

        std::string entry = "main";
        if (_CProcess.ExistOption("e"))
        {
            entry = _CProcess.GetOption("e");
        }

        GLR::Runtime::Initialize(host, port, GLR_initializer, &_CProcess);
        if (_CProcess.ExistOption("g"))
        {
            GALA_DEBUG("RUNNING Gar package\n");
            std::string file=_CProcess.GetOption("g");
            //not absolute path
            if (file[0]!='/')
            {
                if (cwd!=NULL)
                {
                    char path[256] = {};
                    snprintf(path,sizeof(path),"%s/bin/%s",cwd,file.c_str());
                    file=path;
                }
            }
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
    }
    catch (const std::string &except)
    {
        std::cerr<<"exception: "<<except<<std::endl;
        return 1;
    }
    catch (Galaxy::GalaxyRT::CException &except)
    {
        std::cerr<<except.what()<<std::endl;
        Runtime::GetInstance()._ElegantExit();
        return EX_IOERR;
    }
    catch (...)
    {
        Runtime::GetInstance()._ElegantExit();
        return EX_IOERR;
    }
    return 0;
}

