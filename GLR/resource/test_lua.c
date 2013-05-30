/*
 * test_lua.c
 *
 *  Created on: Apr 25, 2013
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

#ifndef LUA_SCRIPT
#define LUA_SCRIPT "test.lua"
#endif

#ifdef __cplusplus
extern "C"
{
#endif



#ifdef __cplusplus
}
#endif

int main(int argc, char *argv[])
{
    resx_environ_t resxenv;
    if (resx_environ_open(&resxenv) != 0)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n", __FILE__,
                __FUNCTION__, __LINE__);
        exit(EXIT_FAILURE);
    }

    const int32_t length = resx_environ_read(&resxenv, "/" LUA_SCRIPT);
    char * const buff = (char *)malloc(length + 1);
    if (buff == NULL)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, malloc.\n",
                __FILE__, __FUNCTION__, __LINE__);
        exit(EXIT_FAILURE);
    }
    buff[length] = '\0';

    size_t retval = resx_read(buff, length, &resxenv);
    buff[retval] = '\0';
    fprintf(stdout, "File %s, Function %s, Line %d, length = %" PRId32 ", "
            "retval = %zu, buf = {%s}\n", __FILE__, __FUNCTION__, __LINE__,
            length, retval, buff);

    lua_State * const state = luaL_newstate();
    luaL_openlibs(state);

    switch(luaL_loadbuffer(state, buff, retval, LUA_SCRIPT))
    {
        case LUA_ERRSYNTAX:
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d, "
                    "syntax error during pre-compilation.\n", __FILE__,
                    __FUNCTION__, __LINE__);
        }
        case LUA_ERRMEM:
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d, "
                    "memory allocation error.\n", __FILE__, __FUNCTION__,
                    __LINE__);
        }
    }

    switch(lua_pcall(state, 0, LUA_MULTRET, 0))
    {
        case LUA_ERRRUN:
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d, "
                    "a runtime error.\n", __FILE__, __FUNCTION__,
                    __LINE__);
        }
        case LUA_ERRMEM:
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d, "
                    "memory allocation error.\n", __FILE__, __FUNCTION__,
                    __LINE__);
        }
        case LUA_ERRERR:
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d, "
                    " error while running the error handler function.\n",
                    __FILE__, __FUNCTION__, __LINE__);
        }
    }
    fprintf(stdout, "\n");

    lua_close(state);
    exit(EXIT_SUCCESS);
}
