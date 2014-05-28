#include "MQ_SQSync.hpp"
#include "MQException.hpp"
#include "MQT.hpp"
#include <string.h>
#include <time.h>
#include <errno.h>

namespace Galaxy
{

namespace AMQ
{
/*CMQNoLock*/
void CMQNoLock::Lock() const
{
	
}

void CMQNoLock::UnLock() const
{
	
}


/*CSQLock*/
void CSQLock::Init()
{
	INT                     rc;
	pthread_mutexattr_t     _Attr;
	
	rc = pthread_mutexattr_init(&_Attr);
	if (rc != 0)
	{
		THROW_MQAPIEXCEPTION("pthread_mutexattr_init",rc);
		return;
	}
	
	rc =  pthread_mutexattr_setpshared(&_Attr, PTHREAD_PROCESS_SHARED);    
	if (rc != 0)
	{
		THROW_MQAPIEXCEPTION("pthread_mutexattr_setpshared",rc);
		return;
	}
	
	{  
		rc = pthread_mutex_init(&(_MTX), &_Attr);
		if (rc != 0)
		{
		THROW_MQAPIEXCEPTION("pthread_mutex_init",rc);
		return;
		}
		
		_STA = 0;
	}
	
	//memcpy(&_BGN,"M:>>",4);
	//memcpy(&_END,"<<:M",4);
}

PBYTE CSQLock::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}
	
bool CSQLock::Locked() const
{
	return (_STA!=0);		
}

void CSQLock::Lock() const
{
	CSQLock *_This = (CSQLock *)this;
	INT rc = pthread_mutex_lock(&(_This->_MTX));
	if (rc != 0)
	{
		THROW_MQAPIEXCEPTION("pthread_mutex_lock",rc);
		return;
	}
	
	_This->_STA = 1;
}

void CSQLock::UnLock() const
{
	CSQLock *_This = (CSQLock *)this;
	
	_This->_STA = 0;
	INT rc = pthread_mutex_unlock(&(_This->_MTX));
	if (rc != 0)
	{        
		_This->_STA = 1;
		
		THROW_MQAPIEXCEPTION("pthread_mutex_unlock",rc);
		return;
	} 
}

bool CSQLock::TryLock() const
{
	CSQLock *_This = (CSQLock *)this;
	INT rc = pthread_mutex_trylock(&(_This->_MTX));
	if(rc==0)
	{
		_This->_STA = 1;
		return true;
	}
	else
	{
		return false;	
	}
}

bool CSQLock::TimedLock(SHORT timeout) const
{
	if(timeout < 0)
	{
		Lock();	
		return true;
	}
	else
	{
		CSQLock *_This = (CSQLock *)this;
		struct timespec _tm;
		
		_tm.tv_sec = timeout;
		_tm.tv_nsec = 0;
		
		INT rc = pthread_mutex_timedlock(&(_This->_MTX), &_tm);
		if(rc == 0)
		{ 
			_This->_STA = 1;
			return true;   
		}
		else if (rc == ETIMEDOUT)
		{
			return false; 
		}
		else
		{
			THROW_MQAPIEXCEPTION("pthread_mutex_timedlock",rc);
			return false;
		}
	}
}

pthread_mutex_t *CSQLock::GetMutex() const
{
	return (pthread_mutex_t *) &_MTX;
}


/*CSQCond*/
void CSQCond::Init(CSQLock &_TheLocker)
{
	INT                     rc;
	pthread_condattr_t      _Attr;
	

	rc = pthread_condattr_init(&_Attr);
	if (rc != 0)
	{
		THROW_MQAPIEXCEPTION("pthread_condattr_init",rc);
		return;
	}
	
	rc =  pthread_condattr_setpshared(&_Attr, PTHREAD_PROCESS_SHARED);    
	if (rc != 0)
	{
		THROW_MQAPIEXCEPTION("pthread_condattr_setpshared",rc);
		return;
	}
	
	rc = pthread_cond_init(&(_CND), &_Attr);
	if (rc != 0)
	{
		THROW_MQAPIEXCEPTION("pthread_cond_init",rc);
		return;
	}
	
	_WAT = 0;
	
	_LCK = ((PBYTE)&(_TheLocker)) - ((PBYTE)this);
	
	if(!T_MQEquPtr< CSQLock >(_TheLocker,Locker()))
	{
		THROW_MQEXCEPTION("Init Failed!");
		return;	
	}
}

const CSQLock &CSQCond::Locker() const
{
	CSQLock *_TheLocker = (CSQLock *)(((PBYTE)this) + _LCK);	
	return *_TheLocker;
}

PBYTE CSQCond::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}


