#ifndef  __LUACPP_HPP__
#define __LUACPP_HPP__
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#define DEBUG(msg) printf("DEBUG: %s  %d  msg:%s  \n",__func__,__LINE__,msg)

#define CALL_CPP_FUNCTION(L,func)   \
try\
{\
    func;\
}\
catch(std::exception &e)\
{\
    return luaL_error(L,"%s",e.what());\
}\
catch(...)\
{\
    return luaL_error(L,"Unknown exception %s  %d",__func__,__LINE__);\
}

//t[k]=v ,table t is on the top of stack
#ifndef setfield_int
#define setfield_int(L, v, k)   do {        \
        lua_pushnumber((L), (v));           \
        lua_setfield((L), -2, (k));         \
    } while (0)
#endif

//t[k]=v ,table t is on the top of stack,argument "v" must be a zero-terminated string
//we can't use lua_pushlstring here,because it will push exactly "len" chars to lua(including '\0')
#ifndef setfield_string
#define setfield_string(L, v, k)    do {    \
        lua_pushstring((L), (v));           \
        lua_setfield((L), -2, (k));         \
    } while (0)
#endif

#endif   //  __LUACPP_HPP__
