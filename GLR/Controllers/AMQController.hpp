#ifndef __AMQCONTROLLER__HPP
#define __AMQCONTROLLER__HPP
#include "Bus.hpp"
#include "Runtime/Thread.hpp"
#include "LuaNode.hpp"
#include <string>

#include "AMQ.hpp"

#include "ControllerInterface.hpp"

#pragma once

#if 0
#pragma pack(1) 
struct AMQAddr{
    int m_queueno;
    LN_ID_TYPE m_gpid;
};

struct AMQHeader
{
    AMQAddr m_to;
    AMQAddr m_from;

//    uint32_t m_corrId;  //会话号
//    uint32_t m_MsgId;	//自产生的MsgId，只有From

    unsigned char m_content[0];
};
#pragma pack() 
#endif


#define ValidQueue(q) (((q)>=0)&&(q)<65535)
#define INVALID_QUEUE  -1


namespace GLR
{

    typedef GLRPROTOCOL AMQHeader;
    class AMQWorker: public Galaxy::GalaxyRT::CRunnable
    {
    public:
        AMQWorker(const Galaxy::AMQ::CSQSuite &nq);
        ~AMQWorker();
    public:
        virtual void *Run(const Galaxy::GalaxyRT::CThread &t);
        virtual void Clean(const Galaxy::GalaxyRT::CThread &);
    private:
        const Galaxy::AMQ::CSQSuite &m_nq;
    };

    class BlockBuffer
    {
    public:
        BlockBuffer()
            :m_buf(NULL),m_size(0)
        {
        }
        void resize(UINT len)
        {
            if (m_buf!=NULL)
            {
                delete [] m_buf;
            }
            m_buf=new char[len];
            m_size=len;
        }

        UINT size() const
        {
            return m_size;
        }

        PSTR get() const
        {
            return (PSTR)m_buf;
        }

        ~BlockBuffer()
        {
            delete [] m_buf;
        }
    private:
        char *m_buf;
        UINT m_size;
    };


    class AMQController : public IController
    {
        enum AMQ_CMD
        {
            AMQ_PUT,
            AMQ_GET,
            AMQ_SET_QUEUE,
        };

    public:
        AMQController(const std::string &path,int queueno=INVALID_QUEUE);
        ~AMQController();

        using IController::Request;
        virtual void Request( lua_State*);

    private:
        void Put(lua_State *);
        void Get(lua_State *);
        void InvalidType(lua_State *l);
        void SetQueue(lua_State *l);
    private:
        void StartWorker(int queue);
    private:
        const std::string m_amq_path;
        int m_queue;
        Galaxy::AMQ::CGalaxyMQ m_amq;
        AMQWorker *m_worker;
        Galaxy::GalaxyRT::CThread *m_thread;
    };
}

#endif //__AMQCONTROLLER__HPP
