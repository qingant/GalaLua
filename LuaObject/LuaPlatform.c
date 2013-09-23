#include "Lua.h"

#ifndef GLR_VER
#define GLR_VER "0.1.0"
#endif
const char *version=GLR_VER;

#if defined(_AIX)
const char *platform= "AIX";
#endif   //_AIX

#if defined(__linux__)

#if defined(__x86_64__) || defined(_M_X64)
const char *platform= "Linux64";

#elif defined(__i386) || defined(_M_IX86)
const char *platform= "Linux32";
#endif 

#endif   //__linux__


#if defined(__GNUC__) || defined(__GNUG__)
const char *compiler="gcc "__VERSION__;

#elif defined(__IBMC__) || defined(__IBMCPP__)
const char *compiler="xlc "__xlc__;
#endif

void RegisterInfo(lua_State *l)
{
    lua_pushstring(l,"info");

    lua_newtable(l);

    setfield_string(l,version,"version");
    setfield_string(l,platform,"platform");
    setfield_string(l,compiler,"compiler");
    
    lua_settable(l,-3);
}


int luaopen_platform(lua_State *l)
{

    lua_newtable(l);

    RegisterInfo(l);

    return 1;
}
