#include "LuaNode.hpp"
#include "Schedule.hpp"
#include "GLR.hpp"
#include "lualib.h"
#include "Process.hpp"
#include "NamedProcessTable.hpp"
#include <algorithm>
//defines of static data member
#define MAX_PROCESS_ID (1024*10)
using namespace GLR;
#if defined(HAVE_SUSE)
std::vector<Process *> Process::NodeMap(MAX_PROCESS_ID,(Process *)0);
#else
std::vector<Process *> Process::NodeMap(MAX_PROCESS_ID, NULL);
#endif
//std::vector<Process*>(10240);
Galaxy::GalaxyRT::CRWLock Process::ProcessMapLock;
int32_t Process::NodeId;
int32_t Process::NodeCount = 0;
uint32_t Process::MsgIdGen = 0;
#define RESERVED_PID (32)

Process::Process(int id)
    : _Stack(luaL_newstate()),
    _Id(id == 0 ? NodeId : id)
{
    ++NodeId;
    // step over supervisor binded pid
    if (NodeId == 1)
    {
        NodeId += RESERVED_PID;    // reserve first 12 id for special services
    }

    if (_Stack == NULL)
    {
        THROW_EXCEPTION_EX("Create Lua Stack Failure");
    }
    time(&_Status._Start);
    InitNode();
}


Process::~Process(void)
{
    lua_close(_Stack);
}

void Process::SendMsg(const LN_MSG_TYPE& msg)
{
    GALA_DEBUG("Send %d", _Id);
    GALA_DEBUG("This(%p) LuaState(%p)", this, _Stack);


    Galaxy::GalaxyRT::CLockGuard _gl(&_Lock);
    bool isEmpty = _Channel.Empty();


    if (isEmpty && (State() == ProcessStatus::RECV_WAIT))
    {
        // TODO: expose more info to lua
        Galaxy::GalaxyRT::CLockGuard _gl(&_IntLock);
        GLRPROTOCOL *head = (GLRPROTOCOL *)&msg[0];
        BuildMessageReturnValues(head);
        _Status._NArg = 3;
        _Status._State = Process::ProcessStatus::RECV_RETURN;
        //StackDump();
        GALA_DEBUG("PutTask");
        Runtime::GetInstance().GetSchedule().PutTask(*this);
        GALA_DEBUG("Return");

    } else
    {
        GALA_DEBUG("Put!");
        _Channel.Put(msg);
    }
}

LN_MSG_TYPE Process::RecvMsg()
{
    //while (_Channel.empty())
    //{
    //    ++_Waiter;
    //    _Cond.wait(_gl);
    //}
    LN_MSG_TYPE v = _Channel.TryGet();
    return v;
}

LN_ID_TYPE Process::CreateNode()
{
    return CreateNode(0);
}
int Process::Spawn(lua_State *l)
{

    try
    {
        if (l == NULL)
        {
            THROW_EXCEPTION_EX("Fatal Error");
        }
        //int count = lua_gettop(l);
        // Get Arguments From Caller State
        size_t len = 0;
        const char *module = NULL;
        module = lua_tolstring(l, 1, &len);
        if (module == NULL)
        {
            THROW_EXCEPTION_EX("Extract Para Error");
        }


        const char *method = NULL;
        method = lua_tolstring(l, 2, &len);

        if (module == NULL)
        {
            THROW_EXCEPTION_EX("Extract Para Error");
        }

        // Create Lua Node
        LN_ID_TYPE node_id = CreateNode();
        Process& node = GetNodeById(node_id);


        // set node parent id(for supervisor tree)
        lua_getglobal(l, "__id__");
        int parentId = luaL_checkint(l, -1);
        node._ParentId = parentId;
        lua_pop(l,1);


        if (GLR::Runtime::_GarFile.empty())
        {
            node.Entry(module, method);
        } else
        {
            node.EntryGar(GLR::Runtime::_GarFile, module, method);
        }


        // Set arguments for the new-created glr process
        SetArgumentsForSpawnedProcess(l, node, 3);


        // Return Value to Calling Lua Node
        lua_pushboolean(l, 1);
        lua_pushinteger(l, node_id);


       // put to schedule queue
        node.Start(Schedule::GetInstance());
        return 2;
        // Op on Self State
        //lua_getglobal(this->_Stack, buf);
    }
    catch (Galaxy::GalaxyRT::CException& e)
    {
        lua_pushnil(l);
        lua_pushstring(l, e.what());
        return 2;
    }


}

