#include "stdafx.h"
#include "lua.hpp"
#include <vector>
#include <string>
#include <exception>
#ifdef _AIX
#include <unordered_map>
#endif
#include "Runtime/Queue.hpp"
#include "Runtime/Process.hpp"
#include "Bus.hpp"
#include "Schedule.hpp"
#include "Process.hpp"
#pragma once

namespace GLR
{

    class Runtime
    {
    public:
        enum LoadEntryFrom
        {
            L_STRING=0,
            L_FILE=1,
        };

    private:
    	Runtime(const std::string &, int);
    private:
        static Runtime *_Instance;
        typedef void (*GLR_INITIALIZER)(lua_State*);
    public:
        static GLR_INITIALIZER _Initializer;
        static Galaxy::GalaxyRT::CProcess *_pCProcess;
    public:
    	~Runtime();
        static Runtime &GetInstance();
        static void Initialize(const std::string &host, int port, GLR_INITIALIZER func = NULL, Galaxy::GalaxyRT::CProcess *_p = NULL);
        Bus & GetBus() const { return *_Bus; }
        Schedule & GetSchedule() const { return *_Schedule; }
        Process  & GetProcess(int pid) const;
    public:
        static std::string _GarFile;
    public:
        void Entry(const std::string &path, const std::string &entry);
        void Entry( const std::string &gar,const std::string &module, const std::string &entry );
        void EntryEx(const std::string &path,LoadEntryFrom  from=L_FILE);
    private:
        void Initialize();
    public:

        int NodeId() const { return _NodeId; }
        void NodeId(int val) { _NodeId = val; }
        const std::string &Host() const { return _Host; }
        void Host(const std::string &val) { _Host = val; }
        time_t Stamp() const {return _Stamp;}
        void _ElegantExit();
        void ElegantExit();
    private:
        Schedule *_Schedule;
        Bus      *_Bus;
        std::string _Host;
       int _NodeId;
       time_t  _Stamp;


    };
    class Exiter:public Galaxy::GalaxyRT::CRunnable
    {
    public:
        virtual void* Run( const Galaxy::GalaxyRT::CThread &t );
    };

};
