#include "LuaGlobal.hpp"

Globals::Globals()
{

}

Globals::~Globals()
{

}

void Globals::Erase(const std::string &id)
{
    _VarMap.erase(id);
}

void Globals::Put(const std::string& id, void **data, const std::string& type)
{
    UserData ud;
    ud.Content = *data;
    ud.Name = type;
    _VarMap.insert(std::make_pair(id, ud));
}

const Globals::UserData& Globals::Get(const std::string& id)
{
    return _VarMap[id];
}

Globals LuaGlobal::GlobalVars;
const char *const LuaGlobal::FINALIZER="close";
Galaxy::GalaxyRT::CPthreadMutex LuaGlobal::GlobalMutex;

int LuaGlobal::Finalize(lua_State *l)
{
    //get the metatable name
    if (lua_getmetatable(l, 1) == 0)
    {
        return luaL_error(l, "userdata do not has metatable  %s %d", __func__, __LINE__);
    }
    lua_getfield(l, -1, "__my_id");
    const char *id = luaL_checkstring(l, -1);
    lua_pop(l,1);

    Galaxy::GalaxyRT::CLockGuard m(&GlobalMutex);
    const Globals::UserData& ud=GlobalVars.Get(id);
   
    --ud.Count;

    if (ud.Count==0)
    {
        //恢复metatable.__index的值
        lua_pushvalue(l,-1);
        lua_setfield(l,-2,"__index");

        lua_getfield(l,-1,FINALIZER);
        lua_pushvalue(l,1);
        lua_call(l,1,0);
        GlobalVars.Erase(id);
    }

    return 0;
}

int LuaGlobal::__Index(lua_State *l)
{  
    const char *key=luaL_checkstring(l,2);
    if (strcmp(key,FINALIZER)==0)
    {
        lua_pushcfunction(l,Finalize);
    }
    else
    {
        lua_getmetatable(l, 1);
        lua_getfield(l,-1,key);
    }
    return 1;
}

//XXX:调用此函数时，metatable必须在栈顶
inline void LuaGlobal::ModifyState(lua_State *l,const char *id)
{
    lua_pushvalue(l, -1);

    lua_pushstring(l,id);
    lua_setfield(l, -2, "__my_id");
    
    //修改metatable.__index的值，以修改对部分key的访问
    lua_pushcfunction(l,__Index);
    lua_setfield(l,-2,"__index");
    
    //XXX:直接去掉gc，global变量采用手动垃圾回收，
    //会导致userdata在非global变量时也没有gc
    lua_pushnil(l);
    lua_setfield(l,-2,"__gc");
    
    //设置metatable
    lua_setmetatable(l, -3);
    lua_settop(l, -2);
}

int LuaGlobal::RegisterGlobal(lua_State *l)
{
    Galaxy::GalaxyRT::CLockGuard m(&GlobalMutex);
    void *content = lua_touserdata(l, 2);
    const char *id = luaL_checkstring(l, 1);

    const Globals::UserData& ud = GlobalVars.Get(id);

    if (ud.Content != NULL)
    {
        lua_pushnil(l);
        std::string err(id);
        err += " global variable existed";
        lua_pushstring(l, err.c_str());
        return 2;
    }

    //get the metatable
    if (lua_getmetatable(l, 2) == 0)
    {
        return luaL_error(l, "userdata do not has metatable  %s %d", __func__, __LINE__);
    }

    //所有要注册为global变量的对象都必须设置__my_name
    lua_getfield(l, -1, "__my_name");
    const char *type = luaL_checkstring(l, -1);
    lua_pop(l,1);

    ModifyState(l,id);

    GlobalVars.Put(id, (void **)content, type);

    return 0;
}

int LuaGlobal::GetGlobal(lua_State *l)
{
    Galaxy::GalaxyRT::CLockGuard m(&GlobalMutex);
    const char *id = luaL_checkstring(l, 1);
    const Globals::UserData& ud = GlobalVars.Get(id);

    if (ud.Content == NULL)
    {
        lua_pushnil(l);
        std::string err(id);
        err += " not found";
        lua_pushstring(l, err.c_str());
        return 2;
    } 
    else
    {
        ++ud.Count; 

        void *p = lua_newuserdata(l, sizeof(ud.Content));
        memcpy(p, &ud.Content, sizeof(ud.Content));
        luaL_getmetatable(l, ud.Name.c_str());

        ModifyState(l,id);

        return 1;
    }
}

extern "C" int luaopen_global(lua_State *l)
{

    static const  luaL_Reg reg[] = {
        { "global", LuaGlobal::RegisterGlobal},
        { "get_global", LuaGlobal::GetGlobal},
        {NULL,NULL}
    };

    luaL_register(l, "global", reg);
    return 1;
}

