
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

using namespace Galaxy::AMQ;

class BlockBuffer:public IBlock
{
public:
    BlockBuffer(const char *str,int len):size(len)
    {
        assert(len<8196);
        CRT_memcpy(buf,str,len);
    }
    BlockBuffer():size(8196)
    {
    }
    PBYTE RAWEntry() const
    {
        return (PBYTE)buf;
    }
    UINT RAWSize() const
    {
        return size;
    }
private:
    char buf[8196];
    int size;
};

class IMQ
{
public:
//virtual void BKD(UINT _BKDCode,...) const = 0;
//virtual const IMQueueArray &NQArray() const = 0;
//virtual const IMQueueArray &DQArray() const = 0;
//virtual const IPoolerView &PoolerView() const = 0;
};

class CGalaxyMQ4Lua
{
public:
    static int init(lua_State *L)
    {
        const char *path=luaL_checkstring(L,1);

        const Galaxy::AMQ::CGalaxyMQ **mq=(const Galaxy::AMQ::CGalaxyMQ **)lua_newuserdata(L,sizeof(Galaxy::AMQ::CGalaxyMQ *));

        try
        {
            *mq=new Galaxy::AMQ::CGalaxyMQ(path);
        }
        catch(std::exception &e)
        {
            return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
        }
        catch(...)
        {
            return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

        }
        luaL_getmetatable(L, "CGalaxyMQ");
        lua_setmetatable(L,-2);

        return 1;
    }
 
    static int finalizer(lua_State *L)
    {
        CGalaxyMQ **mq=(CGalaxyMQ **)luaL_checkudata(L,-1,"CGalaxyMQ");
        delete *mq;
        *mq=NULL;
        return 0;
    }
    static int PoolerView(lua_State *L)
    {
        CGalaxyMQ *p=lua_checkCGalaxyMQ(L,1);
        const Galaxy::AMQ::IPoolerView **ppoolview=(const Galaxy::AMQ::IPoolerView **)lua_newuserdata(L,sizeof(Galaxy::AMQ::IPoolerView *));
         
        CALL_CPP_FUNCTION(L,*ppoolview=&(p->PoolerView()));
        
        luaL_getmetatable(L,"PoolerView");
        lua_setmetatable(L,-2);

        return 1;
    }
    static int NQArray(lua_State *L)
    {
        CGalaxyMQ *p=lua_checkCGalaxyMQ(L,1);
        const Galaxy::AMQ::IMQueueArray **pp=(const Galaxy::AMQ::IMQueueArray **)lua_newuserdata(L,sizeof(Galaxy::AMQ::IMQueueArray*));
         
        CALL_CPP_FUNCTION(L,*pp=&(p->NQArray()));
        
        luaL_getmetatable(L,"IMQueueArray");
        lua_setmetatable(L,-2);

        return 1;
    }
    static int DQArray(lua_State *L)
    {
        CGalaxyMQ *p=lua_checkCGalaxyMQ(L,1);
        const Galaxy::AMQ::IMQueueArray **pp=(const Galaxy::AMQ::IMQueueArray **)lua_newuserdata(L,sizeof(Galaxy::AMQ::IMQueueArray*));
         
        CALL_CPP_FUNCTION(L,*pp=&(p->DQArray()));
        
        luaL_getmetatable(L,"IMQueueArray");
        lua_setmetatable(L,-2);

        return 1;
    }
    static int Get(lua_State *L)
    {
        CGalaxyMQ *p=lua_checkCGalaxyMQ(L,1);
        const Galaxy::AMQ::IMQueue **pp=(const Galaxy::AMQ::IMQueue**)lua_newuserdata(L,sizeof(Galaxy::AMQ::IMQueue*));

        int index =luaL_checkint(L,2);
        if (!isValidChannel(index))
        {
            return luaL_error(L,"%d is not a valid channel %s  %d",index,__func__,__LINE__);
        }

        CALL_CPP_FUNCTION(L,*pp=&((*p)[index]));
        
        luaL_getmetatable(L,"IMQueue");
        lua_setmetatable(L,-2);

        return 1;
    }
    static int InstanceOfSuite(lua_State *L)
    {
        CGalaxyMQ *p=lua_checkCGalaxyMQ(L,1);
        const Galaxy::AMQ::IMQ **pp=(const Galaxy::AMQ::IMQ**)lua_newuserdata(L,sizeof(Galaxy::AMQ::IMQ *));
         
        CALL_CPP_FUNCTION(L,*pp=&(p->InstanceOfSuite()));
        
        luaL_getmetatable(L,"IMQ");
        lua_setmetatable(L,-2);

        return 1;
    }

private:
    static CGalaxyMQ *lua_checkCGalaxyMQ(lua_State *L,int n)
    {
        return *(CGalaxyMQ **)luaL_checkudata(L,n,"CGalaxyMQ");
    }
public:
    static void register_CGalaxyMQ(lua_State *L)
    {
        struct luaL_Reg f[]={
            {"new",init},
            {NULL,NULL}
        };

        struct luaL_Reg m[]={
            {"__gc",finalizer},
            {"finalizer",finalizer},
            {"pooler_view",PoolerView}, 
            {"DQArray",DQArray}, 
            {"NQArray",NQArray}, 
            {"get",Get}, 
            {"instance_of_suite",InstanceOfSuite}, 
            {NULL,NULL}
        };

        luaL_newmetatable(L,"CGalaxyMQ");
        luaL_register(L,NULL,m);

        lua_pushvalue(L,-1);
        lua_setfield(L,-2,"__index");

        luaL_register(L,"AMQ",f);

    }

};

