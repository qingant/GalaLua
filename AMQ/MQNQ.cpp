
#include "MQNQ.hpp"
#include "MQCarrier.hpp"
#include "MQException.hpp"

using namespace Galaxy::AMQ;

/*CNmlQList*/
CNmlQList::CNmlQList(SQLIST &_TheList,const CNodes &_TheNodes)
   :CListByNodes(_TheList,_TheNodes)
{
    
}

CNmlQList::~CNmlQList()
{

}

void CNmlQList::Push(CNodes::THEPAGEID _PageId) const
{
   CListByNodes::Put2Head(_PageId);   
}

void CNmlQList::Append(CNodes::THEPAGEID _PageId) const
{
   CListByNodes::Put2Tail(_PageId);   
}


/*CNmlQSync*/
CNmlQSync::CNmlQSync(SQNRMLIST &_TheEntry)
   :_GLK((_TheEntry._GLK)), \
    _GET((_TheEntry._EVT._GET),(_TheEntry._GLK)) , \
    _PUT((_TheEntry._EVT._PUT),(_TheEntry._GLK))
{
    
}

CNmlQSync::~CNmlQSync()
{
    
}

const CMutexSuite &CNmlQSync::Locker() const
{
   return _GLK;    
}

const CConditionSuite &CNmlQSync::Hibernator_GET() const
{
   return _GET;    
}

const CConditionSuite &CNmlQSync::Hibernator_PUT() const
{
   return _PUT;    
}



/*CNmlQSuite*/
CNmlQSuite::CNmlQSuite(SQNRMLIST & _TheEntry,UINT _TheIndex,const CPooler &_ThePooler)
   :_RAWEntry(_TheEntry),_Pooler(_ThePooler),_Index(_TheIndex), \
    _List((_RAWEntry._LST),_Pooler.Nodes()),_Sync(_RAWEntry) ,_View(_RAWEntry)
{
    
}

CNmlQSuite::~CNmlQSuite()
{
    
}

PSQNRMLIST CNmlQSuite::RAWEntry() const
{
   	return &_RAWEntry;    
}

UINT CNmlQSuite::Id() const
{
    return _Index;    
}

UINT CNmlQSuite::GetMaxDepth() const
{
	return RAWEntry()->_Limit;	
}

void CNmlQSuite::SetMaxDepth(UINT _Depth) const
{
	CMutexLockGuard _LGuard(_Sync.Locker());
	RAWEntry()->_Limit = _Depth;	
}

void CNmlQSuite::Hibernate_GET() const
{
   if(_List.Empty())
   {
      for(;;)
      {
         _Sync.Hibernator_GET().Wait();
         if(!_List.Empty())
         {
            break;
         }
            
         //要不要？
         //CRT_usleep(50);
      }
   }
}

bool CNmlQSuite::TimedHibernate_GET(SHORT _timeout) const
{
	
	if(_timeout < 0)
	{
		Hibernate_GET();  
	}
	else if(_timeout == 0)
	{
		return (!_List.Empty());
	}
	else
	{
		if(_List.Empty())
		{
			for(;;)
			{
				bool _Result = _Sync.Hibernator_GET().TimedWait(_timeout);
				    
				if(!_Result)
				{
				   return (_Result);    
				}
				
				if(!_List.Empty())
				{
				   break;
				}
			}
		}
	}
    
   return (true);  
}

void CNmlQSuite::Wakeup_GET() const
{
	const CConditionSuite &_H = _Sync.Hibernator_GET();
	if(_H.Lurkers()>0)
	{
		_H.NotifyOne(); 
	}
}

void CNmlQSuite::Hibernate_PUT() const
{
   PSQNRMLIST	_RawPtr = RAWEntry();
   if(_List.Total() >= _RawPtr->_Limit)
   {
      for(;;)
      {
         _Sync.Hibernator_PUT().Wait();
         if(_List.Total() < _RawPtr->_Limit)
         {
            break;
         }
            
         //要不要？
         //CRT_usleep(50);
      }
   }
}

bool CNmlQSuite::TimedHibernate_PUT(SHORT _timeout) const
{
	if(_timeout < 0)
	{
		Hibernate_PUT();  
	}
	else
	{
		PSQNRMLIST	_RawPtr = RAWEntry();
		
		if(_timeout == 0)
		{
			return (_List.Total() < _RawPtr->_Limit);
		}
		else
		{
			if(_List.Total() >= _RawPtr->_Limit)
			{
				for(;;)
				{
					bool _Result = _Sync.Hibernator_PUT().TimedWait(_timeout);
					    
					if(!_Result)
					{
					   return (_Result);    
					}
					
					if(_List.Total() < _RawPtr->_Limit)
					{
					   break;
					}
				}
			}
		}
	}
    
	return (true);  
}

