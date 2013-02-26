/* -*- C++ -*- */

/* Time-stamp: <2012-04-17 09:28:33 星期二 by OCaml> */

/**
 * @file PQueue.hpp
 * @author qingant
 */

#ifndef PQUEUE_H
#define PQUEUE_H
#include "Sync.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {
	
#define QUEUE_DEPTH (10240)
#define CELL_PAGE_SIZE (1024)
      typedef struct
      {
         CHAR Magic[4];              // Magic Number
         UINT Size;                  // Share Memory Size
         size_t UnitSize;            // Size of a Unit
         size_t Count;               // Totol Count of Units means Size/UnitSize
         pthread_mutex_t Mutex;     // Mutex
         pthread_cond_t  Condition; // Condition Var
         LONG          SerialNo;     // 序列号
         size_t        First;
         size_t        Last;
         // USHORT        FreeList[1];
         // pthread_mutex_t SerailNoMutex;
         time_t        OpStamp;      // 操作时间戳
         pid_t         Pid;          // 操作进程号
         BYTE          BitMap[1024];
         BYTE          Trunk[1];
      }QueueHeader, *PQueueHeader;

      typedef struct
      {
         bool Used;
         LONG ID;
         // time_t Stamp;
         size_t Next;
         USHORT Len;
         CHAR   MSG[128];
      }QueueMSG, *PQueueMSG;

      typedef struct
      {
         USHORT ID;
         LONG   Waiting;
         pthread_mutex_t Mutex;     // Mutex
         pthread_cond_t  Condition; // Condition Var
      }QClent, *PQClient;

// typedef strcut
// {
//     bool Usable;
//     USHORT Self;
//     USHORT Next;
// }Offset, *POffset;

      class PQueue:public CNonCopyAble
      {
      private:
         PVOID    _Mem;
         PQueueHeader _pHead;
         size_t   _Size;
         SHORT    _Pages;
         std::auto_ptr<CRawCondition> _Cond;
      public:
         friend class  __QueueOperation;
         PQueue(PVOID p, size_t size);
         virtual ~PQueue();

         INT Send(CPVOID buf, size_t len, SHORT timeout = -1) ;
         INT SendNoWait(const VOID *buf, size_t len);
         INT Recv(PVOID buf, size_t len, SHORT timeout = -1) ;
         INT RecvNoWait(PVOID buf, size_t len);
         INT Put(CPVOID buf, size_t len, SHORT timeout = -1) ;
         INT PutNoWait(const VOID *buf, size_t len);
         INT Get(PVOID buf, size_t len, SHORT timeout = -1) ;
         INT GetNoWait(PVOID buf, size_t len) ;

      private:
         bool      __check_and_restore();
         size_t      __trunk_malloc();
         bool      __trunk_free(size_t);
      };

   } /// GalaxyRT
} /// Galaxy

#endif /* PQUEUE_H */
