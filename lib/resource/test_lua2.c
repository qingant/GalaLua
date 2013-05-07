/*
 * test_lua2.c
 *
 *  Created on: May 6, 2013
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

static char buff[BUFSIZ + 1];

static int resxloader(lua_State * const L)
{
    static bool initialized = false;
    static resx_environ_t resxenv =
    {
            .stream = NULL, .offset = RESOURCE_CHAOS,
            .length = 0, .nodesize = sizeof(resx_node_t)
    };

    size_t length;
    const char * const name = luaL_checklstring(L, 1, &length);
    ++length; // 将字符串结尾的'\0'也计算在内
    length = ((length < sizeof(buff) - 1) ? length : (sizeof(buff) - 1));
    buff[0] = '/';
    memcpy(&buff[1], name, length);
    buff[sizeof(buff) - 1] = '\0';
    if (name != NULL)
    {
        fprintf(stdout, "File %s, Function %s, Line %d, name = '%s', "
                "buff = '%s'.\n", __FILE__, __FUNCTION__, __LINE__, name,
                &buff[0]);
    }

    if (!initialized)
    {
        resx_environ_init(&resxenv);
#if defined(linux) || defined(__linux) || defined(__linux__)
        if (resx_environ_open(&resxenv) != 0)
        {
            return luaL_error(L, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
        }
#else
        const char * const resxname = luaL_checkstring(L, 2);
        fprintf(stdout, "File %s, Function %s, Line %d, resxname = %s.\n",
                __FILE__, __FUNCTION__, __LINE__, resxname);
        if (resx_environ_open(&resxenv,resxname) != 0)
        {
            return luaL_error(L, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
        }
#endif
        initialized = true;
    }
    int32_t retval = resx_environ_read(&resxenv, &buff[0]);
    if ((size_t) retval > sizeof(buff))
    {
        return luaL_error(L, "Error: File %s, Function %s, Line %d, "
                "Resource too long, '%s'.\n", __FILE__, __FUNCTION__, __LINE__,
                &buff[0]);
    }
    else if (retval > 0)
    {
        if (resx_read(&buff[0], retval, &resxenv) < retval)
        {
            return luaL_error(L, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
        }
//        fprintf(stdout, "File %s, Function %s, Line %d, buff = {%s}.\n",
//                __FILE__, __FUNCTION__, __LINE__, &buff[0]);
        buff[retval] = '\0';
        if ((retval = luaL_loadbuffer(L, &buff[0], retval, name)) == LUA_ERRSYNTAX)
        {
            return luaL_error(L, "Error: File %s, Function %s, Line %d, "
                    "syntax error during precompilation.\n", __FILE__,
                    __FUNCTION__, __LINE__);
        }
        else if (retval == LUA_ERRMEM)
        {
            return luaL_error(L, "Error: File %s, Function %s, Line %d, "
                    "memory allocation error.\n", __FILE__,
                    __FUNCTION__, __LINE__);
        }
#if LUA_VERSION_NUM == 502
        else if (retval == LUA_ERRGCMM)
        {
            return luaL_error(L, "Error: File %s, Function %s, Line %d, "
                    "error while running a __gc metamethod.\n", __FILE__,
                    __FUNCTION__, __LINE__);
        }
#endif
    }
    else if (retval == -1)
    {
        return luaL_error(L, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
    }
    else if (retval == -2)
    {
        return luaL_error(L, "Error: File %s, Function %s, Line %d, "
                "No resource '%s'.\n", __FILE__, __FUNCTION__, __LINE__,
                &buff[0]);
    }

    return 1;
}

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

    lua_State * const state = luaL_newstate();
    luaL_openlibs(state);

    lua_getglobal(state, LUA_LOADLIBNAME);

    lua_createtable(state, 1, 0);
    lua_pushcfunction(state, resxloader);
    lua_rawseti(state, -2, 1);
    lua_setfield(state, -2, "loaders");

    lua_getfield (state, -1, "loaded");
    lua_pushboolean(state, true);
    lua_setfield(state, -2, RESXLOADER);

    if (argc > 1)
    {
        fprintf(stdout, "File %s, Function %s, Line %d.\n", __FILE__,
                __FUNCTION__, __LINE__);
        FILE * const fp = fopen(argv[1], "rb");
        if (fp == NULL)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d.\n", __FILE__,
                    __FUNCTION__, __LINE__);
            exit(EXIT_FAILURE);
        }
        size_t length = resx_fstream_read(&buff[0], 1U, sizeof(buff) - 1, fp);
//        buff[length] = '\0';
//        fprintf(stdout, "File %s, Function %s, Line %d, %s.\n", __FILE__,
//                    __FUNCTION__, __LINE__, &buff[0]);

        int errcode = luaL_loadbuffer(state, buff, length, argv[1]) || lua_pcall(state, 0, 0, 0);
        if (errcode)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__,
                    lua_tostring(state, -1));
            lua_pop(state, 1);
        }
    }

    lua_close(state);
    exit(EXIT_SUCCESS);
}
