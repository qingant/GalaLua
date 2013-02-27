/* -*- C++ -*- */

/* Time-stamp: <2013-02-25 01:38:01 星期一 by matao> */

/**
* @file   BusController.cpp
* @author Ma Tao <qingant@gmail.com>
* @date   Mon Feb 25 01:38:00 2013
*
* @bref
*
*/
#include "BusController.hpp"
#include "GLR.hpp"

GLR::BusWorker::BusWorker( MLinkMap &lm, Galaxy::GalaxyRT::CPthreadMutex &lock, POLLERTYPE & pl)
    :_LinkMap(lm), _Mutex(lock), _Poller(pl)
{

}

GLR::BusWorker::~BusWorker()
{

}

void * GLR::BusWorker::Run( const Galaxy::GalaxyRT::CThread & )
{
    while (true)
    {
        Galaxy::GalaxyRT::CSelector::EV_PAIRS evs = _Poller.WaitEvent(2000);
        for (size_t i = 0; i!= evs.size(); ++i)
        {
            Galaxy::GalaxyRT::CSelector::EV_PAIR &ev = evs[i];
            MessageStack *ms = _LinkMap[ev.first];
            try
            {
                if (ev.second&Galaxy::GalaxyRT::EV_ERR || 
                    ev.second&Galaxy::GalaxyRT::EV_HUP ||
                    ev.second&Galaxy::GalaxyRT::EV_NVALID ||
                    ev.second&Galaxy::GalaxyRT::EV_RDHUP
                    )
                {
                    ms->OnErr(ev, _Poller);
                    THROW_EXCEPTION_EX("");

                } 
                else if (ev.second&Galaxy::GalaxyRT::EV_IN)
                {
                    ms->OnRecv(ev, _Poller);
                }
                else if (ev.second&Galaxy::GalaxyRT::EV_OUT)
                {
                    ms->OnSend(ev, _Poller);
                }
                else
                {
                    ms->OnErr(ev, _Poller);
                }
            }
            catch (Galaxy::GalaxyRT::CException& e)
            {
                _LinkMap[ev.first] = NULL;
                _Poller.Remove(ev.first);
                delete ms;
            }

        }
    }
    return NULL;
}

void GLR::BusWorker::Clean( const Galaxy::GalaxyRT::CThread & )
{

}




GLR::MessageServerStack::MessageServerStack( Galaxy::GalaxyRT::CSocket* sock, MLinkMap &lm ,RouteMap &m)
    :MessageStack(sock), _LinkMap(lm),_Router(m)
{

}

GLR::MessageServerStack::~MessageServerStack()
{

}

void GLR::MessageServerStack::OnErr( Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE& )
{

}

void GLR::MessageServerStack::OnRecv( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev, POLLERTYPE &_Poller )
{
    int recvfd = _Sock->Accept(NULL, NULL);
    _LinkMap[recvfd] = new MessageLinkStack(new Galaxy::GalaxyRT::CSocket(recvfd), _Router);
    _Poller.Register(recvfd, Galaxy::GalaxyRT::EV_IN);
    //_Poller.Remove(ev.first, Galaxy::GalaxyRT::EV_IN);
    //Runtime::GetInstance().GetBus().Response(t.Pid, 1, LUA_TNUMBER, recvfd);
    //_RecvTasks.Get();
}

void GLR::MessageServerStack::OnSend( Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE& )
{
    THROW_EXCEPTION_EX("Not Supported Operation");
}

GLR::MessageStack::MessageStack( Galaxy::GalaxyRT::CSocket *sock )
    :_Sock(sock)
{

}

GLR::MessageStack::~MessageStack()
{

}

