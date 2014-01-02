#include "AMQ/MQDQ.hpp"
#include "AMQ/MQNQ.hpp"
#include "AMQ/MQBlock.hpp"
#include "AMQ/GalaxyMQ.hpp"
#include <exception>
#include <assert.h>
#include <limits.h>
#include "LuaCPP.hpp"

//IMQueue::EMQCHL
#define isValidChannel(chl) (chl>=0 && chl<=2)

//32*1024
const int pageSize=32768;
using namespace Galaxy::AMQ;

class BlockBuffer:public IBlock
{
public:
    BlockBuffer(const char *str,int len=pageSize):
        buf(str,len)
    {
    }
    BlockBuffer(int len=pageSize):
        buf(len,0)
    {
    }
    PBYTE RAWEntry() const
    {
        return (PBYTE)&buf[0];
    }
    UINT RAWSize() const
    {
        return buf.length();
    }
private:
    std::string buf;
};

class IMQView4Lua
{
public:
    static int Total(lua_State *L)
    {
        IMQView *view=lua_checkIMQView(L,1);
   //     int chl=luaL_checkint(L,2);

        int size=0;

        CALL_CPP_FUNCTION(L,size=view->Total(IMQueue::MQC_BAT));

        lua_pushnumber(L,size);
        return 1;
    }
    static int Head(lua_State *L)
    {
        IMQView *view=lua_checkIMQView(L,1);
  //      int chl=luaL_checkint(L,2);

        int size=0;

//        CALL_CPP_FUNCTION(L,view->Head((IMQueue::EMQCHL)chl));
        CALL_CPP_FUNCTION(L,size=view->Head(IMQueue::MQC_BAT));

        lua_pushnumber(L,size);
        return 1;
    }
    static int Tail(lua_State *L)
    {
        IMQView *view=lua_checkIMQView(L,1);
//        int chl=luaL_checkint(L,2);

        int size=0;

        CALL_CPP_FUNCTION(L,size=view->Tail(IMQueue::MQC_BAT));

        lua_pushnumber(L,size);
        return 1;
    }
    static int LurkersInGet(lua_State *L)
    {
        IMQView *view=lua_checkIMQView(L,1);
 //       int chl=luaL_checkint(L,2);

        int size=0;

        CALL_CPP_FUNCTION(L,size=view->LurkersInGet(IMQueue::MQC_BAT));

        lua_pushnumber(L,size);
        return 1;
    }
    static int LurkersInPut(lua_State *L)
    {
        IMQView *view=lua_checkIMQView(L,1);
        int size=0;
        CALL_CPP_FUNCTION(L,size=view->LurkersInPut());

        lua_pushnumber(L,size);
        return 1;
    }
private:
    static IMQView *lua_checkIMQView(lua_State *L,int n)
    {
        return *(IMQView **)luaL_checkudata(L,n,METATABLE);
    }
public:
    static void register_IMQView(lua_State *L)
    {
        struct luaL_Reg m[]={
            {"lurkers_in_put",LurkersInPut},
            {"lurkers_in_get",LurkersInGet},
            {"head",Head},
            {"total",Total},
            {"tail",Tail},
            {NULL,NULL}
        };

        luaL_newmetatable(L,METATABLE);
        luaL_register(L,NULL,m);

        lua_pushvalue(L,-1);
        lua_setfield(L,-2,"__index");
    }
public:
    static const char *METATABLE;
};

const char *IMQView4Lua::METATABLE="IMQView";


class IMQueue4Lua
{
public:
    static int View(lua_State *L)
    {
        IMQueue *m=lua_checkMQ(L,1);

        const Galaxy::AMQ::IMQView **v=(const Galaxy::AMQ::IMQView **)lua_newuserdata(L,sizeof(Galaxy::AMQ::IMQView *));
        *v=&(m->View());

        luaL_getmetatable(L, IMQView4Lua::METATABLE);
        lua_setmetatable(L,-2);

        return 1;
    }

    static int Get(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);

        BlockBuffer block;
        UINT len;

        CALL_CPP_FUNCTION(L,pMQ->Get(IMQueue::MQC_BAT,block,len));