Process& Process::GetNodeById(LN_ID_TYPE id)
{
    if ((size_t)id >= NodeMap.size())
    {
        THROW_EXCEPTION_EX("Process Not Exist");
    }

    Process *p = NodeMap[id];

    if (p == NULL)
    {
        THROW_EXCEPTION_EX("Process Not Exist");
    }
    return *p;


}

int GLR::Process::GetOption(lua_State *l)
{
    Galaxy::GalaxyRT::CProcess *_pCProcess = GLR::Runtime::_pCProcess;
    if (_pCProcess == NULL)
    {

        luaL_error(l, "_pCProcess is NULL");
        return 0;
    }
    lua_newtable(l);
    for (size_t i = 0; i != _pCProcess->Arguments.size(); ++i)
    {
        lua_pushstring(l, _pCProcess->Arguments[i].c_str());
        lua_rawseti(l, -2, i);
    }

    return 1;
}
int GLR::Process::SetOptions(lua_State *l)
{
    Galaxy::GalaxyRT::CProcess *_pCProcess = GLR::Runtime::_pCProcess;
    if (_pCProcess == NULL)
    {
        luaL_error(l, "pCProcess is null");
        return 0;
    }

    const char *str = luaL_checkstring(l, 1);
    int i = _pCProcess->SetArgument(str);
    lua_pushnumber(l, i);
    return 1;
}

void Process::InitNode(void)
{
    luaL_openlibs(_Stack);

    // built in module glr
    static const  luaL_Reg glr_reg[] =
    {
        { "spawn", Spawn },
        { "spawn_ex", SpawnEx},
        { "send", SendMsgToNode },
        { "recv", Recieve },
        { "int", Interrupt },
        { "all", AllProcesses },
        { "status", Status },
        { "node_addr", GetNodeAddr },
        { "get_path", GetFilePath },
        { "kill", Kill },
        { "set_options", SetOptions },
        { "get_option", GetOption },
        { "msg_available", MessageAvailable },
        { "glr_stamp", GLRStamp },
        { "exit", Exit },
        { NULL, NULL },
    };
    luaL_register(_Stack, "_glr", glr_reg);
    lua_setglobal(_Stack, "_glr");

    lua_getglobal(_Stack, "_glr");
    lua_pushstring(_Stack, "id");
    lua_pushinteger(_Stack, _Id);
    lua_settable(_Stack, -3);


    NamedProcessTable::CreateOpTable(_Stack);
    lua_setfield(_Stack, -2, "npt");
    // Some useful const
    // lua_getglobal(_Stack, "glr");
    lua_pushstring(_Stack, "AF_UNIX");
    lua_pushinteger(_Stack, AF_UNIX);
    lua_settable(_Stack, -3);

    // lua_getglobal(_Stack, "glr");
    lua_pushstring(_Stack, "AF_INET");
    lua_pushinteger(_Stack, AF_INET);
    lua_settable(_Stack, -3);

    lua_pushstring(_Stack, "KILL");
    lua_pushinteger(_Stack, GLRPROTOCOL::KILL);
    lua_settable(_Stack, -3);

    lua_pushstring(_Stack, "CLOSED");
    lua_pushinteger(_Stack, GLRPROTOCOL::CLOSED);
    lua_settable(_Stack, -3);

    lua_pushstring(_Stack, "APP");
    lua_pushinteger(_Stack, GLRPROTOCOL::APP);
    lua_settable(_Stack, -3);

    lua_pushstring(_Stack, "EXIT");
    lua_pushinteger(_Stack, GLRPROTOCOL::EXIT);
    lua_settable(_Stack, -3);


    //StackDump();
    //lua_pop(_Stack,1);

    // some global var
    lua_pushinteger(_Stack, _Id);
    lua_setglobal(_Stack, "__id__");

    if (GLR::Runtime::_Initializer != NULL)
    {
        GLR::Runtime::_Initializer(_Stack);    //user defined initialize hook
    }


    // getnid

    lua_getglobal(_Stack, "require");
    lua_pushstring(_Stack, "glr");
    if (lua_pcall(_Stack, 1, 1, 0) != 0)
    {
        GALA_ERROR("Import `glr` Failed");
        StackDump();
        const char *msg = luaL_checkstring(_Stack, -1);
        THROW_EXCEPTION_EX(msg);
    }
    lua_setglobal(_Stack, "glr");


    _Status._State = ProcessStatus::CREATED;
    // hook
    //lua_sethook(_Stack, LuaNode::Preempt, LUA_MASKCOUNT, 1000);
}