class IPoolerView4Lua
{
public:
    static int Pages(lua_State *L)
    {
        IPoolerView *poolview=lua_checkPoolerView(L,1);
        int pages=poolview->Pages();

        lua_pushnumber(L,pages);

        return 1;
    }
    static int PageSize(lua_State *L)
    {
        IPoolerView *poolview=lua_checkPoolerView(L,1);
        int size=poolview->PageSize();

        lua_pushnumber(L,size);

        return 1;
    }
    static int Total(lua_State *L)
    {
        IPoolerView *poolview=lua_checkPoolerView(L,1);
        int size=poolview->PageSize();

        lua_pushnumber(L,size);

        return 1;
    }
    static int Head(lua_State *L)
    {
        IPoolerView *poolview=lua_checkPoolerView(L,1);
        int head=poolview->Head();

        lua_pushnumber(L,head);

        return 1;
    }
    static int Tail(lua_State *L)
    {
        IPoolerView *poolview=lua_checkPoolerView(L,1);
        int tail=poolview->Tail();

        lua_pushnumber(L,tail);

        return 1;
    }
    static int Lurkers(lua_State *L)
    {
        IPoolerView *poolview=lua_checkPoolerView(L,1);
        int lurker=poolview->Lurkers();

        lua_pushnumber(L,lurker);

        return 1;
    }
private:
    static IPoolerView* lua_checkPoolerView(lua_State *L,int n)
    {
        return *(IPoolerView **)luaL_checkudata(L,n,"IPoolerView");
    }
public:
    static void register_IPoolerView(lua_State *L)
    {

        struct luaL_Reg m[]={
            {"pages",Pages},
            {"page_size",PageSize},
            {"head",Head},
            {"total",Total},
            {"tail",Tail},
            {"lurkers",Lurkers},
            {NULL,NULL}
        };

        luaL_newmetatable(L,"IPoolerView");
        luaL_register(L,NULL,m);

        lua_pushvalue(L,-1);
        lua_setfield(L,-2,"__index");
    }
};

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

        luaL_getmetatable(L, "IMQueue");
        lua_setmetatable(L,-2);

        return 1;
    }

private:
    static IMQueueArray * lua_checkMQA(lua_State *L,int n)
    {
        return *(IMQueueArray **)luaL_checkudata(L,n,"IMQueueArray");
    }
public:
    static void register_IMQueueArray(lua_State *L)
    {
        struct luaL_Reg m[]={
            {"get",Get},
            {"count",Count},
            {NULL,NULL}
        };

        luaL_newmetatable(L,"IMQueueArray");

        luaL_register(L,NULL,m);

        lua_pushvalue(L,-1);
        lua_setfield(L,-2,"__index");
    }
};

