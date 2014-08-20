#include "MQ_SQueue.hpp"
#include "MQException.hpp"
#include "MQT.hpp"
#include "MQ_SQNodes.hpp"
#include "MQ_SQAllocator.hpp"



namespace Galaxy
{
namespace AMQ
{
	
#define TIMED_NOWAIT		(0)

void CSQPrivatePooler::Init(UINT _TheCatchCount,const CSQPooler &_ThePooler)
{
	_Lock.Init();
	_List.Init();
	
	_Catch = _TheCatchCount;
	Pooler(_ThePooler);

	
	if(!T_MQEquPtr< CSQPooler >(Pooler(),_ThePooler))
	{
		THROW_MQEXCEPTION("Set Pooler Failed!");
		return;		
	}
	
	CMQLockGuard< CSQEvent > _LGuard(_Lock);
	
	const CSQNode *_TheNode = NULL;
	for(UINT i=0;i<_Catch;i++)
	{
		_TheNode = _ThePooler.Get();
		if(_TheNode==NULL)
		{
			THROW_MQNULLEXCEPTION(_TheNode);
			return;		
		}
		
		_List.Put2Head(*_TheNode);
	}	
}

PBYTE CSQPrivatePooler::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}

UINT CSQPrivatePooler::Count() const
{
	return _List.Count();		
}

const CSQPooler &CSQPrivatePooler::Pooler() const
{
	CSQPooler *_ThePooler = (CSQPooler *)(((PBYTE)this) + _Pooler);	
	return *_ThePooler;
}

void CSQPrivatePooler::Pooler(const CSQPooler &_ThePooler)
{
	_Pooler = ((PBYTE) &_ThePooler) - ((PBYTE)this);
}

void CSQPrivatePooler::Hibernate() const
{
   if(_List.Count()==0)
   {
      for(;;)
      {
         _Lock.Wait();
         if(_List.Count()!=0)
         {
            break;
         }
      }
   }
}

bool CSQPrivatePooler::TimedHibernate(SHORT timeout) const
{
	if(timeout == TIMED_NOWAIT)
	{
		return (_List.Count()!=0);
	}
	else if(timeout < TIMED_NOWAIT)
	{
		Hibernate();
	}
	else
	{
		if(_List.Count()==0)
		{
			for(;;)
			{
				if (!_Lock.TimedWait(timeout))
				{
					return false;
				}
			
		         if(_List.Count()!=0)
		         {
		            break;
		         }
			}
		}
	}
	
	return true; 	
}

void CSQPrivatePooler::Wakeup() const
{
	if(_Lock.Lurkers()>0)
	{
		_Lock.NotifyOne();
		__asm__ __volatile__ ("nop;nop;");  
	}	
}


const CSQNode *CSQPrivatePooler::Get() const
{
	const CSQNode *_TheNode = NULL;
	
	{
		CMQLockGuard< CSQEvent > _LGuard(_Lock);
		
		if(_List.Count() == 0)
		{
			_TheNode = Pooler().Get();	
		}
		
		if(_TheNode==NULL)
		{
	        Hibernate();
	        
	        _TheNode = _List.Get();
	        if(ISNULL(_TheNode))
	        {
				THROW_MQNULLEXCEPTION(_TheNode);
	        }
		}
	}
	
    __asm__ __volatile__("nop;nop;");
    
    return (_TheNode);	
}

const CSQNode *CSQPrivatePooler::TimedGet(SHORT timeout) const
{
	const CSQNode *_TheNode = NULL;
	
	{
		CMQLockGuard< CSQEvent > _LGuard(_Lock);
		
		if(_List.Count() == 0)
		{
			_TheNode = Pooler().Get();	
		}
		
		if(_TheNode==NULL)
		{
	        if(TimedHibernate(timeout))
	        {
		        _TheNode = _List.Get();
		        if(ISNULL(_TheNode))
		        {
					THROW_MQNULLEXCEPTION(_TheNode);
		        }
	        }
		}
	}
	
    __asm__ __volatile__("nop;nop;");
    
    return (_TheNode);	
}

void CSQPrivatePooler::Put(const CSQNode &_TheNode) const
{	
	_TheNode.Page(NULL); //上层必须释放PAGE
	//_TheNode.Length(0);  //长度置零为了效率取消


	{
		CMQLockGuard< CSQEvent > _LGuard(_Lock);	
		if(_List.Count() < _Catch)
		{
			_List.Put2Head(_TheNode);
		}
		else
		{
			Pooler().Put(_TheNode);		
		}
		
		Wakeup();
	}
}

void CSQPrivatePooler::_NurseryPut(const CSQNode &_TheNode) const
{
	Put(_TheNode);	
}

void CSQPrivatePooler::Reset() const
{
	CMQLockGuard< CSQEvent > _LGuard(_Lock);
	_Lock.NotifyAll();	
}


/*CSQDoorQueue*/
void CSQDoorQueue::Init(UINT _TheDepth)
{
	_Door.Init();
	_List.Init();
	
	_MaxDepth = _TheDepth;
}

PBYTE CSQDoorQueue::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}