int Process::SendMsgToNode(lua_State *l)
{
    lua_getglobal(l, "__id__");
    //lua_pushlstring("id");
    LN_ID_TYPE self_id = luaL_checkinteger(l, -1);
    lua_pop(l, 1);

    LN_ID_TYPE id = luaL_checkinteger(l, 1);

    size_t len = 0;
    const char *msg = luaL_checklstring(l, 2, &len);

    uint32_t msgid = MsgIdGen, corrid = 0;

    // TODO: atomic_add
    MsgIdGen++;


    if (lua_gettop(l) == 3 && lua_istable(l,-1))
    {
        lua_getfield(l, 3, "msgid");
        if (!lua_isnil(l, -1))
        {
            msgid = luaL_checkinteger(l, -1);
        }
        lua_pop(l,1);

        lua_getfield(l, 3, "corrid");
        corrid = luaL_checkinteger(l, -1);
        lua_pop(l,1);
    }

    LN_MSG_TYPE  pack_msg(len + sizeof(GLRPROTOCOL), 0);
    GLRPROTOCOL *head = (GLRPROTOCOL *)&pack_msg[0];
    head->_Protocol._Type = GLRPROTOCOL::APP;
    head->_Protocol._Version = 2;
    CRT_time((time_t*)&head->_Protocol._Stamp);

    head->_Msg._MsgId = msgid;
    head->_Msg._CorrId = corrid;
    head->_Route._ToGpid = id;
    head->_Protocol._Length = len + sizeof(GLRPROTOCOL) - 4; //Content Length
    head->_Route._FromGpid = self_id;
    head->_Host._V2._Port = GLR::Runtime::GetInstance().NodeId();
    memcpy(head->_Host._V2._Host, GLR::Runtime::GetInstance().Host().c_str(), GLR::Runtime::GetInstance().Host().size());
    memcpy((void *)&pack_msg[sizeof(*head)], msg, len);
    //GetNodeById(id).SendMsg(pack_msg);
    SendMsgToNode(id, pack_msg);

    lua_newtable(l);

    lua_pushinteger(l, msgid);
    lua_setfield(l, -2, "msgid");

    lua_pushinteger(l, corrid);
    lua_setfield(l, -2, "corrid");

    lua_pushinteger(l, head->_Protocol._Stamp);
    lua_setfield(l, -2, "stamp");

    return 1;
}

void Process::Preempt(lua_State *l, lua_Debug *ar)
{

    lua_getinfo(l, "n", ar);
    printf("Line hook %d\n", ar->currentline);
    lua_yield(l, 0);
}

void Process::Resume()
{
    //int narg = 0;
    int rt = 0;
    /* if (_Status.State() == ProcessStatus::RUNNING)
    {
    THROW_EXCEPTION_EX("Fatal Erorr: Corrupt Process Schedule");
    }*/
    //printf("Resumed!\n");
    //printf("Node(%d) Go To Here\n", _Id);
    //Status();
    Galaxy::GalaxyRT::CLockGuard _Gl(&_IntLock);
    assert(_Status._NArg < 20);
    //lua_pushstring(_Stack, "Resume!");
    _Status._State = ProcessStatus::RUNNING;
    _Status._Tick++;

#if LUA_VERSION_NUM==501
    rt = lua_resume(_Stack, _Status._NArg);
#elif  LUA_VERSION_NUM==502
    rt = lua_resume(_Stack, NULL,_Status._NArg);
#endif

    if (_Status._Killed)
    {
        _Status._State = Process::ProcessStatus::KILLED;
        //Destory(_Id);
        //_Gl.~CLockGuard();
        //_Gl::~CLockGuard();

        //Destory(_Id);   // very very dangerous
        THROW_EXCEPTION_EX("Killed");
        return;
    }

    _Status._NArg = 0;
    if (rt == LUA_YIELD)
    {
        if (State() != ProcessStatus::RECV_WAIT && State() != ProcessStatus::INT_WAIT)
        {

            _Status._State = ProcessStatus::YIELD;
            return;
        }
    } else if (rt == 0)
    {
        _Status._State = ProcessStatus::STOPED;
        THROW_EXCEPTION_EX("Stopped!");
        return;
    }
    //lua_newthread()
    else
    {
        GALA_DEBUG("Resume Return (%d)\n", rt);
        StackDump();

        luaL_traceback(_Stack,_Stack,NULL,1);
        const char *msg = luaL_checkstring(_Stack, -1);
        GALA_DEBUG(msg);
        _Status._State = ProcessStatus::STOPED;
        THROW_EXCEPTION_EX(msg);

    }

}

