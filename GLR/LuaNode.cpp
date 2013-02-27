#include "LuaNode.hpp"
#include "Schedule.hpp"
#include "GLR.hpp"
//defines of static data member
using namespace GLR;
std::tr1::unordered_map<int,Process*> Process::NodeMap;
Galaxy::GalaxyRT::CRWLock Process::Lock;
int32_t Process::NodeId;


Process::Process(void)
    :_Stack(lua_open()),
    _Id(NodeId++)
{
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

void Process::SendMsg( const LN_MSG_TYPE &msg )
{
    Galaxy::GalaxyRT::CLockGuard _gl(&_Lock);
    bool isEmpty = _Channel.Empty();


    if (isEmpty && (State() == ProcessStatus::RECV_WAIT))
    {
        lua_pushboolean(_Stack, 1);
        lua_pushlstring(_Stack, msg.c_str(), msg.size());
        _Status._NArg = 2;
        _Status._State = Process::ProcessStatus::RECV_RETURN;
        StackDump();
        Runtime::GetInstance().GetSchedule().PutTask(*this);

    }
    else
    {
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
    std::auto_ptr<Process> node(new Process());
    printf("Node(%p:%d) Created!\n", node.get(), node->_Id);
    Galaxy::GalaxyRT::CRWLockAdapter _RL(Lock, Galaxy::GalaxyRT::CRWLockInterface::WRLOCK);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_RL);
    NodeMap.insert(std::make_pair(node->_Id, node.get()));
    return node.release()->_Id;
}
int Process::Spawn( lua_State *l )
{
    if (l == NULL)
    {
        THROW_EXCEPTION_EX("Fatal Error");
    }
    // Get Arguments From Caller State
    size_t len = 0;
    const char *module = NULL;
    module = lua_tolstring(l,1,&len);
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
    Process &node = GetNodeById(node_id);
    node.LoadFile(module);
    //luaL_loadfile(node._Stack, module);
    // Register Node Entry
    //lua_getglobal(node._Stack, method);
    node.PushFun(method);
    node.StackDump();

    // put to schedule queue
    node.Start(Schedule::GetInstance());

    // Return Value to Calling Lua Node
    lua_pushboolean(l, 1);
    lua_pushinteger(l, node_id);
    return 2;

    // Op on Self State
    //lua_getglobal(this->_Stack, buf);

}

Process & Process::GetNodeById( LN_ID_TYPE id)
{

    Process *p = NodeMap[id];
    if (p == NULL)
    {
        THROW_EXCEPTION_EX("Process Not Exist");
    }
    return *p;


}

void Process::InitNode( void )
{
    luaL_openlibs(_Stack);

    // built in module glr
    static const  luaL_Reg glr_reg[] =
    {
        {"spawn", Spawn},
        {"send", SendMsgToNode},
        {"recv", Recieve},
        {"int", Interrupt},
        {"all", AllProcesses},
        {"status", Status},
        {"global", RegisterGlobal},
        {"get_global", GetGlobal},
        {NULL, NULL},
    };
    luaL_register(_Stack, "glr", glr_reg);
    lua_setglobal(_Stack, "glr");

    lua_getglobal(_Stack, "glr");
    lua_pushstring(_Stack,"id");
    lua_pushinteger(_Stack, _Id);
    lua_settable(_Stack, -3);

    // Some useful const
    lua_getglobal(_Stack, "glr");
    lua_pushstring(_Stack,"AF_UNIX");
    lua_pushinteger(_Stack, AF_UNIX);
    lua_settable(_Stack, -3);   

    lua_getglobal(_Stack, "glr");
    lua_pushstring(_Stack,"AF_INET");
    lua_pushinteger(_Stack, AF_INET);
    lua_settable(_Stack, -3);   




    //StackDump();
    //lua_pop(_Stack,1);


    // some global var
    lua_pushinteger(_Stack, _Id);
    lua_setglobal(_Stack, "__id__");

    // getnid



    StackDump();


    _Status._State = ProcessStatus::CREATED;
    // hook
    //lua_sethook(_Stack, LuaNode::Preempt, LUA_MASKCOUNT, 1000);
}

int Process::SendMsgToNode( lua_State *l )
{
    LN_ID_TYPE id = luaL_checkinteger(l, 1);
    size_t len = 0;
    const char *msg = luaL_checklstring(l, 2, &len);
    GetNodeById(id).SendMsg(LN_MSG_TYPE(msg, len));
    lua_pushboolean(l, 1);
    return 1;
}

void Process::Preempt( lua_State *l, lua_Debug *ar )
{

    lua_getinfo(l, "n", ar);
    printf("Line hook %d\n", ar->currentline);
    lua_yield(l, 0);
}

void Process::Resume()
{
    //int narg = 0;
    Galaxy::GalaxyRT::CLockGuard _Gl(&_IntLock);
    int rt = 0;
    /* if (_Status.State() == ProcessStatus::RUNNING)
    {
    THROW_EXCEPTION_EX("Fatal Erorr: Corrupt Process Schedule");
    }*/
    //printf("Resumed!\n");
    //printf("Node(%d) Go To Here\n", _Id);
    //Status();


    //lua_pushstring(_Stack, "Resume!");

    _Status._State = ProcessStatus::RUNNING;
    _Status._Tick++;
    rt = lua_resume(_Stack, _Status._NArg);
    _Status._NArg = 0;
    if (rt == LUA_YIELD)
    {
        if (State()!= ProcessStatus::RECV_WAIT && State() != ProcessStatus::INT_WAIT)
        {

            _Status._State = ProcessStatus::YIELD;
            return;
        }  
    }
    else if (rt == 0)
    {
        _Status._State = ProcessStatus::STOPED;
        return;
    }
    //lua_newthread()
    else
    {
        StackDump();
        printf("Resume Return (%d)\n", rt);
        THROW_EXCEPTION_EX("Error Occur While Node Running");

    }

}

int Process::Recieve( lua_State *l )
{

    lua_getglobal(l, "__id__");
    //lua_pushlstring("id");
    LN_ID_TYPE self_id = luaL_checkinteger(l, -1);
    Process &self = GetNodeById(self_id);
    {
        Galaxy::GalaxyRT::CLockGuard _gl(&self._Lock);
        if (self._Channel.Empty())
        {

            self._Status._State = ProcessStatus::RECV_WAIT;
            //lua_pushstring(l, "SUSPEND");
            //return lua_yield(self._Stack, 0);
            return self.Yield();
        }
    }
    LN_MSG_TYPE msg = self.RecvMsg();
    lua_pushboolean(l, 1);
    lua_pushlstring(self._Stack, msg.c_str(), msg.size());
    return 2;
}

void Process::DoString( const std::string &code )
{
    luaL_dostring(_Stack, code.c_str());
}

void Process::PushFun( const std::string &fname )
{
    lua_getglobal(_Stack,fname.c_str());
}

void Process::Status()
{
    printf("LuaNode Status:\n");
    for (typeof(NodeMap.begin()) it = NodeMap.begin(); it != NodeMap.end(); ++it)
    {
        printf("LuaNode [%d] of Object(%p) Status(%d)\n",it->first, it->second, it->second->State());
    }

}

void Process::Start( Schedule &sche)
{
    sche.PutTask(*this);   
}

void Process::LoadFile( const std::string &path )
{

    if ((luaL_loadfile(_Stack, path.c_str()) !=0) || (lua_pcall(_Stack, 0, 0, 0) != 0))
    {
        StackDump();
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        THROW_EXCEPTION_EX(msg);
    }
    else
    {
        printf("Load File(%s) Succeed\n", path.c_str());
    }
}

void Process::StackDump()
{
    int top = lua_gettop(_Stack);
    printf("Stack of Node(%p:%d):\n", this, _Id);
    for (int i = 1; i<=top; ++i)
    {
        int type = lua_type(_Stack, i);
        printf("    %d : ",i);
        switch (type)
        {
        case LUA_TSTRING:
            printf("'%s'", lua_tostring(_Stack, i));
            break;
        case LUA_TBOOLEAN:
            printf(lua_toboolean(_Stack, i)?"true":"false");
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

int Process::Debug( lua_State *l )
{
    luaL_loadfile(l, NULL);
    return 0;
}

void Process::Interrupt( int device)
{
    Runtime::GetInstance().GetBus().Interrupt(device, _Stack);

}

int Process::Interrupt( lua_State *l )
{

    lua_getglobal(l, "__id__");
    LN_ID_TYPE id = luaL_checkinteger(l, -1);
    Process &n = Process::GetNodeById(id);
    n._Status._State = ProcessStatus::GLR_CALL;
    int dev = luaL_checkinteger(l, 1);
    //Galaxy::GalaxyRT::CLockGuard _Gl(&n._Lock);
    Runtime::GetInstance().GetBus().Interrupt(dev, l);
    if (n._Status._State == ProcessStatus::INT_RESP)
    {
        return n._Status._NArg;
    }
    printf("Yield\n");
    return n.Yield();
}

void Process::Destory( LN_ID_TYPE pid)
{
    Galaxy::GalaxyRT::CRWLockAdapter _RL(Lock, Galaxy::GalaxyRT::CRWLockInterface::WRLOCK);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_RL);
    Process *p = NodeMap[pid];
    if (p == NULL)
    {
        return;
    }
    NodeMap[pid] = NULL;
    delete p;
}

void Process::SendMsgToNode( LN_ID_TYPE pid, const std::string &msg)
{
    Galaxy::GalaxyRT::CRWLockAdapter _RL(Lock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_RL);
    GetNodeById(pid).SendMsg(msg);
}

int Process::Status( lua_State *l )
{

    Galaxy::GalaxyRT::CRWLockAdapter _RL(Lock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_RL);

    int pid = luaL_checkinteger(l, 1);
    Process &nd = GetNodeById(pid);
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
    return 2;
}

int Process::AllProcesses( lua_State *l )
{
    Galaxy::GalaxyRT::CRWLockAdapter _RL(Lock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_RL);
    lua_pushboolean(l, 1);
    lua_newtable(l);
    int i = 0;
    for (std::tr1::unordered_map<int, Process*>::iterator it = NodeMap.begin(); it != NodeMap.end(); ++it, ++i)
    {
        lua_pushinteger(l, it->first);
        lua_rawseti(l, -2, i);
    }
    return 2;
}

GLR::Globals::Globals()
{

}

GLR::Globals::~Globals()
{

}

void GLR::Globals::Put( const std::string &id, void **data, const std::string &type)
{
    UserData ud;
    ud.Content = *data;
    ud.Name = type;
    _VarMap.insert(std::make_pair(id, ud));
}

const GLR::Globals::UserData & GLR::Globals::Get( const std::string &id )
{
    return _VarMap[id];
}

GLR::Globals GLR::Process::GlobalVars;

int GLR::Process::RegisterGlobal( lua_State *l )
{
    void *ud = lua_touserdata(l, 2);
    const char *id = luaL_checkstring(l, 1);
    const char *type = luaL_checkstring(l, 3);
    GlobalVars.Put(id, (void**)ud, type);
    lua_pushboolean(l, 1);
    return 1;
}

int GLR::Process::GetGlobal( lua_State *l )
{
    const char *id = luaL_checkstring(l, 1);
    const Globals::UserData &ud = GlobalVars.Get(id);
    void *p = lua_newuserdata(l, sizeof(ud.Content));
    memcpy(p, &ud.Content, sizeof(ud.Content));
    luaL_getmetatable(l,ud.Name.c_str());
    lua_setmetatable(l, -2);
    return 1;
}
