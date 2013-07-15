#include "LuaNode.hpp"
#include "Schedule.hpp"
#include "GLR.hpp"
#include "lualib.h"
#include "Process.hpp"
#include "resource.h"
#include <algorithm>
//defines of static data member
using namespace GLR;
std::vector<Process*> Process::NodeMap(10240); 
//std::vector<Process*>(10240);
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
    GALA_DEBUG("Send %d", _Id);
    GALA_DEBUG("This(%p) LuaState(%p)", this, _Stack);

    Galaxy::GalaxyRT::CLockGuard _gl(&_Lock);
    bool isEmpty = _Channel.Empty();


    if (isEmpty && (State() == ProcessStatus::RECV_WAIT))
    {
        Galaxy::GalaxyRT::CLockGuard _gl(&_IntLock);
        GALA_DEBUG("Push Direct!To %d %d", Id(), msg.size());
        GLR_BUS_HEAD *head = (GLR_BUS_HEAD*)&msg[0];
        GALA_DEBUG("Reintepret");
        lua_pushinteger(_Stack, head->Head.Type);
        lua_newtable(_Stack);
        lua_pushstring(_Stack, head->Source.Host);
        lua_setfield(_Stack, -2, "host");

        lua_pushinteger(_Stack, head->Source.Port);
        lua_setfield(_Stack, -2, "port");

        lua_pushinteger(_Stack, head->Source.Gpid);
        lua_setfield(_Stack, -2, "gpid");
        lua_pushlstring(_Stack, &msg[sizeof(*head)], msg.size() - sizeof(*head));
        _Status._NArg = 3;
        _Status._State = Process::ProcessStatus::RECV_RETURN;
        //StackDump();
        GALA_DEBUG("PutTask");
        Runtime::GetInstance().GetSchedule().PutTask(*this);
        GALA_DEBUG("Return");

    }
    else
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
    //std::auto_ptr<Process> node(new Process());
    Process *node = new Process();
    printf("Node(%p:%d) Created!\n", node, node->_Id);
    Galaxy::GalaxyRT::CRWLockAdapter _RL(Lock, Galaxy::GalaxyRT::CRWLockInterface::WRLOCK);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_RL);
    //NodeMap.insert(std::make_pair(node->_Id, node));
    NodeMap[node->_Id] = node;
    return node->_Id;
}
int Process::Spawn( lua_State *l )
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
        //node.LoadFile(module);
        // node.DoString(std::string(module, len));
        //lua_getglobal(node._Stack, "loadstring");
        //lua_pushlstring(node._Stack, module, len);
        if (GLR::Runtime::_GarFile.empty())
        {
            node.Entry(module, method);  
        }
        else
        {
            node.EntryGar(GLR::Runtime::_GarFile,module, method);  
        }

        /*
        lua_getglobal(node._Stack, "require");
        lua_pushstring(node._Stack, module);
        if (lua_pcall(node._Stack, 1,1,0) != 0)
        {
        const char *msg = luaL_checklstring(node._Stack, -1, NULL);
        THROW_EXCEPTION_EX(msg);
        }


        node.StackDump();

        lua_getfield(node._Stack, -1, method);
        node.StackDump();
        */
        //luaL_loadfile(node._Stack, module);
        // Register Node Entry
        //lua_getglobal(node._Stack, method);
        // node.PushFun(method);
        int i = 3;
        for ( ; i <= lua_gettop(l); ++i)
        {
            int type = lua_type(l,i);
            size_t len = 0;
            const char * str = NULL;
            switch (type)
            {
            case LUA_TSTRING:
                str = luaL_checklstring(l,i,&len);
                lua_pushlstring(node._Stack,str, len);
                break;
            case LUA_TBOOLEAN:
                len = lua_toboolean(l,i);
                lua_pushboolean(node._Stack, len);
                break;
            case LUA_TNUMBER:
                len = luaL_checkinteger(l, i);
                lua_pushinteger(node._Stack, len);
                break;
            default:
                i = 1024;                    // 跳出循环
                break;
            }
        }

        node._Status._NArg = (i-3);
        // put to schedule queue


        // Return Value to Calling Lua Node
        lua_pushboolean(l, 1);
        lua_pushinteger(l, node_id);

        node.Start(Schedule::GetInstance());
        return 2;
        // Op on Self State
        //lua_getglobal(this->_Stack, buf);
    }
    catch (Galaxy::GalaxyRT::CException &e)
    {
        lua_pushnil(l);
        lua_pushstring(l, e.what());
        return 2;
    }


}

