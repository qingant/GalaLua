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

GLR::BusWorker::BusWorker(RouteMap &rm, MLinkMap &lm, Galaxy::GalaxyRT::CPthreadMutex &lock, POLLERTYPE & pl)
    :_Router(rm), _LinkMap(lm), _Mutex(lock), _Poller(pl)
{

}

GLR::BusWorker::~BusWorker()
{

}

void * GLR::BusWorker::Run( const Galaxy::GalaxyRT::CThread & )
{
    while (true)
    {
        Galaxy::GalaxyRT::CSelector::EV_PAIRS evs = _Poller.WaitEvent(30);
        for (size_t i = 0; i!= evs.size(); ++i)
        {
            Galaxy::GalaxyRT::CSelector::EV_PAIR &ev = evs[i];
            MessageStack *ms = _LinkMap[ev.first];

            if (ms==NULL) continue;

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
                GALA_DEBUG("Close Connection:%s", e.what());
                MessageLinkStack *mls = ((MessageLinkStack*)ms);
                GLR_BUS_HEAD msg;
                memset(&msg, 0, sizeof(msg));
                msg.Head.Type = MSG_HEAD::CLOSED;
                msg.Head.GPid = -1;
                msg.Head.Len = sizeof(MSG_HEAD) - 4;

                memcpy(msg.Source.Host, mls->_Id.c_str(), mls->_Id.size());

                Runtime::GetInstance().GetBus().Send(mls->Gpid(), std::string((const char*)&msg, sizeof(msg)), MSG_HEAD::CLOSED);
                Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
                _LinkMap[ev.first] = NULL;
                _Router.erase(mls->_Id);
                GALA_DEBUG("FD(%d) Removed!", ev.first);
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

void GLR::MessageServerStack::OnRecv( Galaxy::GalaxyRT::CSelector::EV_PAIR &/*ev*/, POLLERTYPE &_Poller )
{
    int recvfd = _Sock->Accept(NULL, NULL);
    _LinkMap[recvfd] = new MessageLinkStack(new Galaxy::GalaxyRT::CSocket(recvfd), _Router);
    _Poller.Register(recvfd, Galaxy::GalaxyRT::EV_IN);
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
    GLR_BUS_HEAD *pMsg = (GLR_BUS_HEAD *)msg.c_str();
    GALA_DEBUG("OnMessage");
    SHORT timeout = 30;
    if (pMsg->Head.Type == MSG_HEAD::REGISTER)
    {
        if (msg.size() < sizeof(GLR_BUS_HEAD))
        {
            THROW_EXCEPTION_EX("Fatal Error");
        }

        char id[64] = {0};
        //snprintf(id, sizeof(id), "%s::%d", pMsg->Source.Host, pMsg->Source.Port);
        GLR_ADDR *pAddr = (GLR_ADDR*)&msg[sizeof(MSG_HEAD)];

        //        printf("****************%s::%d------%d\n",pAddr->Host, ntohl(pAddr->Port),pAddr->Port);
        snprintf(id, sizeof(id), "%s::%d", pAddr->Host, ntohl(pAddr->Port));
        //uint32_t len = htonl(sizeof(*pMsg));
        //_Sock->SegmentSend(-1, (const char*)&len, sizeof(len));
        pMsg->Head.Type = MSG_HEAD::REGISTER_OK;
        pMsg->Head.Len = htonl(sizeof(GLR_BUS_HEAD) - 4);
        if(!_Sock->SegmentSend(timeout, msg.c_str(), sizeof(GLR_BUS_HEAD)))
        {
            _Sock->Close();
            return;
        }
        if (_Router.has_key(id))
        {
            _Sock->Close();
            return;
        }
        
        _Router.insert(std::make_pair(std::string(id), _Sock->GetFD()));
        GALA_DEBUG("%s registered", id);
        _Id = id;
        //Runtime::GetInstance().GetBus().Send(_Gpid, msg);
    }
    else if (pMsg->Head.Type == MSG_HEAD::APP)
    {

        pMsg->Source.Gpid = ntohl( pMsg->Source.Gpid);
        pMsg->Source.Port = ntohl( pMsg->Source.Port);
        Runtime::GetInstance().GetBus().Send(ntohl(pMsg->Head.GPid), msg);
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

void GLR::MessageLinkStack::OnRecv( Galaxy::GalaxyRT::CSelector::EV_PAIR &/*ev*/, POLLERTYPE &/*_Poller*/)
{
    //int fd = ev.first;
    GALA_DEBUG("%d", _RecvTask.Len);
    if (_RecvTask.HeadCurrent < 4)
    {
        size_t len = _Sock->Recv(((char*)&_RecvTask.Len)+_RecvTask.HeadCurrent, 4-_RecvTask.HeadCurrent);
        _RecvTask.HeadCurrent += len;
        if (_RecvTask.HeadCurrent == 4)
        {
            _RecvTask.Len = ntohl(_RecvTask.Len);
            if(_RecvTask.Len > 1024*1024*500)
            {
                THROW_EXCEPTION_EX("Recv Data Too Long");
            }
            _RecvTask.Buffer.resize(_RecvTask.Len + 4, 0);
            *((uint32_t*)&_RecvTask.Buffer[0]) = _RecvTask.Len;
            _RecvTask.Current = 4;
        }
    }
    else
    {
        ssize_t len = _Sock->Recv(&_RecvTask.Buffer[_RecvTask.Current],_RecvTask.Len -_RecvTask.Current+4);
        if (len < 0)
        {
            THROW_EXCEPTION_EX("recv error");
        }
        _RecvTask.Current += len;
        if ((_RecvTask.Current - 4) == _RecvTask.Len)
        {
            OnMessage(_RecvTask.Buffer);
            _RecvTask = Task();
        }
    }

}

void GLR::MessageLinkStack::OnSend( Galaxy::GalaxyRT::CSelector::EV_PAIR &ev, POLLERTYPE &_Poller)
{
    int fd = ev.first;

    if (_SendTask.Current == _SendTask.Buffer.size())
    {
        if (_SendTaskQ.Empty())
        {
            return;
        }else
        {

            _SendTask = _SendTaskQ.Get();
        }

    }

    Task &t = _SendTask;
    size_t len = _Sock->Send(&t.Buffer[t.Current], t.Buffer.size() - t.Current);
    t.Current += len;
    if (t.Current == t.Buffer.size())
    {
        if (_SendTaskQ.Empty())
        {
            _Poller.Remove(fd, Galaxy::GalaxyRT::EV_OUT);
        }

    }
}

void GLR::MessageLinkStack::PutSendTask( const std::string &msg, int pid, int src_pid )
{
    Task t(msg);
    GLR_BUS_HEAD *head = (GLR_BUS_HEAD *)&t.Buffer[0];
    //head->GPid = htonl(pid);
    head->Head.Type = MSG_HEAD::APP;
    head->Head.GPid = htonl(pid);
    memcpy(head->Source.Host, Runtime::GetInstance().Host().c_str(), Runtime::GetInstance().Host().size());
    head->Source.Port = htonl(Runtime::GetInstance().NodeId());
    head->Source.Gpid = htonl(src_pid);
    //head->Type = MSG_HEAD::APP;
    _SendTaskQ.Put(t);
}

void GLR::MessageLinkStack::RegisterTo( const std::string &host, int port, short timeout )
{
    char id[64] = {0};
    snprintf(id, sizeof(id), "%s::%d", host.c_str(), port);
    _Id = id;
    std::string buf(sizeof(MSG_HEAD)+sizeof(GLR_ADDR), 0);
    MSG_HEAD *pHead = (MSG_HEAD*)buf.c_str();
    GLR_ADDR *pAddr = (GLR_ADDR *)&buf[sizeof(MSG_HEAD)];
    pHead->Type = MSG_HEAD::REGISTER;
    pHead->Len = htonl(sizeof(MSG_HEAD) + sizeof(GLR_ADDR) - 4);
    memcpy(pAddr->Host, Runtime::GetInstance().Host().c_str(), Runtime::GetInstance().Host().size());
    pAddr->Port = htonl(Runtime::GetInstance().NodeId());
    //uint32_t len = htonl(sizeof(GLR_MSG));
    //_Sock->SegmentSend(-1, (const char*)&len, sizeof(len));
    if(!_Sock->SegmentSend(timeout, buf.c_str(), buf.size()))
    {
        THROW_EXCEPTION_EX("Register Timeout");
    }

    if(!_Sock->SegmentRecv(timeout, (char*)&buf[0], sizeof(GLR_BUS_HEAD)))
    {
        THROW_EXCEPTION_EX("Register Timeout");
    }
    // _Sock->SegmentRecv(-1, (char*)&msg, sizeof(msg));

    if (pHead->Type == MSG_HEAD::REGISTER_OK)
    {
        GALA_DEBUG("ok");
        return;
    }
    else
    {
        THROW_EXCEPTION_EX("REGIST Error");
    }
}

GLR::BusController::BusController()
#if defined(HAVE_SUSE)
    :_LinkMap(10240, (MessageStack *)0),
#else
    :_LinkMap(10240, NULL),
#endif
    _Worker(_Router, _LinkMap, _Mutex, _Poller)
{
    const std::string &host = Runtime::GetInstance().Host();
    int port = Runtime::GetInstance().NodeId();
    if (port > 0)
    {
        Galaxy::GalaxyRT::CTCPSocketServer *s = new Galaxy::GalaxyRT::CTCPSocketServer(host, port);
        GALA_DEBUG("%d", s->GetFD());
        _LinkMap[s->GetFD()] = new MessageServerStack(s, _LinkMap, _Router);
        _Poller.Register(s->GetFD(), Galaxy::GalaxyRT::EV_IN);
    }


    Galaxy::GalaxyRT::CThread *t = new Galaxy::GalaxyRT::CThread(_Worker, 1);
    t->Start();
}

GLR::BusController::~BusController()
{

}

void GLR::BusController::GetAllLinks( lua_State *l )
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);

    try
    {
        std::map<std::string,int> all=GetAllLinks();
        //        printf("%s  %d %d size:%d  \n",__func__,__LINE__,pid,all.size());
        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TTABLE,all);
    }
    catch (Galaxy::GalaxyRT::CException& e)
    {
        std::string errmsg = "Get All links Error";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }

}

std::map<std::string,int> GLR::BusController::GetAllLinks(void)
{
    std::map<std::string,int> ret;
    for (unsigned int i=0;i<_LinkMap.size();++i)
    {
        if (_LinkMap[i]!=NULL && typeid(MessageLinkStack)==typeid(*_LinkMap[i]))
        {
            MessageLinkStack *p=(MessageLinkStack *)_LinkMap[i];
            //            printf("*******************%s  %d  %s",__func__,__LINE__,p->_Id.c_str());

            ret[p->_Id]=p->_SendTaskQ.Size();
        }
    }
    return ret;
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
        break;
    case NODE_GET_ALL_LINKS:
        GetAllLinks(l);
        break;
    case NODE_CLOSE:
        DoNodeClose(l);
        break;
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

    //default des_pid=0
    int des_pid = luaL_optint(l, 5,0);
    int timeout = 30;

    char id[64] = {0};
    snprintf(id, sizeof(id), "%s::%d", host, port);
    if(_Router.has_key(id))
    {
        Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TNUMBER, _Router.find_ex(id));
        return;
    }
    else
    {
        std::auto_ptr<Galaxy::GalaxyRT::CTCPSocketClient> c(new Galaxy::GalaxyRT::CTCPSocketClient(host, port));
        int cFd = c->GetFD();
        std::auto_ptr<MessageLinkStack> ms(new MessageLinkStack(c.release(), _Router)); // as above
        try
        {
            ms->RegisterTo(host, port, timeout);
        }
        catch (Galaxy::GalaxyRT::CException& e)
        {
            std::string errmsg = "Cannot Register";
            Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
            return;
        }
        ms->_Gpid = des_pid;
        _LinkMap[cFd] = ms.get();
        _Router.insert(std::make_pair(std::string(id), cFd));
        _Poller.Register(cFd, Galaxy::GalaxyRT::EV_IN);

        GALA_ERROR("Return %d", pid);
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TBOOLEAN, 1, LUA_TNUMBER, cFd);
        ms.release();
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
        GALA_DEBUG("Cannot find id(%s)", id);
        std::string errmsg = "Not Registered";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg.c_str(), errmsg.size());
        return;
    }
    int fd = _Router.find_ex(id);

    Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
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
    ms->PutSendTask(std::string(msg, len), des_pid, pid);
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
        const char *errmsg="not connected";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TBOOLEAN, 0,LUA_TSTRING,errmsg,strlen(errmsg));
        return;
    }

    int fd = _Router.find_ex(id);
    MessageLinkStack *ms = (MessageLinkStack*)(_LinkMap[fd]);
    if (ms == NULL)
    {
        const char *errmsg= "Connetion Closed";
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TBOOLEAN, 0,LUA_TSTRING,errmsg,strlen(errmsg));
        return;
    }
    Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TBOOLEAN, 1);
}

void GLR::BusController::DoNodeClose( lua_State *l )
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);

    const char* host = luaL_checkstring(l, 3);
    int port = luaL_checkinteger(l, 4);
    char id[64] = {0};
    snprintf(id, sizeof(id), "%s::%d", host, port);
    try
    {
        Galaxy::GalaxyRT::CLockGuard _Gl(&_Mutex);
        int fd = _Router.find_ex(id);
        if (_LinkMap[fd] != NULL)
        {
            _LinkMap[fd]->getSocket().Close();
        }
        else
        {
            _Router.erase(id);
            GALA_ERROR("Fatal Error: Null Pointer");
        }
    }
    catch (const Galaxy::GalaxyRT::CException &e)
    {
        GALA_ERROR(e.what());
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, e.what(),strlen(e.what()));
    }

}
