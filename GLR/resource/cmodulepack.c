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

void resx_openlibs(lua_State * const state)
{
    const luaL_Reg lualibray[] =
    {
            { .name = (const char *) NULL, .func = (lua_CFunction) NULL }
    };

    for(const luaL_Reg *ite = &lualibray[0]; ite->func != (lua_CFunction) NULL;
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
