/* -*- C++ -*- */

/* Time-stamp: <2013-02-20 10:05:48 星期三 by matao> */

/**
* @file   SocketController.cpp
* @author Ma Tao <qingant@gmail.com>
* @date   Wed Feb 20 10:05:47 2013
*
* @bref
*
*/
#include "SocketController.hpp"
#include "GLR.hpp"
#include "LuaNode.hpp"
#include <algorithm>

GLR::SocketWorker::SocketWorker(LinkMap &lm, Galaxy::GalaxyRT::CRWLock &lock,
                                Galaxy::GalaxyRT::CPollSelector &poller)
                                :_LinkMap(lm), _Mutex(lock, Galaxy::GalaxyRT::CRWLockInterface::WRLOCK), _Poller(poller)

{ 

}

GLR::SocketWorker::~SocketWorker()
{

}

void * GLR::SocketWorker::Run( const Galaxy::GalaxyRT::CThread & )
{
    while (true)
    {
        Galaxy::GalaxyRT::CSelector::EV_PAIRS evs = _Poller.WaitEvent(5*1000);
        for (size_t i = 0; i!= evs.size(); ++i)
        {
            Galaxy::GalaxyRT::CSelector::EV_PAIR &ev = evs[i];
            //GALA_ERROR("Fd(%d) EVent(%d)", ev.first, ev.second);
            if (ev.second&Galaxy::GalaxyRT::EV_IN)
            {
                GALA_DEBUG("EV_IN");
                OnRecv(ev);
            }
            else if (ev.second&Galaxy::GalaxyRT::EV_OUT)
            {
                OnSend(ev);
            }
            else if (ev.second&Galaxy::GalaxyRT::EV_ERR ||
                ev.second&Galaxy::GalaxyRT::EV_HUP ||
                ev.second&Galaxy::GalaxyRT::EV_NVALID ||
                ev.second&Galaxy::GalaxyRT::EV_RDHUP
                )
            {
                OnErr(ev);
            }
            else
            {
                OnErr(ev);
            }
        }
    }
    return NULL;
}

void GLR::SocketWorker::Clean( const Galaxy::GalaxyRT::CThread & )
{

}

void GLR::SocketWorker::OnErr( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev )
{
    //GALA_ERROR("Fd(%d)", ev.first);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
    LinkStack *ls = _LinkMap[ev.first];
    if (ls == NULL)
    {
        return;
    }
    ls->OnErr(ev, _Poller);
    _Poller.Remove(ev.first);
    _LinkMap[ev.first] = NULL; 
    delete ls;
 
}

void GLR::SocketWorker::OnRecv( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev )
{
    int fd = ev.first;
    LinkStack *ls = _LinkMap[fd];
    assert(ls!=NULL);
    try
    {

        ls->OnRecv(ev, _Poller);
    }
    catch (Galaxy::GalaxyRT::CException &e)
    {
        GALA_ERROR(e.what());
        OnErr(ev);

    }
}

void GLR::SocketWorker::OnSend( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev )
{
    int fd = ev.first;
    LinkStack *ls = _LinkMap[fd];
    try
    {

        if (ls != NULL)
        {
            ls->OnSend(ev, _Poller);
        }
    }
    catch (Galaxy::GalaxyRT::CException &e)
    {

        OnErr(ev);
    }

}


GLR::SocketController::SocketController()
#if defined(HAVE_SUSE)
    :_LinkMap(10240, (LinkStack *)0),_Mutex(_RWLock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK),_Worker(_LinkMap, _RWLock, _Poller)
#else
    :_LinkMap(10240, NULL),_Mutex(_RWLock, Galaxy::GalaxyRT::CRWLockInterface::RDLOCK),_Worker(_LinkMap, _RWLock, _Poller)
#endif
{
    Galaxy::GalaxyRT::CThread *t = new Galaxy::GalaxyRT::CThread(_Worker, 1);
    t->Start();
}

GLR::SocketController::~SocketController()
{

}

