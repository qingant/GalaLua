
#include "MQDQ.hpp"
#include "MQCarrier.hpp"
#include "MQException.hpp"

using namespace Galaxy::AMQ;
/*CDsbQList*/
CDsbQList::CDsbQList(SQLIST &_TheList,const CNodes &_TheNodes)
   :CListByNodes(_TheList,_TheNodes)
{

}

CDsbQList::~CDsbQList()
{

}

void CDsbQList::Push(CNodes::THEPAGEID _PageId) const
{
   CListByNodes::Put2Head(_PageId);   
}

void CDsbQList::Append(CNodes::THEPAGEID _PageId) const
{
   CListByNodes::Put2Tail(_PageId);   
}


/*CDsbQListArray*/ 
CDsbQListArray::CDsbQListArray(SQCHLLIST &_TheRAWDQ,const CNodes &_TheNodes)
   :_Items()
{
   for(UINT i = 0;i<MAXPQCHL;i++)
   {
      CDsbQList *_DsbLST = new CDsbQList(_TheRAWDQ._CHL[i],_TheNodes);
      if(ISNULL(_DsbLST))
      {
         THROW_MQEXCEPTION("New DSB List Failure!");
         break;     
      }
        
      _Items.push_back(_DsbLST);  
   }
    
   if(MAXPQCHL != _Items.size())
   {
      THROW_MQEXCEPTION("Build DSB List Array Error!");   
   }   
}

CDsbQListArray::~CDsbQListArray()
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

UINT CDsbQListArray::Count() const
{
   return _Items.size();    
}

const CDsbQList &CDsbQListArray::operator[](UINT _Index) const
{
   if(_Index >= MAXPQCHL)
   {
      THROW_MQEXCEPTION("List DSB out of Range!");
      _Index = 0;
   }
    
   const CDsbQList *_DsbLst = _Items[_Index];
   return (*_DsbLst);  
}


/*CDsbQSyncArray*/  
CDsbQSyncArray::CDsbQSyncArray(SQCHLLIST &_TheRAWDQ)
   :_GLK((_TheRAWDQ._GLK)),_PUT((_TheRAWDQ._EVT._PUT),(_TheRAWDQ._GLK)),_GET()
{
   for(UINT i = 0;i<MAXPQCHL;i++)
   {
      CConditionSuite *_TheCond = new CConditionSuite((_TheRAWDQ._EVT._GET[i]),(_TheRAWDQ._GLK));
      if(ISNULL(_TheCond))
      {
         THROW_MQEXCEPTION("New Cond Sync Failure!");
         break;     
      }
        
      _GET.push_back(_TheCond);  
   }
    
   if(MAXPQCHL != _GET.size())
   {
      THROW_MQEXCEPTION("Build Cond Sync Array Error!");   
   }   
}

CDsbQSyncArray::~CDsbQSyncArray()
{
   for(UINT i=0;i<_GET.size();i++)
   {
      if(_GET[i]!=NULL)
      {
         delete _GET[i];
         _GET[i] = NULL;
      }
   }

   _GET.clear();    
}

const CMutexSuite &CDsbQSyncArray::Locker() const
{
   return _GLK;   
}

UINT CDsbQSyncArray::Count() const
{
   return _GET.size();    
}

const CConditionSuite &CDsbQSyncArray::operator[](UINT _Index) const
{
   if(_Index >= MAXPQCHL)
   {
		return _PUT;
   }
   else
   {
    
   		const CConditionSuite *_TheCond = _GET[_Index];
   		if(ISNULL(_TheCond))
   		{
   			THROW_MQEXCEPTION("The Cond Sync is NULL!");	
   		}
   		return (*_TheCond);
   }
}


/*CDsbQSuite*/
CDsbQSuite::CDsbQSuite(SQCHLLIST &_TheEntry,UINT _TheIndex,const CPooler &_ThePooler)
   :_RAWEntry(_TheEntry),_Pooler(_ThePooler),_Index(_TheIndex),
    _ListArray(_RAWEntry,_Pooler.Nodes()),_SyncArray(_RAWEntry) ,_View(_RAWEntry)
{
    
}

