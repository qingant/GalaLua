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
        Galaxy::GalaxyRT::CSelector::EV_PAIRS evs = _Poller.WaitEvent(3000);
        for (size_t i = 0; i!= evs.size(); ++i)
        {
            Galaxy::GalaxyRT::CSelector::EV_PAIR &ev = evs[i];
            MessageStack *ms = _LinkMap[ev.first];

            if (ms==NULL) continue;

            try
            {

                if (ev.second&Galaxy::GalaxyRT::EV_IN)
                {
                    ms->OnRecv(ev, _Poller);
                }
                else if (ev.second&Galaxy::GalaxyRT::EV_OUT)
                {
                    ms->OnSend(ev, _Poller);
                }
                else if (ev.second&Galaxy::GalaxyRT::EV_ERR ||
                    ev.second&Galaxy::GalaxyRT::EV_HUP ||
                    ev.second&Galaxy::GalaxyRT::EV_NVALID ||
                    ev.second&Galaxy::GalaxyRT::EV_RDHUP
                    )
                {
                    ms->OnErr(ev, _Poller);
                    THROW_EXCEPTION_EX("");

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
                GLRPROTOCOL msg;
                memset(&msg, 0, sizeof(msg));
                msg._Protocol._Type = GLRPROTOCOL::CLOSED;
                msg._Route._ToGpid = -1;
                msg._Protocol._Length = 0;

                memcpy(msg._Host._V2._Host, mls->_Id.c_str(), mls->_Id.size());

                Runtime::GetInstance().GetBus().Send(mls->Gpid(), std::string((const char*)&msg, sizeof(msg)), GLRPROTOCOL::CLOSED);
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
    GLRPROTOCOL *pMsg = (GLRPROTOCOL *)msg.c_str();
    GALA_DEBUG("OnMessage");
    SHORT timeout = 30;
    if (pMsg->_Protocol._Type == GLRPROTOCOL::REGISTER)
    {
        if (msg.size() < sizeof(GLRPROTOCOL))
        {
            THROW_EXCEPTION_EX("Fatal Error");
        }

        char id[64] = {0};
        // TODO: protocol version check
        snprintf(id, sizeof(id), "%s::%d", pMsg->_Host._V2._Host, ntohl(pMsg->_Host._V2._Port));
        pMsg->_Protocol._Type = GLRPROTOCOL::REGISTER_OK;
        pMsg->_Protocol._Length = htonl(sizeof(GLRPROTOCOL) - sizeof(pMsg->_Protocol._Length));
        if(!_Sock->SegmentSend(timeout, msg.c_str(), sizeof(GLRPROTOCOL)))
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
        _Host = (const char *)pMsg->_Host._V2._Host;
        _Port = ntohl(pMsg->_Host._V2._Port);
        //Runtime::GetInstance().GetBus().Send(_Gpid, msg);
    }
    else if (pMsg->_Protocol._Type == GLRPROTOCOL::APP)
    {

        // convert to host endian for all needed fields
        pMsg->_Route._FromGpid = ntohl( pMsg->_Route._FromGpid);
        pMsg->_Host._V2._Port = ntohl( pMsg->_Host._V2._Port);
        pMsg->_Protocol._Stamp = ntohl( pMsg->_Protocol._Stamp);
        pMsg->_Msg._MsgId = ntohl( pMsg->_Msg._MsgId);
        pMsg->_Msg._CorrId = ntohl( pMsg->_Msg._CorrId);


        Runtime::GetInstance().GetBus().Send(ntohl(pMsg->_Route._ToGpid), msg);
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

    //TODO: recv 8 bytes, check magic number and type
    if (_RecvTask.HeadCurrent < 4)
    {
        size_t len = _Sock->Recv(((char*)&_RecvTask.Len)+_RecvTask.HeadCurrent, 4-_RecvTask.HeadCurrent);
        _RecvTask.HeadCurrent += len;
        if (_RecvTask.HeadCurrent == 4)
        {
            _RecvTask.Len = ntohl(_RecvTask.Len);
            GALA_DEBUG("recv len:%d \n",_RecvTask.Len);
            if(_RecvTask.Len > 1024*1024*500)
            {
                const char *rejectMessage = "GLR: you are invalid visitor, rejected!\n";
                _Sock->Send(rejectMessage, strlen(rejectMessage));
                THROW_EXCEPTION_EX("Recv Data Too Long");
            }
            _RecvTask.Buffer.resize(_RecvTask.Len + 4, 0);
            *((uint32_t*)&_RecvTask.Buffer[0]) = _RecvTask.Len;
            _RecvTask.Current = 4;
        }
    }
    else
    {
        ssize_t len = _Sock->Recv(&_RecvTask.Buffer[_RecvTask.Current],
                _RecvTask.Len -_RecvTask.Current + 4);
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

void GLR::MessageLinkStack::PutSendTask( const std::string &msg, int to_pid, int src_pid, uint32_t msgid, uint32_t corrid )
{
    // the constructor set GLRPROTOCOL._Protocol._Length by network endian
    Task t(msg);


    GLRPROTOCOL *head = (GLRPROTOCOL *)&t.Buffer[0];
    head->_Protocol._Type = GLRPROTOCOL::APP;

    head->_Protocol._Magic[0] = 'G';
    head->_Protocol._Magic[1] = 'L';

    head->_Msg._CorrId = htonl(corrid);
    head->_Msg._MsgId = htonl(msgid);

    head->_Protocol._Version = 2;
    head->_Route._ToGpid = htonl(to_pid);
    memcpy(head->_Host._V2._Host, Runtime::GetInstance().Host().c_str(), Runtime::GetInstance().Host().size());
    head->_Host._V2._Port = htonl(Runtime::GetInstance().NodeId());
    head->_Route._FromGpid = htonl(src_pid);

    CRT_time((time_t*)&head->_Protocol._Stamp);
    head->_Protocol._Stamp = htonl(head->_Protocol._Stamp);
    _SendTaskQ.Put(t);
}

void GLR::MessageLinkStack::RegisterTo( const std::string &host, int port, short timeout )
{
    char id[64] = {0};
    snprintf(id, sizeof(id), "%s::%d", host.c_str(), port);
    _Id = id;
    std::string buf(sizeof(GLRPROTOCOL), 0);
    GLRPROTOCOL *pHead = (GLRPROTOCOL*)buf.c_str();
    pHead->_Protocol._Type = GLRPROTOCOL::REGISTER;

    pHead->_Protocol._Magic[0] = 'G';
    pHead->_Protocol._Magic[1] = 'L';

    pHead->_Protocol._Length = htonl(sizeof(GLRPROTOCOL) - sizeof(pHead->_Protocol._Length));

    CRT_time((time_t*)&pHead->_Protocol._Stamp);
    pHead->_Protocol._Version = 2;
    memcpy(pHead->_Host._V2._Host, Runtime::GetInstance().Host().c_str(), Runtime::GetInstance().Host().size());
    pHead->_Host._V2._Port = htonl(Runtime::GetInstance().NodeId());
    //uint32_t len = htonl(sizeof(GLR_MSG));
    //_Sock->SegmentSend(-1, (const char*)&len, sizeof(len));
    if(!_Sock->SegmentSend(timeout, buf.c_str(), buf.size()))
    {
        THROW_EXCEPTION_EX("Send Register Timeout");
    }

    if(!_Sock->SegmentRecv(timeout, (char*)&buf[0], sizeof(GLRPROTOCOL)))
    {
        THROW_EXCEPTION_EX("Recv Register Timeout");
    }
    // _Sock->SegmentRecv(-1, (char*)&msg, sizeof(msg));

    if (pHead->_Protocol._Type == GLRPROTOCOL::REGISTER_OK)
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
            GALA_DEBUG("%s\n",e.what());
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

uint32_t GLR::BusController::MsgIdGen = 0xffffffff;

void GLR::BusController::DoNodeSend( lua_State *l )
{
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    lua_pop(l, 1);

    lua_getfield(l, 3, "host");
    // XXX: luaL_error cross C++ call
    const char *host = luaL_checkstring(l, -1);
    lua_pop(l, 1);

    lua_getfield(l, 3, "port");
    int port = luaL_checkinteger(l, -1);
    lua_pop(l, 1);

    lua_getfield(l, 3, "gpid");
    int des_pid = luaL_checkinteger(l, -1);
    lua_pop(l, 1);

    uint32_t msgid = MsgIdGen, corrid = 0;
    // TODO: atomic_add
    MsgIdGen--;


    if (lua_gettop(l) == 5 && lua_istable(l, -1))
    {
        lua_getfield(l, 5, "msgid");
        if (!lua_isnil(l, -1))
        {
            msgid = luaL_checkinteger(l, -1);
        }
        lua_pop(l,1);

        lua_getfield(l, 5, "corrid");
        corrid = luaL_checkinteger(l, -1);
        lua_pop(l,1);
    }
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
    msg = luaL_checklstring(l, 4, &len);
    ms->PutSendTask(std::string(msg, len), des_pid, pid, msgid, corrid);
    _Poller.Register(fd, Galaxy::GalaxyRT::EV_OUT);

    std::map<std::string, int> attr;
    attr["corrid"] = corrid;
    attr["msgid"] = msgid;
    //TODO: add stamp

    Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TTABLE, attr);
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
