
#include "MQCond.hpp"
#include "MQException.hpp"

#define  _MQCONDMASK_		(0xECEDEEEF)

using namespace Galaxy::AMQ;

/*CConditionSuite*/
CConditionSuite::CConditionSuite(SQCOND &_TheCond,SQMUTEX &_TheMutex)
   :_Cond(_TheCond),_Mutex(_TheMutex)
{
	if(_MQCONDMASK_!=_Cond._MSK)
	{
		THROW_MQEXCEPTION("Invalid Event Object");	
	}	
}

CConditionSuite::~CConditionSuite()
{
 
}


pthread_cond_t *CConditionSuite::GetCond() const
{
   return &(_Cond._CND);    
}

pthread_mutex_t *CConditionSuite::GetMutex() const
{
   return &(_Mutex._MTX);  
}

void CConditionSuite::Wait() const
{
   _Cond._WAT++;
   INT rc = CRT_pthread_cond_wait(GetCond(),GetMutex());
   if (rc != 0)
   {
      if(_Cond._WAT > 0)
      {
      	_Cond._WAT--;
      }

      THROW_MQEXCEPTION("");
      return;
   }   
}

void CConditionSuite::NotifyOne() const
{
	INT rc = CRT_pthread_cond_signal(GetCond());
	if (rc != 0)
	{
		THROW_MQEXCEPTION("");
		return;
	}

	if(_Cond._WAT > 0)
	{
		_Cond._WAT--;
	}
}

void CConditionSuite::NotifyAll() const
{
   INT rc = CRT_pthread_cond_broadcast(GetCond());
   if (rc != 0)
   {
      THROW_MQEXCEPTION("");
      return;
   }
   
   _Cond._WAT = 0;
}

bool CConditionSuite::TimedWait(SHORT timeout) const
{
	time_t		   _TheTime = 0;
	struct timespec _tm;
	
	CRT_time(&_TheTime);
	
	_tm.tv_sec =  _TheTime + timeout;
	_tm.tv_nsec = 0;
	
	
	_Cond._WAT++;
   
	INT rc = CRT_pthread_cond_timedwait(GetCond(),GetMutex(),&_tm);
	if(rc!= 0)
	{
		if(_Cond._WAT > 0)
		{
			_Cond._WAT--;
		}
	
		if (rc == ETIMEDOUT)
		{
		  return false; 
		}
		else
		{
		  THROW_MQEXCEPTION("");
		  return false;
		}
	}
   
	return true;
}

UINT CConditionSuite::Lurkers() const
{
	return _Cond._WAT;	
}


/*CConditionCreator*/
CConditionCreator::CConditionCreator(SQCOND &_TheCond)
{
   INT                     rc;
   pthread_condattr_t      _Attr;
    
   CRT_memset(&_Attr,0,sizeof(_Attr));
    
   rc = CRT_pthread_condattr_init(&_Attr);
   if (rc != 0)
   {
      THROW_MQEXCEPTION("");
      return;
   }

   rc =  CRT_pthread_condattr_setpshared(&_Attr, PTHREAD_PROCESS_SHARED);    
   if (rc != 0)
   {
      THROW_MQEXCEPTION("");
      return;
   }
    
   CRT_memset(&_TheCond,0,sizeof(SQCOND));
    
   rc = CRT_pthread_cond_init(&(_TheCond._CND), &_Attr);
   if (rc != 0)
   {
      THROW_MQEXCEPTION("");
      return;
   }
   
   _TheCond._WAT = 0; 
   _TheCond._MSK = _MQCONDMASK_; 
}

CConditionCreator::~CConditionCreator()
{
    
}