CDsbQSuite::~CDsbQSuite()
{
    
}

PSQCHLLIST CDsbQSuite::RAWEntry() const
{
   	return &_RAWEntry;  
}

UINT CDsbQSuite::Id() const
{
    return _Index;    
}

void CDsbQSuite::Hibernate_GET(IMQueue::EMQCHL _Chl,UINT &_IndexOfList) const
{  
   switch(_Chl)
   {
      case(IMQueue::MQC_RTL):
      {
         _IndexOfList = 0;

		 const CDsbQList &_TheList0 = _ListArray[0];
         if(_TheList0.Empty())
         {
            for(;;)
            {
               _SyncArray[0].Wait();
        
               if(!_TheList0.Empty())
               {
                  break;
               }
            }
         } 
            
         break; 
      }
      case(IMQueue::MQC_QRY):
      {
         _IndexOfList = 1;
         
         const CDsbQList &_TheList0 = _ListArray[0];
         const CDsbQList &_TheList1 = _ListArray[1];

         if((_TheList1.Empty()) && (_TheList0.Empty()))
         {
            for(;;)
            {
               _SyncArray[1].Wait();
        
               if((!_TheList1.Empty())||(!_TheList0.Empty()))
               {
                  break;
               }
            }
         } 
            
         if(!_TheList1.Empty())
         {
            _IndexOfList = 1;    
         }
         else if(!_TheList0.Empty())
         {
            _IndexOfList = 0;    
         }
            
         break;
      }
      case(IMQueue::MQC_BAT):
      {
         _IndexOfList = 2;
         const CDsbQList &_TheList0 = _ListArray[0];
         const CDsbQList &_TheList1 = _ListArray[1];
         const CDsbQList &_TheList2 = _ListArray[1];

         if((_TheList2.Empty()) && (_TheList0.Empty())&& (_TheList1.Empty()))
         {
            for(;;)
            {
               _SyncArray[2].Wait();
        
               if((!_TheList2.Empty())||(!_TheList0.Empty())||(!_TheList1.Empty()))
               {
                  break;
               }
            }
         } 
            
         if(!_TheList2.Empty())
         {
            _IndexOfList = 2;    
         }
         else if(!_TheList0.Empty())
         {
            _IndexOfList = 0;    
         }
         else if(!_TheList1.Empty())
         {
            _IndexOfList = 1;    
         }
            
         break;
      }    
      default:
      {
         THROW_MQEXCEPTION("Unknow Q Channel!");
         break; 
      }  
   }; 
}

