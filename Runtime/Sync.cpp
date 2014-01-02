
/* -*- C++ -*- */

/* Time-stamp: <2012-04-23 08:21:40 星期一 by OCaml> */

/**
* @file Sync.cpp
* @author qingant
*/


#include "Sync.hpp"

namespace Galaxy
{
    namespace GalaxyRT
    {

#define MY_THROW(msg) (throw CException(__FILE__, __PRETTY_FUNCTION__, __LINE__, errno, 0, msg) )

        CMutex::~CMutex()
        {

        }

        CPthreadMutex::CPthreadMutex()
        {
            INT  rc = pthread_mutex_init(&_mutex, NULL);
            if (rc != 0)
            {
                THROW_SYSERROR( );
            }
        }

        pthread_mutex_t &CPthreadMutex::GetMutex()
        {
            return _mutex;
        }

        void CPthreadMutex::Lock() const
        {
            INT rc = pthread_mutex_lock(&_mutex);
            if (rc != 0)
            {
                THROW_SYSERROR( );
            }

        }

        void CPthreadMutex::UnLock() const
        {
            INT rc = pthread_mutex_unlock(&_mutex);
            if (rc != 0)
            {
                THROW_SYSERROR( );
            }

        }
        void CPthreadMutex::TryLock() const
        {
            INT rc = pthread_mutex_trylock(&_mutex);

            if( rc == EBUSY )
            {
                THROW_GENERALERROR( CMutexBusyError, "The mutex could not be acquired because it is already locked" );
            }
            else if (rc != 0)
            {
                THROW_SYSERROR( );
            }
        }
        CPthreadMutex::~CPthreadMutex()
        {
            INT rc = pthread_mutex_destroy(&_mutex);
            if (rc != 0)
            {
                THROW_SYSERROR( );
            }
        }


        CRawMutex::CRawMutex(pthread_mutex_t *m)
            :_Mutex(m)
        {

        }

        CRawMutex::~CRawMutex()
        {

        }
        void CRawMutex::Lock() const
        {
            INT rc = pthread_mutex_lock(_Mutex);
            if (rc != 0)
            {
                THROW_SYSERROR( );
            }
        }

        void CRawMutex::UnLock() const
        {
            INT rc = pthread_mutex_unlock(_Mutex);
            if (rc != 0)
            {
                THROW_SYSERROR( );
            }


        }
        void CRawMutex::TryLock() const
        {
            INT rc = pthread_mutex_trylock(_Mutex);
            if ( rc == EBUSY)
            {
                THROW_GENERALERROR( CMutexBusyError, "The mutex could not be acquired because it is already locked" );
            }
            else if (rc != 0)
            {
                THROW_SYSERROR( );
            }
        }




        CSemaphore::CSemaphore(const std::string &name, mode_t mode):_name(name)
        {
            Open(name, mode);
        }
        INT CSemaphore::Open(const std::string &name, mode_t mode, UINT defaultValue)
        {
            INT flags = O_RDWR|O_CREAT;
            // INT flags = O_RDWR;

            _psem = CRT_sem_open(name.c_str(), flags, mode, defaultValue);
            //Name = name;
            if (_psem == SEM_FAILED)
            {
                THROW_SYSERROR( );
            }
            return 1;
        }


        INT CSemaphore::Test() const
        {
            INT val;
            int rc = CRT_sem_getvalue(_psem, &val);
            if ( rc == -1 )
            {
                THROW_SYSERROR( );
            }
            return val;
        }


        void CSemaphore::Lock() const
        {
            CRT_sem_wait(_psem);
        }


        void CSemaphore::TryLock() const
        {
            INT rc =  CRT_sem_trywait(_psem);
            if( rc == EAGAIN)
            {
                THROW_GENERALERROR( CSemTryWaitError, "CSemphore Error" );
            }
            else if (rc != 0)
            {
                THROW_SYSERROR( );
            }
        }

        void CSemaphore::UnLock() const
        {
            int rc = CRT_sem_post(_psem);
            if( rc != 0 )
            {
                THROW_SYSERROR( );
            }
        }

        INT CSemaphore::Remove()
        {
            int rc = CRT_sem_unlink(_name.c_str());
            if( rc != 0 )
            {
                THROW_SYSERROR( );
            }

            return rc;
        }

        INT CSemaphore::Close()
        {
            return CRT_sem_close(_psem);

        }
        CSemaphore::~CSemaphore()
        {
            Close();
        }


        CLockGuard::CLockGuard(const CMutex *lock, bool isSync)
            :_lock(lock)
        {
            if (isSync)
            {
                _lock->Lock();
            }else
            {
                _lock->TryLock();
            }

        }