void GLR::MessageLinkStack::OnMessage( const std::string &msg )
{
    GLR_MSG *pMsg = (GLR_MSG*)msg.c_str();
    GALA_DEBUG("OnMessage");
    if (pMsg->Type == REGISTER)
    {

        char id[64] = {0};
        snprintf(id, sizeof(id), "%s::%d", pMsg->Source.Host, pMsg->Source.Port);
        uint32_t len = htonl(msg.size());
        _Sock->SegmentSend(-1, (const char*)&len, sizeof(len));
        pMsg->Type = REGISTER_OK;
        _Sock->SegmentSend(-1, msg.c_str(), msg.size());
        _Router.insert(std::make_pair(std::string(id), _Sock->GetFD()));
        GALA_DEBUG("%s registered", id);
        //Runtime::GetInstance().GetBus().Send(_Gpid, msg);
    }
    else if (pMsg->Type == APPLICATION)
    {
        GALA_DEBUG("App Msg (%d)", _Gpid);
        Runtime::GetInstance().GetBus().Send(ntohl(pMsg->Desination.GPid), pMsg->Content);
    }
}

GLR::MessageLinkStack::MessageLinkStack( Galaxy::GalaxyRT::CSocket *sock, RouteMap &m)
    :MessageStack(sock),_Gpid(0),_Router(m)
{

}


GLR::MessageLinkStack::~MessageLinkStack()
{

}

void GLR::MessageLinkStack::OnErr( Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE& )
{

}

void GLR::MessageLinkStack::OnRecv( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev, POLLERTYPE &_Poller)
{
    //int fd = ev.first;
    if (_RecvTask.Len == 0)
    {
        size_t len = _Sock->Recv((char*)&_RecvTask.Len, 4-_RecvTask.Current);
        _RecvTask.Current += len;
        if (_RecvTask.Current == 4)
        {
            _RecvTask.Len = ntohl(_RecvTask.Len);
            _RecvTask.Buffer.resize(_RecvTask.Len);
            _RecvTask.Current = 0;
        }
    }
    else
    {
        size_t len = _Sock->Recv(&_RecvTask.Buffer[_RecvTask.Current],_RecvTask.Len -_RecvTask.Current);
        _RecvTask.Current += len;
        GALA_ERROR("%d %d", len, _RecvTask.Len);
        if (_RecvTask.Current == _RecvTask.Len)
        {
            OnMessage(_RecvTask.Buffer);
            //Runtime::GetInstance().GetBus().Send(_Gpid, _RecvTask.Buffer);
            _RecvTask = Task();
        }
    }

}

void GLR::MessageLinkStack::OnSend( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev, POLLERTYPE &_Poller)
{
    int fd = ev.first;
    Task &t = _SendTaskQ.Head();
    size_t len = _Sock->Send(&t.Buffer[t.Current], t.Buffer.size() - t.Current);
    t.Current += len;
    if (t.Current == t.Buffer.size())
    {
        _SendTaskQ.Get();
        if (_SendTaskQ.Empty())
        {
            _Poller.Remove(fd, Galaxy::GalaxyRT::EV_OUT);
        }

    }
}

void GLR::MessageLinkStack::PutSendTask( const std::string &msg, int pid )
{
    Task t(msg);
    GLR_MSG *head = (GLR_MSG *)&t.Buffer[4];
    head->Desination.GPid = htonl(pid);
    head->Type = APPLICATION;
    _SendTaskQ.Put(t);
}

void GLR::MessageLinkStack::RegisterTo( const std::string &host, int port)
{
    char id[64] = {0};
    snprintf(id, sizeof(id), "%s::%d", host.c_str(), port);
    _Id = id;
    GLR_MSG msg;
    msg.Type = REGISTER;
    memcpy(msg.Source.Host, Runtime::GetInstance().Host().c_str(), Runtime::GetInstance().Host().size());
    msg.Source.Port = Runtime::GetInstance().NodeId();
    uint32_t len = htonl(sizeof(GLR_MSG));
    _Sock->SegmentSend(-1, (const char*)&len, sizeof(len));
    _Sock->SegmentSend(-1, (const char*)&msg, sizeof(msg));

    _Sock->SegmentRecv(-1, (char*)&len, sizeof(len));
    _Sock->SegmentRecv(-1, (char*)&msg, sizeof(msg));

    if (msg.Type == REGISTER_OK)
    {
        GALA_ERROR("ok");
        return;
    }
    else
    {
        THROW_EXCEPTION_EX("REGIST Error");
    }


}

