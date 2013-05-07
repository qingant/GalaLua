/*
 * resxloader.c
 *
 *  Created on: Apr 26, 2013
 *      Author: esuomyekcim
 */

#include <stdbool.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "resource.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RESXLOADER_NAME
#define RESXLOADER_NAME     "resxloader"
#endif

static bool initialized = false;

static char buff[BUFSIZ + 1];

static resx_environ_t resxenv =
{
        .stream = NULL, .offset = RESOURCE_CHAOS,
        .length = 0, .nodesize = sizeof(resx_node_t)
};

static int resxloader(lua_State * const L)
{
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
        fprintf(stdout, "File %s, Function %s, Line %d, buff = {%s}.\n",
                __FILE__, __FUNCTION__, __LINE__, &buff[0]);
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

static const struct luaL_Reg library[] =
{
        { .name = RESXLOADER_NAME, .func = resxloader },
        { .name = (const char *) NULL, .func = (lua_CFunction) NULL }
};

int luaopen_resxloader(lua_State * const L)
{
#if LUA_VERSION_NUM == 501
    luaL_register(L, RESXLOADER_NAME, library);
#elif LUA_VERSION_NUM == 502
    luaL_newlib(L, &library[0]);
#endif
    return 1;
}

#ifdef __cplusplus
}
#endif
