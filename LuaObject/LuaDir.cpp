#include "LuaCPP.hpp"
#include "Runtime/Logger.hpp"


static int LUA_dir(lua_State *l)
{
    const char *dir_path = luaL_checkstring(l, 1);
    DIR *dp =  opendir(dir_path);
    if (dp != NULL)
    {
        lua_newtable(l);
        char entry[sizeof(dirent) + 256] = {};
        dirent *entp = (dirent*)entry;
        dirent *result = NULL;
        int i = 0;
        while(readdir_r(dp, entp, &result) == 0)
        {

            if(result == NULL)
            {
                break;
            }
            ++i;
            lua_pushstring(l, entp->d_name);
            lua_rawseti(l, -2, i);
        }
        closedir(dp);
        return 1;
        
    }
    return luaL_error(l, "error");
}



/*static int lstat(luaState *l)
{
    return 0;
}*/

static int LUA_dir_type(lua_State *l)
{
    const char *path = luaL_checkstring(l, 1);
    struct stat buf = {};
    if (lstat(path, &buf) == 0)
    {
        if (S_ISDIR(buf.st_mode))
        {
            lua_pushstring(l, "dir");
        }
        else if (S_ISREG(buf.st_mode))
        {
            lua_pushstring(l, "reg");
        }
        else
        {
            lua_pushnil(l);
        }
        return 1;

    }
    return luaL_error(l, "error");
}

extern "C" int luaopen_dir(lua_State *L)
{

    luaL_Reg f[]={
        {"dir",LUA_dir},
        {"dir_type",LUA_dir_type},
        {NULL,NULL}
    };
    luaL_register(L,NULL,f);
    return 0;
}