int Process::Recieve(lua_State *l)
{
    Galaxy::GalaxyRT::CRWLockAdapter _RL(ProcessMapLock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_RL);

    lua_getglobal(l, "__id__");
    //lua_pushlstring("id");
    LN_ID_TYPE self_id = luaL_checkinteger(l, -1);
    Process& self = GetNodeById(self_id);
    {
        Galaxy::GalaxyRT::CLockGuard _gl(&self._Lock);
        if (self._Channel.Empty())
        {

            self._Status._State = ProcessStatus::RECV_WAIT;
            if (lua_gettop(self._Stack) == 2 && lua_isnumber(self._Stack,1))
            {
                GALA_DEBUG("Call Timeout Version");
                int timeout = lua_tointeger(self._Stack, 1);
                self.SetTimeOut(timeout);
            }


            //lua_pushstring(l, "SUSPEND");
            //return lua_yield(self._Stack, 0);
            GALA_DEBUG("No Msg %d", self_id);
            return self.Yield();
        }
    }
    try
    {
        LN_MSG_TYPE msg = self.RecvMsg();

        GLRPROTOCOL *head = (GLRPROTOCOL *)&msg[0];
        self.BuildMessageReturnValues(head);
        return 3;
    }
    catch (Galaxy::GalaxyRT::CException& e)
    {
        GALA_DEBUG("%s", e.what());
        lua_pushnil(l);
        lua_pushstring(l, e.what());
        return 2;
    }


}

void Process::DoString(const std::string& code)
{
    if ((luaL_loadstring(_Stack, code.c_str()) != 0) && (lua_pcall(_Stack, 0, 0, 0) != 0))
    {
        StackDump();
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        THROW_EXCEPTION_EX(msg);
    } else
    {
        _Path = "";
        printf("Load String Succeed\n");
    }
}

void Process::PushFun(const std::string& fname)
{
    lua_getglobal(_Stack, fname.c_str());

}

void Process::Status()
{
    printf("LuaNode Status:\n");
    for (typeof(NodeMap.begin()) it = NodeMap.begin(); it != NodeMap.end(); ++it)
    {
        printf("LuaNode [%d] of Object(%p) Status(%d)\n", (*it)->_Id, (*it), (*it)->State());
    }

}

void Process::Start(Schedule& sche)
{
    sche.PutTask(*this);
}

void Process::LoadString(const std::string& path)
{

    if (luaL_loadstring(_Stack, path.c_str()) != 0)
    {
        StackDump();
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        THROW_EXCEPTION_EX(msg);
    } else
    {
        _Path = "";
        lua_pushstring(_Stack, "__main__");
        _Status._NArg = 1; // one argument
        printf("Load string Succeed\n");
    }

}
void Process::LoadFile(const std::string& path)
{

    if (luaL_loadfile(_Stack, path.c_str()) != 0)
    {
        StackDump();
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        THROW_EXCEPTION_EX(msg);
    } else
    {
        _Path = path;
        lua_pushstring(_Stack, "__main__");
        _Status._NArg = 1; // one argument
        printf("Load File(%s) Succeed\n", path.c_str());
    }

}

void Process::StackDump()
{
    int top = lua_gettop(_Stack);
    printf("Stack of Node(%p:%d):\n", this, _Id);
    for (int i = 1; i <= top; ++i)
    {
        int type = lua_type(_Stack, i);
        printf("    %d : ", i);
        switch (type)
        {
        case LUA_TSTRING:
            printf("'%s'", lua_tostring(_Stack, i));
            break;
        case LUA_TBOOLEAN:
            printf(lua_toboolean(_Stack, i) ? "true" : "false");
            break;
        case LUA_TNUMBER:
            printf("%g", lua_tonumber(_Stack, i));
            break;
        default:
            printf("<object of %s>", lua_typename(_Stack, i));
            break;
        }
        printf("\n");

    }
    printf("---------------------\n");

}