void CNmlQSuite::Wakeup_PUT() const
{
	const CConditionSuite &_H = _Sync.Hibernator_PUT();
	if(_H.Lurkers()>0)
	{
		_H.NotifyOne(); 
	}
}

void CNmlQSuite::_Put(CNodes::THEPAGEID _PageId,bool _ExPosHead) const
{
	CMutexLockGuard _LGuard(_Sync.Locker());
	
	if(RAWEntry()->_Limit > 0)
	{
		Hibernate_PUT();	
	}
	
	if(_ExPosHead)
	{
		_List.Push(_PageId);  
	}
	else
	{
		_List.Append(_PageId);    
	}
	
	
	Wakeup_GET();
}

bool CNmlQSuite::_TimedPut(CNodes::THEPAGEID _PageId,bool _ExPosHead,SHORT _timeout) const
{
	bool _Result = true;

	{
		CMutexLockGuard _LGuard(_Sync.Locker());
		
		if(RAWEntry()->_Limit > 0)
		{
			_Result = TimedHibernate_PUT(_timeout);
		}
	      
		if(_Result)
		{
			if(_ExPosHead)
			{
			 	_List.Push(_PageId);  
			}
			else
			{
			 	_List.Append(_PageId);    
			}
			
			
			Wakeup_GET();
		}
	}

	return _Result;
}


CNodes::THEPAGEID CNmlQSuite::_Get() const
{
   CNodes::THEPAGEID _PageId = 0;

   {
       CMutexLockGuard _LGuard(_Sync.Locker());
       Hibernate_GET();
    
       _PageId =  _List.Get();
       
       
       PSQNRMLIST	_RawPtr = RAWEntry();
	   if((_RawPtr->_Limit > 0) && (_List.Total() < _RawPtr->_Limit))
	   {
			Wakeup_PUT();
	   }
   }
   
   __asm__ __volatile__("nop;nop;"); 

   //if((_PageId == 0)|| (_PageId > _Pooler.Nodes().Pages()))
   //{
   //   THROW_MQEXCEPTION("_Node Id Incorrect!");    
   //}
    
   return _PageId; 
}

CNodes::THEPAGEID CNmlQSuite::_TimedGet(SHORT _timeout) const
{
   CNodes::THEPAGEID _PageId = 0;

   {
      CMutexLockGuard _LGuard(_Sync.Locker());
        
      if(TimedHibernate_GET(_timeout))
      {
         _PageId = _List.Get();
         //if((_PageId == 0)|| (_PageId > _Pooler.Nodes().Pages()))
         //{
         //   THROW_MQEXCEPTION("_Node Id Incorrect!");    
         //}
         
       	  PSQNRMLIST	_RawPtr = RAWEntry();
	   	  if((_RawPtr->_Limit > 0) && (_List.Total() < _RawPtr->_Limit))
		  {
			  Wakeup_PUT();
		  }
      }
   }
   
   __asm__ __volatile__("nop;nop;"); 
    
   return _PageId;  
}


UINT CNmlQSuite::Get(IMQueue::EMQCHL,const IBlock &_Block,UINT &_Length) const
{
   CNodes::THEPAGEID   _PageId = 0;
    
   {
      CPoolerNursery      _Nursery(_Pooler);
      _PageId = _Get();
      _Nursery.Enter(_PageId);
        
      //Access
      {
	      const CNode *_Node = _Pooler.Nodes().Items()[_PageId];
	      _Length = (_Node->Length() > _Block.RAWSize()) ? _Block.RAWSize() : _Node->Length() ; 
	      //CRT_memcpy(_Block.RAWEntry(),_Node->PageEntry(),_Length); 
	      CMQCarrier _Carrier(_Block.RAWEntry(),_Node->PageEntry(),_Length);
  	  }
       
   }

   return (_PageId & 0x0FFFFFFF);    
}

UINT CNmlQSuite::TimedGet(IMQueue::EMQCHL,const IBlock &_Block,UINT &_Length, SHORT _Timeout) const
{
	CNodes::THEPAGEID   _PageId = 0;
	{
		CPoolerNursery      _Nursery(_Pooler);
		_PageId = _TimedGet(_Timeout);
	
		if(_PageId != 0)
		{
			_Nursery.Enter(_PageId);
		    
			//Access
			{
				const CNode *_Node = _Pooler.Nodes().Items()[_PageId];
				_Length = (_Node->Length() > _Block.RAWSize()) ? _Block.RAWSize() : _Node->Length() ; 
				//CRT_memcpy(_Block.RAWEntry(),_Node->PageEntry(),_Length); 
				CMQCarrier _Carrier(_Block.RAWEntry(),_Node->PageEntry(),_Length);
			}
		
		} 
	}
	
	return (_PageId & 0x0FFFFFFF);    
}