UINT CSQCond::Lurkers() const
{
	return _WAT;	
}

void CSQCond::Wait() const
{

   CSQCond *_This = (CSQCond *)this;

   _This->_WAT++;
   INT rc = pthread_cond_wait(&(_This->_CND),Locker().GetMutex());
   if (rc != 0)
   {
      if(_This->_WAT > 0)
      {
		 _This->_WAT--;
      }

      THROW_MQAPIEXCEPTION("pthread_cond_wait",rc);
      return;
   }   
}

void CSQCond::NotifyOne() const
{
	CSQCond *_This = (CSQCond *)this;

	INT rc = pthread_cond_signal(&(_This->_CND));
	if (rc != 0)
	{
		THROW_MQAPIEXCEPTION("pthread_cond_signal",rc);
		return;
	}

	if(_This->_WAT > 0)
	{
		_This->_WAT--;
	}
}

void CSQCond::NotifyAll() const
{
   CSQCond *_This = (CSQCond *)this;
   
   INT rc = pthread_cond_broadcast(&(_This->_CND));
   if (rc != 0)
   {
		THROW_MQAPIEXCEPTION("pthread_cond_broadcast",rc);
		return;
   }

   _This->_WAT = 0;
}

bool CSQCond::TimedWait(SHORT timeout) const
{
	time_t _TheTime = 0;
	struct timespec _tm;
	
	time(&_TheTime);
	
	_tm.tv_sec =  _TheTime + timeout;
	_tm.tv_nsec = 0;
	
	
	CSQCond *_This = (CSQCond *)this;

	_This->_WAT++;
   
	INT rc = pthread_cond_timedwait(&(_This->_CND),(Locker().GetMutex()),&_tm);
	if(rc!= 0)
	{
		if(_This->_WAT > 0)
		{
			_This->_WAT--;
		}
	
		if (rc == ETIMEDOUT)
		{
		  	return false; 
		}
		else
		{
			THROW_MQAPIEXCEPTION("pthread_cond_timedwait",rc);
			return false;
		}
	}
   
	return true;
}



/*CSQEvent*/
void CSQEvent::Init()
{
	_Lock.Init(); //这个在前

	_Cond.Init(_Lock);
}

PBYTE CSQEvent::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}

void CSQEvent::Lock() const
{
	_Lock.Lock();	
}

void CSQEvent::UnLock() const
{
	_Lock.UnLock();	
}

bool CSQEvent::TryLock() const
{
	return _Lock.TryLock();	
}

bool CSQEvent::TimedLock(SHORT timeout) const
{
	return _Lock.TimedLock(timeout);
}

UINT CSQEvent::Lurkers() const
{
	return _Cond.Lurkers();	
}

void CSQEvent::Wait() const
{
	_Cond.Wait();  
}

void CSQEvent::NotifyOne() const
{
	_Cond.NotifyOne();
}

void CSQEvent::NotifyAll() const
{
	_Cond.NotifyAll();
}

bool CSQEvent::TimedWait(SHORT timeout) const
{
	return _Cond.TimedWait(timeout);
}



/*CSQDoor*/
void CSQDoor::Init()
{
	_Lock.Init(); //这个在前

	_Threshold1.Init(_Lock);
	_Threshold2.Init(_Lock);
}

PBYTE CSQDoor::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}

void CSQDoor::Lock() const
{
	_Lock.Lock();	
}

void CSQDoor::UnLock() const
{
	_Lock.UnLock();	
}

bool CSQDoor::TryLock() const
{
	return _Lock.TryLock();	
}

bool CSQDoor::TimedLock(SHORT timeout) const
{
	return _Lock.TimedLock(timeout);
}

const CSQCond &CSQDoor::Threshold1() const
{
	return _Threshold1;	
}

const CSQCond &CSQDoor::Threshold2() const
{
	return _Threshold2;	
}






};

};
