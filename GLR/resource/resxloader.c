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

#include <limits.h>
#ifdef __linux__
#include <linux/limits.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX    4096
#endif

#ifndef LUA_OK
#define LUA_OK  0
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "resource.h"
#include "resxloader.h"

void resx_openlibs(lua_State * const state)
{

    fprintf(stderr, "File %s, Function %s, Line %d, gettop() = %d.\n",
            __FILE__, __FUNCTION__, __LINE__, lua_gettop(state));

    lua_getfield(state, LUA_GLOBALSINDEX, LUA_LOADLIBNAME); /* 'package' */
    lua_getfield(state, -1, "loaded"); /* package.loaded */

    const luaL_Reg *ite = NULL;
    for(ite = &lualibray[0]; ite->func != (lua_CFunction) NULL;
            ++ite)
    {
        fprintf(stderr, "File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, ite->name);

        lua_getfield(state, -1, ite->name); /* package.loaded[pathname] */
        lua_pushcfunction(state, ite->func);
        lua_pushstring(state, ite->name);
        if (lua_pcall(state, 1, 1, 0) != LUA_OK)
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
        }
        if (!lua_toboolean(state, -2))
        {
            lua_setfield(state, -3, ite->name);
            lua_pop(state, 1);
        }
        else
        {
            lua_pop(state, 2);
        }
    }
    lua_pop(state, 2);

    fprintf(stderr, "File %s, Function %s, Line %d, gettop() = %d.\n",
            __FILE__, __FUNCTION__, __LINE__, lua_gettop(state));

}

int resx_loader(lua_State * const state)
{
    fprintf(stderr, "File %s, Function %s, Line %d.\n", __FILE__, __FUNCTION__, __LINE__);
    static bool initialized = false;
    static resx_environ_t resxenv;
    if (!initialized)
    {
        resx_environ_init(&resxenv);
#if defined(linux) || defined(__linux) || defined(__linux__)
        fprintf(stdout, "File %s, Function %s, Line %d.\n", __FILE__, __FUNCTION__, __LINE__);
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
    size_t length;
    const char * const pathname = luaL_checklstring(state, 1, &length);
#ifndef SUFFIX_LEN
#define SUFFIX_LEN  8
#endif
    length += SUFFIX_LEN;
    char * const buf = (char *) malloc(length);
    if (buf == NULL)
    {
        return luaL_error(state, "Error: File %s, Function %s, Line %d, "
                "malloc failed.\n", __FILE__, __FUNCTION__, __LINE__);
    }
    strncpy(buf, pathname, length);
    fprintf(stdout, "File %s, Function %s, Line %d, buf = %s.\n",
            __FILE__, __FUNCTION__, __LINE__, &buf[0]);
    char *tmp = NULL;
    for (tmp = strchr(&buf[0], '.'); tmp != NULL; tmp = strchr(&buf[0], '.'))
    {
        *tmp = '/';
    }
    fprintf(stdout, "File %s, Function %s, Line %d, buf = %s.\n",
            __FILE__, __FUNCTION__, __LINE__, &buf[0]);
    strncat(buf, ".lua", SUFFIX_LEN);
    buf[length -1] = '\0';
    fprintf(stdout, "File %s, Function %s, Line %d, buf = %s.\n",
            __FILE__, __FUNCTION__, __LINE__, &buf[0]);
    int32_t retval = resx_environ_read(&resxenv, &buf[0]);
    free(buf);
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
        /*
        return luaL_error(state, "Error: File %s, Function %s, Line %d, "
                "No resource '%s'.\n", __FILE__, __FUNCTION__, __LINE__,
                pathname);
        */
        fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                "No resource '%s'.\n", __FILE__, __FUNCTION__, __LINE__, 
                pathname);
        lua_pushnil(state);
    }
    return 1;
}

int resx_require(const char * const pathname, lua_State * const state)
{
    const int initialdepth = lua_gettop(state);

    fprintf(stdout, "File %s, Function %s, Line %d, pathname = %s, initial depth = %d\n",
            __FILE__, __FUNCTION__, __LINE__, pathname, initialdepth);

    lua_getfield(state, LUA_GLOBALSINDEX, LUA_LOADLIBNAME); /* 'package' */
    lua_getfield(state, -1, "loaded"); /* package.loaded */
//    lua_getfield(state, LUA_REGISTRYINDEX, "_LOADED"); /* package.loaded */
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
//        lua_getfield(state, LUA_ENVIRONINDEX, "loaders"); /* package.loaders */
        if (!lua_istable(state, -1))
        {
            return luaL_error(state, "package.loaders must be a table");
        }
        const size_t siz = lua_objlen(state, -1) + 1;
        lua_pushstring(state, "");  /* error message accumulator */
        size_t idx;
        for (idx = 1; idx < siz; ++idx)
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
                if (lua_pcall(state, 1, 1, 0) != LUA_OK)
                {
                    return luaL_error(state, "Error: File %s, Function %s, Line %d.\n",
                            __FILE__, __FUNCTION__, __LINE__);
                }
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
    fprintf(stdout, "File %s, Function %s, Line %d, current depth = %d\n", 
            __FILE__, __FUNCTION__, __LINE__, currentdepth);
    lua_pop(state, currentdepth - initialdepth);
    return 0;
}

void resx_setloader(lua_State * const state, int (*func)(lua_State *))
{
    fprintf(stderr, "File %s, Function %s, Line %d, gettop() = %d.\n", __FILE__, __FUNCTION__, __LINE__, lua_gettop(state));
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
    fprintf(stderr, "File %s, Function %s, Line %d, gettop() = %d.\n", __FILE__, __FUNCTION__, __LINE__, lua_gettop(state));
}

int resx_main_execute(lua_State * const state, const char *name)
{
    fprintf(stderr, "File %s, Function %s, Line %d.\n", __FILE__, __FUNCTION__, __LINE__);
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
    fprintf(stderr, "File %s, Function %s, Line %d.\n", __FILE__, __FUNCTION__, __LINE__);
    return lua_gettop(state) - initialdepth;
}

void GLR_initializer(lua_State * state)
{
    fprintf(stderr, "File %s, Function %s, Line %d.\n", __FILE__, __FUNCTION__, __LINE__);
    resx_setloader(state, resx_loader);
    resx_openlibs(state);
    fprintf(stderr, "File %s, Function %s, Line %d.\n", __FILE__, __FUNCTION__, __LINE__);
}

#ifdef __cplusplus
}
#endif