bool CDsbQSuite::TimedHibernate_GET(IMQueue::EMQCHL _Chl,SHORT _timeout,UINT &_IndexOfList) const
{  
	if(_timeout < 0)
	{
		Hibernate_GET(_Chl,_IndexOfList);
	}
	else if(_timeout == 0)
	{
	   switch(_Chl)
	   {
	      case(IMQueue::MQC_RTL):
	      {
	         _IndexOfList = 0;
	         
	         const CDsbQList &_TheList0 = _ListArray[0];
	         return (!_TheList0.Empty());
	      }
	      case(IMQueue::MQC_QRY):
	      {
	         _IndexOfList = 1;
	         
	         const CDsbQList &_TheList0 = _ListArray[0];
	         const CDsbQList &_TheList1 = _ListArray[1];
		            
	         if(!_TheList1.Empty())
	         {
	            _IndexOfList = 1;
	            return true;    
	         }
	         else if(!_TheList0.Empty())
	         {
	            _IndexOfList = 0;
	            return true;    
	         }
	         else
	         {
	         	return false;	
	         }
	      }
	      case(IMQueue::MQC_BAT):
	      {
	         _IndexOfList = 2;
	         
	         const CDsbQList &_TheList0 = _ListArray[0];
	         const CDsbQList &_TheList1 = _ListArray[1];
	         const CDsbQList &_TheList2 = _ListArray[2];
		            
	         if(!_TheList2.Empty())
	         {
	            _IndexOfList = 2;  
	            return true;   
	         }
	         else if(!_TheList0.Empty())
	         {
	            _IndexOfList = 0;   
	            return true;  
	         }
	         else if(!_TheList1.Empty())
	         {
	            _IndexOfList = 1; 
	            return true;    
	         }
	         else
	         {
	         	return false; 	
	         }
	      }    
	      default:
	      {
	         THROW_MQEXCEPTION("Unknow Q Channel!");
	         return false; 
	      }  
	   };	
	}
	else
	{   
	   switch(_Chl)
	   {
	      case(IMQueue::MQC_RTL):
	      {
	         _IndexOfList = 0;
	         
	         const CDsbQList &_TheList0 = _ListArray[0];
	         if(_TheList0.Empty())
	         {
	            for(;;)
	            {
	               bool rt = _SyncArray[0].TimedWait(_timeout);
	               if ( !rt )
	               {
	                  return rt;
	               }
	               if(!_TheList0.Empty())
	               {
	                  break;
	               }
	            }
	         } 
	            
	         break; 
	      }
	      case(IMQueue::MQC_QRY):
	      {
	         _IndexOfList = 1;
	         
	         const CDsbQList &_TheList0 = _ListArray[0];
	         const CDsbQList &_TheList1 = _ListArray[1];
	
	         if((_TheList1.Empty()) && (_TheList0.Empty()))
	         {
	            for(;;)
	            {
	               bool rt = _SyncArray[1].TimedWait(_timeout);
	
	               if ( !rt )
	               {
	                  return rt;
	               }
	        
	               if((!_TheList1.Empty())||(!_TheList0.Empty()))
	               {
	                  break;
	               }
	            }
	         } 
	            
	         if(!_TheList1.Empty())
	         {
	            _IndexOfList = 1;    
	         }
	         else if(!_TheList0.Empty())
	         {
	            _IndexOfList = 0;    
	         }
	            
	         break;
	      }
	      case(IMQueue::MQC_BAT):
	      {
	         _IndexOfList = 2;
	         
	         const CDsbQList &_TheList0 = _ListArray[0];
	         const CDsbQList &_TheList1 = _ListArray[1];
	         const CDsbQList &_TheList2 = _ListArray[2];
	
	         if((_TheList2.Empty()) && (_TheList0.Empty())&& (_TheList1.Empty()))
	         {
	            for(;;)
	            {
	               bool rt = _SyncArray[2].TimedWait(_timeout);
	
	               if ( !rt )
	               {
	                  return rt;
	               }
	        
	               if((!_TheList2.Empty())||(!_TheList0.Empty())||(!_TheList1.Empty()))
	               {
	                  break;
	               }
	            }
	         } 
	            
	         if(!_TheList2.Empty())
	         {
	            _IndexOfList = 2;    
	         }
	         else if(!_TheList0.Empty())
	         {
	            _IndexOfList = 0;    
	         }
	         else if(!_TheList1.Empty())
	         {
	            _IndexOfList = 1;    
	         }
	            
	         break;
	      }    
	      default:
	      {
	         THROW_MQEXCEPTION("Unknow Q Channel!");
	         return false; 
	      }  
	   };
	}
   
   return true;
}

