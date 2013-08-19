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

GLR::SocketWorker::SocketWorker(LinkMap &lm, Galaxy::GalaxyRT::CPthreadMutex &lock,
                                Galaxy::GalaxyRT::CPollSelector &poller)
                                :_LinkMap(lm), _Mutex(lock), _Poller(poller)

{

}

GLR::SocketWorker::~SocketWorker()
{

}

void * GLR::SocketWorker::Run( const Galaxy::GalaxyRT::CThread & )
{
    while (true)
    {
        Galaxy::GalaxyRT::CSelector::EV_PAIRS evs = _Poller.WaitEvent(-1);
        for (size_t i = 0; i!= evs.size(); ++i)
        {
            Galaxy::GalaxyRT::CSelector::EV_PAIR &ev = evs[i];
            GALA_ERROR("Fd(%d) EVent(%d)", ev.first, ev.second);
            if (ev.second&Galaxy::GalaxyRT::EV_ERR ||
                ev.second&Galaxy::GalaxyRT::EV_HUP ||
                ev.second&Galaxy::GalaxyRT::EV_NVALID ||
                ev.second&Galaxy::GalaxyRT::EV_RDHUP
                )
            {
                OnErr(ev);
            }
            else if (ev.second&Galaxy::GalaxyRT::EV_IN)
            {
                GALA_DEBUG("EV_IN");
                OnRecv(ev);
            }
            else if (ev.second&Galaxy::GalaxyRT::EV_OUT)
            {
                OnSend(ev);
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
    GALA_ERROR("Fd(%d)", ev.first);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
    LinkStack *ls = _LinkMap[ev.first];
    if (ls == NULL)
    {
        return;
    }
    ls->OnErr(ev, _Poller);
    _Poller.Remove(ev.first);
    delete ls;
    _LinkMap[ev.first] = NULL;
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
        GALA_DEBUG(e.what());
        OnErr(ev);

    }
}

void GLR::SocketWorker::OnSend( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev )
{
    int fd = ev.first;
    LinkStack *ls = _LinkMap[fd];
    try
    {

        ls->OnSend(ev, _Poller);
    }
    catch (Galaxy::GalaxyRT::CException &e)
    {

        OnErr(ev);
    }

}


GLR::SocketController::SocketController()
#if defined(HAVE_SUSE)
    :_LinkMap(10240, (LinkStack *)0),_Worker(_LinkMap, _Mutex, _Poller)
#else
    :_LinkMap(10240, NULL),_Worker(_LinkMap, _Mutex, _Poller)
#endif
{
    Galaxy::GalaxyRT::CThread *t = new Galaxy::GalaxyRT::CThread(_Worker, 1);
    t->Start();
}

GLR::SocketController::~SocketController()
{

}

void GLR::SocketController::Request( lua_State *l)
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
        DoRecv(l);
        break;
    case SC_ACCEPT:
        DoAccept(l);
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

        //lua_pushinteger(l, s->GetFD());
        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TNUMBER, s->GetFD());
    }
    else
    {
        const char *path = luaL_checkstring(l, 4);
        int port = luaL_checkinteger(l, 5);
        Galaxy::GalaxyRT::CTCPSocketServer *s = new Galaxy::GalaxyRT::CTCPSocketServer(path, port);
        _LinkMap[s->GetFD()] = new StreamServerStack(s, _LinkMap);
        //lua_pushinteger(l, s->GetFD());
        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TNUMBER, s->GetFD());
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
        _LinkMap[c->GetFD()] = new StreamLinkStack(c);
        //lua_pushinteger(l, c->GetFD());
        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TNUMBER, c->GetFD());
    }
    else
    {
        const char *host = luaL_checkstring(l, 4);
        int port = luaL_checkinteger(l, 5);
        Galaxy::GalaxyRT::CTCPSocketClient *c = new Galaxy::GalaxyRT::CTCPSocketClient(host, port);
        _LinkMap[c->GetFD()] = new StreamLinkStack(c);

        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TNUMBER, c->GetFD());
        //Runtime::GetInstance().GetBus().Response(pid,1);
    }
}

void GLR::SocketController::DoRecv( lua_State *l )
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    int fd = luaL_checkinteger(l,3);
    int len = luaL_checkinteger(l, 4);
    GALA_DEBUG("RECV %d %d", fd,  len);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
    LinkStack *ls = _LinkMap[fd];
    if (ls == NULL)
    {
        std::string errmsg = "Invalid Fd";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }

    Runtime::GetInstance().GetBus().IntSuspend(pid);


    ls->PutRecvTask(pid, len);
    _Poller.Register(fd, Galaxy::GalaxyRT::EV_IN);
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
    LinkStack *ls = _LinkMap[fd];
    if (ls == NULL)
    {
        std::string errmsg = "Invalid Fd";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }

    Runtime::GetInstance().GetBus().IntSuspend(pid);
    ls->PutSendTask(pid, std::string(buf, len));
    _Poller.Register(fd, Galaxy::GalaxyRT::EV_OUT);
}

