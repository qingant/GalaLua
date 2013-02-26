#include "stdafx.h"
#include "lua.hpp"
#include <vector>
#include <string>
#include <exception>
#include <tr1/unordered_map>
#include "Runtime/Queue.hpp"
#include "Bus.hpp"
#include "Schedule.hpp"
#pragma once

namespace GLR
{
    class Runtime
    {
    private:
    	Runtime(const std::string &, int);
    private:
        static Runtime *_Instance;
    public:
    	~Runtime();
        static Runtime &GetInstance();
        static void Initialize(const std::string &host, int port);
        Bus & GetBus() const { return *_Bus; }
        Schedule & GetSchedule() const { return *_Schedule; }
        Process  & GetProcess(int pid) const;
    public:
        void Entry(const std::string &path, const std::string &entry);
    private:
        void Initialize();
    public:

        int NodeId() const { return _NodeId; }
        void NodeId(int val) { _NodeId = val; }
        const std::string &Host() const { return _Host; }
        void Host(const std::string &val) { _Host = val; }
 
    private:
        Schedule *_Schedule;
        Bus      *_Bus;
        std::string _Host;
       int _NodeId;

    };

};