#include "LuaCPP.hpp"
#include <sys/types.h>
#include <unistd.h>

static int l_getpid(lua_State *l)
{
    lua_pushnumber(l,getpid());
    return 1;
}
static int l_getuid(lua_State *l)
{
    lua_pushnumber(l,getuid());
    return 1;
}


extern "C" int luaopen_osext(lua_State *l)
{
    struct luaL_Reg f[]={
        {"getpid",l_getpid},
        {"getuid",l_getuid},
        {NULL,NULL}
    };

    luaL_register(l,"OSEXT",f);
    return 1;
}