class IMQueue4Lua    
{
public:
    static int View(lua_State *L)
    {
        IMQueue *m=lua_checkMQ(L,1);

        const Galaxy::AMQ::IMQView **v=(const Galaxy::AMQ::IMQView **)lua_newuserdata(L,sizeof(Galaxy::AMQ::IMQView *));
        *v=&(m->View());
        
        luaL_getmetatable(L, "IMQView");
        lua_setmetatable(L,-2);

        return 1;
    }

    static int Get(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);
        int i =luaL_checkint(L,2);
        if (!isValidChannel(i))
        {
            return luaL_error(L,"%d is not a valid channel %s  %d",i,__func__,__LINE__);
        }

        BlockBuffer block;
        UINT len;
        UINT id;

        id=pMQ->Get((IMQueue::EMQCHL )i,block,len);
        
        //return 3 members , maybe should return table  instead?
        lua_pushnumber(L,id);
        lua_pushlstring(L,(const char *)block.RAWEntry(),len);    //it's not the zero-terminated string
        lua_pushnumber(L,len);
        
        return 3;
    }

    static int TimedGet(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);
        int i =luaL_checkint(L,2);
        short timeout =luaL_checkint(L,3);

        if (!isValidChannel(i))
        {
            return luaL_error(L,"%d is not a valid channel %s  %d",i,__func__,__LINE__);
        }

        BlockBuffer block;
        UINT len;
        UINT id;

        id=pMQ->TimedGet((IMQueue::EMQCHL)i,block,len,timeout);
        
        //return 3 members , maybe should return table  instead?
        lua_pushnumber(L,id);
        lua_pushlstring(L,(const char *)block.RAWEntry(),len);    //it's not the zero-terminated string
        lua_pushnumber(L,len);
        
        return 3;
    }

    static int PutEx(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);
        int i =luaL_checkint(L,2);
        if (!isValidChannel(i))
        {
            return luaL_error(L,"%d is not a valid channel %s  %d",i,__func__,__LINE__);
        }
        const char *buf=luaL_checkstring(L,3);
        
        if (lua_type(L,4)!=LUA_TBOOLEAN)
        {
            return luaL_error(L,"not a boolean %s   %d",__func__,__LINE__);
        }
        bool _ExPosHead=lua_toboolean(L,4);

        BlockBuffer block(buf,strlen(buf)+1);
        UINT len;
        UINT id;

        id=pMQ->PutEx((IMQueue::EMQCHL)i,block,len,_ExPosHead);

        lua_pushnumber(L,id); 
        return 1;
    }
    static int Put(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);
        int i =luaL_checkint(L,2);
        if (!isValidChannel(i))
        {
            return luaL_error(L,"%d is not a valid channel %s  %d",i,__func__,__LINE__);
        }
        const char *buf=luaL_checkstring(L,3);

        BlockBuffer block(buf,strlen(buf)+1);
        UINT len;
        UINT id;

        id=pMQ->Put((IMQueue::EMQCHL)i,block,len);    //why should we pass  argument len, it just size of block

        lua_pushnumber(L,id); 