void CDsbQSuite::Wakeup_GET(IMQueue::EMQCHL _Chl,UINT _CurrDepth) const
{
   switch(_Chl)
   {
      case(IMQueue::MQC_RTL):
      {
         if(_CurrDepth > 0)
         {
			const CConditionSuite &_H = _SyncArray[0];
			if(_H.Lurkers()>0)
			{
				_H.NotifyOne();
				
				_CurrDepth--; 
			}	
         }  
      }
      case(IMQueue::MQC_QRY):
      {
         if(_CurrDepth > 0)
         {
			const CConditionSuite &_H = _SyncArray[1];
			if(_H.Lurkers()>0)
			{
				_H.NotifyOne();
				
				_CurrDepth--; 
			}	
         } 
      }
      case(IMQueue::MQC_BAT):
      {
         if(_CurrDepth > 0)
         {
			const CConditionSuite &_H = _SyncArray[2];
			if(_H.Lurkers()>0)
			{
				_H.NotifyOne();
				
				_CurrDepth--; 
			}	
         }
         
         break;
      }    
      default:
      {
         break; 
      }  
   };   
}

inline UINT SQCL_SumTotal(PSQCHLLIST _RawPtr)
{
   UINT  _AllTotal = 0;
   for(UINT i=0;i<MAXPQCHL;i++)
   {
  		_AllTotal+= (_RawPtr->_CHL[i]._Total);	
   }
   
   return _AllTotal;
}

void CDsbQSuite::Hibernate_PUT() const
{
   PSQCHLLIST  _RAWPtr = RAWEntry();

   if(SQCL_SumTotal(_RAWPtr) >= _RAWPtr->_Limit)
   {
      for(;;)
      {
         _SyncArray[0xFF].Wait();
         if(SQCL_SumTotal(_RAWPtr) < _RAWPtr->_Limit)
         {
            break;
         }
      }
   }
}

bool CDsbQSuite::TimedHibernate_PUT(SHORT _timeout) const
{
	if(_timeout < 0)
	{
		Hibernate_PUT();  
	}
	else
	{
		PSQCHLLIST  _RAWPtr = RAWEntry();
		
		if(_timeout == 0)
		{
			return (SQCL_SumTotal(_RAWPtr) < _RAWPtr->_Limit);	
		}
		else
		{
			if(SQCL_SumTotal(_RAWPtr) >= _RAWPtr->_Limit)
			{
				for(;;)
				{
					bool _Result = _SyncArray[0xFF].TimedWait(_timeout);
					    
					if(!_Result)
					{
					   return (_Result);    
					}
					
					if(SQCL_SumTotal(_RAWPtr) < _RAWPtr->_Limit)
					{
					   break;
					}
				}
			}
		}
	}
    
	return (true);  
}

void CDsbQSuite::Wakeup_PUT() const
{
	const CConditionSuite &_H = _SyncArray[0xFF];
	if(_H.Lurkers()>0)
	{
		_H.NotifyOne(); 
	}
}


void CDsbQSuite::_Put(IMQueue::EMQCHL _Chl,CNodes::THEPAGEID _PageId,bool _ExPosHead) const
{
  switch(_Chl)
  {
     case(IMQueue::MQC_RTL):
     {
        {
           CMutexLockGuard _LGuard(_SyncArray.Locker());
           
	       if(RAWEntry()->_Limit > 0)
	       {
	      	  Hibernate_PUT();	
	       }
           
           const CDsbQList &_TheList = _ListArray[0];
	       if(_ExPosHead)
	       {
	          _TheList.Push(_PageId);  
	       }
	       else
	       {
	          _TheList.Append(_PageId);    
	       }
           
           Wakeup_GET(_Chl,_TheList.Total());
        }
            
        break;
     }
     case(IMQueue::MQC_QRY):
     {
        {
           CMutexLockGuard _LGuard(_SyncArray.Locker());
           
           
	       if(RAWEntry()->_Limit > 0)
	       {
	      	  Hibernate_PUT();	
	       }

           const CDsbQList &_TheList = _ListArray[1];
	       if(_ExPosHead)
	       {
	          _TheList.Push(_PageId);  
	       }
	       else
	       {
	          _TheList.Append(_PageId);    
	       }

           Wakeup_GET(_Chl,_TheList.Total());
        }  
        break;
     }
     case(IMQueue::MQC_BAT):
     {
        {
           CMutexLockGuard _LGuard(_SyncArray.Locker());
           
	       if(RAWEntry()->_Limit > 0)
	       {
	      	  Hibernate_PUT();	
	       }

           const CDsbQList &_TheList = _ListArray[2];
	       if(_ExPosHead)
	       {
	          _TheList.Push(_PageId);  
	       }
	       else
	       {
	          _TheList.Append(_PageId);    
	       }

           Wakeup_GET(_Chl,_TheList.Total());
        }    
        break;
     }    
     default:
     {
        THROW_MQEXCEPTION("Unknow Q Channel!");
        break; 
     }  
  };
}