void GLR::SocketController::DoAccept( lua_State *l )
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    int fd = luaL_checkinteger(l,3);
    Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
    LinkStack *ls = _LinkMap[fd];
    if (ls == NULL)
    {
        std::string errmsg = "Invalid Fd";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }
    ls->PutAcceptTask(pid);
    _Poller.Register(fd, Galaxy::GalaxyRT::EV_IN);
    Runtime::GetInstance().GetBus().IntSuspend(pid);
}

void GLR::SocketController::DoClose( lua_State *l )
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    int fd = luaL_checkinteger(l,3);

    LinkStack *ls = _LinkMap[fd];
    if (ls == NULL)
    {
        std::string errmsg = "already closed";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }

    ls->_Sock->Shutdown(SHUT_RDWR);
    _Poller.Register(fd, Galaxy::GalaxyRT::EV_IN);
    Runtime::GetInstance().GetBus().IntSuspend(pid);
}






GLR::LinkStack::LinkStack( Galaxy::GalaxyRT::CSocket *sock )
    :_Sock(sock)
{

}

GLR::LinkStack::~LinkStack()
{

}

void GLR::StreamLinkStack::PutAcceptTask( int pid )
{
    Task t;
    t.Type = ACCEPT;
    t.Pid = pid;
    _RecvTasks.Put(t);
}

void GLR::StreamLinkStack::PutSendTask( int pid, const std::string & buf)
{
    Task t;
    t.Type = SEND;
    t.Pid = pid;
    t.Current = 0;
    t.Buffer = buf;

    _SendTasks.Put(t);
}
void GLR::StreamLinkStack::PutRecvTask( int pid, size_t len )
{
    Task t;
    t.Type = RECV;
    t.Pid = pid;
    t.RecvArg.Len = len;
    t.Buffer.resize(len);
    t.Current = 0;
    _RecvTasks.Put(t);
}

GLR::StreamLinkStack::StreamLinkStack( Galaxy::GalaxyRT::CSocket *sock )
    :LinkStack(sock)
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
        Runtime::GetInstance().GetBus().Response(t.Pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
    }
    while (!ls->_SendTasks.Empty())
    {
        Task t = ls->_SendTasks.Get();
        std::string errmsg = "IO Error";
        Runtime::GetInstance().GetBus().Response(t.Pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
    }
}

void GLR::StreamLinkStack::OnRecv( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev, POLLERTYPE &_Poller )
{
    //_Sock->SetNonBlocking();
    GALA_DEBUG("Recv");
    int fd = ev.first;
    Task &t = _RecvTasks.Head();
    if (t.Type == StreamLinkStack::RECV)
    {
        assert(t.RecvArg.Len - t.Current!=0);
        size_t len = _Sock->Recv(&t.Buffer[t.Current],
            t.RecvArg.Len - t.Current);

        t.Current += len;

        assert(t.Current <= t.RecvArg.Len);
        if (t.Current == t.RecvArg.Len)
        {
            int pid = t.Pid;
            //Process &n = Runtime::GetInstance().GetProcess(pid);
            //lua_pushlstring(n.Stack(), t.Buffer.c_str(), t.RecvArg.Len);
            _Poller.Remove(fd, Galaxy::GalaxyRT::EV_IN);
            GALA_DEBUG("Removed! %d", fd);
            Runtime::GetInstance().GetBus().Response(pid,1, LUA_TSTRING, t.Buffer.c_str(), t.Buffer.size());
            _RecvTasks.Get();

        }
    }
    else if (t.Type == StreamLinkStack::ACCEPT)
    {

    }

}

void GLR::StreamLinkStack::OnSend( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev , POLLERTYPE &_Poller)
{

    //_Sock->SetNonBlocking();
    int fd = ev.first;
    Task &t = _SendTasks.Head();
    size_t len = _Sock->Send(&t.Buffer[t.Current], t.Buffer.size() - t.Current);
    t.Current += len;
    GALA_ERROR("Current(%ld)", t.Current);
    if (t.Current == t.Buffer.size())
    {
        int pid = t.Pid;

        _SendTasks.Get();
        if (_SendTasks.Empty())
        {
            GALA_DEBUG("Remove");
            _Poller.Remove(fd, Galaxy::GalaxyRT::EV_OUT);
        }


        Runtime::GetInstance().GetBus().Response(pid,1, LUA_TBOOLEAN, 1);
    }
}



GLR::StreamServerStack::StreamServerStack( Galaxy::GalaxyRT::CSocket *sock, LinkMap &lm)
    :LinkStack(sock),_LinkMap(lm)
{

}

GLR::StreamServerStack::~StreamServerStack()
{

}

void GLR::StreamServerStack::PutAcceptTask( int pid )
{
    StreamLinkStack::Task t;
    t.Type = StreamLinkStack::ACCEPT;
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
    StreamLinkStack::Task &t = _RecvTasks.Head();
    int recvfd = _Sock->Accept(NULL, NULL);
    _LinkMap[recvfd] = new StreamLinkStack(new Galaxy::GalaxyRT::CSocket(recvfd));
    _Poller.Remove(ev.first, Galaxy::GalaxyRT::EV_IN);
    Runtime::GetInstance().GetBus().Response(t.Pid, 1, LUA_TNUMBER, recvfd);
    _RecvTasks.Get();

}

void GLR::StreamServerStack::OnSend( Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE& )
{
    THROW_EXCEPTION_EX("Not Supported");
}

