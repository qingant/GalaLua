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
#include <vector>
namespace GLR
{
    typedef Galaxy::GalaxyRT::CPollSelector POLLERTYPE;
    const   size_t GLR_PAGE_SIZE = 4096;
    const   size_t GLR_STEP_SIZE = 4096 / 4;
    class   TcpCache
    {
    public:
        TcpCache(size_t size, size_t step) : _Buf(size, 0), _Step(step),_Begin(0), _End(0){
        }
        ~TcpCache(){
        }
        inline char *Cursor(){
            return &_Buf[_End];
        }
        inline void Grow(size_t len){
            _End += len;
            if (Reserved() == 0){
                if ((_End - _Begin) > (_Buf.size() - _Step))
                {
                    _Buf.resize(_Buf.size() * 2);
                } else
                {
                    memmove(&_Buf[0], &_Buf[_Begin], _End - _Begin);
                    _End = _End - _Begin;
                    _Begin = 0;
                }
            }
        }
        inline size_t Step()
        {
            return _Step;
        }
        inline void Eat(size_t len)
        {
            if (_Begin + len <= _End)
            {
                _Begin += len; 
            }else
            {
                THROW_EXCEPTION_EX("Out of Range");
            }
            if (_Begin != _End)
            {
                return;
            } else
            {
                _Begin = 0;
                _End = 0;
            }
        }
        inline const char* Get() const{
            return &_Buf[_Begin];
        }
        inline size_t Reserved() const
        {
            return (_Buf.size() - _End + 1);
        }
        inline size_t DataSize() const
        {
            return (_End - _Begin);
        }
        inline size_t GetLine() const
        {
            GALA_DEBUG("Begin:%ld, END:%ld", _Begin, _End);
            for (size_t cursor = _Begin; cursor!=_End; ++cursor)
            {
                if ((_Buf[cursor] == '\n')||
                    ((_Buf[cursor] == '\r') && (_Buf[++cursor] == '\n')))
                {
                    return (cursor - _Begin + 1);
                }
            }
            return -1;
        }
    private:
        std::string _Buf;
        size_t      _Step;
        size_t      _Begin, _End;
    };

    class LinkStack
    {
    public:
        enum TaskType
        {
            ACCEPT,
            RECV,
            SEND,
            WEEP,
            RECV_LINE,
        };
        struct Task{
            TaskType Type;
            int      Pid;
            size_t      Current;
            union{
                struct  
                {
                    size_t Len;       // 收取长度
                    int    Tick;      // 调用进程的中断序号
                    //std::string Buffer;
                }RecvArg;
                struct 
                {
                    //std::string Buffer;
                }SendArg;
            };
            std::string Buffer;
        };

    public:
        friend class SocketWorker;
        friend class SocketController;
        LinkStack(Galaxy::GalaxyRT::CSocket*);
        virtual ~LinkStack();
    public:
        virtual void PutRecvTask(int /*pid*/, size_t /*len*/, int){THROW_EXCEPTION_EX("Operation Not Supported");}
        virtual void PutRecvLineTask(int /*pid*/, int){THROW_EXCEPTION_EX("Operation Not Supported");}
        virtual bool FastRecvReturn(int /*pid*/, TaskType, size_t){THROW_EXCEPTION_EX("Operation Not Supported");}
        virtual void PutSendTask(int , const std::string &){THROW_EXCEPTION_EX("Operation Not Supported");}
        virtual void PutAcceptTask(int, int ){THROW_EXCEPTION_EX("Operation Not Supported");}

    public:
        virtual void OnErr(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&) = 0;
        virtual void OnRecv(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&) = 0;
        virtual void OnSend(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&) = 0;
        //virtual LinkStack *OnAccept(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&) = 0;
    public:
        int GetDummyFd() const {
            return (_Stamp<<16) + _Sock->GetFD();
        }
        bool CheckDummyFd(int fd) const{
            int stamp = (0xffff0000 & fd)>>16;
            return (stamp == _Stamp);
        }
        static int GetRealFd(int fd){
            return (0x0000ffff&fd);
        } 
    protected:
        std::auto_ptr<Galaxy::GalaxyRT::CSocket>  _Sock;
        const int                                   _Stamp;

    };
    class StreamLinkStack: public LinkStack
    {
    public:


        typedef Galaxy::GalaxyRT::_TypeDef_FIFO<Task>::CFIFOQueue TaskQueue;       
        StreamLinkStack(Galaxy::GalaxyRT::CSocket*);
        ~StreamLinkStack();
        virtual void PutRecvTask(int pid, size_t len, int);
        virtual void PutSendTask(int pid, const std::string &);
        virtual void PutRecvLineTask(int , int);
        virtual bool FastRecvReturn(int /*pid*/, TaskType, size_t);
    public:
        virtual void OnErr(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnRecv(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnSend(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        //LinkStack *OnAccept(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);


    private:
        void Response(POLLERTYPE &_Poller );
    private:
        TaskQueue      _SendTasks;
        TaskQueue      _RecvTasks;
        //Galaxy::GalaxyRT::CPthreadMutex _Lock;
        TcpCache       _Cache;
    };

    typedef std::vector<LinkStack*>  LinkMap;

    class StreamServerStack: public LinkStack
    {
    private:
        enum 
        {
            FD_QUEUE_SIZE = 64
        };
    public:
        //typedef Galaxy::GalaxyRT::_TypeDef_FIFO<Task>::CFIFOQueue TaskQueue;       
        StreamServerStack(Galaxy::GalaxyRT::CSocket*, LinkMap &);
        ~StreamServerStack();
        virtual void PutAcceptTask(int pid, int tick);
    public:
        virtual void OnErr(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnRecv(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnSend(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        //LinkStack *OnAccept(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
    private:
        StreamLinkStack::TaskQueue      _RecvTasks;
        LinkMap                         &_LinkMap;
        int                             _OnFlyFd;
    };

    //typedef Galaxy::GalaxyRT::ConcurrentMap<int, LinkStack*> LinkMap;
    //typedef std::tr1::unordered_map<int, LinkStack*> LinkMap;

    class SocketWorker: public Galaxy::GalaxyRT::CRunnable
    {
    public:
        SocketWorker(LinkMap &,Galaxy::GalaxyRT::CRWLock &, 
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
        Galaxy::GalaxyRT::CRWLockAdapter _Mutex;
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
            SC_READLINE,
        };

    public:      
        //friend class SocketWorker;

    public:
        using IController::Request;
        SocketController();
        ~SocketController();
        virtual int  Version()const{return 2;}
        virtual void Request(lua_State*, int);
    private:
        inline void DoServe(lua_State*);
        inline void DoConn(lua_State*);
        inline void DoRecv(lua_State*, int);
        inline void DoSend(lua_State*);
        inline void DoAccept(lua_State*, int);
        inline void DoClose(lua_State*);
        inline void DoRecvLine(lua_State*, int);
    private:

        LinkMap _LinkMap;
        Galaxy::GalaxyRT::CRWLock       _RWLock;
        Galaxy::GalaxyRT::CRWLockAdapter _Mutex;
        //Galaxy::GalaxyRT::CPthreadMutex _Mutex;
        POLLERTYPE _Poller;
        SocketWorker  _Worker;
    };



}
