#include "Platform.hpp"

using namespace GLR;

const char *GLRInfo::version="0.1.0";

#if defined(_AIX)
const char *GLRInfo::platform= "AIX";
#endif   //_AIX

#if defined(__linux__)

#if defined(__x86_64__) || defined(_M_X64)
const char *GLRInfo::platform= "Linux64";

#elif defined(__i386) || defined(_M_IX86)
const char *GLRInfo::platform= "Linux32";
#endif 

#endif   //__linux__


#if defined(__GNUC__) || defined(__GNUG__)
const char *GLRInfo::compiler="gcc "__VERSION__;

#elif defined(__IBMC__) || defined(__IBMCPP__)
const char *GLRInfo::compiler="xlc "__xlc__;
#endif

void GLRInfo::RegisterInfo(lua_State *l)
{
    lua_pushstring(l,"info");

    lua_newtable(l);

    setfield_string(l,GLRInfo::version,"version");
    setfield_string(l,GLRInfo::platform,"platform");
    setfield_string(l,GLRInfo::compiler,"compiler");
    
    lua_settable(l,-3);
}


extern "C" int luaopen_platform(lua_State *l)
{

    lua_newtable(l);

    GLRInfo::RegisterInfo(l);

    return 1;
}