bool CDsbQSuite::_TimedPut(IMQueue::EMQCHL _Chl,CNodes::THEPAGEID _PageId,bool _ExPosHead,SHORT _timeout) const
{
  bool _Result = true;

  switch(_Chl)
  {
     case(IMQueue::MQC_RTL):
     {
        {
           CMutexLockGuard _LGuard(_SyncArray.Locker());
           
	       if(RAWEntry()->_Limit > 0)
	       {
	      	  _Result = TimedHibernate_PUT(_timeout);	
	       }
           
           if(_Result)
           {
	           const CDsbQList &_TheList = _ListArray[0];
		       if(_ExPosHead)
		       {
		          _TheList.Push(_PageId);  
		       }
		       else
		       {
		          _TheList.Append(_PageId);    
		       }
	           
	           Wakeup_GET(_Chl,_TheList.Total());
       	   }
        }
            
        break;
     }
     case(IMQueue::MQC_QRY):
     {
        {
           CMutexLockGuard _LGuard(_SyncArray.Locker());
           
           
	       if(RAWEntry()->_Limit > 0)
	       {
	      	  _Result = TimedHibernate_PUT(_timeout);	
	       }

		   if(_Result)
		   {
	           const CDsbQList &_TheList = _ListArray[1];
		       if(_ExPosHead)
		       {
		          _TheList.Push(_PageId);  
		       }
		       else
		       {
		          _TheList.Append(_PageId);    
		       }
	
	           Wakeup_GET(_Chl,_TheList.Total());
       	   }
        }  
        break;
     }
     case(IMQueue::MQC_BAT):
     {
        {
           CMutexLockGuard _LGuard(_SyncArray.Locker());
           
	       if(RAWEntry()->_Limit > 0)
	       {
	      	  _Result = TimedHibernate_PUT(_timeout);	
	       }
		   
		   if(_Result)
		   {
	           const CDsbQList &_TheList = _ListArray[2];
		       if(_ExPosHead)
		       {
		          _TheList.Push(_PageId);  
		       }
		       else
		       {
		          _TheList.Append(_PageId);    
		       }
	
	           Wakeup_GET(_Chl,_TheList.Total());
       	   }
        }    
        break;
     }    
     default:
     {
        THROW_MQEXCEPTION("Unknow Q Channel!");
        _Result = false;
        break; 
     }  
  };
  
  return _Result;
}

CNodes::THEPAGEID CDsbQSuite::_Get(IMQueue::EMQCHL _Chl,UINT &_TheIndexOfList) const
{
	UINT    _IndexOfList = 0xFFFFFFFF;
	CNodes::THEPAGEID	_Result = 0;
   	
	{
		CMutexLockGuard _LGuard(_SyncArray.Locker());
		
		Hibernate_GET(_Chl,_IndexOfList);
		
		if(_IndexOfList < MAXPQCHL)
		{
			_Result =  _ListArray[_IndexOfList].Get();
			
			_TheIndexOfList = _IndexOfList; //·µ»Ø
			
			PSQCHLLIST _RawPtr = RAWEntry();
			if((_RawPtr->_Limit > 0) && (SQCL_SumTotal(_RawPtr) < _RawPtr->_Limit))
			{
				Wakeup_PUT();
			}
		}
		else
		{
			THROW_MQEXCEPTION("Q Channel Out of Range!");   
		}
	}
	
	
	__asm__ __volatile__("nop;nop;"); 
	
	return (_Result);
}