void Process::MoveValue(lua_State *src, lua_State *dst, int index)
{
    int type = lua_type(src, index);
    const char *str=NULL;
    size_t len=0;
    switch (type)
    {
        case LUA_TSTRING:
            str = luaL_checklstring(src, index, &len);
            lua_pushlstring(dst, str, len);
            break;
        case LUA_TBOOLEAN:
            len = lua_toboolean(src, index);
            lua_pushboolean(dst, len);
            break;
        case LUA_TNUMBER:
            len = luaL_checkinteger(src, index);
            lua_pushinteger(dst, len);
            break;
        case LUA_TNIL:
            lua_pushnil(dst);
            break;
        case LUA_TTABLE:
            MoveTable(src,dst,index);
            break;
        default:
            THROW_EXCEPTION_EX("not supported argument");
            break;
    }

}

void Process::MoveTable(lua_State *src, lua_State *dst, int index)
{
    if (lua_istable(src,index)==0) {
        THROW_EXCEPTION_EX("not a lua table");
    }

    int src_top=lua_gettop(src);
    //XXX: @index never use negative number
    if (index<0) {
        index=src_top+index+1;
    }

    lua_newtable(dst);
    lua_pushnil(src);  /* first key */
    int top=-1;
    while (lua_next(src, index) != 0) {
        /* uses 'key' (at index -2) and 'value' (at
           * index -1) */
        top=lua_gettop(dst);
        try
        {

            MoveValue(src,dst,-2);
            MoveValue(src,dst,-1);

            lua_settable(dst,-3);
        }
        catch(Galaxy::GalaxyRT::CException& e)
        {
            //recover stack and go on
            lua_settop(dst,top);
        }

        /* removes 'value'; keeps 'key' for next
           * iteration */
        lua_pop(src, 1);
    }

}

void Process::SetArgumentsForSpawnedProcess(lua_State *l, Process &node, int begin_index)
{
    int i = begin_index, j = 0;
    int top=lua_gettop(l);
    for (; i <=top; ++i, ++j)
    {
        MoveValue(l,node._Stack,i);
    }

    // using += for ":"-call in entry
    node._Status._NArg += (j);
}

void Process::BuildMessageReturnValues(GLRPROTOCOL *head)
{
    lua_pushinteger(this->_Stack, head->_Protocol._Type);

    lua_newtable(this->_Stack); // desc

    lua_newtable(this->_Stack); // addr
    lua_pushstring(this->_Stack, (const char*)head->_Host._V2._Host);
    lua_setfield(this->_Stack, -2, "host");

    lua_pushinteger(this->_Stack, head->_Host._V2._Port);
    lua_setfield(this->_Stack, -2, "port");

    lua_pushinteger(this->_Stack, head->_Route._FromGpid);
    lua_setfield(this->_Stack, -2, "gpid");

    lua_setfield(this->_Stack, -2, "addr");

    lua_newtable(this->_Stack); // attr

    // timestamp
    lua_pushinteger(this->_Stack, head->_Protocol._Stamp);
    lua_setfield(this->_Stack, -2, "stamp");

    // msgid
    // XXX: if _MsgId converted to native endian?
    lua_pushinteger(this->_Stack, (int)head->_Msg._MsgId);
    lua_setfield(this->_Stack, -2, "msgid");

    lua_pushinteger(this->_Stack, (int)head->_Msg._CorrId);
    lua_setfield(this->_Stack, -2, "corrid");

    lua_setfield(this->_Stack, -2, "attr");

    int len= head->_Protocol._Length - sizeof(*head) + 4;
    if (len>0)
    {
        lua_pushlstring(this->_Stack, (const char*)head->_Content,len );
    }
    else
    {
        lua_pushnil(this->_Stack);
    }
}

Process::ProcessStatus::STATE Process::State() const
{
    return _Status._State;
}

int Process::Yield()
{
    //_ReDepth = 1;
    //lua_pushstring(_Stack, "SUSPEND");
    return lua_yield(_Stack, 0);
}

int Process::Debug(lua_State *l)
{
    luaL_loadfile(l, NULL);
    return 0;
}

/*void Process::Interrupt(int device)
{
Runtime::GetInstance().GetBus().Interrupt(device, _Stack);

}*/