Process & Process::GetNodeById( LN_ID_TYPE id)
{
    if (id >= NodeMap.size())
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
    Galaxy::GalaxyRT::CProcess *_pCProcess=GLR::Runtime::_pCProcess;
    if (_pCProcess==NULL)
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
    Galaxy::GalaxyRT::CProcess *_pCProcess=GLR::Runtime::_pCProcess;
    if (_pCProcess==NULL)
    {
        luaL_error(l, "pCProcess is null");
        return 0;
    }

    const char *str=luaL_checkstring(l,1);
    int i=_pCProcess->SetArgument(str);
    lua_pushnumber(l,i);
    return 1;
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
        {"node_addr", GetNodeAddr},
        {"get_path", GetFilePath},
        {"kill", Kill},
        {"set_options", SetOptions},
        {"get_option", GetOption},
        {"msg_available", MessageAvailable},
        {"glr_stamp", GLRStamp},
        {"exit", Exit},
        {NULL, NULL},
    };
    luaL_register(_Stack, "_glr", glr_reg);
    lua_setglobal(_Stack, "_glr");

    lua_getglobal(_Stack, "_glr");
    lua_pushstring(_Stack,"id");
    lua_pushinteger(_Stack, _Id);
    lua_settable(_Stack, -3);

    // Some useful const
    // lua_getglobal(_Stack, "glr");
    lua_pushstring(_Stack,"AF_UNIX");
    lua_pushinteger(_Stack, AF_UNIX);
    lua_settable(_Stack, -3);   

    // lua_getglobal(_Stack, "glr");
    lua_pushstring(_Stack,"AF_INET");
    lua_pushinteger(_Stack, AF_INET);
    lua_settable(_Stack, -3);   

    lua_pushstring(_Stack, "KILL");
    lua_pushinteger(_Stack, MSG_HEAD::KILL);
    lua_settable(_Stack, -3);   

    lua_pushstring(_Stack, "CLOSED");
    lua_pushinteger(_Stack, MSG_HEAD::CLOSED);
    lua_settable(_Stack, -3);   

    //StackDump();
    //lua_pop(_Stack,1);

    // some global var
    lua_pushinteger(_Stack, _Id);
    lua_setglobal(_Stack, "__id__");


    // getnid

    //GALA_DEBUG((const char*)glr_lua);
    if (luaL_loadbuffer(_Stack, (const char*)glr_lua, strlen((const char*)glr_lua), "glr")!= 0)
    {
        GALA_DEBUG("Load String Failure");
        StackDump();
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        THROW_EXCEPTION_EX(msg);
    }

    lua_pushstring(_Stack, "glr");
    //StackDump();


    if(lua_pcall(_Stack, 1, 1, 0) != 0)
    {
        GALA_DEBUG("Call String Failure");
        StackDump();
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        THROW_EXCEPTION_EX(msg);
    }


    StackDump();
    lua_setglobal(_Stack, "glr");
    if (GLR::Runtime::_Initializer != NULL)
    {
        GLR::Runtime::_Initializer(_Stack);    //user defined initialize hook
    }


    _Status._State = ProcessStatus::CREATED;
    // hook
    //lua_sethook(_Stack, LuaNode::Preempt, LUA_MASKCOUNT, 1000);
}

int Process::SendMsgToNode( lua_State *l )
{
    lua_getglobal(l, "__id__");
    //lua_pushlstring("id");
    LN_ID_TYPE self_id = luaL_checkinteger(l, -1);
    LN_ID_TYPE id = luaL_checkinteger(l, 1);
    size_t len = 0;
    const char *msg = luaL_checklstring(l, 2, &len);
    LN_MSG_TYPE  pack_msg(len+sizeof(GLR_BUS_HEAD), 0);
    GLR_BUS_HEAD *head = (GLR_BUS_HEAD *)&pack_msg[0];
    head->Head.Type = MSG_HEAD::APP;
    head->Head.GPid = id;
    head->Head.Len = len+sizeof(*head) - 4;
    head->Source.Gpid = self_id;
    memcpy((void*)&pack_msg[sizeof(*head)], msg, len);
    GetNodeById(id).SendMsg(pack_msg);
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
    int rt = 0;
    /* if (_Status.State() == ProcessStatus::RUNNING)
    {
    THROW_EXCEPTION_EX("Fatal Erorr: Corrupt Process Schedule");
    }*/
    //printf("Resumed!\n");
    //printf("Node(%d) Go To Here\n", _Id);
    //Status();
    Galaxy::GalaxyRT::CLockGuard _Gl(&_IntLock);
    assert(_Status._NArg < 10);
    //lua_pushstring(_Stack, "Resume!");
    _Status._State = ProcessStatus::RUNNING;
    _Status._Tick++;
    rt = lua_resume(_Stack, _Status._NArg);
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
        if (State()!= ProcessStatus::RECV_WAIT && State() != ProcessStatus::INT_WAIT)
        {

            _Status._State = ProcessStatus::YIELD;
            return;
        }  
    }
    else if (rt == 0)
    {
        _Status._State = ProcessStatus::STOPED;
        THROW_EXCEPTION_EX("Stopped!");
        return;
    }
    //lua_newthread()
    else
    {
        GALA_DEBUG("Resume Return (%d)\n", rt);
        lua_getglobal(_Stack, "debug");
        lua_getfield(_Stack, -1, "traceback");
        lua_pcall(_Stack, 0, 1, 0);      // this call should never fail
        const char *msg = luaL_checkstring(_Stack, -1);
        GALA_DEBUG(msg);
        THROW_EXCEPTION_EX(msg);

    }

}

