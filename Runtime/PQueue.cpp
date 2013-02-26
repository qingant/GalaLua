/* -*- C++ -*- */

// Time-stamp: <2012-04-17 10:00:36 星期二 by OCaml>

/**
 * @file    PQueue.cpp
 * @author  Ma Tao <qingant@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 *
 * @section DESCRIPTION
 *
 *
 */

#include "PQueue.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {

      class __QueueOperation
      {
      public:
         __QueueOperation(PQueue *p)
         {
            CRT_time(&(p->_pHead->OpStamp));
            p->_pHead->Pid = CRT_getpid();
         }
         ~__QueueOperation()
         {

         }
      };
      PQueue::PQueue(PVOID p, size_t size)
         :_Mem(p),
          _pHead((PQueueHeader)p),
          _Size(size)
      {
         PQueueHeader pHead = (PQueueHeader)_Mem;
         if (CRT_memcmp(pHead->Magic,"G1V1", 4) != 0)
         {
			CRT_memcpy(pHead->Magic, "G1V1", 4);
			pHead->Size = _Size;
			pHead->UnitSize = sizeof(QueueMSG);
			pHead->Count = (_Size - sizeof(*pHead))/pHead->UnitSize;
			pthread_mutexattr_t mattr;
			pthread_condattr_t  cattr;
			if (!(pthread_mutexattr_init(&mattr) == 0 && pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED) == 0))
			{
               pthread_mutexattr_destroy(&mattr);
               THROW_EXCEPTION_EX("Pthread Mutex Attr Init Failure");
			}

			if (!(pthread_condattr_init(&cattr) == 0 && pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED) == 0))
			{
               pthread_condattr_destroy(&cattr);
               THROW_EXCEPTION_EX("Pthread Mutex Attr Init Failure");
			}

			if (pthread_mutex_init(&pHead->Mutex, &mattr) != 0)
			{
               THROW_EXCEPTION_EX("Pthread Mutex Init Failure");
			}
			if (pthread_cond_init(&pHead->Condition, &cattr) != 0)
			{
               THROW_EXCEPTION_EX("Pthread Mutex Init Failure");
			}
			pthread_mutexattr_destroy(&mattr);
			pthread_condattr_destroy(&cattr);
			CRT_memset(pHead->BitMap, 0, sizeof(pHead->BitMap));
			pHead->SerialNo = 0;
			pHead->First = pHead->Last = 0;
         }
         _Pages = (pHead->Count-1)/CELL_PAGE_SIZE;
         _Cond.reset(new CRawCondition(&pHead->Condition, &pHead->Mutex));
      }
      PQueue::~PQueue()
      {

      }

      INT PQueue::Send(CPVOID buf, size_t len, SHORT timeout)
      {
         return Put(buf, len, timeout);
      }
      INT PQueue::SendNoWait(const VOID *buf, size_t len)
      {
         return PutNoWait(buf, len);
      }
      INT PQueue::Recv(PVOID buf, size_t len, SHORT timeout)
      {
         return Get(buf, len, timeout);
      }
      INT PQueue::RecvNoWait(PVOID buf, size_t len)
      {
         return GetNoWait(buf, len);
      }
      INT PQueue::Put(CPVOID buf, size_t len, SHORT timeout )
      {
         __check_and_restore();
         CLockGuard lock(&_Cond->GetLock());
         PQueueHeader pHead = ((PQueueHeader)_Mem);
         PQueueMSG pTrunk = (PQueueMSG)pHead->Trunk;
         size_t idx = 0;
         time_t start = CRT_time(NULL);
         if (timeout == -1)
         {
			while ((idx = __trunk_malloc()) == 0)
			{
               _Cond->Wait();
               printf("Wait Return\n");

			}
         }
         else
         {

			while ((idx = __trunk_malloc()) == 0)
			{
               if (timeout <= 0 || (!_Cond->Wait(timeout)))
               {
                  THROW_EXCEPTION_EX("Put TimeOut");

               }
               else
               {
                  time_t now = CRT_time(NULL);
                  timeout -= (now - start);
               }
			}
         }

         PQueueMSG pMsg = &pTrunk[idx];

         pMsg->ID = ++pHead->SerialNo;
         // CRT_time(&pMsg->Stamp);
         size_t length = len < pHead->UnitSize?len:pHead->UnitSize;
         CRT_memset(&pMsg->MSG, 0, sizeof(pMsg->MSG));
         CRT_memcpy(pMsg->MSG, buf, length);
         pMsg->Len = length;
         pTrunk[pHead->Last].Next = idx;
         pHead->Last = idx;
         __QueueOperation op(this);
         lock.~CLockGuard();
         _Cond->NotifyOne();
         return 0;
      }
      INT PQueue::PutNoWait(const VOID *buf, size_t len)
      {
         return this->Put(buf, len, 0);
      }
      bool PQueue::__check_and_restore()
      {
         CHAR _pid[256] = {0};
         CRT_snprintf(_pid, sizeof(_pid), "/proc/%d/", _pHead->Pid);
         if (CRT_access(_pid, F_OK) != 0)
         {
			try
			{
               _Cond->GetLock().TryLock();
               _Cond->GetLock().UnLock();


			}
			catch (const CException &e)
			{
               _Cond->GetLock().UnLock();
			}
			return false;
         }
         return true;
      }

      INT PQueue::Get(PVOID buf, size_t len, SHORT timeout )
      {
         __check_and_restore();
         CLockGuard lock(&_Cond->GetLock());
         __QueueOperation op(this);

         PQueueHeader pHead = ((PQueueHeader)_Mem);
         PQueueMSG pTrunk = (PQueueMSG)pHead->Trunk;
         time_t start = CRT_time(NULL);

         if (timeout == -1)
         {
			while (0 == pHead->Last)
			{
               _Cond->Wait();
			}
         }
         else
         {

			while (0 == pHead->Last)
			{
               if (timeout <= 0 || (!_Cond->Wait(timeout)))
               {
                  THROW_EXCEPTION_EX("Get TimeOut");

               }
               else
               {
                  time_t now = CRT_time(NULL);
                  timeout -= (now - start);
               }
			}
         }
         if (pTrunk[0].Next == pHead->Last)
         {


			size_t tmp;
			tmp = pTrunk[0].Next;


			pHead->Last = 0;
			pTrunk[0].Next = -1;

			size_t size = pTrunk[tmp].Len;
			int length = size < len?size:len;
			CRT_memcpy(buf, (CPSTR)&pTrunk[tmp].MSG, length);
			__trunk_free(tmp);


         }
         else
         {
			size_t tmp;
			tmp = pTrunk[0].Next;
			pTrunk[0].Next = pTrunk[tmp].Next;
			size_t size = pTrunk[tmp].Len;
			size_t length = size < len?size:len;
			CRT_memcpy(buf, (CPSTR)&pTrunk[tmp].MSG, length);
			__trunk_free(tmp);

         }
         lock.~CLockGuard();
         _Cond->NotifyOne();
         return 0;
      }
      INT PQueue::GetNoWait(PVOID buf, size_t len)
      {
         return Get(buf, len, 0);
      }

      size_t PQueue::__trunk_malloc()
      {
         PQueueHeader pHead = ((PQueueHeader)_Mem);
         PQueueMSG pMsg = (PQueueMSG)pHead->Trunk;
         SHORT pg_idx = 0;
         for (;pHead->BitMap[pg_idx] != 0 && pg_idx != _Pages; ++pg_idx);

         if (pg_idx == _Pages || pHead->BitMap[pg_idx] != 0)
         {
			return 0;
         }
         for (size_t i = 1+pg_idx*CELL_PAGE_SIZE; i != (size_t)(pg_idx+1)*CELL_PAGE_SIZE+1; ++i )
         {
			if (!pMsg[i].Used)
			{
               if (i == (size_t)(pg_idx+1)*CELL_PAGE_SIZE)
               {
                  pHead->BitMap[(i - 1)/CELL_PAGE_SIZE] = 1;
               }
               pMsg[i].Used = true;
               return i;
			}
         }
         return 0;
      }
      bool      PQueue::__trunk_free(size_t idx)
      {
         PQueueHeader pHead = ((PQueueHeader)_Mem);
         PQueueMSG pMsg = (PQueueMSG)pHead->Trunk;
         SHORT pgs = sizeof(pHead->BitMap);

         pHead->BitMap[(idx - 1)/pgs] = 0;
         pMsg[idx].Used = false;
         return true;
      }

   } /// namespace GalaxyRT
} /// namepace Galaxy