CNodes::THEPAGEID CDsbQSuite::_TimedGet(IMQueue::EMQCHL _Chl, SHORT timeout,UINT &_TheIndexOfList) const
{
	UINT    _IndexOfList = 0xFFFFFFFF;
	CNodes::THEPAGEID	_Result = 0;
   
	{ 
		CMutexLockGuard _LGuard(_SyncArray.Locker());
		
		if(TimedHibernate_GET(_Chl,timeout,_IndexOfList))
		{

			if(_IndexOfList < MAXPQCHL)
			{
				_Result =  _ListArray[_IndexOfList].Get();
				
				_TheIndexOfList = _IndexOfList; //·µ»Ø
				
				PSQCHLLIST _RawPtr = RAWEntry();
				if((_RawPtr->_Limit > 0) && (SQCL_SumTotal(_RawPtr) < _RawPtr->_Limit))
				{
					Wakeup_PUT();
				}
			}
			else
			{
				THROW_MQEXCEPTION("Q Channel Out of Range!");   
			}
		}
	}
	
	__asm__ __volatile__("nop;nop;"); 
	
	return (_Result);
}


UINT CDsbQSuite::TimedGet(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length, SHORT timeout) const
{
	CPoolerNursery      _Nursery(_Pooler);
	UINT                _IndexOfList = 0x0F;
	CNodes::THEPAGEID   _Id = _TimedGet(_Chl, timeout,_IndexOfList);
	if ( _Id == 0 )
	{
		return 0;
	}
	_Nursery.Enter(_Id);
    
	//Access
	{
		const CNode *_Node = _Pooler.Nodes().Items()[_Id];
		_Length = (_Node->Length() > _Block.RAWSize()) ? _Block.RAWSize() : _Node->Length() ; 
		
		//CRT_memcpy(_Block.RAWEntry(),_Node->PageEntry(),_Length); 
		CMQCarrier _Carrier(_Block.RAWEntry(),_Node->PageEntry(),_Length);
	}

	return ((_Id & 0x0FFFFFFF)| (((UINT)_IndexOfList << 28) & 0xF0000000));    
}


UINT CDsbQSuite::Get(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length) const
{   
   CPoolerNursery      _Nursery(_Pooler);
   UINT                _IndexOfList = 0x0F;
   CNodes::THEPAGEID   _Id = _Get(_Chl,_IndexOfList);
   
   _Nursery.Enter(_Id);
    
   //Access
	{
	   const CNode *_Node = _Pooler.Nodes().Items()[_Id];
	   
	   _Length = (_Node->Length() > _Block.RAWSize()) ? _Block.RAWSize() : _Node->Length() ; 
	   
	   //CRT_memcpy(_Block.RAWEntry(),_Node->PageEntry(),_Length); 
	   CMQCarrier _Carrier(_Block.RAWEntry(),_Node->PageEntry(),_Length);
	}

   return ((_Id & 0x0FFFFFFF)| (((UINT)_IndexOfList << 28) & 0xF0000000));    
}

UINT CDsbQSuite::Put(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length) const
{
   return PutEx(_Chl,_Block,_Length,false);      
}

UINT CDsbQSuite::TimedPut(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length, SHORT timeout) const
{
   return TimedPutEx(_Chl,_Block,_Length,false, timeout);      
}