int Process::Recieve( lua_State *l )
{
    Galaxy::GalaxyRT::CRWLockAdapter _RL(Lock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_RL);

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
            GALA_DEBUG("No Msg %d", self_id);
            return self.Yield();
        }
    }
    try
    {
        LN_MSG_TYPE msg = self.RecvMsg();
        GALA_DEBUG("Get %d Size(%d)", self_id, msg.size());

        GLR_BUS_HEAD *head = (GLR_BUS_HEAD*)&msg[0];
        lua_pushinteger(self._Stack, head->Head.Type);
        lua_newtable(self._Stack);
        lua_pushstring(self._Stack, head->Source.Host);
        lua_setfield(self._Stack, -2, "host");

        lua_pushinteger(self._Stack, head->Source.Port);
        lua_setfield(self._Stack, -2, "port");

        lua_pushinteger(self._Stack, head->Source.Gpid);
        lua_setfield(self._Stack, -2, "gpid");
        lua_pushlstring(self._Stack, &msg[sizeof(*head)], msg.size() - sizeof(*head));
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

void Process::DoString( const std::string &code )
{
    if ((luaL_loadstring(_Stack, code.c_str()) !=0) && (lua_pcall(_Stack, 0, 0, 0) != 0))
    {
        StackDump();
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        THROW_EXCEPTION_EX(msg);
    }
    else
    {
        _Path = "";
        printf("Load String Succeed\n");
    }
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
        printf("LuaNode [%d] of Object(%p) Status(%d)\n",(*it)->_Id, (*it), (*it)->State());
    }

}

void Process::Start( Schedule &sche)
{
    sche.PutTask(*this);   
}

void Process::LoadString(const std::string &path )
{

    if (luaL_loadstring(_Stack, path.c_str()) !=0)
    {
        StackDump();
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        THROW_EXCEPTION_EX(msg);
    }
    else
    {
        _Path = "";
        lua_pushstring(_Stack, "__main__");
        _Status._NArg = 1; // one argument
        printf("Load string Succeed\n");
    }

}
void Process::LoadFile( const std::string &path )
{

    if (luaL_loadfile(_Stack, path.c_str()) !=0)
    {
        StackDump();
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        THROW_EXCEPTION_EX(msg);
    }
    else
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
    }else
    {
        if (p->_Status._State == Process::ProcessStatus::RECV_WAIT ||
            p->_Status._State == Process::ProcessStatus::INT_WAIT 
            )
        {
            NodeMap[pid] = NULL;
            delete p;
            //p->_Status._Killed = true;
        }
        else
        {
            p->_Status._Killed = true;
        }


    }

}

void Process::SendMsgToNode( LN_ID_TYPE pid, const std::string &msg, MSG_HEAD::MSG_TYPE type)
{
    Galaxy::GalaxyRT::CRWLockAdapter _RL(Lock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_RL);
    try
    {
        /*       std::string pack_msg(msg.size() + sizeof(MSG_HEAD),0);
        MSG_HEAD *head = (MSG_HEAD*)&pack_msg[0];
        head->Type = type;
        head->GPid = -1;
        head->Len = msg.size();
        memcpy((void*)&pack_msg[sizeof(*head)], msg.c_str(), msg.size());*/
        GetNodeById(pid).SendMsg(msg);
    }
    catch (Galaxy::GalaxyRT::CException &e)
    {
        GALA_DEBUG("Send To (%d) err:\n%s", pid, e.what());
    }


}

