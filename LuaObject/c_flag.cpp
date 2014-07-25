#include<iostream>
#include<fcntl.h>
#include<lua.hpp>
#include<string.h>

extern "C"
int luaopen_c_flag(lua_State *l)
{
    /*
	const struct luaL_Reg _obj[] = {
            {"O_RDONLY",O_RDONLY},
            {"O_WRONLY", O_WRONLY},
            {"O_RDWR", O_RDWR},
            {"O_CREAT", O_CREAT},
            {"O_APPEND", O_APPEND},
            {"O_TRUNC", O_TRUNC},
            {"O_EXCL", O_EXCL},
            {"O_TEXT", O_TEXT},
            {"O_BINARY", O_BINARY},
            {"S_IFMT", S_IFMT},
            {"S_IFDIR", S_IFDIR},
            {"S_IFIFO", S_IFIFO},
            {"S_IFCHR", S_IFCHR},
            {"S_IFBLK", S_IFBLK},
            {"S_IFREG", S_IFREG},
            {"S_IREAD", S_IREAD},
            {"S_IWRITE", S_IWRITE},
            {"S_IEXEC", S_IEXEC},
            {NULL, NULL}
    };

    luaL_newmetatable(l, Foo_Wrapper::type_id);

    luaL_register(l, NULL, _obj);
    return 1;
    */
    lua_newtable(l);

    lua_pushinteger(l,O_RDONLY);
    lua_setfield(l,-2,"O_RDONLY");

    lua_pushinteger(l,O_WRONLY);
    lua_setfield(l,-2,"O_WRONLY");

    lua_pushinteger(l,O_RDWR);
    lua_setfield(l,-2,"O_RDWR");

    lua_pushinteger(l,O_CREAT);
    lua_setfield(l,-2,"O_CREAT");

    lua_pushinteger(l,O_APPEND);
    lua_setfield(l,-2,"O_APPEND");

    lua_pushinteger(l,O_TRUNC);
    lua_setfield(l,-2,"O_TRUNC");

    lua_pushinteger(l,O_EXCL);
    lua_setfield(l,-2,"O_EXCL");
    /*
    lua_pushinteger(l,O_TEXT);
    lua_setfield(l,-2,"O_TEXT");

    lua_pushinteger(l,O_BINARY);
    lua_setfield(l,-2,"O_BINARY");
	*/
    lua_pushinteger(l,S_IFMT);
    lua_setfield(l,-2,"S_IFMT");

    lua_pushinteger(l,S_IFDIR);
    lua_setfield(l,-2,"S_IFDIR");

    lua_pushinteger(l,S_IFIFO);
    lua_setfield(l,-2,"S_IFIFO");

    lua_pushinteger(l,S_IFCHR);
    lua_setfield(l,-2,"S_IFCHR");

    lua_pushinteger(l,S_IFBLK);
    lua_setfield(l,-2,"S_IFBLK");

    lua_pushinteger(l,S_IFREG);
    lua_setfield(l,-2,"S_IFREG");
    /*
    lua_pushinteger(l,S_IREAD);
    lua_setfield(l,-2,"S_IREAD");

    lua_pushinteger(l,S_IWRITE);
    lua_setfield(l,-2,"S_IWRITE");

    lua_pushinteger(l,S_IEXEC);
    lua_setfield(l,-2,"S_IEXEC");
	*/
    return 1;
}
