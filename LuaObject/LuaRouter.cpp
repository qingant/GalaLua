#include "LuaCPP.hpp"
#include "Router.hpp"

class Router4Lua:public Galaxy::GalaxyRT::CNonCopyAble
{
public: 
    static int init(lua_State *L) // Router(const std::string &path);
    {
        const char *path=luaL_checkstring(L,1);
        Router **pp=(Router **)lua_newuserdata(L,sizeof(Router *));

        CALL_CPP_FUNCTION(L,*pp=new Router(std::string(path)));

        luaL_getmetatable(L,NameOfMetatable);
        lua_setmetatable(L,-2);

        return 1;
    }

    static int finalizer(lua_State *L)
    {
        Router **pp=(Router **)luaL_checkudata(L,1,NameOfMetatable);

        delete *pp;

        *pp=NULL;

        DEBUG("finalizer");

        return 0;
    }

//    void Register(const std::string &name,const std::string &field,const std::string &host,int port,int gpid);
    static int Register(lua_State *L)
    {
        Router *p=CheckRouter(L,1);

        const char *name=luaL_checkstring(L,2);
        const char *field=luaL_checkstring(L,3);
        const char *host=luaL_checkstring(L,4);
        if (name==NULL || field==NULL ||host==NULL)
        {
            return luaL_error(L,"string expected!  %s  %d ",__func__,__LINE__);
        }
        int port=luaL_checkint(L,5);
        int gpid=luaL_checkint(L,6);

        CALL_CPP_FUNCTION(L,p->Register(std::string(name),std::string(field),std::string(host),port,gpid));
        
        return 0;
    }
//    void Delete(const std::string &name);
    static int Delete(lua_State *L)
    {
        Router *p=CheckRouter(L,1);
        const char *name=luaL_checkstring(L,2);
        if (name==NULL )
        {
            return luaL_error(L,"string expected!  %s  %d ",__func__,__LINE__);
        }

        CALL_CPP_FUNCTION(L,p->Delete(std::string(name)));
    
        return 0;
    }

    //Item FindByName(const std::string &);
    static int FindByName(lua_State *L)
    {
        Router *p=CheckRouter(L,1);
        const char *name=luaL_checkstring(L,2);
        if (name==NULL )
        {
            return luaL_error(L,"string expected!  %s  %d ",__func__,__LINE__);
        }
        
        Item i; 
        CALL_CPP_FUNCTION(L,i=p->FindByName(std::string(name)));
        
        PushItem2table(L,i);

        return 1;
    }
    //std::vector<Item> FindByField(const std::string &);
    static int FindByField(lua_State *L)
    {
        Router *p=CheckRouter(L,1);
        const char *field=luaL_checkstring(L,2);
        if (field==NULL )
        {
            return luaL_error(L,"string expected!  %s  %d ",__func__,__LINE__);
        }
        std::vector<Item> v;
        CALL_CPP_FUNCTION(L,v=p->FindByField(std::string(field)));
        
        lua_newtable(L);
        for (unsigned int i=0;i<v.size();++i)
        {
            PushItem2table(L,v[i]);
            lua_rawseti(L,-2,i+1);
        }

        return 1;
    }
private:
    static Router *CheckRouter(lua_State *L,int n)
    {
        return *(Router **)luaL_checkudata(L,n,NameOfMetatable);
    }

    static void PushItem2table(lua_State *L,Item &item)
    {
        lua_newtable(L);
        setfield_int(L,item.Usable,"usable");
        setfield_string(L,item.Name,"name");
        setfield_string(L,item.Field,"field");

        lua_newtable(L);
        setfield_string(L,item.Addr.Host,"host");
        setfield_int(L,item.Addr.Port,"port");
        setfield_int(L,item.Addr.Gpid,"gpid");
        lua_setfield(L,-2,"addr");
    }
public:
    static const char *NameOfMetatable;
};
const char *Router4Lua::NameOfMetatable="Router4Lua";

extern "C" int luaopen_router(lua_State *L)
{
    struct luaL_Reg f[]={
        {"new",Router4Lua::init},
        {NULL,NULL}
    };
    struct luaL_Reg m[]={
        {"__gc",Router4Lua::finalizer},
        {"finalizer",Router4Lua::finalizer},
        {"register",Router4Lua::Register},
        {"delete",Router4Lua::Delete},
        {"find_by_field",Router4Lua::FindByField},
        {"find_by_name",Router4Lua::FindByName},
        {NULL,NULL}
    };
    
    luaL_newmetatable(L,Router4Lua::NameOfMetatable);
    luaL_register(L,NULL,m);

    lua_pushvalue(L,-1);
    lua_setfield(L,-2,"__index");

    luaL_register(L,"Router",f);

    return 1;
}
