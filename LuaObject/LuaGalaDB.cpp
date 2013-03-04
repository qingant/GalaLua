
#include "GalaDB.hpp"
#include <string>
#include <iostream>
#include "LuaCPP.hpp"

using namespace Galaxy::DB;

class GalaDB4Lua
{
public:
    static int init(lua_State *L);
    static int Put(lua_State *L);
    static int Get(lua_State *L);
    static int Keys(lua_State *L);
    static int Shrink(lua_State *L);
    static int Delete(lua_State *L);
    static int finalizer(lua_State *L);
private:
    static GalaDB *CheckGalaDB(lua_State *L,int n);
    static const char *_tname;
};

const char *GalaDB4Lua::_tname="GalaDB";

int GalaDB4Lua::Put(lua_State *L)
{
    GalaDB *p=CheckGalaDB(L,1);
    const char *k=luaL_checkstring(L,2);
    const char *v=luaL_checkstring(L,3);

    if (k==NULL || v==NULL)
    {
        return luaL_error(L,"Error:string expected!  %s  %d",__func__,__LINE__);
    }
    std::string key(k);
    std::string value(v);

    std::cout <<"args:"<<key<<std::endl;
    std::cout <<"args:"<<value<<std::endl;

    CALL_CPP_FUNCTION(L,p->Put(key,value));
    /*
    try
    {
        p->Put(key,value);
    }
    catch(std::exception &e)
    {
        return luaL_error(L,"%s",e.what());
    }
    catch (...)
    {
        return luaL_error(L,"Unknown exception %s   %d",__func__,__LINE__);
    }
    */

    return 0;
}
int GalaDB4Lua::Get(lua_State *L)
{
    GalaDB *p=CheckGalaDB(L,1);
    const char *k=luaL_checkstring(L,2);
    if (k==NULL)
    {
        return luaL_error(L,"Error:string expected!  %s  %d",__func__,__LINE__);
    }
    std::string key(k);

    std::cout <<"args:"<<key<<std::endl;

    std::string value;
    CALL_CPP_FUNCTION(L,value=p->Get(key));
/*
    try
    {
        value=p->Get(key);
    }
    catch(std::exception &e)
    {
        return luaL_error(L,"%s",e.what());
    }
    catch (...)
    {
        return luaL_error(L,"Unknown exception %s   %d",__func__,__LINE__);
    }
*/
    lua_pushstring(L,value.c_str());
    return 1;
}
int GalaDB4Lua::Keys(lua_State *L)
{
    GalaDB *p=CheckGalaDB(L,1);
    std::vector<std::string> key_vec;

    CALL_CPP_FUNCTION(L,key_vec=p->Keys());
    /*
    try
    {
        key_vec=p->Keys();
    }
    catch(std::exception &e)
    {
        return luaL_error(L,"%s",e.what());
    }
    catch (...)
    {
        return luaL_error(L,"Unknown exception %s   %d",__func__,__LINE__);
    }
    */
    lua_newtable(L);
    for (unsigned int i=0;i<key_vec.size();++i)
    {
        lua_pushstring(L,key_vec[i].c_str());
        lua_rawseti(L,-2,i+1);
    }

    return 1;    //return array
}

int GalaDB4Lua::Delete(lua_State *L)
{
    GalaDB *p=CheckGalaDB(L,1);
    const char *k=luaL_checkstring(L,2);
    if(k==NULL)
    {
        return luaL_error(L,"string expected!  %s  %d",__func__,__LINE__);
    }

    std::string key(k);
    std::cout <<"args:"<<key<<std::endl;

    CALL_CPP_FUNCTION(L,p->Delete(key));

    return 0;
}
int GalaDB4Lua::Shrink(lua_State *L)
{
    GalaDB *p=CheckGalaDB(L,1);
    
    CALL_CPP_FUNCTION(L,p->Shrink());
    /*
    try
    {
        p->Shrink();
    }
    catch (std::exception &e)
    {
        return luaL_error(L,"%s",e.what());
    }
    catch(...)
    {
        return luaL_error(L,"Unknown exception  %s   %d",__func__,__LINE__);
    }
    */

    return 0;
}

GalaDB *GalaDB4Lua::CheckGalaDB(lua_State *L,int n)
{
    return *(GalaDB **)luaL_checkudata(L,n,_tname);    
}

int GalaDB4Lua::finalizer(lua_State *L)
{
    GalaDB **pp=(GalaDB **)luaL_checkudata(L,1,_tname);

    delete *pp;

    *pp=NULL;

    return 0;
}

int GalaDB4Lua::init(lua_State *L)
{
    const char *token=luaL_checkstring(L,1);
    if (token==NULL)     
    {
        return luaL_error(L,"Error:string expected! %s  %d",__func__,__LINE__);
    }

    GalaDB **p=(GalaDB **)lua_newuserdata(L,sizeof(GalaDB *));

    CALL_CPP_FUNCTION(L,*p=new GalaDB(token));

    luaL_getmetatable(L,"GalaDB");
    lua_setmetatable(L,-2);

    return 1;
}

extern "C" int luaopen_galadb(lua_State *L)
{
    int ret=0;
    struct luaL_Reg f[]={
        {"new",GalaDB4Lua::init},
        {NULL,NULL}
    };
    struct luaL_Reg m[]={
        {"__gc",GalaDB4Lua::finalizer},
        {"finalizer",GalaDB4Lua::finalizer},
        {"put",GalaDB4Lua::Put},
        {"get",GalaDB4Lua::Get},
        {"keys",GalaDB4Lua::Keys},
        {"shrink",GalaDB4Lua::Shrink},
        {"delete",GalaDB4Lua::Delete},
        {NULL,NULL}
    };

    ret=luaL_newmetatable(L,"GalaDB");
    if (ret==0)
    {
        return luaL_error(L,"metatable \"GalaDB\" existed %s  %d",__func__,__LINE__);
    }
    luaL_register(L,NULL,m);     //register into metatable
    
    lua_pushvalue(L,-1);
    lua_setfield(L,-2,"__index");
    setfield_string(L,"GalaDB","__my_name");

    luaL_register(L,"Galadb",f); 

    return 1;
}