UINT CSQDoorQueue::Count() const
{
	return _List.Count();	
}

UINT CSQDoorQueue::MaxDepth() const
{
	return _MaxDepth;	
}

void CSQDoorQueue::MaxDepth(UINT _TheDepth) const
{
	CSQDoorQueue *_This = (CSQDoorQueue *)this;	
	_This->_MaxDepth = _TheDepth;	
}


void CSQDoorQueue::GET_Hibernate() const
{
   if(_List.Count()==0)
   {
      const CSQCond &_TheCond = _Door.Threshold1();

      for(;;)
      {
         _TheCond.Wait();
         if(_List.Count()!=0)
         {
            break;
         }
      }
   }
}


bool CSQDoorQueue::GET_TimedHibernate(SHORT timeout) const
{
	if(timeout == TIMED_NOWAIT)
	{
		return (_List.Count()!=0);
	}
	else if(timeout < TIMED_NOWAIT)
	{
		GET_Hibernate();
	}
	else
	{
		if(_List.Count()==0)
		{
			const CSQCond &_TheCond = _Door.Threshold1();

			for(;;)
			{
				if (!_TheCond.TimedWait(timeout))
				{
					return false;
				}
			
		         if(_List.Count()!=0)
		         {
		            break;
		         }
			}
		}
	}
	
	return true; 
}

void CSQDoorQueue::GET_Wakeup() const
{
	const CSQCond &_TheCond = _Door.Threshold1();
	if(_TheCond.Lurkers()>0)
	{
		_TheCond.NotifyOne();
		__asm__ __volatile__ ("nop;nop;");  
	}
}


void CSQDoorQueue::PUT_Hibernate() const
{
   if((_MaxDepth > 0) && (_List.Count() >= _MaxDepth))
   {
      const CSQCond &_TheCond = _Door.Threshold2();
      for(;;)
      {
         _TheCond.Wait();
         if(_List.Count() < _MaxDepth)
         {
            break;
         }
      }
   }
}


bool CSQDoorQueue::PUT_TimedHibernate(SHORT timeout) const
{
	if(_MaxDepth == 0) //无最大深度
	{
		return true;	
	}

	if(timeout == TIMED_NOWAIT)
	{
		return (_List.Count() < _MaxDepth);
	}
	else if(timeout < TIMED_NOWAIT)
	{
		PUT_Hibernate();
	}
	else
	{
		if(_List.Count() >= _MaxDepth)
		{
			const CSQCond &_TheCond = _Door.Threshold2();
			for(;;)
			{
				if (!_TheCond.TimedWait(timeout))
				{
					return false;
				}
			
				if(_List.Count() < _MaxDepth)
				{
					break;
				}
			}
		}
	}
   
	return true; 
}


