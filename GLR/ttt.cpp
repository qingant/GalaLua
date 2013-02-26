extern "C"
{

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
int luaopen_libtest(lua_State *L);
}

#include <iostream>
#include <cstring>

class CPPClass
{
public:
    CPPClass(const char *name):_Name(name)
    {

    }

    ~CPPClass()
    {
        std::cout<<_Name<<":Over"<<std::endl;
    }

    int AddMethod(int a,int b)
    {
        return (a+b);
    }

    void ShowName(void)
    {
        std::cout <<_Name<<std::endl;
    }

private:
    std::string _Name;
};

class CPPClass4Lua
{
public:
    static int init(lua_State *L)
    {
        const char *name=luaL_checkstring(L,1);

        CPPClass **p=(CPPClass **)lua_newuserdata(L,sizeof(CPPClass *));
        *p=new CPPClass(name);
        
        luaL_getmetatable(L,"CPPClass");
        lua_setmetatable(L,-2);

        return 1;
    }

    static int finalizer(lua_State *L)
    {
        CPPClass **p=(CPPClass**)luaL_checkudata(L,1, "CPPClass");

        delete (*p);
        *p = NULL;
        return 0;
        //memset(luaL_checkudata(L,1,"CPPClass"),sizeof(void*), 0);    //将userdata设为NULL
    }

    static int AddMethod(lua_State *L)
    {
        CPPClass *p=CheckUserdata(L,1);
        int a=luaL_checknumber(L,2);
        int b=luaL_checknumber(L,3);

        int ret=p->AddMethod(a,b);
        
        lua_pushnumber(L,ret);
        return 1;
    }

    static int ShowName(lua_State *L)
    {
        CPPClass *p=CheckUserdata(L,1);

        p->ShowName();
        
        return 0;    //没有返回值
    }
private:
    static CPPClass *CheckUserdata(lua_State *L,int n)
    {
        return *(CPPClass**)luaL_checkudata(L,n,"CPPClass");
    }
};

int luaopen_libtest(lua_State *L)
{
    int ret=0;
    struct luaL_Reg f[]={
        {"__call",CPPClass4Lua::init},
        {"__gc",CPPClass4Lua::finalizer},
        {"add_method",CPPClass4Lua::AddMethod},
        {"show_name",CPPClass4Lua::ShowName},
        {NULL,NULL}
    };
    struct luaL_Reg f1[]={
        {"__call",CPPClass4Lua::init},
        {NULL,NULL}
    };
    ret=luaL_newmetatable(L,"CPPClass");
    if(ret==0)
    {
        return luaL_error(L,"newmetatable name CPPClass existed   %s   %d ",__func__,__LINE__);   
    }
    luaL_register(L,NULL,f);
    lua_pushvalue(L,-1);
    lua_setfield(L,-2,"__index");
    lua_pushvalue(L, -1);
    lua_setmetatable(L,-2);
    // lua_pushcfunction(L, CPPClass4Lua::init);
    // lua_setfield(L, -2, "__call");
    lua_setglobal(L,"CPPClass");
    //luaL_register(L,"CPPClass", f1);

    return 1;
}

int main(void)
{
    lua_State * l = luaL_newstate();
    luaL_openlibs(l);
    luaopen_libtest(l);
    int erred = luaL_dofile(l, "ttt.lua");
    if(erred)
        std::cout << "Lua error: " << luaL_checkstring(l, -1) << std::endl;
    lua_close(l);

    return 0;
}