//        lua_pushnumber(L,len);    //len=block.RawSize()
        return 1;
    }

    static int TimedPut(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);

        int i =luaL_checkint(L,2);
        if (!isValidChannel(i))
        {
            return luaL_error(L,"%d is not a valid channel %s  %d",i,__func__,__LINE__);
        }
        const char *buf=luaL_checkstring(L,3);
        
        int timeout=luaL_checkint(L,4);
        if (timeout>SHRT_MAX)
        {
            luaL_error(L,"number %d is not short  %s   %d",timeout,__func__,__LINE__);
        }

        BlockBuffer block(buf,strlen(buf)+1);
        UINT len;
        UINT id;

        id=pMQ->TimedPut((IMQueue::EMQCHL)i,block,len,timeout);

        lua_pushnumber(L,id); 
        return 1;
    }
    static int TimedPutEx(lua_State *L)
    {
        IMQueue *pMQ=lua_checkMQ(L,1);

        int i =luaL_checkint(L,2);
        if (!isValidChannel(i))
        {
            return luaL_error(L,"%d is not a valid channel %s  %d",i,__func__,__LINE__);
        }
        const char *buf=luaL_checkstring(L,3);
        
        if (lua_type(L,4)!=LUA_TBOOLEAN)
        {
            return luaL_error(L,"not a boolean %s   %d",__func__,__LINE__);
        }
        bool _ExPosHead=lua_toboolean(L,4);
        int timeout=luaL_checkint(L,5);
        if (timeout>SHRT_MAX)
        {
            luaL_error(L,"number %d is not short  %s   %d",timeout,__func__,__LINE__);
        }

        BlockBuffer block(buf,strlen(buf)+1);
        UINT len;
        UINT id;

        id=pMQ->TimedPutEx((IMQueue::EMQCHL)i,block,len,_ExPosHead,timeout);

        lua_pushnumber(L,id); 
        return 1;
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
        return *(IMQueue **)luaL_checkudata(L,n,"IMQueue");
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

        luaL_newmetatable(L,"IMQueue");
        luaL_register(L,NULL,m);

        lua_pushvalue(L,-1);
        lua_setfield(L,-2,"__index");
    }
};
class IMQView4Lua
{
public:
    static int Total(lua_State *L)
    {
        IMQView *view=lua_checkIMQView(L,1);
        int chl=luaL_checkint(L,2);
         
        int size=0;
        
        CALL_CPP_FUNCTION(L,view->Total((IMQueue::EMQCHL)chl));

        lua_pushnumber(L,size);
        return 1;
    }
    static int Head(lua_State *L)
    {
        IMQView *view=lua_checkIMQView(L,1);
        int chl=luaL_checkint(L,2);
         
        int size=0;
        
        CALL_CPP_FUNCTION(L,view->Head((IMQueue::EMQCHL)chl));

        lua_pushnumber(L,size);
        return 1;
    }
    static int Tail(lua_State *L)
    {
        IMQView *view=lua_checkIMQView(L,1);
        int chl=luaL_checkint(L,2);
         
        int size=0;
        
        CALL_CPP_FUNCTION(L,view->Tail((IMQueue::EMQCHL)chl));

        lua_pushnumber(L,size);
        return 1;
    }
    static int LurkersInGet(lua_State *L)
    {
        IMQView *view=lua_checkIMQView(L,1);
        int chl=luaL_checkint(L,2);
         
        int size=0;
        
        CALL_CPP_FUNCTION(L,view->LurkersInGet((IMQueue::EMQCHL)chl));

        lua_pushnumber(L,size);
        return 1;
    }    
    static int LurkersInPut(lua_State *L)
    {
        IMQView *view=lua_checkIMQView(L,1);
        int size=0;
        CALL_CPP_FUNCTION(L,view->LurkersInPut());

        lua_pushnumber(L,size);
        return 1;
    }
private:
    static IMQView *lua_checkIMQView(lua_State *L,int n)
    {
        return *(IMQView **)luaL_checkudata(L,n,"IMQView");
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

        luaL_newmetatable(L,"IMQView");
        luaL_register(L,NULL,m);

         /*typedef enum
         {
            // real time ,highest prior
            MQC_RTL=0,
            // query
            MQC_QRY=1,
            // bat
            MQC_BAT=2
         }EMQCHL;
        */
        setfield_int(L,IMQueue::MQC_RTL,"MQC_RTL");
        setfield_int(L,IMQueue::MQC_QRY,"MQC_QRY");
        setfield_int(L,IMQueue::MQC_BAT,"MQC_BAT");
        
        lua_pushvalue(L,-1);
        lua_setfield(L,-2,"__index");
    }
};

extern "C" int luaopen_amq(lua_State *L)
{
    IPoolerView4Lua::register_IPoolerView(L);    
    IMQueueArray4Lua::register_IMQueueArray(L);
    IMQueue4Lua::register_IMQueue(L);
    IMQView4Lua::register_IMQView(L);
    CGalaxyMQ4Lua::register_CGalaxyMQ(L);

    return 1;
}