int Process::Interrupt(lua_State *l)
{

    lua_getglobal(l, "__id__");
    LN_ID_TYPE id = luaL_checkinteger(l, -1);
    lua_pop(l,1);
    Process& n = Process::GetNodeById(id);
    n._Status._State = ProcessStatus::GLR_CALL;
    int dev = luaL_checkinteger(l, 1);
    int device;
    short timeout;
    device = (0xff000000&dev)>>24;
    timeout = 0x0000ffff&dev;
    timeout = timeout == 0?-1:timeout;
    if (device > MAX_DEV_NUMBER)  // TODO: remove this 
    {
        luaL_error(l, "wrong int no.");
    }

    //lua_pushinteger(l, n._Status._Tick);   //在栈顶上推入中断序列号（超时返回时候判断之用）
    Runtime::GetInstance().GetBus().Interrupt(device, n._Status._Tick, l);
    if (n._Status._State == ProcessStatus::INT_RESP)
    {
        return n._Status._NArg;
    }
    if (timeout != -1)
    {
        n.SetTimeOut(timeout);
    }

    printf("Yield\n");
    return n.Yield();
}
void Process::SendExitMsg()
{
    std::string buffer(sizeof(GLRPROTOCOL), 0);
    GLRPROTOCOL *pExitMsg = (GLRPROTOCOL *)&buffer[0];
    pExitMsg->_Protocol._Type = GLRPROTOCOL::EXIT;
    pExitMsg->_Route._ToGpid = _ParentId;
    pExitMsg->_Route._FromGpid = _Id;
    SendMsgToNode(_ParentId, buffer, GLRPROTOCOL::EXIT);
}
void Process::Destory(LN_ID_TYPE pid)
{
    Process *p = NodeMap[pid];
    if (p == NULL)
    {
        return;
    } else
    {
        if (p->_Status._State == Process::ProcessStatus::RECV_WAIT ||
            p->_Status._State == Process::ProcessStatus::INT_WAIT ||
            p->_Status._State == Process::ProcessStatus::STOPED ||
            p->_Status._State == Process::ProcessStatus::KILLED
            )
        {
            // Send Message To Parent Process
            try
            {
                p->SendExitMsg();
                NamedProcessTable::Clean(p->_Stack);
                //NamedProcessTable::UnRegister(p->_Stack);
            }
            catch (const Galaxy::GalaxyRT::CException &e)
            {
                GALA_ERROR(e.what());
            }



            Galaxy::GalaxyRT::CRWLockAdapter _WL(ProcessMapLock, Galaxy::GalaxyRT::CRWLockInterface::WRLOCK);
            Galaxy::GalaxyRT::CLockGuard _Gl(&_WL);
            {
                Galaxy::GalaxyRT::CLockGuard _GL1(& p->_IntLock); 
                NodeMap[pid] = NULL;
            }
            delete p;
            NodeCount--;
            if (NodeCount == 0)
            {
                //TODO: destroy objects in globals
                Runtime::GetInstance().ElegantExit();
            }

            //p->_Status._Killed = true;
        } else
        {
            p->_Status._Killed = true;
        }

    }

}

