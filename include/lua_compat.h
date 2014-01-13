#ifndef __LUA_COMPAT_H
#define __LUA_COMPAT_H

#if LUA_VERSION_NUM==502

    #define lua_setfenv(L,i)   lua_setuservalue(L,i)
    #define lua_getfenv(L,i)   lua_getuservalue(L,i)

#elif LUA_VERSION_NUM==501

    #define lua_getuservalue(L,i)   lua_getfenv(L,i)
    #define lua_setuservalue(L,i)   lua_setfenv(L, i)

static void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
    luaL_checkstack(L, nup, "too many upvalues");
    for (; l->name != NULL; l++) {  /* fill the table with given functions */
        int i;
        for (i = 0; i < nup; i++)  /* copy upvalues to the top */
            lua_pushvalue(L, -nup);
        lua_pushstring(L, l->name);
        lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
        lua_settable(L, -(nup + 3));
    }
    lua_pop(L, nup);  /* remove upvalues */
}

#else

    #error "noly support lua 5.1 or 5.2"

#endif

#endif  /*__LUA_COMPAT_H*/
