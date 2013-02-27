#include "LuaCPP.hpp"
#include "Configure.hpp"

class ConfigurePlt4Lua
{
public:
    static int init(lua_State *L)
    {
        const char *path=luaL_checkstring(L,1);
        const char *mode=luaL_optstring(L,2,"r");
        int prot=PROT_READ;


        if(path==NULL)
        {
            return luaL_error(L,"string expected!  %s  %d",__func__,__LINE__);
        }

        if (strcmp(mode,"w")==0)
        {
            prot|=PROT_WRITE;
        }

        std::string path_str(path);

        ConfigurePlt **p=(ConfigurePlt **)lua_newuserdata(L,sizeof(ConfigurePlt *));

        CALL_CPP_FUNCTION(L,*p=new ConfigurePlt(path_str,prot));
        
        luaL_getmetatable(L,_tname);
        lua_setmetatable(L,-2);

        return 1;
    }

    static int finalizer(lua_State *L)
    {
        ConfigurePlt **pp=(ConfigurePlt **)luaL_checkudata(L,1,_tname);

        delete *pp;

        *pp=NULL;

        return 0;
    }
    static int Put(lua_State *L)
    {
        ConfigurePlt *p=CheckConfigurePlt(L,1);    

        const char *path=luaL_checkstring(L,2);
        const char *value=luaL_checkstring(L,3);

        if (path==NULL ||value==NULL)
        {
            return luaL_error(L,"string expected! %s  %d",__func__,__LINE__);
        }

        std::string path_str(path);
        //std::string value_str(value);

        CALL_CPP_FUNCTION(L,p->Put(path_str,const_cast<char *>(value)));
        
        return 0;
    } 

    static int Get(lua_State *L)
    {
        ConfigurePlt *p=CheckConfigurePlt(L,1);    

        const char *path=luaL_checkstring(L,2);

        std::string path_str(path);
        std::string value;

        CALL_CPP_FUNCTION(L,value=p->Get(path_str));
        
        lua_pushstring(L,value.c_str());

        return 1;
    }

    static int Get_ex(lua_State *L)
    {
        ConfigurePlt *p=CheckConfigurePlt(L,1);    

        const char *path=luaL_checkstring(L,2);
        if(path==NULL)
        {
            return luaL_error(L,"string expected! %s  %d",__func__,__LINE__);
        }
        std::string path_str(path);
        std::pair<std::string,std::string>value;

        CALL_CPP_FUNCTION(L,value=p->Get_ex(path_str));
        
        lua_pushstring(L,value.first.c_str());
        lua_pushstring(L,value.second.c_str());
         
        return 2;      //return a pair
    }

    static int Load(lua_State *L)
    {
        ConfigurePlt *p=CheckConfigurePlt(L,1);

        const char *path=luaL_checkstring(L,2);
        if(path==NULL)
        {
            return luaL_error(L,"string expected! %s   %d",__func__,__LINE__);    
        }
        std::string path_str(path);
        CALL_CPP_FUNCTION(L,p->Load(path_str));

        return 0;
    }

    static int Save(lua_State *L)
    {
        ConfigurePlt *p=CheckConfigurePlt(L,1);

        const char *path=luaL_checkstring(L,2);
        if(path==NULL)
        {
            return luaL_error(L,"string expected! %s   %d",__func__,__LINE__);    
        }

        std::string path_str(path);
        CALL_CPP_FUNCTION(L,p->Save(path_str));

        return 0;
    }
    static int LoadGRP(lua_State *L)  //const std::string &,int);
    {
        ConfigurePlt *p=CheckConfigurePlt(L,1);

        const char *path=luaL_checkstring(L,2);
        int g=luaL_checkint(L,3);

        if(path==NULL)
        {
            return luaL_error(L,"string expected! %s   %d",__func__,__LINE__);    
        }

        std::string path_str(path);
        CALL_CPP_FUNCTION(L,p->LoadGRP(path_str,g));

        return 0;
    }

    static int SaveGRP(lua_State *L)  //const std::string &,int);
    {
        ConfigurePlt *p=CheckConfigurePlt(L,1);

        const char *path=luaL_checkstring(L,2);
        int g=luaL_checkint(L,3);

        if(path==NULL)
        {
            return luaL_error(L,"string expected! %s   %d",__func__,__LINE__);    
        }

        std::string path_str(path);
        CALL_CPP_FUNCTION(L,p->SaveGRP(path_str,g));

        return 0;
    }
//    PBYTE GetLowerLeavel(int);    //返回指针？

private:
    static ConfigurePlt *CheckConfigurePlt(lua_State *L,int n)
    {
        ConfigurePlt **pp=(ConfigurePlt **)luaL_checkudata(L,n,_tname);

        return *pp;
    }

public:
    static const char *_tname;
};

const char *ConfigurePlt4Lua::_tname="ConfigurePlt";


extern "C"
int luaopen_configure(lua_State *L)
{
    int ret=0;
    struct luaL_Reg f[]={
        {"new",ConfigurePlt4Lua::init},
        {NULL,NULL}
    };
    struct luaL_Reg m[]={
//        {"new",ConfigurePlt4Lua::init},
        {"__gc",ConfigurePlt4Lua::finalizer},
        {"finalizer",ConfigurePlt4Lua::finalizer},
        {"get",ConfigurePlt4Lua::Get},
        {"put",ConfigurePlt4Lua::Put},
        {"get_ex",ConfigurePlt4Lua::Get_ex},
        {"load",ConfigurePlt4Lua::Load},
        {"save",ConfigurePlt4Lua::Save},
        {"loadGRP",ConfigurePlt4Lua::LoadGRP},
        {"saveGRP",ConfigurePlt4Lua::SaveGRP},
        {NULL,NULL}
    };

    ret=luaL_newmetatable(L,ConfigurePlt4Lua::_tname);
    if (ret==0)
    {
        return luaL_error(L,"%s  existed!  %s  %d",ConfigurePlt4Lua::_tname,__func__,__LINE__);
    }
    luaL_register(L,NULL,m);
    
    lua_pushvalue(L,-1);
    lua_setfield(L,-1,"__index");

    luaL_register(L,"Configure",f);

    return 1;
}
