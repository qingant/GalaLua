#ifndef  __GALA_LUA_H__
#define  __GALA_LUA_H__

#ifdef __cplusplus
extern "C" {
#endif   //__cplusplus

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#ifdef __cplusplus
}
#endif   //__cplusplus

#define DEBUG(msg) printf("DEBUG: %s  %d  msg:%s  \n",__func__,__LINE__,msg)

//t[k]=v ,table t is on the top of stack
// void setfield_int(lua_State *L, int v, const char *k)
#ifndef setfield_int
#define setfield_int(L, v, k)   do {        \
        lua_pushnumber((L), (v));           \
        lua_setfield((L), -2, (k));         \
    } while (0)
#endif

//t[k]=v ,table t is on the top of stack,argument "v" must be a zero-terminated string
//we can't use lua_pushlstring here,because it will push exactly "len" chars to lua(including '\0')
// void setfield_string(lua_State *L, const char *v, const char *k)
#ifndef setfield_string
#define setfield_string(L, v, k)    do {    \
        lua_pushstring((L), (v));           \
        lua_setfield((L), -2, (k));         \
    } while (0)
#endif

#endif   //__GALA_LUA_H__
