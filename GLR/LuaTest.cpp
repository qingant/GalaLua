// LuaTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "GLR.hpp"
#include "Process.hpp"
#include <unistd.h>


using namespace GLR;

class GlrApp:public Galaxy::GalaxyRT::IApp      
{
public:
    GlrApp(int argc, CHAR **argv, const std::string &optfmt):IApp(argc, argv, NULL, optfmt)
    {
        
        GLR::Runtime::_pCProcess = _Process.get();
    }
    virtual void Run()
    {
        if (_Process->ExistOption("D"))
        {
            std::string err_log("/dev/null"),out_log("/dev/null");
            if (_Process->ExistOption("E"))
            {
                err_log = _Process->GetOption("E");
            }

            if (_Process->ExistOption("O"))
            {
                out_log = _Process->GetOption("O");
            }

            daemonize(out_log.c_str(),err_log.c_str());
        }
        initGlrSettings();
        _Run();

    }
    ~GlrApp()
    {

    }
private:
    void _Run()
    {
        try
        {

            GLR::Runtime::Initialize(host, port, NULL, _Process.get());
            if (_Process->ExistOption("g"))
            {
                GALA_DEBUG("RUNNING Gar package\n");
                std::string file=_Process->GetOption("g");
                char cwd[256] = {};
                getcwd(cwd, sizeof(cwd));

                //not absolute path
                if (file[0]!='/')
                {
                    if (cwd!=NULL)
                    {
                        char path[256] = {};
                        CRT_snprintf(path,sizeof(path),"%s/bin/%s",cwd,file.c_str());
                        file=path;
                    }
                }
                std::string module;
                if (_Process->ExistOption("m"))
                {
                    module=_Process->GetOption("m");
                }
                GLR::Runtime::GetInstance().Entry(file,module,entry);
            }
            else if (_Process->ExistOption("m"))
            {
                std::string file=_Process->GetOption("m");
                GLR::Runtime::GetInstance().Entry(file,entry);
            }
            else
            {
                GLR::Runtime::GetInstance().EntryEx(_Process->Arguments[1]);
            }
            //_CProcess.SetArgument("What the hell? Do not work?");
            while (true)
            {
                usleep(1024*1024*1024);
            }
        }
        catch (...)
        {
            Runtime::GetInstance()._ElegantExit();
        }
    }
private:
    std::string host;
    int         port;
    std::string path,cpath,entry;
private:
    void initGlrSettings()
    {

        if (_Process->ExistOption("h"))
        {
            host = _Process->GetOption("h");
        }
        else
        {
            host = "0.0.0.0";
        }
        port = 0;
        if (_Process->ExistOption("p"))
        {
            port = atoi(_Process->GetOption("p").c_str());
        }
        cpath = "./\?.so;";
        char *cwd = NULL;
        char clib_path[256] = {};

        char lib_path[256] = {};
        cwd = getenv("GDK_HOME");
        if (cwd != NULL)
        {
            CRT_snprintf(clib_path, sizeof(clib_path), "%s/lib/lua/?.so;%s/lib/lua/?.so;", cwd, getenv("HOME"));
            CRT_snprintf(lib_path, sizeof(lib_path), "%s/lib/lua/?.lua;%s/lib/lua/?.lua;", cwd, getenv("HOME"));
        }

        std::string path = "./\?.lua;";
        path += lib_path;
        if (_Process->ExistOption("d"))
        {
            path += _Process->GetOption("d");
        }

        setenv("LUA_PATH", path.c_str(), 1);


        cpath += clib_path;
        if (_Process->ExistOption("c"))
        {
            cpath += _Process->GetOption("c");
        }

        setenv("LUA_CPATH", cpath.c_str(), 1);

        entry = "main";
        if (_Process->ExistOption("e"))
        {
            entry = _Process->GetOption("e");
        }


    }
    static void daemonize(const char *out_log,const char *err_log)
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

        for (int i=0;i<1024;++i)
        {
            close(i);
        }

        fd = CRT_open("/dev/null",O_RDWR,0);
        if (fd == -1)
        {
            exit(1);
        }
        
        fd = CRT_open(out_log,O_WRONLY|O_CREAT|O_APPEND, 0644);  
        if (fd == -1 && (fd=dup2(0,1))==-1)
        {
            exit(1); 
        }

        fd = CRT_open(err_log,O_WRONLY|O_CREAT|O_APPEND, 0644);
        if (fd == -1 && (fd=dup2(0,2))==-1)
        {
            exit(1);
        }
        /*resettign File Creation Mask */  
        CRT_umask(027);  
    }
};


int main( int argc, char* argv[] )
{
    GlrApp app(argc, argv, "m:?e:d:?h:?p:?c:?D");
    app.Run();
    return 0;
}