void Process::SendMsgToNode(LN_ID_TYPE pid, const std::string& msg, GLRPROTOCOL::MSG_TYPE /*type*/)
{

    try
    {
        Galaxy::GalaxyRT::CRWLockAdapter _RL(ProcessMapLock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
        Galaxy::GalaxyRT::CLockGuard _Gl(&_RL);

        GetNodeById(pid).SendMsg(msg);
    }
    catch (Galaxy::GalaxyRT::CException& e)
    {
        if (GetNodeExceptionHandle(pid))
        {
            SendMsgToNode(pid, msg);
        }
        GALA_DEBUG("Send To (%d) err:\n%s", pid, e.what());
    }


}

int Process::Status(lua_State *l)
{

    Galaxy::GalaxyRT::CRWLockAdapter _RL(ProcessMapLock,Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_RL);

    int pid = luaL_checkinteger(l, 1);

    try
    {
        Process& nd = GetNodeById(pid);

        lua_pushboolean(l, 1);
        lua_newtable(l);
        lua_pushinteger(l, nd._Status._State);
        lua_setfield(l, -2, "state");

        lua_pushinteger(l, nd._Status._Start);
        lua_setfield(l, -2, "start_stamp");

        lua_pushinteger(l, nd._Status._LastOp);
        lua_setfield(l, -2, "last_op_stamp");

        lua_pushinteger(l, nd._Status._Tick);
        lua_setfield(l, -2, "sche_tick");

        lua_pushinteger(l, nd._Channel.Size());
        lua_setfield(l, -2, "msg_num");
//XXX:thread unsafe
//        lua_getglobal(nd._Stack,"__info__");
//        if (lua_istable(nd._Stack,-1)){
//            move_table(nd._Stack,l,lua_gettop(nd._Stack));
//            lua_setfield(l,-2,"info");
//        }
//        else{
//            lua_pop(nd._Stack,1);
//        }
//
        
        return 2;
    }
    catch (Galaxy::GalaxyRT::CException& e)
    {
        lua_pushnil(l);
        lua_pushstring(l, e.what());
        return 2;
    }

}

int Process::AllProcesses(lua_State *l)
{
    Galaxy::GalaxyRT::CRWLockAdapter _RL(ProcessMapLock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_RL);
    lua_pushboolean(l, 1);
    lua_newtable(l);
    for (size_t i = 0; i != NodeMap.size(); ++i)
    {
        Process *p = NodeMap[i];
        if (p != NULL)
        {
            lua_pushinteger(l, p->_Id);
            lua_rawseti(l, -2, i);
        }

    }
    return 2;
}



int GLR::Process::GetNodeAddr(lua_State *l)
{

    lua_pushstring(l, Runtime::GetInstance().Host().c_str());
    lua_pushinteger(l, Runtime::GetInstance().NodeId());
    return 2;
}

int GLR::Process::GetFilePath(lua_State *l)
{
    lua_getglobal(l, "__id__");
    LN_ID_TYPE id = luaL_checkinteger(l, -1);
    Process& n = Process::GetNodeById(id);
    lua_pushstring(l, n._Path.c_str());
    return 1;
}

int GLR::Process::Kill(lua_State *l)
{
    try
    {
        int gpid = luaL_checkinteger(l, 1);
        Destory(gpid);
        return 0;
    }
    catch (const Galaxy::GalaxyRT::CException &e)
    {
        GALA_ERROR(e.what());
        return 0;
    }
}
void GLR::Process::EntryGar(const std::string& Gar, const std::string& module, const std::string& entry, ...)
{
    lua_getglobal(_Stack, "glr");

    lua_pushstring(_Stack, Gar.c_str());
    lua_setfield(_Stack,-2, "__gar__");

    lua_getfield(_Stack,-1,"run_gar");
    lua_pushstring(_Stack, Gar.c_str());
    if (lua_pcall(_Stack, 1, 1, 0) != 0)
    {
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        //StackDump();
        THROW_EXCEPTION_EX(msg);
    }
    Entry(module, entry);

}
void GLR::Process::Entry(const std::string& module, const std::string& entry, ...)
{
    lua_getglobal(_Stack, "require");
    lua_pushstring(_Stack, module.c_str());

    if (lua_pcall(_Stack, 1, 1, 0) != 0)
    {
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        StackDump();
        THROW_EXCEPTION_EX(msg);
    }
    //XXX:only those return "table" and "userdata" is valid lua module in GLR??
    if (!(lua_istable(_Stack, -1) || lua_isuserdata(_Stack, -1)))
    {
        THROW_EXCEPTION_EX(module + ": " + "not a valid lua module");
    }

    char tmp[1024] = { };
    std::string::const_iterator it = entry.begin(), it1 = entry.begin();
    do
    {
        it = std::find(it1, entry.end(), '.');
        memset(tmp, 0, sizeof(tmp));
        // for ":"
        if (it == entry.end())
        {
            it = std::find(it1, entry.end(), ':');
            if (it != entry.end())
            {
                std::copy(it1, it, tmp);
                lua_getfield(_Stack, -1, tmp);
                memset(tmp, 0, sizeof(tmp));
                std::copy(it+1, entry.end(), tmp);
                lua_getfield(_Stack,-1, tmp);

                lua_pushvalue(_Stack, -2);
                _Status._NArg += 1;

                it = entry.end();
                break;
            }

        }
        std::copy(it1, it, tmp);
        it1 = it + 1;
        lua_getfield(_Stack, -1, tmp);
        StackDump();
    }
    while (it != entry.end());

    if (!lua_isfunction(_Stack, -1) &&
            (!lua_isfunction(_Stack, -2) && _Status._NArg == 1))
    {
        const char *type = lua_typename(_Stack, lua_type(_Stack, -1));

        THROW_EXCEPTION_EX(module + ": can't call entry '" + entry + "'(a " + type + " value)");
    }
}

int GLR::Process::MessageAvailable(lua_State *l)
{
    lua_getglobal(l, "__id__");
    //lua_pushlstring("id");
    LN_ID_TYPE self_id = luaL_checkinteger(l, -1);
    Process& self = GetNodeById(self_id);
    lua_pushboolean(l, self._Channel.Empty() ? 0 : 1);
    return 1;
}

int GLR::Process::GLRStamp(lua_State *l)
{
    lua_pushinteger(l, Runtime::GetInstance().Stamp());
    return 1;
}

int GLR::Process::Exit(lua_State */*l*/)
{
    //TODO: �жϵ��÷���̺ţ�ֻ������̣�0�ţ�����exit
    //TODO: ʹ�������źţ����ſɿص��˳�����
    return kill(getpid(), SIGKILL);
}

GLR::LN_ID_TYPE GLR::Process::CreateNode(int id)
{

    Galaxy::GalaxyRT::CRWLockAdapter _WL(ProcessMapLock, Galaxy::GalaxyRT::CRWLockInterface::WRLOCK);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_WL);
    if (id != 0 && NodeMap[id] != NULL)
    {
        GALA_ERROR("Cannot Create Process(id:%d)", id);
        THROW_EXCEPTION_EX("Cannot Create Process");
    }
    else if (id == 0 && (size_t)NodeId >= NodeMap.size())
    {
        size_t current = NodeId % NodeMap.size();
        if (current == 0)
        {
            NodeId += RESERVED_PID+1;
            current += RESERVED_PID+1;
        }

        for (size_t i = current; i != NodeMap.size(); ++i)
        {
            GALA_DEBUG("ID: %d; Process: %p;", i, NodeMap[i]);
            if (NodeMap[i] == NULL)
            {
                id = i;
                break;
            }
        }
        if (id == 0)
        {
            for (size_t i = RESERVED_PID + 1; i != current; ++i) 
            {
                if (NodeMap[i] == NULL)
                {
                    id = i;
                    break;
                }
            }
        }



        if (id==0)
        {
            GALA_ERROR("Cannot Spawn Process: cannot create more then %zu processes", NodeMap.size());
            THROW_EXCEPTION_EX("Cannot Spawn Process");
        }


    }


    Process *node = new Process(id);
    printf("Node(%p:%d) Created!\n", node, node->_Id);
    //NodeMap.insert(std::make_pair(node->_Id, node));
    NodeMap[node->_Id] = node;
    NodeCount++;
    return node->_Id;
}