void GLR::SocketController::Request( lua_State *l, int tick)
{

    //Process &n = Runtime::GetInstance().GetProcess(pid);
    int type = luaL_checkinteger(l,2);
    switch ((OPType)type)
    {
    case SC_SERVE:
        DoServe(l);
        break;
    case SC_CONN:
        DoConn(l);
        break;
    case SC_RECV:
        DoRecv(l, tick);
        break;
    case SC_ACCEPT:
        DoAccept(l, tick);
        break;
    case SC_SEND:
        DoSend(l);
        break;
    case SC_CLOSE:
        DoClose(l);
        break;
    case SC_SHUTDOWN:
        DoClose(l);
        break;
    case SC_READLINE:
        DoRecvLine(l, tick);
        break;
    case SC_POLL:
        DoPoll(l);
        break;
    case SC_ACCEPT_POLL:
        DoAcceptPoll(l);
        break;
        //default:
        //   break;
    }
}

void GLR::SocketController::DoServe( lua_State *l )
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    int domain = luaL_checkinteger(l, 3);
    if (domain == AF_UNIX)
    {
        const char *path = luaL_checkstring(l, 4);
        Galaxy::GalaxyRT::CUNSocketServer *s = new Galaxy::GalaxyRT::CUNSocketServer(path);
        _LinkMap[s->GetFD()] = new StreamServerStack(s, _LinkMap);
        LinkStack *ls = _LinkMap[s->GetFD()];

        //lua_pushinteger(l, s->GetFD());
        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TNUMBER, ls->GetDummyFd());
    }
    else
    {
        const char *path = luaL_checkstring(l, 4);
        int port = luaL_checkinteger(l, 5);
        Galaxy::GalaxyRT::CTCPSocketServer *s = new Galaxy::GalaxyRT::CTCPSocketServer(path, port);
        _LinkMap[s->GetFD()] = new StreamServerStack(s, _LinkMap);
        LinkStack *ls = _LinkMap[s->GetFD()];
        //lua_pushinteger(l, s->GetFD());
        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TNUMBER, ls->GetDummyFd());
    }
}

void GLR::SocketController::DoConn( lua_State *l )
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    int  domain = luaL_checkinteger(l, 3);
    if (domain == AF_UNIX)
    {
        const char *path = luaL_checkstring(l,4);
        Galaxy::GalaxyRT::CUNSocketClient *c = new Galaxy::GalaxyRT::CUNSocketClient(path);
        LinkStack *ls =  new StreamLinkStack(c);
        _LinkMap[c->GetFD()] = ls;
        //lua_pushinteger(l, c->GetFD());
        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TNUMBER, ls->GetDummyFd());
    }
    else
    {
        int dev = luaL_checkinteger(l, 1);
        short timeout;
        timeout = 0x0000ffff&dev;
        timeout = timeout == 0?-1:timeout;
        const char *host = luaL_checkstring(l, 4);
        int port = luaL_checkinteger(l, 5);
        Galaxy::GalaxyRT::CTCPSocketClient *c = new Galaxy::GalaxyRT::CTCPSocketClient(host, port,10, timeout);
        Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex); 
        LinkStack *ls =  new StreamLinkStack(c);
        _LinkMap[c->GetFD()] = ls;

        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TNUMBER, ls->GetDummyFd());
        //Runtime::GetInstance().GetBus().Response(pid,1);
    }
}

//************************************
// Method:    DoRecv
// FullName:  GLR::SocketController::DoRecv
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: lua_State * l
// lua_State Stack: [INTID][COMMAND][FD][LEN][TIMEOUT(optional)][Tick]
//************************************
void GLR::SocketController::DoRecv( lua_State *l , int tick)
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    int fd = luaL_checkinteger(l,3);
    int len = luaL_checkinteger(l, 4);
    GALA_DEBUG("RECV %d %d", fd,  len);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
    LinkStack *ls = _LinkMap[LinkStack::GetRealFd(fd)];

    if (ls == NULL||(!ls->CheckDummyFd(fd)))
    {
        std::string errmsg = "Invalid Fd";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }
    if (ls->FastRecvReturn(pid, LinkStack::RECV, len))
    {
        return;
    }

    Runtime::GetInstance().GetBus().IntSuspend(pid);


    ls->PutRecvTask(pid, len, tick);
    _Poller.Register(LinkStack::GetRealFd(fd), Galaxy::GalaxyRT::EV_IN);
}