        CLockGuard::~CLockGuard()
        {
            _lock->UnLock();
        }




        CCondition::CCondition()
        {
            pthread_cond_init(&_cond_var, NULL);
        }
        bool CCondition::Wait()
        {
            INT ret = pthread_cond_wait(&_cond_var, &_lock.GetMutex());
            if (ret!=0)
            {
                std::string errmsg(1024,0);
                CRT_snprintf(&errmsg[0], sizeof(errmsg), "pthread_cond_wait error with reason %d", ret);
                THROW_GENERALERROR(CException,errmsg);
            }
            return true;

        }
        void CCondition::NotifyOne()
        {
            INT ret = pthread_cond_signal(&_cond_var);
            if (ret!=0)
            {
                std::string errmsg(1024,0);
                CRT_snprintf(&errmsg[0], sizeof(errmsg), "pthread_cond_wait error with reason %d", ret);
                THROW_GENERALERROR(CException,errmsg);
            }

        }
        void CCondition::NotifyAll()
        {
            INT ret = pthread_cond_broadcast(&_cond_var);
            if (ret!=0)
            {
                std::string errmsg(1024,0);
                CRT_snprintf(&errmsg[0], sizeof(errmsg), "pthread_cond_wait error with reason %d", ret);
                THROW_GENERALERROR(CException,errmsg);
            }

        }
        CPthreadMutex &CCondition::GetLock()
        {
            return _lock;

        }

        CCondition::~CCondition()
        {
            pthread_cond_destroy(&_cond_var);
        }


        CRWLock::CRWLock()
        {
            pthread_rwlock_init(&_Lock, NULL);
        }

        void CRWLock::ReadLock() const
        {
            INT rc = pthread_rwlock_rdlock(&_Lock);
            if (rc != 0)
            {
                CHAR sztmp[2048];
                CRT_snprintf(sztmp, sizeof(sztmp), "pthread_rwlock_lock failed with %d returned", rc);
                MY_THROW(sztmp);
            }
        }
        void CRWLock::TryReadLock() const
        {
            INT rc = pthread_rwlock_tryrdlock(&_Lock);
            if( rc == EBUSY )
            {
                THROW_GENERALERROR( CRwlockBusyError, "Lock busy now" );
            }
            else if (rc != 0)
            {
                CHAR sztmp[2048];
                CRT_snprintf(sztmp, sizeof(sztmp), "pthread_rwlock_lock failed with %d returned", rc);
                MY_THROW(sztmp);
            }
        }
        void CRWLock::WriteLock() const
        {
            INT rc = pthread_rwlock_wrlock(&_Lock);
            if (rc != 0)
            {
                CHAR sztmp[2048];
                CRT_snprintf(sztmp, sizeof(sztmp), "pthread_rwlock_lock failed with %d returned", rc);
                MY_THROW(sztmp);
            }
        }
        void CRWLock::TryWriteLock() const
        {
            INT rc = pthread_rwlock_trywrlock(&_Lock);
            if( rc == EBUSY )
            {
                THROW_GENERALERROR( CRwlockBusyError, "Lock busy now" );
            }
            else if (rc != 0)
            {
                THROW_SYSERROR( );
            }
        }
        void CRWLock::UnLock() const
        {
            INT rc = pthread_rwlock_unlock(&_Lock);
            if (rc != 0)
            {
                THROW_SYSERROR( );
            }
        }
        CRWLock::~CRWLock()
        {
            INT rc = pthread_rwlock_destroy(&_Lock);
            if (rc != 0)
            {
                THROW_SYSERROR( );
            }
        }



        CRWLockAdapter::CRWLockAdapter(CRWLockInterface &lock, CRWLock::LOCK_TYPE type)
            :_Lock(lock),
            _Type(type)
        {

        }
        void CRWLockAdapter::Lock() const
        {
            if (_Type == CRWLock::RDLOCK)
            {
                _Lock.ReadLock();
            }
            else
            {
                _Lock.WriteLock();
            }
        }

        void CRWLockAdapter::UnLock() const
        {
            _Lock.UnLock();
        }
        void CRWLockAdapter::TryLock() const
        {
            if (_Type == CRWLock::RDLOCK)
            {
                _Lock.TryReadLock();
            }
            else
            {
                _Lock.TryWriteLock();
            }
        }

        CRWLockAdapter::~CRWLockAdapter()
        {

        }

        //CFileRWLock

