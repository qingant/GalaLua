#include "LuaGlobal.hpp"

Globals::Globals()
{

}

Globals::~Globals()
{

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

//All userdata pass into this function should have registered
//it's metatable name to it's metatable with key "__my_name"
int LuaGlobal::RegisterGlobal(lua_State *l)
{
    void *ud = lua_touserdata(l, 2);
    const char *id = luaL_checkstring(l, 1);

    //get the metatable name
    if (lua_getmetatable(l, 2) == 0)
    {
        return luaL_error(l, "userdata do not has metatable  %s %d", __func__, __LINE__);
    }
    //!+ ��Ҫ����������;���"__my_name"����
    lua_getfield(l, -1, "__my_name");

    const char *type = luaL_checkstring(l, -1);

    GlobalVars.Put(id, (void **)ud, type);
    lua_pushvalue(l, -1);
    return 0;
}

int LuaGlobal::GetGlobal(lua_State *l)
{
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
        void *p = lua_newuserdata(l, sizeof(ud.Content));
        memcpy(p, &ud.Content, sizeof(ud.Content));
        luaL_getmetatable(l, ud.Name.c_str());

        lua_pushvalue(l, -1);

        //clear __gc
        lua_pushnil(l);
        lua_setfield(l, -2, "__gc");

        lua_setmetatable(l, -3);
        lua_settop(l, -2);

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

