/* -*- C++ -*- */

/* Time-stamp: <2013-02-25 01:37:41 星期一 by matao> */

/**
* @file   BusController.hpp
* @author Ma Tao <qingant@gmail.com>
* @date   Mon Feb 25 01:37:39 2013
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

namespace GLR
{
    typedef Galaxy::GalaxyRT::CPollSelector POLLERTYPE;
    enum GLR_MSG_TYPE
    {
        REGISTER = 0,
        REGISTER_OK = 1,
        REGISTER_FAIL = 2,
        APPLICATION = 3,
    };
#pragma  pack(1)
    struct GLR_MSG
    {
        //uint32_t Len;
        unsigned char Type;
        struct 
        {
            char Host[20];
            uint32_t  Port;
            uint32_t  GPid;
        } Source, Desination;
        char Content[1];
        GLR_MSG()
        {
            memset(this, 0, sizeof(*this));
        }
    };
#pragma pack()
    class MessageStack
    {
    public:
        MessageStack(Galaxy::GalaxyRT::CSocket*);
        virtual ~MessageStack();
    public:
        virtual void OnErr(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&) = 0;
        virtual void OnRecv(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&) = 0;
        virtual void OnSend(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&) = 0;
    protected:
        std::auto_ptr<Galaxy::GalaxyRT::CSocket>  _Sock;
    };
    typedef std::vector<MessageStack*>  MLinkMap;
    typedef Galaxy::GalaxyRT::ConcurrentMap<std::string, int> RouteMap;

    class MessageLinkStack: public MessageStack
    {
    public:
        struct Task
        {
            size_t Current;
            uint32_t Len;
            size_t HeadCurrent;
            std::string Buffer;
            Task():Current(0), Len(0), HeadCurrent(0){}
            Task(const std::string &buf):Current(0), Len(buf.size())
            {
                assert(buf.size()<1024*1024);
                Buffer.resize(sizeof(GLR_MSG)+buf.size()+3, 0);
                *((uint32_t*)&Buffer[0]) = htonl(buf.size()+sizeof(GLR_MSG) -1);
                GALA_DEBUG("--------------- %d   %d", *((uint32_t*)&Buffer[0]), buf.size()+sizeof(GLR_MSG) -1);
                memcpy(&Buffer[sizeof(GLR_MSG)+3], buf.c_str(), buf.size());

            }
        };
        typedef Galaxy::GalaxyRT::_TypeDef_FIFO<Task>::CFIFOQueue TaskQueue;       
    public:
        MessageLinkStack(Galaxy::GalaxyRT::CSocket*, RouteMap&);
        ~MessageLinkStack();
    public:
        void PutSendTask(const std::string &, int);
    public:
        virtual void OnErr(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnRecv(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnSend(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
    private:
        void OnMessage(const std::string &);
    public:
        void RegisterTo(const std::string &, int);
        int Gpid() const {return _Gpid;}
    public:
        int  _Gpid;
        Task _RecvTask, _SendTask;
        TaskQueue _SendTaskQ;
        RouteMap  &_Router;
        std::string _Id;

    };

    class MessageServerStack: public MessageStack
    {
    public:
        MessageServerStack(Galaxy::GalaxyRT::CSocket*, MLinkMap &, RouteMap &);
        ~MessageServerStack();
    public:
        virtual void OnErr(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnRecv(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnSend(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);

    public:
        MLinkMap  &_LinkMap;
        RouteMap &_Router;
    };

    class BusWorker: public Galaxy::GalaxyRT::CRunnable
    {
    public:
        BusWorker(MLinkMap &, Galaxy::GalaxyRT::CPthreadMutex&, POLLERTYPE &);
        ~BusWorker();
    public:
        virtual void *Run(const Galaxy::GalaxyRT::CThread &t);
        virtual void Clean(const Galaxy::GalaxyRT::CThread &);
    private:
        MLinkMap    &_LinkMap;
        Galaxy::GalaxyRT::CPthreadMutex &_Mutex;
        POLLERTYPE &_Poller;

    };
    class BusController : public IController
    {
    public:
        enum OPType
        {
            //NODE_CONN,
            NODE_REG,
            NODE_SEND,
            NODE_CHECK_REG,
        };
    public:
        BusController(); 
        ~BusController();
        virtual void Request(lua_State *);
    private:
        void DoNodeReg(lua_State*);
        void DoIsReg(lua_State*);
        void DoNodeSend(lua_State*);
        void DoCheckReg(lua_State*);
    private:
        MLinkMap _LinkMap;
        Galaxy::GalaxyRT::CPthreadMutex _Mutex;
        POLLERTYPE _Poller;
        RouteMap   _Router;
        BusWorker  _Worker;
    };
}