UINT CDsbQSuite::PutEx(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length,bool _ExPosHead) const
{
   if(_Block.RAWSize() > _Pooler.Nodes().PageSize())
   {
      THROW_MQEXCEPTION("Block too large!"); 
      return 0;
   }
   else
   {   
      CPoolerNursery      _Nursery(_Pooler);
      CNodes::THEPAGEID   _Id = _Pooler.Get(CNode::QTYP_DQ,Id());
      _Nursery.Enter(_Id);
        
      //Access
      {
	      const CNode *_Node = _Pooler.Nodes().Items()[_Id];
	      _Length = _Block.RAWSize() ; 
	      //CRT_memcpy(_Node->PageEntry(),_Block.RAWEntry(),_Length);
	      CMQCarrier _Carrier(_Node->PageEntry(),_Block.RAWEntry(),_Length);
		  _Node->Length(_Length);
  	  }

      

      _Put(_Chl,_Id,_ExPosHead);
       
      _Nursery.Release();
        
      return ((_Id & 0x0FFFFFFF)| (((UINT)_Chl << 28) & 0xF0000000)); 
   } 
}

UINT CDsbQSuite::TimedPutEx(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length,bool _ExPosHead, SHORT timeout) const
{
   if(_Block.RAWSize() > _Pooler.Nodes().PageSize())
   {
      THROW_MQEXCEPTION("Block too large!"); 
      return 0;
   }
   else
   {   
      UINT	_Result = 0;

      {
	      CPoolerNursery      _Nursery(_Pooler);
	      CNodes::THEPAGEID   _Id = _Pooler.TimedGet(CNode::QTYP_DQ,Id(), timeout);
	      if(_Id != 0)
	      {
		      _Nursery.Enter(_Id);
		        
		      //Access
		      {
			      const CNode *_Node = _Pooler.Nodes().Items()[_Id];
			      _Length = _Block.RAWSize() ; 
			      //CRT_memcpy(_Node->PageEntry(),_Block.RAWEntry(),_Length);
			      CMQCarrier _Carrier(_Node->PageEntry(),_Block.RAWEntry(),_Length);
			      _Node->Length(_Length);
		  	  }
		      
		      
		      if(_TimedPut(_Chl,_Id,_ExPosHead,timeout))
		      {       
		      	 _Nursery.Release();
		      	 
		      	 _Result = ((_Id & 0x0FFFFFFF)| (((UINT)_Chl << 28) & 0xF0000000));
		      }
	  	  }
  	  }

      return _Result;
   } 
}


void CDsbQSuite::Reset() const
{
	CMutexLockGuard _LGuard(_SyncArray.Locker());
	for(UINT i=0;i<MAXPQCHL;i++)
	{
		_SyncArray[i].NotifyAll();
	}
	_SyncArray[0xFF].NotifyAll();
}


UINT CDsbQSuite::GetMaxDepth() const
{
	return RAWEntry()->_Limit;	
}

void CDsbQSuite::SetMaxDepth(UINT _Depth) const
{
	CMutexLockGuard _LGuard(_SyncArray.Locker());
	RAWEntry()->_Limit = _Depth;	
}

const IMQView &CDsbQSuite::View() const
{
    return (_View);    
}

/*CDsbQArray*/
CDsbQArray::CDsbQArray(const CPooler & _ThePooler)
   :_Items()
{
   SQDQ &_RAWDQEntry=*(_ThePooler.Anchor().GetDistributaryQueueEntry());

   for(UINT i = 0;i<_RAWDQEntry._DQ;i++)
   {
      CDsbQSuite *_DQSuite = new CDsbQSuite(_RAWDQEntry._Q[i],i,_ThePooler);
      if(ISNULL(_DQSuite))
      {
         THROW_MQEXCEPTION("New DQ Failure!");
         break;     
      }
        
      _Items.push_back(_DQSuite);  
   }
    
   if(_RAWDQEntry._DQ != _Items.size())
   {
      THROW_MQEXCEPTION("Build CQ Array Error!");   
   }  
}

CDsbQArray::~CDsbQArray()
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

UINT CDsbQArray::Count() const
{   
   return _Items.size();   
}

const IMQueue &CDsbQArray::operator[](UINT _Index) const
{
   const CDsbQSuite *_DQSuite = _Items[ (_Index % Count())];
   return (*_DQSuite);
}
