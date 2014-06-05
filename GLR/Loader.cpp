/**
 * @author: Ma Tao
 */

#include "Loader.hpp"

using namespace GLR;

Controller::Controller(const std::string &name,void *arg)
    :_Name(name),
     _Module(NULL),
     _Driver(NULL)
{
    Load(arg);
}

Controller::~Controller()
{
    UnLoad();
}
void Controller::Load(void *arg)
{
    _Module = dlopen(_Name.c_str(), RTLD_NOW|RTLD_LOCAL);
    if (_Module == NULL)
    {
		char err[ERR_MSG_MAX_SIZE]={0};
		char *dlerr=dlerror();

		snprintf(err,sizeof(err),"Cannot Load Dll: %s",dlerr);
		THROW_EXCEPTION_EX(err);
    }
    GLR_CTL_INIT_FUNC init_func = (GLR_CTL_INIT_FUNC)dlsym(_Module, "Initialize");
    _Driver = init_func(arg);
    if (_Driver == NULL)
    {
		char err[ERR_MSG_MAX_SIZE]={0};
		char *dlerr=dlerror();

		snprintf(err,sizeof(err),"Cannot Load Controller: %s",dlerr);
		THROW_EXCEPTION_EX(err);
    }
    GLR::Bus::GetInstance().RegisterDevice(_Driver);
}

void Controller::UnLoad()
{
    GLR_CTL_FINALIZE_FUNC finalizer = (GLR_CTL_FINALIZE_FUNC)dlsym(_Module, "Finalize");
    if (finalizer != NULL)
    {
        finalizer(NULL);
    }
    delete _Driver;
    _Driver = NULL;

    int rt = dlclose(_Module);
    if (rt != 0)
    {
		char err[ERR_MSG_MAX_SIZE]={0};
		char *dlerr=dlerror();

		snprintf(err,sizeof(err),"Cannot Close Dll: %s",dlerr);
		THROW_EXCEPTION_EX(err);
    }
    _Module = NULL;
}




Loader::Loader()
{

}

Loader::~Loader()
{
}


int Loader::Load(lua_State *l)
{
    Galaxy::GalaxyRT::CLockGuard _(&GetInstance()->_Mutex); 
    const char *name = luaL_checkstring(l, 1);

    try
    {
        Controller *controller = GetInstance()->_Controllers[name];
        if (controller != NULL)
        {
            std::string err(name);
            err+=" had alreadly loaded";
            THROW_EXCEPTION_EX(err.c_str());
        }

        GetInstance()->_Controllers[name] = new Controller(name,l);
    }
    catch (const Galaxy::GalaxyRT::CException &e)
    {
        luaL_error(l, e.what());
    }
    catch (const std::exception &e)
    {
        luaL_error(l, e.what());
    }
    
    catch (...)
    {
        std::string err("unknown exception while loading ");
        err+=name;
        luaL_error(l,err.c_str());
    }
    
    return 0;
}

int Loader::UnLoad(lua_State *l)
{
    Galaxy::GalaxyRT::CLockGuard _(&GetInstance()->_Mutex);
    const char *name = luaL_checkstring(l, 1);
    Controller *controller = GetInstance()->_Controllers[name];
    if (controller != NULL)
    {
        delete controller;
        GetInstance()->_Controllers.erase(name);
    }
    else
    {
        luaL_error(l, "%s not exist", name);
    }
    return 0;
}

Loader * GLR::Loader::GetInstance()
{
	if (_Instance == NULL)
	{
		Galaxy::GalaxyRT::CLockGuard _(&_Lock);
		if (_Instance == NULL)
		{
			_Instance = new Loader;
		}
		
	}
	return _Instance;
	
}

Galaxy::GalaxyRT::CPthreadMutex GLR::Loader::_Lock;


Loader * GLR::Loader::_Instance = NULL;
