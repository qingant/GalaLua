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
void setfield_int(lua_State *L,int v,const char *k)
{
    lua_pushnumber(L,v);
    lua_setfield(L,-2,k);
}
//t[k]=v ,table t is on the top of stack
void setfield_string(lua_State *L,const char *v,const char *k)
{
    lua_pushstring(L,v);
    lua_setfield(L,-2,k);
}

#endif   //  __LUACPP_HPP__
