#include<iostream>
#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<lua.hpp>

extern "C"
int luaopen_c_flag(lua_State *l)
{
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

    lua_pushinteger(l,S_IREAD);
    lua_setfield(l,-2,"S_IREAD");

    lua_pushinteger(l,S_IWRITE);
    lua_setfield(l,-2,"S_IWRITE");

    lua_pushinteger(l,S_IEXEC);
    lua_setfield(l,-2,"S_IEXEC");

    lua_pushinteger(l,SEEK_SET);
    lua_setfield(l,-2,"SEEK_SET");

    lua_pushinteger(l,SEEK_CUR);
    lua_setfield(l,-2,"SEEK_CUR");

    lua_pushinteger(l,SEEK_END);
    lua_setfield(l,-2,"SEEK_END");

    return 1;
}