void GLR::SocketController::DoSend( lua_State *l)
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    int fd = luaL_checkinteger(l,3);
    const char *buf = NULL;
    size_t len = 0;
    buf = luaL_checklstring(l, 4, &len);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
    LinkStack *ls = _LinkMap[LinkStack::GetRealFd(fd)];
    if (ls == NULL||(!ls->CheckDummyFd(fd)))
    {
        std::string errmsg = "Invalid Fd";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }
    if (len == 0)
    {
        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TBOOLEAN, 1);
        return;
    }

    Runtime::GetInstance().GetBus().IntSuspend(pid);
    ls->PutSendTask(pid, std::string(buf, len));
    _Poller.Register(LinkStack::GetRealFd(fd), Galaxy::GalaxyRT::EV_OUT);
}

void GLR::SocketController::DoAccept( lua_State *l , int tick)
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    int fd = luaL_checkinteger(l,3);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
    int realFd = LinkStack::GetRealFd(fd);
    LinkStack *ls = _LinkMap[realFd];
    if (ls == NULL||(!ls->CheckDummyFd(fd)))
    {
        std::string errmsg = "Invalid Fd";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }
    // TODO: on-fly-fd

    Runtime::GetInstance().GetBus().IntSuspend(pid);
    ls->PutAcceptTask(pid, tick);
    _Poller.Register(realFd, Galaxy::GalaxyRT::EV_IN);
}

void GLR::SocketController::DoClose( lua_State *l )
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    int fd = luaL_checkinteger(l,3);
    int realFd = LinkStack::GetRealFd(fd);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
    LinkStack *ls = _LinkMap[realFd];
    if (ls == NULL||(!ls->CheckDummyFd(fd)))
    {
        std::string errmsg = "already closed";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }

    ls->_Sock->Shutdown(SHUT_RDWR);
    _Poller.Register(realFd, Galaxy::GalaxyRT::EV_ERR);
    Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TBOOLEAN, 1);
}

void GLR::SocketController::DoRecvLine( lua_State* l, int tick)
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    int fd = luaL_checkinteger(l,3);
    GALA_DEBUG("RECV %d ", fd);
    int realFd = LinkStack::GetRealFd(fd);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
    LinkStack *ls = _LinkMap[realFd];
    if (ls == NULL||(!ls->CheckDummyFd(fd)))
    {
        std::string errmsg = "Invalid Fd";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }
    if (ls->FastRecvReturn(pid, LinkStack::RECV_LINE, 0))
    {
        return;
    }

    Runtime::GetInstance().GetBus().IntSuspend(pid);


    ls->PutRecvLineTask(pid, tick);
    _Poller.Register(realFd, Galaxy::GalaxyRT::EV_IN);
}

void GLR::SocketController::DoPoll(lua_State *l)
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);

    int fd = luaL_checkinteger(l, 3);
    int realFd = LinkStack::GetRealFd(fd);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
    (void)_Gl;
    LinkStack *ls = _LinkMap[realFd];
    if (ls == NULL||(!ls->CheckDummyFd(fd)))
    {
        std::string errmsg = "Invalid Fd";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }
    ls->PutPollTask(pid);
    _Poller.Register(realFd, Galaxy::GalaxyRT::EV_IN);
    Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TBOOLEAN, 1);
}

void GLR::SocketController::DoAcceptPoll(lua_State *l)
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);

    int fd = luaL_checkinteger(l, 3);
    int realFd = LinkStack::GetRealFd(fd);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
    (void)_Gl;
    LinkStack *ls = _LinkMap[realFd];
    if (ls == NULL||(!ls->CheckDummyFd(fd)))
    {
        std::string errmsg = "Invalid Fd";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }
    ls->PutAcceptPollTask(pid);
    _Poller.Register(realFd, Galaxy::GalaxyRT::EV_IN);
    Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TBOOLEAN, 1);
}







GLR::LinkStack::LinkStack( Galaxy::GalaxyRT::CSocket *sock )
    :_Sock(sock),_Stamp(CRT_time(NULL)& 0xffff)
{
    
}

GLR::LinkStack::~LinkStack()
{

}