void CSQDoorQueue::PUT_Wakeup() const
{
	if((_MaxDepth > 0) && (_List.Count() < _MaxDepth))
	{
		const CSQCond &_TheCond = _Door.Threshold2();
		if(_TheCond.Lurkers()>0)
		{
			_TheCond.NotifyOne();
			__asm__ __volatile__ ("nop;nop;");  
		}
	}
}


void CSQDoorQueue::Reset() const
{
	CMQLockGuard< CSQDoor > _LGuard(_Door);
	_Door.Threshold1().NotifyAll();
	_Door.Threshold2().NotifyAll();
}



const CSQNode *CSQDoorQueue::Get() const
{
	const CSQNode *_TheNode = NULL;
	
	{
		CMQLockGuard< CSQDoor > _LGuard(_Door);

        GET_Hibernate();
        
        _TheNode = _List.Get();
        if(ISNULL(_TheNode))
        {
			THROW_MQNULLEXCEPTION(_TheNode);
        }
		
		PUT_Wakeup();
	}
	
    __asm__ __volatile__("nop;nop;");
    
    return (_TheNode);
}

const CSQNode *CSQDoorQueue::TimedGet(SHORT timeout) const
{
    if(timeout < 0)
    {
		return Get();        		
    }
    else
    {
    	const CSQNode *_TheNode = NULL;
    	
    	{
	    	CMQLockGuard< CSQDoor > _LGuard(_Door);
	
	        if(GET_TimedHibernate(timeout))
	        {
		        _TheNode = _List.Get();
		        if(ISNULL(_TheNode))
		        {
					THROW_MQNULLEXCEPTION(_TheNode);
		        }
		        
		        PUT_Wakeup();
	        }
    	}
    	
    	__asm__ __volatile__("nop;nop;");
    	
    	return (_TheNode); 
	}	
}


void CSQDoorQueue::Put(const CSQNode &_TheNode) const
{
	{
		CMQLockGuard< CSQDoor > _LGuard(_Door);
		
		PUT_Hibernate();
		
		_List.Put2Tail(_TheNode);

		GET_Wakeup();
	}
}

void CSQDoorQueue::_NurseryPut(const CSQNode &_TheNode) const
{
	{
		CMQLockGuard< CSQDoor > _LGuard(_Door);
		
		PUT_Hibernate();
		
		_List.Put2Head(_TheNode);

		GET_Wakeup();
	}
}

bool CSQDoorQueue::TimedPut(const CSQNode &_TheNode,SHORT timeout) const
{
    if(timeout < 0)
    {
		Put(_TheNode);
		return true;
    }
    else
    {   	
    	bool _Result = false;

    	{
	    	CMQLockGuard< CSQDoor > _LGuard(_Door);
	
	        if((_Result = PUT_TimedHibernate(timeout)))
	        {
				_List.Put2Tail(_TheNode);
				
		        GET_Wakeup();
	        }
    	}
    	
    	__asm__ __volatile__("nop;nop;");
    	
    	return (_Result); 
	}
}

/*CSQSuite*/
void CSQSuite::Init(const CSQAllocator &_TheAllocator,const CSQPooler &_ThePooler,UINT _MaxDepth,UINT _CatchCount)
{
	if(!_TheAllocator.Check())
	{
		THROW_MQEXCEPTION(".ALC check Error.");
		return;
	}
	
	if(!_ThePooler.Check())
	{
		THROW_MQEXCEPTION(".PLR check Error.");
		return;
	}
	
	Allocator(_TheAllocator);
	Pooler(_ThePooler);
	_Queue.Init(_MaxDepth);
	_PPL.Init(_CatchCount,_ThePooler);
	
	{
		PBYTE _TheMask = (PBYTE)&_MSK; //.ALC
		_TheMask[0] = 0x2E;
		_TheMask[1] = 0x51;
		_TheMask[2] = 0x55;
		_TheMask[3] = 0x45;
	}
		
}

bool CSQSuite::Check() const
{
	{
		const PBYTE _TheMask = (const PBYTE)&_MSK;
		return (_TheMask[0] == 0x2E) && \
			   (_TheMask[1] == 0x51) && \
			   (_TheMask[2] == 0x55) && \
			   (_TheMask[3] == 0x45);
	}
}