        lua_pushlstring(L,(const char *)block.RAWEntry(),len);    //it's not the zero-terminated string
        return 1;
    }

    static int TimedGet(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);
        short timeout =luaL_checkint(L,2);

        BlockBuffer block;
        UINT len;

        CALL_CPP_FUNCTION(L,pMQ->TimedGet(IMQueue::MQC_BAT,block,len,timeout));

        lua_pushlstring(L,(const char *)block.RAWEntry(),len);    //it's not the zero-terminated string

        return 1;
    }

    static int PutEx(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);

        const char *buf=luaL_checkstring(L,2);

        if (lua_type(L,3)!=LUA_TBOOLEAN)
        {
            return luaL_error(L,"not a boolean %s   %d",__func__,__LINE__);
        }
        bool _ExPosHead=lua_toboolean(L,3);

        BlockBuffer block(buf,strlen(buf)+1);
        UINT len;

        pMQ->PutEx(IMQueue::MQC_RTL,block,len,_ExPosHead);

        return 0;
    }
    static int Put(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);
        size_t len;
        const char *buf=luaL_checklstring(L,2, &len);

        BlockBuffer block(buf,len);

        CALL_CPP_FUNCTION(L,pMQ->Put(IMQueue::MQC_RTL,block,(UINT&)len));

        return 0;
    }

    static int TimedPut(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);

        const char *buf=luaL_checkstring(L,2);

        int timeout=luaL_checkint(L,3);
        if (timeout>SHRT_MAX)
        {
            luaL_error(L,"number %d is not short  %s   %d",timeout,__func__,__LINE__);
        }

        BlockBuffer block(buf,strlen(buf)+1);
        UINT len;

        CALL_CPP_FUNCTION(L,pMQ->TimedPut(IMQueue::MQC_RTL,block,len,timeout));

        return 0;
    }
    static int TimedPutEx(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);

        const char *buf=luaL_checkstring(L,2);

        if (lua_type(L,3)!=LUA_TBOOLEAN)
        {
            return luaL_error(L,"not a boolean %s   %d",__func__,__LINE__);
        }
        bool _ExPosHead=lua_toboolean(L,3);
        int timeout=luaL_checkint(L,4);
        if (timeout>SHRT_MAX)
        {
            luaL_error(L,"number %d is not short  %s   %d",timeout,__func__,__LINE__);
        }

        BlockBuffer block(buf,strlen(buf)+1);
        UINT len;

        CALL_CPP_FUNCTION(L,pMQ->TimedPutEx(IMQueue::MQC_RTL,block,len,_ExPosHead,timeout));

        return 0;
    }

    static int GetMaxDepth(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);

        UINT depth=pMQ->GetMaxDepth();

        lua_pushnumber(L,depth);

        return 1;
    }

    static int SetMaxDepth(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);

        int depth=luaL_checkint(L,2);
        pMQ->SetMaxDepth(depth);

        return 0;   //nothing return
    }

    static int Reset(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);

        pMQ->Reset();
        return 0;   //nothing return
    }

private:
    static IMQueue *lua_checkMQ(lua_State *L,int n)
    {
        return *(IMQueue **)luaL_checkudata(L,n,METATABLE);
    }

public:
    static void register_IMQueue(lua_State *L)
    {
        struct luaL_Reg m[]={
            {"view",View},
            {"get",Get},
            {"timed_get",TimedGet},
            {"put",Put},
            {"put_ex",PutEx},
            {"timed_put",TimedPut},
            {"time_put_ex",TimedPutEx},
            {"reset",Reset},
            {"get_max_depth",GetMaxDepth},
            {"set_max_depth",SetMaxDepth},
            {NULL,NULL}
        };

        luaL_newmetatable(L,METATABLE);
        luaL_register(L,NULL,m);

        lua_pushvalue(L,-1);
        lua_setfield(L,-2,"__index");
    }
public:
    static const char *METATABLE;
};

const char *IMQueue4Lua::METATABLE="IMQueue";


class IMQueueArray4Lua
{
public:
    static int  Count(lua_State *L)
    {
        IMQueueArray *m=lua_checkMQA(L,1);

        lua_pushnumber(L,m->Count());

        return 1;
    }
    static int  Get(lua_State *L)
    {
        IMQueueArray *m=lua_checkMQA(L,1);
        int index=luaL_checkint(L,2);

        const Galaxy::AMQ::IMQueue **mq=(const Galaxy::AMQ::IMQueue **)lua_newuserdata(L,sizeof(Galaxy::AMQ::IMQueue *));
        *mq=&(*m)[index];

        luaL_getmetatable(L, IMQueue4Lua::METATABLE);
        lua_setmetatable(L,-2);

        return 1;
    }

private:
    static IMQueueArray * lua_checkMQA(lua_State *L,int n)
    {
        return *(IMQueueArray **)luaL_checkudata(L,n,METATABLE);
    }
public:
    static void register_IMQueueArray(lua_State *L)
    {
        struct luaL_Reg m[]={
            {"get",Get},
            {"count",Count},
            {NULL,NULL}
        };

        luaL_newmetatable(L,METATABLE);

        luaL_register(L,NULL,m);

        lua_pushvalue(L,-1);
        lua_setfield(L,-2,"__index");
    }
public:
    static const char *METATABLE;

};
const char *IMQueueArray4Lua::METATABLE="IMQueueArray";