UINT CNmlQSuite::Put(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length) const
{
   return PutEx(_Chl,_Block,_Length,false);      
}

UINT CNmlQSuite::PutEx(IMQueue::EMQCHL,const IBlock &_Block,UINT &_Length,bool _ExPosHead) const
{
   if(_Block.RAWSize() > _Pooler.Nodes().PageSize())
   {
      THROW_MQEXCEPTION("Block too large!"); 
      return 0;
   }
   else
   {   
      
      CPoolerNursery      _Nursery(_Pooler);
      CNodes::THEPAGEID   _PageId = _Pooler.Get(CNode::QTYP_NQ,Id());
      _Nursery.Enter(_PageId);
        
      //Access
      {
	      const CNode *_Node = _Pooler.Nodes().Items()[_PageId];
	      _Length = _Block.RAWSize() ; 
	      //CRT_memcpy(_Node->PageEntry(),_Block.RAWEntry(),_Length);
	      CMQCarrier _Carrier(_Node->PageEntry(),_Block.RAWEntry(),_Length);
	      _Node->Length(_Length);
  	  }

      _Put(_PageId,_ExPosHead);

      _Nursery.Release();
        
      return (_PageId & 0x0FFFFFFF); 
   } 
}

UINT CNmlQSuite::TimedPut(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length, SHORT timeout) const
{
   return TimedPutEx(_Chl,_Block,_Length,false, timeout);
}


UINT CNmlQSuite::TimedPutEx(IMQueue::EMQCHL,const IBlock &_Block,UINT &_Length,bool _ExPosHead, SHORT timeout) const
{
   CNodes::THEPAGEID   _PageId = 0;
   
   if(_Block.RAWSize() > _Pooler.Nodes().PageSize())
   {
      THROW_MQEXCEPTION("Block too large!"); 
      return _PageId;
   }
   else
   {   
      CPoolerNursery      _Nursery(_Pooler);

      _PageId = _Pooler.TimedGet(CNode::QTYP_NQ,Id(), timeout);

      if (_PageId != 0)
      {
	      _Nursery.Enter(_PageId);
	        
	      //Access
	      {
		      const CNode *_Node = _Pooler.Nodes().Items()[_PageId];
		      _Length = _Block.RAWSize() ; 
		      //CRT_memcpy(_Node->PageEntry(),_Block.RAWEntry(),_Length);
		      CMQCarrier _Carrier(_Node->PageEntry(),_Block.RAWEntry(),_Length);
		      _Node->Length(_Length);
	  	  }
	      
	      
	      if(_TimedPut(_PageId,_ExPosHead,timeout))
	      {
			 _Nursery.Release();
	      }
      
  	  }
        
      return (_PageId & 0x0FFFFFFF); 
   } 
}

void CNmlQSuite::Reset() const
{
	CMutexLockGuard _LGuard(_Sync.Locker());
	_Sync.Hibernator_GET().NotifyAll(); 
	_Sync.Hibernator_PUT().NotifyAll();
}

const IMQView &CNmlQSuite::View() const
{
    return (_View);    
}

/*CNmlQArray*/
CNmlQArray::CNmlQArray(const CPooler & _ThePooler)
   :_Items()
{
   SQNQ &_RAWNQEntry = *_ThePooler.Anchor().GetNormalQueueEntry();
   
   for(UINT i = 0;i<_RAWNQEntry._NQ;i++)
   {
      CNmlQSuite *_NQSuite = new CNmlQSuite(_RAWNQEntry._Q[i],i,_ThePooler);
      if(ISNULL(_NQSuite))
      {
         THROW_MQEXCEPTION("New NQ Failure!");
         break;     
      }
        
      _Items.push_back(_NQSuite);  
   }
    
   if(_RAWNQEntry._NQ != _Items.size())
   {
      THROW_MQEXCEPTION("Build NQ Array Error!");   
   } 
}

CNmlQArray::~CNmlQArray()
{
   for(UINT i=0;i<_Items.size();i++)
   {
      if(_Items[i]!=NULL)
      {
         delete _Items[i];
         _Items[i] = NULL;
      }
   }

   _Items.clear(); 
}

UINT CNmlQArray::Count() const
{   
   return _Items.size();   
}

const IMQueue &CNmlQArray::operator[](UINT _Index) const
{
   const CNmlQSuite *_NQSuite = _Items[ (_Index % Count())];
   return (*_NQSuite);
}