PBYTE CSQSuite::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}

void CSQSuite::Pooler(const CSQPooler &_ThePooler)
{
	_Pooler = ((PBYTE) &_ThePooler) - ((PBYTE)this);
}

void CSQSuite::Allocator(const CSQAllocator &_TheAllocator)
{
	_Allocator = ((PBYTE) &_TheAllocator) - ((PBYTE)this);	
}

const CSQAllocator &CSQSuite::Allocator() const
{
	CSQAllocator *_TheAllocator = (CSQAllocator *)(((PBYTE)this) + _Allocator);	
	return *_TheAllocator;	
}

const CSQPooler &CSQSuite::Pooler() const
{
	CSQPooler *_ThePooler = (CSQPooler *)(((PBYTE)this) + _Pooler);	
	return *_ThePooler;
}

inline UINT F_ReadData(const CSQPage &_ThePage,UINT _DataLength,PSTR _Buffer,UINT _MaxLength)
{
	const CSQPage *_TheNext = NULL;
	UINT _CurrLen = 0,_Len = 0;
	UINT _Length = (_DataLength > _MaxLength) ? _MaxLength : _DataLength;
	
		
	for(_TheNext = &_ThePage,_CurrLen = 0;(_TheNext!=NULL) && (_CurrLen < _Length);_TheNext = _TheNext->Next())
	{
		_Len = _TheNext->Read(&_Buffer[_CurrLen],_Length - _CurrLen);
		_CurrLen += _Len;
	};
	
	return _CurrLen;
}

UINT CSQSuite::Get(PSTR _Buffer,UINT _MaxLength) const
{
	const CSQNode *_TheNode = NULL;
	UINT _TheLength = 0;
	
	{
		CMQNursery< CSQPrivatePooler,CSQNode >  _NurseryAutoFree(_PPL);	
		_TheNode = _Queue.Get();
		_NurseryAutoFree.Set(*_TheNode);
		
		{
			CMQNursery< CSQAllocator,CSQPage >  _NurseryPageFree(Allocator());
			const CSQPage *_ThePage = _TheNode->Page();
			_TheNode->Page(NULL);
			if(_ThePage!=NULL)
			{
				_NurseryPageFree.Set(*_ThePage);	
				_TheLength = F_ReadData(*_ThePage,_TheNode->Length(),_Buffer,_MaxLength);
			}
		}
		
	}
	
	return _TheLength;
}

UINT CSQSuite::TimedGet(PSTR _Buffer,UINT _MaxLength,SHORT _Timeout) const
{
	const CSQNode *_TheNode = NULL;
	UINT _TheLength = 0;
	
	{
		CMQNursery< CSQPrivatePooler,CSQNode >  _NurseryAutoFree(_PPL);	
		_TheNode = _Queue.TimedGet(_Timeout);
		if(_TheNode!=NULL)
		{
			_NurseryAutoFree.Set(*_TheNode);

			{
				CMQNursery< CSQAllocator,CSQPage >  _NurseryPageFree(Allocator());
				const CSQPage *_ThePage = _TheNode->Page();	
				_TheNode->Page(NULL);
				if(_ThePage!=NULL)
				{
					_NurseryPageFree.Set(*_ThePage);	
					_TheLength = F_ReadData(*_ThePage,_TheNode->Length(),_Buffer,_MaxLength);
				}
				
			}
		}
		
	}
	
	return _TheLength;	
}