int Process::Status( lua_State *l )
{

    Galaxy::GalaxyRT::CRWLockAdapter _RL(Lock,        Galaxy::GalaxyRT::CRWLockInterface::RDLOCK);
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

//All userdata pass into this function should have registered 
//it's metatable name to it's metatable with key "__my_name"
int GLR::Process::RegisterGlobal( lua_State *l )
{
    void *ud = lua_touserdata(l, 2);
    const char *id = luaL_checkstring(l, 1);

    //get the metatable name 
    if(lua_getmetatable(l,2)==0)
    {
        return luaL_error(l,"userdata do not has metatable  %s %d",__func__,__LINE__);
    }
    //!+ 这要求所操作类型具有"__my_name"属性
    lua_getfield(l,-1,"__my_name");

    const char *type = luaL_checkstring(l, -1);

    GlobalVars.Put(id, (void**)ud, type);
    lua_pushvalue(l, -1);
    return 0;
}

int GLR::Process::GetGlobal( lua_State *l )
{
    const char *id = luaL_checkstring(l, 1);
    const Globals::UserData &ud = GlobalVars.Get(id);

    if (ud.Content==NULL)
    {
        lua_pushnil(l);
        std::string err(id);
        err+=" not found";
        lua_pushstring(l,err.c_str());
        return 2;
    }
    else
    {
        void *p = lua_newuserdata(l, sizeof(ud.Content));
        memcpy(p, &ud.Content, sizeof(ud.Content));
        luaL_getmetatable(l,ud.Name.c_str());

        lua_pushvalue(l,-1);

        //clear __gc
        lua_pushnil(l);
        lua_setfield(l,-2,"__gc");

        lua_setmetatable(l, -3);
        lua_settop(l,-2);

        return 1;
    }
}

int GLR::Process::GetNodeAddr( lua_State *l )
{

    lua_pushstring(l, Runtime::GetInstance().Host().c_str());
    lua_pushinteger(l, Runtime::GetInstance().NodeId());
    return 2;
}

int GLR::Process::GetFilePath( lua_State *l )
{
    lua_getglobal(l, "__id__");
    LN_ID_TYPE id = luaL_checkinteger(l, -1);
    Process &n = Process::GetNodeById(id);
    lua_pushstring(l, n._Path.c_str());
    return 1;
}

int GLR::Process::Kill( lua_State *l )
{
    int gpid = luaL_checkinteger(l, 1);
    Destory(gpid);
    return 0;
}
void GLR::Process::EntryGar(const std::string &Gar,const std::string &module, const std::string &entry, ... )
{
    lua_getglobal(_Stack, "glr");
    lua_getfield(_Stack,-1,"run_gar");
    lua_pushstring(_Stack, Gar.c_str());
    if (lua_pcall(_Stack, 1, 1, 0) != 0)
    {
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        StackDump();
        THROW_EXCEPTION_EX(msg);
    }
    Entry(module,entry);

}
void GLR::Process::Entry( const std::string &module, const std::string &entry, ... )
{
    lua_getglobal(_Stack, "require");
    lua_pushstring(_Stack, module.c_str());

    if (lua_pcall(_Stack, 1, 1, 0) != 0)
    {
        const char *msg = luaL_checklstring(_Stack, -1, NULL);
        StackDump();
        THROW_EXCEPTION_EX(msg);
    }
    char tmp[1024] = {};
    std::string::const_iterator it = entry.begin(), it1 = entry.begin();
    do 
    {
        it = std::find(it1, entry.end(), '.');
        memset(tmp,0 , sizeof(tmp));
        std::copy(it1, it, tmp);
        it1 = it + 1;
        lua_getfield(_Stack, -1, tmp);
        StackDump();
    }while (it != entry.end());

}

int GLR::Process::MessageAvailable( lua_State *l )
{
    lua_getglobal(l, "__id__");
    //lua_pushlstring("id");
    LN_ID_TYPE self_id = luaL_checkinteger(l, -1);
    LN_ID_TYPE id = luaL_checkinteger(l, 1);
    Process &self = GetNodeById(self_id);
    lua_pushboolean(l, self._Channel.Empty()?0:1);
    return 1;
}

int GLR::Process::GLRStamp( lua_State *l )
{
    lua_pushinteger(l,Runtime::GetInstance().Stamp());
    return 1;
}

int GLR::Process::Exit( lua_State *l )
{
    //TODO: 判断调用方进程号，只有主进程（0号）方可exit
    //TODO: 使用其他信号，优雅可控的退出程序
    return kill(getpid(), SIGKILL);
}


