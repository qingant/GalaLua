
#include "MQPooler.hpp"
#include "MQException.hpp"
using namespace Galaxy::AMQ;

/*CPoolerList*/
CPoolerList::CPoolerList(SQLIST &_TheList,const CNodes &_TheNodes)
   :CListByNodes(_TheList,_TheNodes)
{
    
}

CPoolerList::~CPoolerList()
{
    
}
    
void CPoolerList::Put(CNodes::THEPAGEID _PageId) const
{
    //ÉèÖÃ×´Ì¬
    const CNode *_Node = Nodes().Items()[_PageId];
    _Node->Release();
    Put2Head(*_Node);   
}

/*CPoolerSync*/
CPoolerSync::CPoolerSync(SQNRMLIST &_TheEntry)
   :_GLK(_TheEntry._GLK), \
    _GET((_TheEntry._EVT._GET),(_TheEntry._GLK))
{
    
}

CPoolerSync::~CPoolerSync()
{
    
}

const CMutexSuite &CPoolerSync::Locker() const
{
   return _GLK;    
}

const CConditionSuite &CPoolerSync::Hibernator() const
{
   return _GET;    
}

/*CPooler*/
CPooler::CPooler(const CAnchor &_TheAnchor)
   :_Anchor(_TheAnchor),_RAWEntry(*(_Anchor.GetPoolerQueueEntry())), \
    _Nodes(_Anchor),_List(_RAWEntry._LST,_Nodes),_Sync(_RAWEntry)
{
   _Anchor.Verify();   
}

CPooler::~CPooler()
{
    
}

const CAnchor &CPooler::Anchor() const
{
   return _Anchor;    
}

const CNodes &CPooler::Nodes() const
{
   return _Nodes;    
}

bool CPooler::TimedHibernate(SHORT timeout) const
{
   if(timeout == 0)
   {
   		return (!_List.Empty());
   }
   else if(timeout < 0)
   {
		Hibernate();
   }
   else
   {
	   bool rt = true;

	   if(_List.Empty())
	   {
	    
	      for(;;)
	      {
	         if (!_Sync.Hibernator().TimedWait(timeout))
	         {
	            rt = false;
	         }
	
	         if (!rt)
	         {
	            return rt;
	         }

	         if(!_List.Empty())
	         {
	            break;
	         }
	
	      }
	   }
   }
   
   return true;    
}

void CPooler::Hibernate() const
{
   if(_List.Empty())
   {
      for(;;)
      {
         _Sync.Hibernator().Wait();
         if(!_List.Empty())
         {
            break;
         }
      }
   }
}

void CPooler::Wakeup() const
{
	const CConditionSuite &_H = _Sync.Hibernator();
	if(_H.Lurkers()>0)
	{
		_H.NotifyOne(); 
	}
}

void CPooler::Put(CNodes::THEPAGEID _PageId) const
{
   //if((_PageId == 0)|| (_PageId > Nodes().Pages()))
   //{
   //   THROW_MQEXCEPTION("_Node Id Incorrect!");    
   //}
   //else
   //{

    {
        CMutexLockGuard _LGuard(_Sync.Locker());

        _List.Put(_PageId);
        
        Wakeup();
    } 

   //}
}

CNodes::THEPAGEID CPooler::TimedGet(CNode::EMQTYPE _Type,UINT _QID, SHORT timeout) const
{   
    if(timeout < 0)
    {
		return Get(_Type,_QID);        		
    }
    else
    {
    	CNodes::THEPAGEID _PageId = 0;
    	CMutexLockGuard _LGuard(_Sync.Locker());

        if (TimedHibernate(timeout))
        {
           _PageId = _List.Get();
	        const CNode *_Node = Nodes().Items()[_PageId];
	        _Node->Occupy(_Type,_QID);
        }
        
        return _PageId; 
	}    
}

CNodes::THEPAGEID CPooler::Get(CNode::EMQTYPE _Type,UINT _QID) const
{
    CNodes::THEPAGEID _PageId = 0;
    
    {
        CMutexLockGuard _LGuard(_Sync.Locker());
        
        Hibernate();
        
        _PageId = _List.Get();
    }
    
    //if((_PageId == 0)|| (_PageId > Nodes().Pages()))
    //{
    //    THROW_MQEXCEPTION("_Node Id Incorrect!"); 
    //    return 0;   
    //}
    
    //ÉèÖÃ×´Ì¬
    {
        const CNode *_Node = Nodes().Items()[_PageId];
        _Node->Occupy(_Type,_QID);
    }
    
	return _PageId;
}


void CPooler::Reset() const
{
	CMutexLockGuard _LGuard(_Sync.Locker());
	_Sync.Hibernator().NotifyAll(); 
}



/*CPoolerNursery*/
CPoolerNursery::CPoolerNursery(const CPooler &_ThePooler)
   :_Pooler(_ThePooler),_List()
{
       
}

CPoolerNursery::~CPoolerNursery()
{
   UINT i;
   for(i=0;i<_List.size();i++)
   {
      _Pooler.Put(_List[i]);
      _List[i] = 0;
   }

   _List.clear();
}

void CPoolerNursery::Enter(CNodes::THEPAGEID _PageId)
{
   if((_PageId == 0)|| (_PageId > _Pooler.Nodes().Pages()))
   {
      THROW_MQEXCEPTION("_Node Id Incorrect!");    
   }
   else
   {
      _List.push_back(_PageId);
   } 
}

void CPoolerNursery::Release()
{
   _List.clear();   
}