UINT CSQSuite::GetCB(RTN_RECVCALLBACK _Callback,PVOID _Object) const
{
	UINT _TheLength = 0;
	if(_Callback)
	{
		const CSQNode *_TheNode = NULL;
		
		{
			CMQNursery< CSQPrivatePooler,CSQNode >  _NurseryAutoFree(_PPL);	
			_TheNode = _Queue.Get();
			_NurseryAutoFree.Set(*_TheNode);
			
			{
				CMQNursery< CSQAllocator,CSQPage >  _NurseryPageFree(Allocator());
				const CSQPage *_ThePage = _TheNode->Page();
				_TheNode->Page(NULL);
				if(_ThePage!=NULL)
				{
					_NurseryPageFree.Set(*_ThePage);	
					
					UINT _MaxLength = _TheNode->Length();
					PSTR _Buffer = NULL;

					try
					{
						_Buffer = _Callback(_MaxLength,_Object);
					}
					catch(...)
					{
						_Buffer = NULL;	
					}
					
					if(_Buffer!=NULL)
					{
						_TheLength = F_ReadData(*_ThePage,_MaxLength,_Buffer,_MaxLength);
					}
				}
			}
			
		}
	}
	
	return _TheLength;
}

UINT CSQSuite::TimedGetCB(SHORT _Timeout,RTN_RECVCALLBACK _Callback,PVOID _Object) const
{
	UINT _TheLength = 0;
	
	if(_Callback)
	{
		const CSQNode *_TheNode = NULL;

		{
			CMQNursery< CSQPrivatePooler,CSQNode >  _NurseryAutoFree(_PPL);	
			_TheNode = _Queue.TimedGet(_Timeout);
			if(_TheNode!=NULL)
			{
				_NurseryAutoFree.Set(*_TheNode);
	
				{
					CMQNursery< CSQAllocator,CSQPage >  _NurseryPageFree(Allocator());
					const CSQPage *_ThePage = _TheNode->Page();	
					_TheNode->Page(NULL);
					if(_ThePage!=NULL)
					{
						_NurseryPageFree.Set(*_ThePage);	
						
						UINT _MaxLength = _TheNode->Length();
						PSTR _Buffer = NULL;
	
						try
						{
							_Buffer = _Callback(_MaxLength,_Object);
						}
						catch(...)
						{
							_Buffer = NULL;	
						}
						
						if(_Buffer!=NULL)
						{
							_TheLength = F_ReadData(*_ThePage,_MaxLength,_Buffer,_MaxLength);
						}

					}
					
				}
			}
			
		}
	}
	
	return _TheLength;
}


inline UINT F_WriteData(const CSQPage &_ThePage,PSTR _Buffer,UINT _DataLength)
{
	const CSQPage *_TheNext = NULL;
	UINT _CurrLen = 0;
	for(_TheNext = &_ThePage,_CurrLen=0;(_TheNext!=NULL) && (_CurrLen< _DataLength);_TheNext = _TheNext->Next())
	{
		_CurrLen += _TheNext->Write(&_Buffer[_CurrLen],_DataLength - _CurrLen);
        //printf("in for: Data:%d, _CurrLen Length:%d  \n",_DataLength,_CurrLen);
	};
	
    //printf("Data:%d, _CurrLen Length:%d  \n",_DataLength,_CurrLen);
	return _CurrLen;
}

void CSQSuite::Put(PCSTR _Buffer,UINT _Length) const
{
	const CSQNode *_TheNode = NULL;
	const CSQAllocator &_TheAllocator = Allocator();
	{
		const CSQPage *_ThePage = NULL;
		CMQNursery< CSQAllocator,CSQPage >  	_NurseryPageFree(_TheAllocator);
		CMQNursery< CSQPrivatePooler,CSQNode >  _NurseryAutoFree(_PPL);	

		_TheNode = _PPL.Get();
		_NurseryAutoFree.Set(*_TheNode);
	
		_ThePage = _TheAllocator.Allocate(_Length);	
		if(ISNULL(_ThePage))
		{
			THROW_MQEXCEPTION("NO ANY MORE PAGES");		
			return;
		}	
        //printf("Length:%d  \n",_Length);
		_NurseryPageFree.Set(*_ThePage);
		
		if(F_WriteData(*_ThePage,(PSTR)_Buffer,_Length)!=_Length)
		{
			THROW_MQEXCEPTION("WRITE PAGES ERROR");		
			return;
		}
		
		//设进NODE里
		_TheNode->Page(_ThePage);
		_TheNode->Length(_Length);
		
		_Queue.Put(*_TheNode);
		
		_NurseryPageFree.Cancel();
		_NurseryAutoFree.Cancel();
	
	}
	
}

