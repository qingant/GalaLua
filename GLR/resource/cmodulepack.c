/*
 * cmodulepack.c
 *
 *  Created on: May 10, 2013
 *      Author: esuomyekcim
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/types.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(linux) || defined(__linux) || defined(__linux__)
#include <sysexits.h>
#endif

#define LUA_LIB

#ifndef RESXLOADER
#define RESXLOADER  "resxloader"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

extern int luaopen_cjson(lua_State *);
extern int luaopen_struct (lua_State *);
extern int luaopen_iconv(lua_State *);
extern int luaopen_minizip(lua_State *);
extern int luaopen__dir(lua_State *);
extern int luaopen_lxp (lua_State *);

void resx_openlibs(lua_State * const state)
{
    const luaL_Reg lualibray[] =
    {
            { "cjson", luaopen_cjson },
            { "struct", luaopen_struct },
            { "iconv", luaopen_iconv },
            { "minizip", luaopen_minizip },
            { "_dir", luaopen__dir },
            { "lxp", luaopen_lxp },
            { (const char *) NULL, (lua_CFunction) NULL }
    };
    fprintf(stderr, "File %s, Function %s, Line %d, gettop() = %d.\n",
            __FILE__, __FUNCTION__, __LINE__, lua_gettop(state));
    lua_getfield(state, LUA_GLOBALSINDEX, LUA_LOADLIBNAME); /* 'package' */
    lua_getfield(state, -1, "loaded"); /* package.loaded */

    for(const luaL_Reg *ite = &lualibray[0]; ite->func != (lua_CFunction) NULL;
            ++ite)
    {
        fprintf(stderr, "File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, ite->name);
        lua_getfield(state, -1, ite->name); /* package.loaded[pathname] */
        lua_pushcfunction(state, ite->func);
        lua_pushstring(state, ite->name);
        lua_call(state, 1, 1);
        if (!lua_toboolean(state, -2))
        {
            fprintf(stderr, "File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
            lua_setfield(state, -3, ite->name);
            lua_pop(state, 1);
        }
        else
        {
            fprintf(stderr, "File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
            lua_pop(state, 2);
        }
    }
    lua_pop(state, 2);
    fprintf(stderr, "File %s, Function %s, Line %d, gettop() = %d.\n",
            __FILE__, __FUNCTION__, __LINE__, lua_gettop(state));
}

#ifdef __cplusplus
}
#endif

/*
int main(int argc, char *argv[])
{
    lua_State * const state = luaL_newstate();
    luaL_openlibs(state);

    resx_openlibs(state);

    if (argc > 1)
    {
        luaL_loadfile(state, argv[1]);
        lua_pcall(state, 0, 0, 0);
    }

    lua_close(state);
    exit(EXIT_SUCCESS);
}
*/
