/* -*- C++ -*- */

/* Time-stamp: <2012-04-23 08:20:18 星期一 by OCaml> */

/**
 * @file Sync.hpp
 * @author qingant
 */

#ifndef SYNC_H
#define SYNC_H
#include <cstub.h>
#include "Object.hpp"
#include "Exception.hpp"

#pragma once

namespace Galaxy
{
   namespace GalaxyRT
   {
	
// Exceptions
// CLockAcquired: exception used to indicate that a lock cannot be acquired
      class CLockAcquired:public CException
      {
      public:
         GALA_DEFINE_EXCEPTION(CLockAcquired, CException);
      };

      class CMutexBusyError: public CException
      {
      public:
         CMutexBusyError(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
			:CException(file, func, line, errcode, control, msg){}
         ~CMutexBusyError() throw(){}
         std::string GetClassName() const
         {
			return std::string("CMutexBusyError");
         }
      };

      class CSemTryWaitError: public CException
      {
      public:
         CSemTryWaitError(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
			:CException(file, func, line, errcode, control, msg){}
         ~CSemTryWaitError() throw(){}
         std::string GetClassName() const
         {
			return std::string("CSemTryWaitError");
         }
      };

      class CRwlockBusyError: public CException
      {
      public:
         CRwlockBusyError(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
			:CException(file, func, line, errcode, control, msg){}
         ~CRwlockBusyError() throw(){}
         std::string GetClassName() const
         {
			return std::string("CRwlockBusyError");
         }
      };

      
      class CMutex:public CNonCopyAble
      {
      public :
         virtual void Lock() const = 0;
         virtual void UnLock() const = 0;
         virtual void TryLock() const = 0;
         virtual ~CMutex();
      };

      class CPthreadMutex:public CMutex
      {
      public:
         CPthreadMutex();
         void Lock() const;
         void UnLock() const;
         void TryLock() const;
         pthread_mutex_t &GetMutex() ;
         virtual ~CPthreadMutex();
      private:
         mutable pthread_mutex_t _mutex;
      };
      
      class CRawMutex:public CMutex
      {
      public:
         CRawMutex(pthread_mutex_t *m);
         void Lock() const;
         void UnLock() const;
         void TryLock() const;
         pthread_mutex_t &GetMutex()
         {
            return *_Mutex;
         }
         ~CRawMutex();
      private:
         pthread_mutex_t *_Mutex;
      };

      class CSemaphore:public CMutex
      {
      public:
         enum {
			PERM_DEFAULT = (0400|0200),
         };

      public:

         CSemaphore(const std::string &name, mode_t mode = PERM_DEFAULT);
         INT Test() const;
         void Lock() const;
         void TryLock() const;
         void UnLock() const;
         INT Remove();
         INT Close();
         virtual ~CSemaphore();
      private:
         //CMutex();
         INT Open(const std::string &name, mode_t mode, UINT defaultValue = 1);
         sem_t *_psem;
         std::string _name;
      };


      class CLockGuard:public CNonCopyAble
      {
      public:
         CLockGuard(const CMutex *lock);
         ~CLockGuard();
      private:
         const CMutex *_lock;
      };


      class CCondition:public CNonCopyAble
      {
      public:
         CCondition();
         bool Wait() ;
         void NotifyOne();
         void NotifyAll();
         CPthreadMutex &GetLock();
         ~CCondition();

      private:
         pthread_cond_t _cond_var;
         CPthreadMutex _lock;
      };
      
      class CRawCondition:public CNonCopyAble
      {
      public:
         CRawCondition(pthread_cond_t*, pthread_mutex_t*);
         bool Wait();
         bool Wait(USHORT timeout);
         void NotifyOne();
         void NotifyAll();
         CRawMutex &GetLock();
         ~CRawCondition();

      private:
         pthread_cond_t *_cond_var;
         CRawMutex _lock;
      };

      class CRWLockInterface:public CNonCopyAble
      {
      public:
         enum LOCK_TYPE {
			RDLOCK,
			WRLOCK
         };
    
         virtual void ReadLock() const = 0;
         virtual void WriteLock() const = 0;	
         virtual void UnLock() const = 0;
         virtual void TryReadLock() const = 0;
         virtual void TryWriteLock() const = 0;

         virtual ~CRWLockInterface()
         {
               
         }
      };

      class CRWLock:public CRWLockInterface
      {
      public:

         CRWLock();
         void ReadLock() const;
         void TryReadLock() const;
         void WriteLock() const;
         void TryWriteLock() const;
         void UnLock() const;
         virtual ~CRWLock();
      private:
         mutable pthread_rwlock_t _Lock;
      };
      
      class CFileRWLock:public CRWLockInterface
      {
      public:

         CFileRWLock(const std::string &file);
         void ReadLock()const;
         void TryReadLock()const;	  
         void WriteLock()const;
         void TryWriteLock()const;
         void UnLock()const;
         bool IsLock()const;
         bool IsLock(pid_t  lockedpid)const;
         virtual ~CFileRWLock();
      private:
         INT _Fd;
      };
 
      class CRWLockAdapter:public CMutex
      {
      public:
         CRWLockAdapter(CRWLockInterface &lock, CRWLockInterface::LOCK_TYPE type);
         void Lock() const;
         void UnLock() const;
         void TryLock() const;
         virtual ~CRWLockAdapter();
      protected:
         CRWLockInterface &_Lock;
         CRWLock::LOCK_TYPE _Type;
      };

      class CFileLock:public CRWLockAdapter
      {
      public:
         CFileLock(const std::string &file)
			:CRWLockAdapter(*(new CFileRWLock(file)), CRWLockInterface::WRLOCK),
			 _FLock((CFileRWLock*)&_Lock)
         {}
         bool IsLocked(const pid_t &pid) const
         {
            return _FLock->IsLock(pid);
         }
         bool IsLocked() const
         {
            return _FLock->IsLock();
         }
     

      private:
         std::auto_ptr<CFileRWLock> _FLock;
      };

   } /// namespace GalaxyRT
} /// namespace Galaxy

#endif /* SYNC_H */

