/**
 * @author: Ma Tao
 */

#include "GLR.hpp"
#include "ControllerInterface.hpp"

#pragma once

namespace GLR {
    class Controller : public Galaxy::GalaxyRT::CNonCopyAble
    {
    public:
        Controller(const std::string &);
        ~Controller();
    private:
        void Load();
        void UnLoad();
    private:
        const std::string _Name;
        void  *_Module;
        IController *_Driver;
    };

    class Loader : public Galaxy::GalaxyRT::CNonCopyAble
    {
    private:
        Loader();
	public:
        ~Loader();
	public:
		static Loader *GetInstance();
	private:
		static Loader *_Instance;
		static Galaxy::GalaxyRT::CPthreadMutex _Lock;
    public:
        static int Load(lua_State *l);
        static int UnLoad(lua_State *l);
    private:
        std::map<std::string, Controller*> _Controllers;
        Galaxy::GalaxyRT::CPthreadMutex    _Mutex;
    };
}