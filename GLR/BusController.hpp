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
//    enum GLR_MSG_TYPE
//    {
//        REGISTER = 0,
//        REGISTER_OK = 1,
//        REGISTER_FAIL = 2,
//        APPLICATION = 3,
//    };
//#pragma  pack(1)
//    struct GLR_MSG
//    {
//        //uint32_t Len;
//        unsigned char Type;
//        uint32_t      Gpid;
//        //struct 
//        //{
//        //    char Host[20];
//        //    uint32_t  Port;
//        //    uint32_t  GPid;
//        //} Source;
//        //struct  
//        //{
//        //    char     _[20];
//        //    uint32_t Serial;
//        //    uint32_t  GPid;
//        //}Desination;
//        char Content[1];
//        GLR_MSG()
//        {
//            memset(this, 0, sizeof(*this));
//        }
//    };
//#pragma pack(1)
//    typedef struct{
//        char   Host[20];              // Host
//        uint32_t   Port;              // Port
//        uint32_t   Gpid;           
//    }GLR_ADDR;
//    struct GLR_BUS_HEAD 
//    {
//        MSG_HEAD Head;
//        GLR_ADDR Source;
//    };
//#pragma pack()
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
                Buffer.resize(sizeof(GLR_BUS_HEAD)+buf.size(), 0);
                *((uint32_t*)&Buffer[0]) = htonl(buf.size()+sizeof(GLR_BUS_HEAD) -4);
                memcpy(&Buffer[sizeof(GLR_BUS_HEAD)], buf.c_str(), buf.size());

            }
        };
        typedef Galaxy::GalaxyRT::_TypeDef_FIFO<Task>::CFIFOQueue TaskQueue;       
    public:
        MessageLinkStack(Galaxy::GalaxyRT::CSocket*, RouteMap&);
        ~MessageLinkStack();
    public:
        void PutSendTask(const std::string &, int, int);
    public:
        virtual void OnErr(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnRecv(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
        virtual void OnSend(Galaxy::GalaxyRT::CSelector::EV_PAIR &, POLLERTYPE&);
    private:
        void OnMessage(const std::string &);
    public:
        void RegisterTo(const std::string &, int, short timeout);
        int Gpid() const {return _Gpid;}
    public:
        int  _Gpid;
        Task _RecvTask, _SendTask;
        TaskQueue _SendTaskQ;
        RouteMap  &_Router;
        std::string _Id;
        //uint32_t    _Port;

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
        BusWorker(RouteMap &, MLinkMap &, Galaxy::GalaxyRT::CPthreadMutex&, POLLERTYPE &);
        ~BusWorker();
    public:
        virtual void *Run(const Galaxy::GalaxyRT::CThread &t);
        virtual void Clean(const Galaxy::GalaxyRT::CThread &);
    private:

        RouteMap    &_Router;
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
            NODE_GET_ALL_LINKS,
        };
    public:
        BusController(); 
        ~BusController();
        virtual void Request(lua_State *);
    private:
        void GetAllLinks( lua_State *l );
        std::map<std::string,int> GetAllLinks( void);

        void DoNodeReg(lua_State*);
        void DoIsReg(lua_State*);
        void DoNodeSend(lua_State*);
        void DoCheckReg(lua_State*);
    private:
        MLinkMap _LinkMap;
        Galaxy::GalaxyRT::CPthreadMutex _Mutex;
        POLLERTYPE _Poller;
        RouteMap _Router;
        BusWorker  _Worker;
    };
}