class IMPoolView4Lua
{
public:
    static int Size(lua_State *L)
    {
        IPoolerView *v=lua_checkMPV(L,1);

        lua_pushnumber(L,v->Pages());

        return 1;
    }
    static int Total(lua_State *L)
    {
        IPoolerView *v=lua_checkMPV(L,1);

        lua_pushnumber(L,v->Total());

        return 1;
    }
    static int Head(lua_State *L)
    {
        IPoolerView *v=lua_checkMPV(L,1);

        lua_pushnumber(L,v->Head());

        return 1;
    }
    static int Tail(lua_State *L)
    {
        IPoolerView *v=lua_checkMPV(L,1);

        lua_pushnumber(L,v->Tail());

        return 1;
    }
    static int Lurkers(lua_State *L)
    {
        IPoolerView *v=lua_checkMPV(L,1);

        lua_pushnumber(L,v->Lurkers());

        return 1;
    }
private:
    static IPoolerView * lua_checkMPV(lua_State *L,int n)
    {
        return *(IPoolerView **)luaL_checkudata(L,n,METATABLE);
    }
public:
    static void register_IMPoolView(lua_State *L)
    {
        struct luaL_Reg m[]={
            {"size",Size},
            {"total",Total},
            {"tail",Tail},
            {"lurkers",Lurkers},
            {NULL,NULL}
        };

        luaL_newmetatable(L,METATABLE);

        luaL_register(L,NULL,m);

        lua_pushvalue(L,-1);
        lua_setfield(L,-2,"__index");
    }
public:
    static const char *METATABLE;
};
const char *IMPoolView4Lua::METATABLE="IMPoolView";


class CGalaxyMQ4Lua
{
public:
    static int init(lua_State *L)
    {
        const char *path=luaL_checkstring(L,1);
        printf("%s\n",path);
        const Galaxy::AMQ::CGalaxyMQ **mq=(const Galaxy::AMQ::CGalaxyMQ **)lua_newuserdata(L,sizeof(Galaxy::AMQ::CGalaxyMQ *));

        try
        {
            if (access(path, F_OK) != 0)
            {
                printf("Created\n");
                Galaxy::AMQ::CGalaxyMQCreator _(std::string(path), std::string("who"), 1024*10, pageSize, 32, 0, 0, 2500, 0, 0);
            }
            *mq=new Galaxy::AMQ::CGalaxyMQ(path);
            printf("%s\n",path);
        }
        catch(std::exception &e)
        {
            return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
        }
        catch(...)
        {
            return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

        }
        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);

        return 1;
    }

    static int finalizer(lua_State *L)
    {
        CGalaxyMQ **mq=(CGalaxyMQ **)luaL_checkudata(L,-1,METATABLE);
        delete *mq;
        *mq=NULL;
        return 0;
    }
    static int NQArray(lua_State *L)
    {
        CGalaxyMQ *p=lua_checkCGalaxyMQ(L,1);
        const Galaxy::AMQ::IMQueueArray **pp=(const Galaxy::AMQ::IMQueueArray **)lua_newuserdata(L,sizeof(Galaxy::AMQ::IMQueueArray*));

        CALL_CPP_FUNCTION(L,*pp=&(p->NQArray()));

        luaL_getmetatable(L,IMQueueArray4Lua::METATABLE);
        lua_setmetatable(L,-2);

        return 1;
    }
    static int PoolView(lua_State *L)
    {
        CGalaxyMQ *p=lua_checkCGalaxyMQ(L,1);
        const Galaxy::AMQ::IPoolerView **pp=(const Galaxy::AMQ::IPoolerView **)lua_newuserdata(L,sizeof(Galaxy::AMQ::IPoolerView*));

        CALL_CPP_FUNCTION(L,*pp=&(p->PoolerView()));

        luaL_getmetatable(L,IMPoolView4Lua::METATABLE);
        lua_setmetatable(L,-2);

        return 1;
    }
private:
    static CGalaxyMQ *lua_checkCGalaxyMQ(lua_State *L,int n)
    {
        return *(CGalaxyMQ **)luaL_checkudata(L,n,METATABLE);
    }
public:
    static const char *METATABLE;

};
const char *CGalaxyMQ4Lua::METATABLE="CGalaxyMQ";


extern "C" int luaopen_amq(lua_State *L)
{
    IMQueueArray4Lua::register_IMQueueArray(L);
    IMQueue4Lua::register_IMQueue(L);
    IMQView4Lua::register_IMQView(L);
    IMPoolView4Lua::register_IMPoolView(L);

    struct luaL_Reg f[]={
        {"new",CGalaxyMQ4Lua::init},
        {NULL,NULL}
    };

    struct luaL_Reg m[]={
        {"__gc",CGalaxyMQ4Lua::finalizer},
        {"finalizer",CGalaxyMQ4Lua::finalizer},
        {"NQArray",CGalaxyMQ4Lua::NQArray},
        {"PoolView",CGalaxyMQ4Lua::PoolView},
        {NULL,NULL}
    };

    luaL_newmetatable(L,CGalaxyMQ4Lua::METATABLE);
    luaL_register(L,NULL,m);

    lua_pushvalue(L,-1);
    lua_setfield(L,-2,"__index");

    setfield_string(L,CGalaxyMQ4Lua::METATABLE,"__my_name");

    luaL_register(L,"AMQ",f);

    return 1;
}