bool CSQSuite::TimedPut(PCSTR _Buffer,UINT _Length,SHORT _Timeout) const
{
	const CSQNode *_TheNode = NULL;
	const CSQAllocator &_TheAllocator = Allocator();
	{
		const CSQPage *_ThePage = NULL;
		CMQNursery< CSQAllocator,CSQPage >  	_NurseryPageFree(_TheAllocator);
		CMQNursery< CSQPrivatePooler,CSQNode >  _NurseryAutoFree(_PPL);	

		_TheNode = _PPL.TimedGet(_Timeout);
		if(_TheNode!=NULL)
		{
			_NurseryAutoFree.Set(*_TheNode);
		
			_ThePage = _TheAllocator.Allocate(_Length);	
			if(ISNULL(_ThePage))
			{
				THROW_MQEXCEPTION("NO ANY MORE PAGES");		
				return false;
			}	
			_NurseryPageFree.Set(*_ThePage);
			
			if(F_WriteData(*_ThePage,(PSTR)_Buffer,_Length)!=_Length)
			{
				THROW_MQEXCEPTION("WRITE PAGES ERROR");		
				return false;
			}
			
			//设进NODE里
			_TheNode->Page(_ThePage);
			_TheNode->Length(_Length);
			
			if(_Queue.TimedPut(*_TheNode,_Timeout))
			{
				_NurseryPageFree.Cancel();
				_NurseryAutoFree.Cancel();
				
				return true;
			}
		}

	
	}
	return false;
}

/*CSQSuiteArray*/
void CSQSuiteArray::Init(UINT _TheTotal,const CSQAllocator &_TheAllocator,const CSQPooler &_ThePooler,UINT _MaxDepth,UINT _CatchCount)
{
	_Total 		= _TheTotal;

	{
		CSQSuite *_TheQueue = NULL;
		
		for(UINT i=0;i<_TheTotal;i++)
		{
			_TheQueue = (CSQSuite *) GetSQSuite(i);	
			if(_TheQueue!=NULL)
			{
				_TheQueue->Init(_TheAllocator,_ThePooler,_MaxDepth,_CatchCount);		
			}
		}
	}
	
	{
		PBYTE _TheMask = (PBYTE)&_MSK;
		_TheMask[0] = 0x2E;
		_TheMask[1] = 0x4D;
		_TheMask[2] = 0x51;
		_TheMask[3] = 0x53;
	}	
}

bool CSQSuiteArray::Check() const
{
	{
		const PBYTE _TheMask = (const PBYTE)&_MSK;
		return (_TheMask[0] == 0x2E) && \
			   (_TheMask[1] == 0x4D) && \
			   (_TheMask[2] == 0x51) && \
			   (_TheMask[3] == 0x53);
	}
}

PBYTE CSQSuiteArray::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}

UINT CSQSuiteArray::Count() const
{
	return _Total;	
}

const CSQSuite *CSQSuiteArray::GetSQSuite(UINT _Index) const
{
	CSQSuite *_TheQueue = NULL;
	if(_Index < _Total)
	{
		PBYTE	_TheEntry = NearPtr();
		_TheQueue = (CSQSuite *) &_TheEntry[(sizeof(CSQSuite) * _Index)];
	}

	return _TheQueue;
}

const CSQSuite *CSQSuiteArray::operator[](UINT _Index) const
{
	const CSQSuite *_TheQueue = GetSQSuite(_Index);
	
	if(_TheQueue!=NULL)
	{
		if(!_TheQueue->Check())
		{
			THROW_MQEXCEPTION(".QUE check Error.");
		}
	}

	return _TheQueue;
}

UINT EvaluateSQueue(UINT _Count)
{
	return (sizeof(CSQSuiteArray)+(sizeof(CSQSuite) * _Count));
}


};

};