bool GLR::Process::GetNodeExceptionHandle(LN_ID_TYPE pid)
{
    (void)pid;
   // TODO: start service by need
   return false;

}

int GLR::Process::SpawnEx( lua_State *l )
{
    try
    {


        //int count = lua_gettop(l);
        // Get Arguments From Caller State
        int bindPid = lua_tointeger(l, 1);
        if (!(bindPid < RESERVED_PID && bindPid >= 1))
        {
            THROW_EXCEPTION_EX("Bind Gpid out of range");
        }

        size_t len = 0;
        const char *module = NULL;
        module = lua_tolstring(l, 2, &len);
        if (module == NULL)
        {
            THROW_EXCEPTION_EX("Extract Para Error");
        }


        const char *method = NULL;
        method = lua_tolstring(l, 3, &len);

        if (module == NULL)
        {
            THROW_EXCEPTION_EX("Extract Para Error");
        }




        // Create Lua Node
        LN_ID_TYPE node_id = CreateNode(bindPid);
        Process& node = GetNodeById(node_id);


        // set node parent id(for supervisor tree)
        lua_getglobal(l, "__id__");
        int parentId = luaL_checkint(l, -1);
        node._ParentId = parentId;
        lua_pop(l,1);


        if (GLR::Runtime::_GarFile.empty())
        {
            node.Entry(module, method);
        } else
        {
            node.EntryGar(GLR::Runtime::_GarFile, module, method);
        }


        // Set arguments for the new-created glr process
        SetArgumentsForSpawnedProcess(l, node, 4);

        // Return Value to Calling Lua Node
        lua_pushboolean(l, 1);
        lua_pushinteger(l, node_id);


        // put to schedule queue
        node.Start(Schedule::GetInstance());
        return 2;
        // Op on Self State
        //lua_getglobal(this->_Stack, buf);
    }
    catch (Galaxy::GalaxyRT::CException& e)
    {
        lua_pushnil(l);
        lua_pushstring(l, e.what());
        return 2;
    }
}

void GLR::Process::SetTimeOut(int timeout)
{
    Process &self = *this;
    while (lua_gettop(self._Stack) != 0)    
    {
        lua_pop(self._Stack,1);
    }
    lua_pushinteger(self._Stack, 0);
    lua_pushinteger(self._Stack, 2);
    lua_pushinteger(self._Stack, timeout);
    Runtime::GetInstance().GetBus().Interrupt(0, self._Status._Tick, self._Stack);
}







