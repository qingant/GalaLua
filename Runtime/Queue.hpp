
/* -*- C++ -*- */

/* Time-stamp: <2012-04-17 10:00:27 星期二 by OCaml> */

/**
* @file Queue.hpp
* @author Ma Tao
*/

#ifndef CQUEUE_H
#define CQUEUE_H


#include "cstub.h"
#include <queue>
#include <vector>
#include "Object.hpp"
#include "Sync.hpp"
#include "Exception.hpp"

namespace Galaxy
{
    namespace GalaxyRT
    {

        class CQueueClosedException:public CException
        {
        public:
            CQueueClosedException(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
                :CException(file, func, line,errcode,control,msg)
            {

            }
            ~CQueueClosedException() throw()
            {

            }
            std::string GetClassName() const
            {
                return std::string("CQueueClosedException");
            }
        };
        class CQueueEmpty:public CException
        {
        public:
            CQueueEmpty(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
                :CException(file, func, line, errcode, control,msg)
            {

            }
            ~CQueueEmpty() throw()
            {

            }
            std::string GetClassName() const
            {
                return std::string("CQueueEmpty");
            }
        };
        template <typename T>
        class CQEmptyGuard
        {
        public:
            CQEmptyGuard(T &r):_Resouce(r)
            {
                _Resouce.LockIfEmpty();
            }
            ~CQEmptyGuard()
            {
                _Resouce.UnLock();
            }
        private:
            static void* operator new(size_t size);    
            T &_Resouce;
        };

        template <typename QueueType, typename DataType>
        class CQueue: public CObject
        {
        public :
            CQueue():m_closed(false),m_shutdown(false),WaitNo(0)
            {
                //IsEmpty = PTHREAD_COND_INITIALIZER;
                //HMutex = TMutex = PTHREAD_MUTEX_INITIALIZER;
                //m_closed = false;
                //m_shutdown = false;
                //WaitNo = 0;

            }
            DataType Get()
            {
                CLockGuard lock(&m_cond.GetLock());
                while (Queue.empty() && !m_closed)
                {
                    ++WaitNo;
                    m_cond.Wait();
                }
                if (m_shutdown || (m_closed && Queue.empty()))
                {
                    THROW_GENERALERROR(CQueueClosedException,"queue closed");
                }
                DataType tmp = Queue.front();
                Queue.pop();
                return tmp;
            }
            void     LockIfEmpty()
            {
                m_cond.GetLock().Lock();
                if (!Queue.empty())
                {
                    m_cond.GetLock().UnLock();
                    THROW_EXCEPTION_EX("Queue Not Empty");
                }
                
            }
            void     UnLock()
            {
                m_cond.GetLock().UnLock();
            }
            DataType &Head()
            {
                CLockGuard lock(&m_cond.GetLock());
                return Queue.front();
            }
            DataType TryGet()
            {
                CLockGuard lock(&m_cond.GetLock());

                if (m_shutdown || Queue.empty())
                {
                    THROW_GENERALERROR(CQueueEmpty, "queue empty");
                }
                DataType tmp(Queue.front());
                Queue.pop();
                return tmp;
            }
            VOID Put(const DataType& el)
            {
                CLockGuard lock(&m_cond.GetLock());
                if (m_closed)
                {
                    THROW_GENERALERROR(CQueueClosedException,"queue closed");
                }
                Queue.push(el);
                if (WaitNo != 0)
                {
                    --WaitNo;
                    m_cond.NotifyOne();
                }
            }
            void Shutdown()
            {
                m_shutdown = true;
                m_closed = true;
                m_cond.NotifyAll();
            }
            void Close()
            {
                m_closed = true;
                m_cond.NotifyAll();
            }
            bool Empty()
            {
                CLockGuard lock(&m_cond.GetLock());

                return Queue.empty();
            }

            size_t Size()
            {
                CLockGuard lock(&m_cond.GetLock());
                return Queue.size();
            }

        private :
            QueueType Queue;
            CCondition m_cond;
            bool m_closed;
            bool m_shutdown;
            INT WaitNo;
        };

        template <typename DataType, typename Compair>
        class CQueueAdapter:public CObject,
            public std::priority_queue<DataType,std::vector<DataType>, Compair>
        {
        public:
            using  std::priority_queue<DataType, std::vector<DataType>, Compair>::top;
            CQueueAdapter()
            {

            }
            const DataType& front() const
            {

                if ( this->empty() )
                {
                    THROW_GENERALERROR( CQueueEmpty, "queue empty" );
                }
                return this->top();
            }
            ~CQueueAdapter()
            {

            }

        };

        // typedef CQueue<std::queue<DataType>, DataType > CFIFOQueue<DataType>;

        template <typename DataType>
        //using CFIFOQueue = CQueue<std::queue<DataType>, DataType >
        struct _TypeDef_FIFO
        {
            typedef CQueue<std::queue<DataType>, DataType > CFIFOQueue;
        };

        template <typename DataType, typename Compair=std::greater<DataType> >
        struct _TypeDef_PRO
        {
            typedef CQueue<CQueueAdapter<DataType, Compair>, DataType > CPriorQueue;
        };

        // using _TypeDef_FIFO::CFIFOQueue;
        // using _TypeDef_PRO::CPriorQueue;

    } /// namespace GalaxyRT
} /// namespace Galaxy

#endif /* CQUEUE_H */