//void GLR::StreamLinkStack::PutAcceptTask( int pid )
//{
//    Task t;
//    t.Type = ACCEPT;
//    t.Pid = pid;
//    _RecvTasks.Put(t);
//}
//
void GLR::StreamLinkStack::PutSendTask( int pid, const std::string & buf)
{
    Task t;
    t.Type = SEND;
    t.Pid = pid;
    t.Current = 0;
    t.Buffer = buf;
    _SendTasks.Put(t);
}
void GLR::StreamLinkStack::PutRecvTask( int pid, size_t len, int tick )
{
    Task t;
    t.Type = RECV;
    t.Pid = pid;
    t.RecvArg.Len = len;
    t.RecvArg.Tick = tick;
    //t.Buffer.resize(len);
    //t.Current = 0;
    _RecvTasks.Put(t);
}

GLR::StreamLinkStack::StreamLinkStack( Galaxy::GalaxyRT::CSocket *sock )
    :LinkStack(sock),
    _Cache(GLR_PAGE_SIZE, GLR_STEP_SIZE),
    _Hanguped(false)
{

}

GLR::StreamLinkStack::~StreamLinkStack()
{

}

void GLR::StreamLinkStack::OnErr( Galaxy::GalaxyRT::CSelector::EV_PAIR &/*ev*/, POLLERTYPE &/*_Poller*/ )
{
    StreamLinkStack *ls = this;
    while (!ls->_RecvTasks.Empty())
    {
        Task t = ls->_RecvTasks.Get();
        std::string errmsg = "IO Error";
        if (t.Type == RECV)
        {
            Runtime::GetInstance().GetBus().ResponseEx(t.Pid,t.RecvArg.Tick, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        }
        else if (t.Type == POLL)
        {
            GLRPROTOCOL msg;
            memset((void*)&msg, 0, sizeof(msg));
            msg._Route._FromGpid = _Sock->GetFD();
            msg._Protocol._Type = GLRPROTOCOL::IOCP;
            msg._Host._V3._Port = SocketController::INT_NO;
            msg._Protocol._Length = sizeof(GLRPROTOCOL) - 4;
            Runtime::GetInstance().GetBus().Send(t.Pid, std::string((const char*)&msg, sizeof(msg)), GLRPROTOCOL::IOCP);
        }
    }
    while (!ls->_SendTasks.Empty())
    {
        Task t = ls->_SendTasks.Get();
        std::string errmsg = "IO Error";
        Runtime::GetInstance().GetBus().Response(t.Pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
    }
}

void GLR::StreamLinkStack::Response(POLLERTYPE &_Poller )
{

    while (!_RecvTasks.Empty())
    {
        Task &t = _RecvTasks.Head();

        // filter canceled request
        if (t.Type != POLL && Runtime::GetInstance().GetBus().IsCanceled(t.Pid, t.RecvArg.Tick))
        {
            _RecvTasks.Get();
            continue;
        }

        // deal
        if (t.Type == RECV)
        {
            if (_Cache.DataSize() >= t.RecvArg.Len)
            {
                bool flag = Runtime::GetInstance().GetBus().ResponseEx(t.Pid, t.RecvArg.Tick, 1, LUA_TSTRING, _Cache.Get(), t.RecvArg.Len);

                _RecvTasks.Get();
                if (flag)
                {
                    _Cache.Eat(t.RecvArg.Len);
                }else
                {
                    continue;
                }
            }else if (this->_Hanguped)
            {
                THROW_EXCEPTION_EX("Connection Peer Closed");
            }else
            {
                break;
            }




        } 
        else if (t.Type == RECV_LINE)
        {
            ssize_t lineCursor = _Cache.GetLine();
            if (lineCursor != -1)
            {
                bool flag = Runtime::GetInstance().GetBus().ResponseEx(t.Pid, t.RecvArg.Tick, 1, LUA_TSTRING, _Cache.Get(), lineCursor);


                _RecvTasks.Get();
                if (flag)
                {
                    _Cache.Eat(lineCursor);
                }else
                {
                    continue;
                }
            }
            else if (this->_Hanguped)
            {
                THROW_EXCEPTION_EX("Connection Peer Closed");
            }
            else
            {
                break;
            }


        }
        else if (t.Type == POLL)
        {
            if (_Cache.DataSize() != 0)
            {
                GLRPROTOCOL msg;
                memset((void*)&msg, 0, sizeof(msg));
                msg._Route._FromGpid = _Sock->GetFD();
                msg._Protocol._Type = GLRPROTOCOL::IOCP;
                msg._Host._V3._Port = SocketController::INT_NO;
                msg._Protocol._Length = sizeof(GLRPROTOCOL) - 4;
                Runtime::GetInstance().GetBus().Send(t.Pid, std::string((const char*)&msg, sizeof(msg)), GLRPROTOCOL::IOCP);

                _RecvTasks.Get();
            }
        }

    }

    if (_RecvTasks.Empty())
    {
        _Poller.Remove(this->_Sock->GetFD(), Galaxy::GalaxyRT::EV_IN);
    }


}
void GLR::StreamLinkStack::OnRecv( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev, POLLERTYPE &_Poller )
{
    Galaxy::GalaxyRT::CLockGuard _(&_Lock);
    
    (void)ev,(void)_;

    //int fd = ev.first;
    if (this->_Hanguped)
    {
        THROW_EXCEPTION_EX("Peer Closed");
    }

    _Sock->SetNonBlocking();
    while (true)
    {
        try
        {
            size_t len = _Sock->Recv(_Cache.Cursor(), _Cache.Reserved()); 
            _Cache.Grow(len);
            GALA_DEBUG("DataSize:%ld Size:%ld", _Cache.DataSize(), len);
        } catch (const Galaxy::GalaxyRT::CSockWouldBlock &e)
        {
            break;
        } catch (const Galaxy::GalaxyRT::CSockPeerClose &e){
            this->_Hanguped = true;
            if (_Cache.DataSize() == 0)
            {
                throw e;
            }
            else
            {
                break;
            }
        }
    }
    Response(_Poller);
    _Sock->SetBlocking();

}

void GLR::StreamLinkStack::OnSend( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev , POLLERTYPE &_Poller)
{

    _Sock->SetNonBlocking();
    int fd = ev.first;
    if (_SendTasks.Empty())
    {
        _Poller.Remove(fd, Galaxy::GalaxyRT::EV_OUT);
        return;
    }

    Task &t = _SendTasks.Head();
    assert(!_SendTasks.Empty());
    assert(t.Buffer.size() != 0);
    size_t len = _Sock->Send(&t.Buffer[t.Current], t.Buffer.size() - t.Current);
    t.Current += len;
    if (t.Current == t.Buffer.size())
    {
        int pid = t.Pid;

        _SendTasks.Get();
        if (_SendTasks.Empty())
        {
            GALA_DEBUG("Remove");
            _Poller.Remove(fd, Galaxy::GalaxyRT::EV_OUT);
        }


        Runtime::GetInstance().GetBus().Response(pid, 1, LUA_TBOOLEAN, 1);
    }
}

void GLR::StreamLinkStack::PutRecvLineTask( int pid, int tick)
{
    Task t;
    t.Type = RECV_LINE;
    t.Pid = pid;
    t.RecvArg.Len = 0;
    t.RecvArg.Tick = tick;
    //t.Buffer.resize(len);
    //t.Current = 0;
    _RecvTasks.Put(t);
}

void GLR::StreamLinkStack::PutPollTask(int pid)
{
    if (_Cache.DataSize() != 0)
    {
        GALA_DEBUG("Polling...: %d", pid);
        GLRPROTOCOL msg;
        memset((void*)&msg, 0, sizeof(msg));
        msg._Route._FromGpid = _Sock->GetFD();
        msg._Protocol._Type = GLRPROTOCOL::IOCP;
        msg._Host._V3._Port = SocketController::INT_NO;
        msg._Protocol._Length = sizeof(GLRPROTOCOL) - 4;
        Runtime::GetInstance().GetBus().Send(pid, std::string((const char*)&msg, sizeof(msg)), GLRPROTOCOL::IOCP);
        return;
    }
    Task t;
    t.Type = POLL;
    t.Pid = pid;
    _RecvTasks.Put(t);
}

bool GLR::StreamLinkStack::FastRecvReturn(int pid, TaskType taskType, size_t len)
{
    Galaxy::GalaxyRT::CLockGuard _(&_Lock);
    (void)_;
    if (!_RecvTasks.Empty())
    {
        return false;
    }
    Galaxy::GalaxyRT::CQEmptyGuard<TaskQueue> _GL(_RecvTasks);
    if (taskType == LinkStack::RECV)
    {
        if (_Cache.DataSize() >= len)
        {
            Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TSTRING, _Cache.Get(), len);
            _Cache.Eat(len);
            return true;
        }

    }else if (taskType == LinkStack::RECV_LINE)
    {
        ssize_t lineCursor = _Cache.GetLine();
        if (lineCursor != -1)
        {
            Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TSTRING, _Cache.Get(), lineCursor);
            _Cache.Eat(lineCursor);
            return true;
        }

    }
    return false;



}





