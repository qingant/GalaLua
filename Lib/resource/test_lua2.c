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

static int resx_loader(lua_State * const state)
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
    //    fprintf(stdout, "File %s, Function %s, Line %d, pathname = {%s}.\n",
    //            __FILE__, __FUNCTION__, __LINE__, pathname);
    int32_t retval = resx_environ_read(&resxenv, pathname);
    if (retval > 0)
    {
        char * const buff = (char *) malloc(retval + 1);
        if (buff == NULL)
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d, malloc "
                    "failed.\n", __FILE__, __FUNCTION__, __LINE__);
        }
        if (resx_fstream_read(buff, 1U, retval, resxenv.stream) < retval)
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
        }
        buff[retval] = '\0';
        const char *pkgname = strrchr(pathname, '/');
        if (pkgname != NULL)
        {
            ++pkgname;
        }
        else
        {
            pkgname = pathname;
        }
        fprintf(stdout, "File %s, Function %s, Line %d, pathname = {%s}, pkgname = {%s}, "
                "buff = {%s}.\n", __FILE__, __FUNCTION__, __LINE__, pathname, pkgname,
                buff);
        retval = luaL_loadbuffer(state, buff, retval, pkgname);
        free(buff);
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

//static void table_pairs(lua_State * const state)
//{
//    const size_t length = lua_objlen(state, -1) + 1;
//    fprintf(stdout, "table begin: length = %zu\n", length - 1);
//    for (size_t idx = 1; idx < length; ++idx)
//    {
//        lua_rawgeti(state, 1, idx);
//        fprintf(stdout, "idx = %d, value = %s\n", idx, lua_tostring(state, -1));
//        lua_pop(state, 1);
//    }
//    fprintf(stdout, "ipairs\n");
//    /* table is in the stack at index 't' */
//    lua_pushnil(state);  /* first key */
//    while (lua_next(state, -2) != 0) {
//      /* uses 'key' (at index -2) and 'value' (at index -1) */
//      printf("%s - %s",
//              lua_typename(state, lua_type(state, -2)),
//             lua_typename(state, lua_type(state, -1)));
//      if (lua_type(state, -2) == LUA_TSTRING)
//      {
//          printf(", name = %s, length = %zu\n", lua_tostring(state, -2), lua_objlen(state, -2));
//      }
//      else
//      {
//          printf("\n");
//      }
//      /* removes 'value'; keeps 'key' for next iteration */
//      lua_pop(state, 1);
//    }
////    lua_pop(state, 1);
//    fprintf(stdout, "table end\n");
//}

static int resx_require(const char * const pathname, lua_State * const state)
{
    lua_getfield(state, LUA_GLOBALSINDEX, LUA_LOADLIBNAME); /* 'package' */
    lua_getfield(state, -1, "loaded"); /* package.loaded */
    lua_getfield(state, -1, pathname); /* package.loaded[pathname] */
    if (lua_toboolean(state, -1))
    { /* is it there ? */
        if (lua_touserdata(state, -1) == NULL)
        { /* not found? */
            return luaL_error(state, "name conflict for module '%s'", pathname);
        }
        return 1;
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
            lua_rawgeti(state, -2, idx);
            if (lua_isnil(state, -1))
            {
                return luaL_error(state, "module '%s' not found: %s",
                        pathname, lua_tostring(state, -2));
            }
            lua_pushstring(state, pathname);
            lua_call(state, 1, 1);
            if (lua_isfunction(state, -1))
            {
                lua_pushlightuserdata(state, NULL);
                lua_setfield(state, -4, pathname); /* package.loaded[pathname] = nil */
                lua_pushstring(state, pathname);
                lua_call(state, 1, 1);
                if (!lua_isnil(state, -1))
                {
                    lua_setfield(state, -3, pathname);
                }
                lua_getfield(state, -3, pathname);
                if (lua_touserdata(state, -1) == NULL)
                {
                    lua_pushboolean(state, true); /* use true as result */
                    lua_pushvalue(state, -1); /* to be returned */
                    /* package.loaded[name] = true */
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
    return 1;
}


#ifdef __cplusplus
}
#endif

int main(int argc, char *argv[])
{
    lua_State * const state = luaL_newstate();
    luaL_openlibs(state);

    lua_getfield(state, LUA_GLOBALSINDEX, LUA_LOADLIBNAME);
    lua_getfield(state, -1, "loaders");
    lua_remove(state, -2);
    /**
     * @todo 此处需要注意判断loaders是否为nil
     */
    if (!lua_istable(state, -1))
    {
        return luaL_error(state, "package.loaders must be a table");
    }
    lua_pushinteger(state, lua_objlen(state, -1) + 1);
    lua_pushcfunction(state, resx_loader);
    lua_settable(state, -3);

    if (argc == 2)
    {
        switch(luaL_loadfile(state, argv[1]))
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
            case LUA_ERRFILE:
            {
                return luaL_error(state, "Error: File %s, Function %s, Line %d, "
                        "LUA_ERRFILE.\n", __FILE__, __FUNCTION__, __LINE__);
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
    }
    else if (argc == 3)
    {
        const char * const pkgname = "hello_lua";
        fprintf(stdout, "File %s, Function %s, Line %d.\n", __FILE__,
                __FUNCTION__, __LINE__);
        resx_require(pkgname, state);
        lua_getfield(state, LUA_GLOBALSINDEX, LUA_LOADLIBNAME);
        lua_getfield(state, -1, "loaded");
        lua_getfield(state, -1, pkgname);
        lua_getfield(state, -1, "main");
        if (lua_pcall(state, 0, LUA_MULTRET, 0) != 0)
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
        }
        lua_getfield(state, -1, "helloworld");
        if (lua_pcall(state, 0, LUA_MULTRET, 0) != 0)
        {
            return luaL_error(state, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
        }
    }

    lua_close(state);
    exit(EXIT_SUCCESS);
}
