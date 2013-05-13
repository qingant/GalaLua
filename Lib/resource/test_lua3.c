/*
 * test_lua3.c
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

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "resource.h"

#ifndef RESXLOADER
#define RESXLOADER  "resxloader"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

static int resx_loader_lua(lua_State * const state)
{
    const char * const luasuffix = ".lua";
    const char * const name = luaL_checkstring(state, 1);
    const size_t namelength = strlen(name);
    const size_t pathlength = namelength + strlen(luasuffix) + 1;
    char * const pathname = (char *) malloc(pathlength);
    if (pathname == NULL)
    {
        return luaL_error(state, "Error: File %s, Function %s, Line %d, malloc.\n",
                __FILE__, __FUNCTION__, __LINE__);
    }
    memcpy(pathname, name, namelength);
    strlcpy(&pathname[namelength], luasuffix, pathlength - namelength);
    return 1;
}

extern int luaopen_posix(lua_State *);

static const luaL_Reg lualibray[] =
{
        { .name = "posix", .func = luaopen_posix },
        { .name = (const char *) NULL, .func = (lua_CFunction) NULL }
};

void resx_openlibs(lua_State * const state)
{
    for(const luaL_Reg *ite = &lualibray[0]; ite->func != (lua_CFunction)NULL;
            ++ite)
    {
        lua_pushcfunction(state, ite->func);
        lua_pushstring(state, ite->name);
        lua_call(state, 1, 0);
    }
}

#ifdef __cplusplus
}
#endif

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
    resx_loader_lua(state);

    lua_close(state);
    exit(EXIT_SUCCESS);
}