GLR::StreamServerStack::StreamServerStack( Galaxy::GalaxyRT::CSocket *sock, LinkMap &lm)
    :LinkStack(sock),_LinkMap(lm),_OnFlyFd(-1)
{

}

GLR::StreamServerStack::~StreamServerStack()
{

}

void GLR::StreamServerStack::PutAcceptTask( int pid , int tick)
{
    StreamLinkStack::Task t;
    t.Type = StreamLinkStack::ACCEPT;
    t.Pid = pid;
    t.RecvArg.Tick = tick;
    _RecvTasks.Put(t);
}

void GLR::StreamServerStack::PutAcceptPollTask(int pid)
{
    Task t;
    t.Type = ACCEPT_POLL;
    t.Pid = pid;
    _RecvTasks.Put(t);
}

void GLR::StreamServerStack::OnErr( Galaxy::GalaxyRT::CSelector::EV_PAIR &/*ev*/, POLLERTYPE &/*_Poller*/ )
{
    StreamServerStack *ls = this;
    while (!ls->_RecvTasks.Empty())
    {
        StreamLinkStack::Task t = ls->_RecvTasks.Get();
        std::string errmsg = "IO Error";
        Runtime::GetInstance().GetBus().Response(t.Pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
    }
}

void GLR::StreamServerStack::OnRecv( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev, POLLERTYPE &_Poller)
{
    (void)ev;

    while (!_RecvTasks.Empty())
    {
        StreamLinkStack::Task &t = _RecvTasks.Head();
        if (t.Type != ACCEPT_POLL && Runtime::GetInstance().GetBus().IsCanceled(t.Pid, t.RecvArg.Tick))
        {
            _RecvTasks.Get();
            continue;
        }

        int recvfd = -1;
        if (_OnFlyFd != -1)
        {
            recvfd = _OnFlyFd;
            _OnFlyFd = -1;
        }else
        {

            _Sock->SetNonBlocking();
            try
            {

                recvfd = _Sock->Accept(NULL, NULL);
                _Sock->SetBlocking();
            }
            catch (const Galaxy::GalaxyRT::CSockWouldBlock &e)
            {
                _Sock->SetBlocking();
                break;
            }
            // TODO: if recvfd > sizeof(_LinkMap)
            _LinkMap[recvfd] = new StreamLinkStack(new Galaxy::GalaxyRT::CSocket(recvfd));
            _Poller.Register(recvfd, Galaxy::GalaxyRT::EV_ERR);
        }

        LinkStack *ls = _LinkMap[recvfd];
        if (t.Type == ACCEPT_POLL)
        {
            std::string buffer(sizeof(GLRPROTOCOL) + 5, 0);

            GLRPROTOCOL &msg = *(GLRPROTOCOL*)&buffer[0];
            msg._Route._FromGpid = _Sock->GetFD();
            msg._Host._V3._Port = SocketController::INT_NO;
            msg._Protocol._Type = GLRPROTOCOL::IOCP;
            msg._Protocol._Length = sizeof(GLRPROTOCOL) + 5 - sizeof(uint32_t);

            //msgpack.pack
            buffer[sizeof(GLRPROTOCOL)] = 0xce;
            *(uint32_t*)&buffer[sizeof(GLRPROTOCOL) + 1] = htonl(ls->GetDummyFd());

            Runtime::GetInstance().GetBus().Send(t.Pid, buffer, GLRPROTOCOL::IOCP);
            _RecvTasks.Get();
            continue;
        }
        else if (Runtime::GetInstance().GetBus().ResponseEx(t.Pid, t.RecvArg.Tick, 1, LUA_TNUMBER, ls->GetDummyFd()))
        {
            _RecvTasks.Get();
        }else
        {
            _OnFlyFd = recvfd;
        }
    }
    if (_RecvTasks.Empty())
    {
        _Poller.Remove(this->_Sock->GetFD(), Galaxy::GalaxyRT::EV_IN);
    }


}

void GLR::StreamServerStack::OnSend( Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE& )
{
    THROW_EXCEPTION_EX("Not Supported");
}

