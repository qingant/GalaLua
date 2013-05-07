/*
 * test_lua.c
 *
 *  Created on: Apr 25, 2013
 *      Author: esuomyekcim
 */

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sysexits.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "resource.h"

#ifndef LUA_SCRIPT
#define LUA_SCRIPT "hello.lua"
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

    int errcode = luaL_loadbuffer(state, buff, retval, LUA_SCRIPT) || lua_pcall(state, 0, 0, 0);
    if (errcode)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__,
                lua_tostring(state, -1));
        lua_pop(state, 1);
    }

    lua_close(state);
    exit(EXIT_SUCCESS);
}