GLR::BusController::BusController()
    :_LinkMap(10240, NULL),
    _Worker(_LinkMap, _Mutex, _Poller)
{
    const std::string &host = Runtime::GetInstance().Host();
    int port = Runtime::GetInstance().NodeId();
    Galaxy::GalaxyRT::CTCPSocketServer *s = new Galaxy::GalaxyRT::CTCPSocketServer(host, port);
    _LinkMap[s->GetFD()] = new MessageServerStack(s, _LinkMap, _Router);
    _Poller.Register(s->GetFD(), Galaxy::GalaxyRT::EV_IN);
    Galaxy::GalaxyRT::CThread *t = new Galaxy::GalaxyRT::CThread(_Worker, 1);
    t->Start();
}

GLR::BusController::~BusController()
{

}

void GLR::BusController::Request( lua_State *l )
{
    int type = luaL_checkinteger(l,2);
    switch ((OPType)type)
    {
    case NODE_REG:
        DoNodeReg(l);
        break;
    case NODE_SEND:
        DoNodeSend(l);
        break;
    case NODE_CHECK_REG:
        DoCheckReg(l);
    default:
        break;
    }
}

void GLR::BusController::DoNodeReg( lua_State *l )
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    const char* host = luaL_checkstring(l, 3);
    int port = luaL_checkinteger(l, 4);
    //int des_pid = luaL_checkinteger(l, 5);
    char id[64] = {0};
    snprintf(id, sizeof(id), "%s::%d", host, port);
    if(_Router.has_key(id))
    {
        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TNUMBER, _Router.find_ex(id));
        return;
    }
    else
    {
        Galaxy::GalaxyRT::CTCPSocketClient *c = new Galaxy::GalaxyRT::CTCPSocketClient(host, port);  // should be guard
        MessageLinkStack *ms = new MessageLinkStack(c, _Router); // as above
        try
        {
            ms->RegisterTo(host, port);
        }
        catch (Galaxy::GalaxyRT::CException& e)
        {
            delete ms;
            std::string errmsg = "Cannot Register";
            Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
            return;
        }
        //   ms->_Gpid = des_pid;
        _LinkMap[c->GetFD()] = ms;
        _Router.insert(std::make_pair(std::string(id), c->GetFD()));
        _Poller.Register(c->GetFD(), Galaxy::GalaxyRT::EV_IN);
        GALA_ERROR("Return %d", pid);
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TBOOLEAN, 1, LUA_TNUMBER, c->GetFD());
    }
}

void GLR::BusController::DoNodeSend( lua_State *l )
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    const char* host = luaL_checkstring(l, 3);
    int port = luaL_checkinteger(l, 4);
    int des_pid = luaL_checkinteger(l, 6);
    char id[64] = {0};
    snprintf(id, sizeof(id), "%s::%d", host, port);
    if (!_Router.has_key(id))
    {
        std::string errmsg = "Not Registered";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }
    int fd = _Router.find_ex(id);
    MessageLinkStack *ms = (MessageLinkStack*)(_LinkMap[fd]);
    if (ms == NULL)
    {
        std::string errmsg = "Not Registered";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }
    const char *msg = NULL;
    size_t len = 0;
    msg = luaL_checklstring(l, 5, &len);
    ms->PutSendTask(std::string(msg, len), des_pid);
    _Poller.Register(fd, Galaxy::GalaxyRT::EV_OUT);
    Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TBOOLEAN, 1);
}

void GLR::BusController::DoCheckReg( lua_State *l)
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    const char* host = luaL_checkstring(l, 3);
    int port = luaL_checkinteger(l, 4);
    char id[64] = {0};
    snprintf(id, sizeof(id), "%s::%d", host, port);
    if (!_Router.has_key(id))
    {
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TBOOLEAN, 0);
        return;
    }
    int fd = _Router.find_ex(id);
    MessageLinkStack *ms = (MessageLinkStack*)(_LinkMap[fd]);
    if (ms == NULL)
    {
        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TBOOLEAN, 0);
        return;
    }
    Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TBOOLEAN, 1);
}
