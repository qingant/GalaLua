/**
 * @author: Ma Tao
 * @date:
 */
#include "NamedProcessTable.hpp"


GLR::NamedProcessTable::NamedProcessTable()
: _RDLock(_Lock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK),
_WRLock(_Lock, Galaxy::GalaxyRT::CRWLockInterface::WRLOCK)
{

}

GLR::NamedProcessTable::~NamedProcessTable()
{

}

GLR::NamedProcessTable::IDTYPE GLR::NamedProcessTable::Get(const std::string &name)
{
	Galaxy::GalaxyRT::CLockGuard _(&_RDLock);
	ID_MAP_TYPE::const_iterator it = _Map.find(name);
	if (it != _Map.end())
	{
		return it->second;
	}
	THROW_EXCEPTION_EX("Process not found");
}

void GLR::NamedProcessTable::Set(const std::string &name, GLR::NamedProcessTable::IDTYPE id)
{
	Galaxy::GalaxyRT::CLockGuard _(&_WRLock);
	ID_MAP_TYPE::const_iterator it = _Map.find(name);
	if (it != _Map.end())
	{
		THROW_ERROR(ServerExist, "");
	}

	_Map[name] = id;
}

void GLR::NamedProcessTable::Del(const std::string &name)
{
	Galaxy::GalaxyRT::CLockGuard _(&_WRLock);
	_Map.erase(name);
}

int GLR::NamedProcessTable::Register(lua_State *l)
{
	const char *name = luaL_checkstring(l, 1);
    lua_getglobal(l, "__id__");
	IDTYPE      id = luaL_checkinteger(l, -1);
	try
	{
		GetInstance()->Set(name, id);
        lua_pushstring(l, name);
        lua_setglobal(l, "__name__");
	}
	catch (const GLR::ServerExist &e)
	{
		// TODO:
		luaL_error(l, "name '%s' has been registered", name);
	}
	return 0;
}

int GLR::NamedProcessTable::Registered(lua_State *l)
{
	GetInstance()->_Registered(l);
	return 1;
}

int GLR::NamedProcessTable::UnRegister(lua_State *l)
{
    try {
        GetInstance()->_UnRegistered(l);
    } catch (const GLR::NotRegistered &){
        luaL_error(l, "NotRegistered");
    }
    return 0;
}


Galaxy::GalaxyRT::CPthreadMutex GLR::NamedProcessTable::_InstanceLock;
GLR::NamedProcessTable * GLR::NamedProcessTable::GetInstance()
{
    if (_Instance == NULL)
    {
        Galaxy::GalaxyRT::CLockGuard _(&_InstanceLock);
        if (_Instance == NULL)
        {
            _Instance = new NamedProcessTable;
        }
    }
	return _Instance;
}

void GLR::NamedProcessTable::_Registered(lua_State *l)
{
	lua_newtable(l);
	Galaxy::GalaxyRT::CLockGuard _(&_RDLock);
	for (ID_MAP_TYPE::const_iterator it = _Map.begin(); it != _Map.end(); ++it)
	{
		lua_pushinteger(l, it->second);
		lua_setfield(l, -2, it->first.c_str());
	}
}

void GLR::NamedProcessTable::_UnRegistered(lua_State *l)
{
    lua_getglobal(l,"__name__");
    if (lua_isnil(l, -1) == 1)
    {
        THROW_ERROR(NotRegistered, "");
    }
    const char *name = luaL_checkstring(l, -1);
    Galaxy::GalaxyRT::CLockGuard _(&_WRLock);
    ID_MAP_TYPE::const_iterator it = _Map.find(name);
    if (it == _Map.end())
    {
        THROW_ERROR(NotRegistered, "");
    }
    _Map.erase(name);
}

void GLR::NamedProcessTable::CreateOpTable(lua_State *l)
{
    const struct luaL_Reg _ops[] = {
        {"register", GLR::NamedProcessTable::Register},
        {"unregister", GLR::NamedProcessTable::UnRegister},
        {"registered", GLR::NamedProcessTable::Registered},
        {NULL, NULL}
    };
    lua_newtable(l);
    luaL_register(l, NULL, _ops);
}

void GLR::NamedProcessTable::Clean(lua_State *l)
{
    lua_getglobal(l, "__name__");
    if (!lua_isnil(l, -1))
    {
        const char *name = lua_tostring(l, -1);
        try
        {
            GetInstance()->Del(name);
        }
        catch (const Galaxy::GalaxyRT::CException &e)
        {
            //TODO: logging
        }
    }
}


GLR::NamedProcessTable * GLR::NamedProcessTable::_Instance = NULL;
