
#include "MQMutex.hpp"
#include "MQException.hpp"

#define  _MQMUTEXMASK_		(0xDCDDDEDF)

using namespace Galaxy::AMQ;

/*CMutexSuite*/
CMutexSuite::CMutexSuite(SQMUTEX &_TheMutex)
   :_Mutex(_TheMutex)
{
	if(_MQMUTEXMASK_!=_Mutex._MSK)
	{
		THROW_MQEXCEPTION("Invalid Lock Object");	
	}	
}

CMutexSuite::~CMutexSuite()
{

}

pthread_mutex_t *CMutexSuite::Entry() const
{    
   return &(_Mutex._MTX);
}

const pthread_mutex_t *CMutexSuite::GetRAWMutex() const
{
   return Entry();    
}
    
void CMutexSuite::Lock() const
{
   INT rc = CRT_pthread_mutex_lock(Entry());
   if (rc != 0)
   {
      THROW_MQEXCEPTION("");
      return;
   }
   
   _Mutex._STA = 1;
}

void CMutexSuite::UnLock() const
{
   _Mutex._STA = 0;
   INT rc = CRT_pthread_mutex_unlock(Entry());
   if (rc != 0)
   {        
      THROW_MQEXCEPTION("");
      return;
   } 
}

bool CMutexSuite::TryLock() const
{
	INT rc = CRT_pthread_mutex_trylock(Entry());
	if(rc==0)
	{
		_Mutex._STA = 1;
		return true;
	}
	else
	{
		return false;	
	}
}

bool CMutexSuite::TimedLock(USHORT timeout) const
{
   struct timespec _tm;

   _tm.tv_sec = timeout;
   _tm.tv_nsec = 0;
   
   INT rc = CRT_pthread_mutex_timedlock(Entry(), &_tm);
   if(rc == 0)
   {
      _Mutex._STA = 1;

      return true;   
   }
   else if (rc == ETIMEDOUT)
   {
      return false; 
   }
   else
   {
      THROW_MQEXCEPTION("");
      return false;
   }
}

bool CMutexSuite::Locked() const
{
	return (_Mutex._STA!=0);		
}




/*CMutexCreator*/
CMutexCreator::CMutexCreator(SQMUTEX &_TheMutex)
{
   INT                     rc;
   pthread_mutexattr_t     _Attr;
    
   CRT_memset(&_Attr,0,sizeof(_Attr));
    
   rc = CRT_pthread_mutexattr_init(&_Attr);
   if (rc != 0)
   {
      THROW_MQEXCEPTION("");
      return;
   }

   rc =  CRT_pthread_mutexattr_setpshared(&_Attr, PTHREAD_PROCESS_SHARED);    
   if (rc != 0)
   {
      THROW_MQEXCEPTION("");
      return;
   }
    
   CRT_memset(&_TheMutex,0,sizeof(SQMUTEX));
    
   rc = CRT_pthread_mutex_init(&(_TheMutex._MTX), &_Attr);
   if (rc != 0)
   {
      THROW_MQEXCEPTION("");
      return;
   }
   
   _TheMutex._STA = 0;
   _TheMutex._MSK = _MQMUTEXMASK_;
}

CMutexCreator::~CMutexCreator()
{
    
}


/*CMutexLockGuard*/
CMutexLockGuard::CMutexLockGuard(const CMutexSuite &_TheMutex)
   :_Mutex(_TheMutex)
{
   _Mutex.Lock();    
}

CMutexLockGuard::~CMutexLockGuard()
{
   _Mutex.UnLock();    
}
