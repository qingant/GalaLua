/* -*- C++ -*- */

/* Time-stamp: <2013-02-20 10:05:34 星期三 by matao> */

/**
* @file   SocketController.hpp
* @author Ma Tao <qingant@gmail.com>
* @date   Wed Feb 20 10:05:33 2013
*
* @bref
*
*/

#include "Bus.hpp"
#include "Runtime/Timer.hpp"
#include "Runtime/Event.hpp"
#include "Runtime/Thread.hpp"
#include "Runtime/Socket.hpp"
#include "Runtime/Queue.hpp"
#include "Runtime/Utils.hpp"

#pragma once
namespace GLR
{
    typedef Galaxy::GalaxyRT::CPollSelector POLLERTYPE;

    class LinkStack
    {


    public:
        friend class SocketWorker;
        friend class SocketController;
        LinkStack(Galaxy::GalaxyRT::CSocket*);
        virtual ~LinkStack();
    public:
        virtual void PutRecvTask(int /*pid*/, size_t /*len*/){THROW_EXCEPTION_EX("Operation Not Supported");}
        virtual void PutSendTask(int , const std::string &){THROW_EXCEPTION_EX("Operation Not Supported");}
        virtual void PutAcceptTask(int ){THROW_EXCEPTION_EX("Operation Not Supported");}

    public:
        virtual void OnErr(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&) = 0;
        virtual void OnRecv(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&) = 0;
        virtual void OnSend(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&) = 0;
        //virtual LinkStack *OnAccept(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&) = 0;
    protected:
        std::auto_ptr<Galaxy::GalaxyRT::CSocket>  _Sock;

    };
    class StreamLinkStack: public LinkStack
    {
    public:
        enum TaskType
        {
            ACCEPT,
            RECV,
            SEND,
            WEEP,
        };
        struct Task{
            TaskType Type;
            int      Pid;
            size_t      Current;
            union{
                struct  
                {
                    size_t Len;
                    //std::string Buffer;
                }RecvArg;
                struct 
                {
                    //std::string Buffer;
                }SendArg;
            };
            std::string Buffer;
        };

        typedef Galaxy::GalaxyRT::_TypeDef_FIFO<Task>::CFIFOQueue TaskQueue;       
        StreamLinkStack(Galaxy::GalaxyRT::CSocket*);
        ~StreamLinkStack();
        virtual void PutRecvTask(int pid, size_t len);
        virtual void PutSendTask(int pid, const std::string &);
        virtual void PutAcceptTask(int pid);
    public:
        virtual void OnErr(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnRecv(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnSend(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        //LinkStack *OnAccept(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
    private:
        TaskQueue      _SendTasks;
        TaskQueue      _RecvTasks;
    };

    typedef std::vector<LinkStack*>  LinkMap;

    class StreamServerStack: public LinkStack
    {
    public:
        //typedef Galaxy::GalaxyRT::_TypeDef_FIFO<Task>::CFIFOQueue TaskQueue;       
        StreamServerStack(Galaxy::GalaxyRT::CSocket*, LinkMap &);
        ~StreamServerStack();
        virtual void PutAcceptTask(int pid);
    public:
        virtual void OnErr(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnRecv(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnSend(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        //LinkStack *OnAccept(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
    private:
        StreamLinkStack::TaskQueue      _RecvTasks;
        LinkMap                         &_LinkMap;
    };
  
    //typedef Galaxy::GalaxyRT::ConcurrentMap<int, LinkStack*> LinkMap;
    //typedef std::tr1::unordered_map<int, LinkStack*> LinkMap;

    class SocketWorker: public Galaxy::GalaxyRT::CRunnable
    {
    public:
        SocketWorker(LinkMap &,Galaxy::GalaxyRT::CPthreadMutex &, 
            Galaxy::GalaxyRT::CPollSelector &);
        ~SocketWorker();
        virtual void *Run(const Galaxy::GalaxyRT::CThread &t);
        virtual void Clean(const Galaxy::GalaxyRT::CThread &);
    private:
        inline void OnErr(Galaxy::GalaxyRT::CSelector::EV_PAIR &);
        inline void OnRecv(Galaxy::GalaxyRT::CSelector::EV_PAIR &);
        inline void OnSend(Galaxy::GalaxyRT::CSelector::EV_PAIR &);
    private:
        LinkMap &_LinkMap;
        Galaxy::GalaxyRT::CPthreadMutex &_Mutex;
        POLLERTYPE &_Poller;

    };
    class SocketController: public IController
    {
    public:
        enum OPType
        {
            SC_SERVE,     // Create Server: type(tcp/UnixDomain) addr [port]
            SC_CONN,      // Connect to server: as above
            SC_RECV,      // Recv: fd length
            SC_SEND,      // Send: fd buffer(string)
            SC_ACCEPT,    // Accept: fd
            SC_CLOSE,
            SC_SHUTDOWN,
        };

    public:      
        //friend class SocketWorker;

    public:
        SocketController();
        ~SocketController();
        virtual void Request(lua_State*);
    private:
        inline void DoServe(lua_State*);
        inline void DoConn(lua_State*);
        inline void DoRecv(lua_State*);
        inline void DoSend(lua_State*);
        inline void DoAccept(lua_State*);
        inline void DoClose(lua_State*);
    private:

        LinkMap _LinkMap;
        Galaxy::GalaxyRT::CPthreadMutex _Mutex;
        POLLERTYPE _Poller;
        SocketWorker  _Worker;
    };



}
