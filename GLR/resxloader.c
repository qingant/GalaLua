/*
 * resxloader.c
 *
 *  Created on: May 20, 2013
 *      Author: esuomyekcim
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "resource.h"

#ifndef LUA_OK
#define LUA_OK  0
#endif

#ifdef __cplusplus
extern "C"
{
#endif

int resx_loader(lua_State * const state)
{
    static bool initialized = false;
    static resx_environ_t resxenv = { .stream = NULL, .offset = RESOURCE_CHAOS,
            .length = 0, .nodesize = sizeof(resx_node_t)
    };
    if (!initialized)
    {
        resx_environ_init(&resxenv);
#if defined(linux) || defined(__linux) || defined(__linux__)
        if (resx_environ_open(&resxenv) != 0)
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
        }
#else
        const char * const resxname = luaL_checkstring(state, 2);
        fprintf(stdout, "File %s, Function %s, Line %d, resxname = %s.\n",
                __FILE__, __FUNCTION__, __LINE__, resxname);
        if (resx_environ_open(&resxenv, resxname) != 0)
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
        }
#endif
        initialized = true;
    }
    const char * const pathname = luaL_checkstring(state, 1);
    int32_t retval = resx_environ_read(&resxenv, pathname);
    if (retval > 0)
    {
        char * const buff = (char *) malloc(retval + 1);
        if (buff == NULL)
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d, malloc "
                    "failed.\n", __FILE__, __FUNCTION__, __LINE__);
        }
        char *cursor = buff;
        if (resx_fstream_read(cursor, 1U, retval, resxenv.stream) < retval)
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
        }
        cursor[retval] = '\0';
        retval = luaL_loadbuffer(state, cursor, retval, pathname);
        free(buff);
        cursor = NULL;
        switch(retval)
        {
            case LUA_ERRSYNTAX:
            {
                return luaL_error(state, "Error: File %s, Function %s, Line %d, "
                        "syntax error during precompilation.\n", __FILE__,
                        __FUNCTION__, __LINE__);
            }
            case LUA_ERRMEM:
            {
                return luaL_error(state, "Error: File %s, Function %s, Line %d, "
                        "memory allocation error.\n", __FILE__,
                        __FUNCTION__, __LINE__);
            }
#if LUA_VERSION_NUM == 502
            case LUA_ERRGCMM:
            {
                return luaL_error(state, "Error: File %s, Function %s, Line %d, "
                        "error while running a __gc metamethod.\n", __FILE__,
                        __FUNCTION__, __LINE__);
            }
#endif
        }
    }
    else if (retval == -1)
    {
        return luaL_error(state, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
    }
    else if (retval == -2)
    {
        return luaL_error(state, "Error: File %s, Function %s, Line %d, "
                "No resource '%s'.\n", __FILE__, __FUNCTION__, __LINE__,
                pathname);
    }
    return 1;
}

int resx_require(const char * const pathname, lua_State * const state)
{
    const int initialdepth = lua_gettop(state);

    lua_getfield(state, LUA_GLOBALSINDEX, LUA_LOADLIBNAME); /* 'package' */
    lua_getfield(state, -1, "loaded"); /* package.loaded */
    lua_getfield(state, -1, pathname); /* package.loaded[pathname] */
    if (lua_toboolean(state, -1))
    { /* is it there ? */
        if (lua_touserdata(state, -1) == NULL)
        { /* not found? */
            return luaL_error(state, "name conflict for module '%s'", pathname);
        }
        lua_pop(state, 3);
        return 0;
    }
    else
    {
        lua_pop(state, 1); /* pop package.loaded[pathname] */
        lua_getfield(state, -2, "loaders"); /* package.loaders */
        if (!lua_istable(state, -1))
        {
            return luaL_error(state, "package.loaders must be a table");
        }
        const size_t siz = lua_objlen(state, -1) + 1;
        lua_pushstring(state, "");  /* error message accumulator */
        for (size_t idx = 1; idx < siz; ++idx)
        {
            lua_rawgeti(state, -2, idx); /* package.loaders[idx] */
            if (lua_isnil(state, -1))
            {
                return luaL_error(state, "module '%s' not found: %s",
                        pathname, lua_tostring(state, -2));
            }
            lua_pushstring(state, pathname);
            if (lua_pcall(state, 1, 1, 0) != LUA_OK)
            {
                return lua_error(state);
            }
            if (lua_isfunction(state, -1))
            {
                lua_pushlightuserdata(state, NULL);
                lua_setfield(state, -4, pathname); /* package.loaded[pathname] = nil */
                lua_pushstring(state, pathname);
                lua_call(state, 1, 1);
                if (!lua_isnil(state, -1))
                { /* package.loaded[pathname] = returned value */
                    lua_setfield(state, -3, pathname);
                }
                lua_getfield(state, -3, pathname);
                if (lua_touserdata(state, -1) == NULL)
                {
                    lua_pushboolean(state, true); /* use true as result */
                    lua_pushvalue(state, -1); /* to be returned */
                    /* package.loaded[pathname] = true */
                    lua_setfield(state, -6, pathname);
                }
                break;
            }
            else if (lua_isstring(state, -1))
            {
                lua_concat(state, 2);
            }
            else
            {
                lua_pop(state, 1);
            }
        }
    }
    const int currentdepth = lua_gettop(state);
    lua_pop(state, currentdepth - initialdepth);
    return 0;
}

void resx_setloader(lua_State * const state, int (*func)(lua_State *))
{
    lua_getfield(state, LUA_GLOBALSINDEX, LUA_LOADLIBNAME);
    lua_getfield(state, -1, "loaders");
    if (!lua_istable(state, -1))
    {
        luaL_error(state, "package.loaders must be a table");
    }
    lua_pushinteger(state, lua_objlen(state, -1) + 1);
    lua_pushcfunction(state, func);
    lua_settable(state, -3);
    lua_pop(state, 2);
}

int resx_main_execute(lua_State * const state, const char *name)
{
    resx_require(name, state);
    const int initialdepth = lua_gettop(state);
    lua_getfield(state, LUA_GLOBALSINDEX, LUA_LOADLIBNAME);
    lua_getfield(state, -1, "loaded");
    lua_remove(state, -2);
    lua_getfield(state, -1, name);
    lua_remove(state, -2);
    lua_getfield(state, -1, "main");
    lua_remove(state, -2);
    if (lua_pcall(state, 0, LUA_MULTRET, 0) != 0)
    {
        return luaL_error(state, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
    }
    return lua_gettop(state) - initialdepth;
}

void GLR_initializer(lua_State * state)
{
    resx_setloader(state, resx_loader);
    resx_openlibs(state);
}

#ifdef __cplusplus
}
#endif