        CFileRWLock::CFileRWLock(const std::string &file)
            :_Fd(::open(file.c_str(), O_RDWR|O_CREAT, S_IRWXU))
        {
            if (_Fd == -1)
            {
                THROW_ERROR(CLockAcquired);
            }	
        }

        void CFileRWLock::ReadLock()const
        {
            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            if(::fcntl(_Fd, F_SETLKW, &lock)!=0)
            {
                THROW_ERROR(CLockAcquired);    	
            }	
        }

        void CFileRWLock::TryReadLock()const
        {
            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            if(::fcntl(_Fd, F_SETLK, &lock)!=0)
            {
                THROW_ERROR(CLockAcquired);    	
            }	
        }

        void CFileRWLock::WriteLock()const
        {
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            if(::fcntl(_Fd, F_SETLKW, &lock)!=0)
            {
                THROW_ERROR(CLockAcquired);    	
            }		
        }
        void CFileRWLock::TryWriteLock()const
        {
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            if(::fcntl(_Fd, F_SETLK, &lock)!=0)
            {
                THROW_ERROR(CLockAcquired);    	
            }		
        }
        void CFileRWLock::UnLock()const
        {
            struct flock lock;
            lock.l_type = F_UNLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;

            if(::fcntl(_Fd, F_SETLK, &lock)!=0)
            {
                THROW_ERROR(CLockAcquired);    	
            }	
        }
        bool CFileRWLock::IsLock()const
        {
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            int rt = ::fcntl(_Fd, F_GETLK, &lock);
            bool locked =true;
            if (rt!=0)
            {
                THROW_ERROR(CLockAcquired);         
            }

            else
            {
                if(F_UNLCK == lock.l_type)
                    locked =false;
            }
            return locked;	
        }
        bool CFileRWLock::IsLock(pid_t  &lockedpid)const
        {
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            lock.l_pid = -1;
            int rt = ::fcntl(_Fd, F_GETLK, &lock);
            bool locked =true;
            if (rt!=0)
                THROW_ERROR(CLockAcquired);
            else
            {
                if(F_UNLCK == lock.l_type)
                    locked =false;
                else
                    lockedpid =lock.l_pid;
            }
            return locked;	
        }
        CFileRWLock::~CFileRWLock()
        {
            CRT_close(_Fd);
            _Fd = -1;	
        }

        //CFileLockAdapter

        // CRawCondition




        CRawCondition::CRawCondition(pthread_cond_t *cond, pthread_mutex_t *mutex)
            :_cond_var(cond),
            _lock(mutex)
        {
        }
        bool CRawCondition::Wait()
        {
            INT ret = pthread_cond_wait(_cond_var, &_lock.GetMutex());
            if (ret!=0)
            {
                std::string errmsg(1024,0);
                CRT_snprintf(&errmsg[0], sizeof(errmsg), "pthread_cond_wait error with reason %d", ret);
                THROW_GENERALERROR(CException,errmsg);
            }
            return true;

        }
        bool CRawCondition::Wait(USHORT timeout)
        {
            struct timespec ts = {timeout, 0};
            INT ret = pthread_cond_timedwait(_cond_var, &_lock.GetMutex(), &ts);
            if (ret == 0)
            {
                return true;
            }
            else if (ret !=0 && errno == ETIMEDOUT)
            {
                return false;
            }
            else
            {
                std::string errmsg(1024,0);
                CRT_snprintf(&errmsg[0], sizeof(errmsg), "pthread_cond_timedwait error with reason %d", ret);
                THROW_GENERALERROR(CException,errmsg);
                return false;
            }

        }
        void CRawCondition::NotifyOne()
        {
            INT ret = pthread_cond_signal(_cond_var);
            if (ret!=0)
            {
                std::string errmsg(1024,0);
                CRT_snprintf(&errmsg[0], sizeof(errmsg), "pthread_cond_wait error with reason %d", ret);
                THROW_GENERALERROR(CException,errmsg);
            }

        }
        void CRawCondition::NotifyAll()
        {
            INT ret = pthread_cond_broadcast(_cond_var);
            if (ret!=0)
            {
                std::string errmsg(1024,0);
                CRT_snprintf(&errmsg[0], sizeof(errmsg), "pthread_cond_wait error with reason %d", ret);
                THROW_GENERALERROR(CException,errmsg);
            }

        }
        CRawMutex &CRawCondition::GetLock()
        {
            return _lock;

        }

        CRawCondition::~CRawCondition()
        {
            // pthread_cond_destroy(&_cond_var);
        }

    } ///namespace GalaxyRT
} ///  namespace Galaxy
