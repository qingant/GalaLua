#include "MQ_SQNodes.hpp"
#include "MQException.hpp"
#include "MQT.hpp"

namespace Galaxy
{
namespace AMQ
{

void CSQNode::Init()
{
	_Next = 0L;
	_PageLKTB = 0L;
	_Length = 0U;
}

PBYTE CSQNode::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}

void CSQNode::Next(const CSQNode *_TheNode) const
{
	CSQNode *_This = (CSQNode *)this;
	if(ISNULL(_TheNode))
	{
		_This->_Next = 0L;		
	}
	else
	{
		_This->_Next = ((PBYTE)_TheNode) - ((PBYTE)this);
	}
}

const CSQNode *CSQNode::Next() const
{
	if(_Next == 0L)
	{
		return NULL;	
	}
	else
	{
		return (CSQNode *)(((PBYTE)this) + _Next);
	}
}


void CSQNode::Page(const CSQPage *_ThePage) const
{
	CSQNode *_This = (CSQNode *)this;
	if(ISNULL(_ThePage))
	{
		_This->_PageLKTB = 0L;		
	}
	else
	{
		_This->_PageLKTB = ((PBYTE)_ThePage) - ((PBYTE)this);
	}
}

const CSQPage *CSQNode::Page() const
{
	if(_PageLKTB == 0L)
	{
		return NULL;	
	}
	else
	{
		return (CSQPage *)(((PBYTE)this) + _PageLKTB);
	}
}

void CSQNode::Length(UINT _TheLength) const
{
	CSQNode *_This = (CSQNode *)this;
	_This->_Length = _TheLength;
}

UINT CSQNode::Length() const
{
	return _Length;
}


	
/*CSQNodeArray*/
void CSQNodeArray::Init(UINT _TheTotal)
{
	_Total 		= _TheTotal;

	{
		CSQNode *_TheNode = NULL;
		
		for(UINT i=0;i<_TheTotal;i++)
		{
			_TheNode = (CSQNode *)this->operator[](i);	
			if(_TheNode!=NULL)
			{
				_TheNode->Init();
				//printf("%d -- this:0x%X;Total:%d;Node:0x%X\n",i,this,_Total,_TheNode);	
			}
		}
	}
	
	{
		PBYTE _TheMask = (PBYTE)&_MSK; //.ALC
		_TheMask[0] = 0x2E;
		_TheMask[1] = 0x4E;
		_TheMask[2] = 0x44;
		_TheMask[3] = 0x53;
	}
	
}

bool CSQNodeArray::Check() const
{
	{
		const PBYTE _TheMask = (const PBYTE)&_MSK;
		return (_TheMask[0] == 0x2E) && \
			   (_TheMask[1] == 0x4E) && \
			   (_TheMask[2] == 0x44) && \
			   (_TheMask[3] == 0x53);
	}
}

PBYTE CSQNodeArray::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}

UINT CSQNodeArray::Count() const
{
	return _Total;	
}

const CSQNode *CSQNodeArray::operator[](UINT _Index) const
{
	CSQNode *_TheNode = NULL;
	if(_Index < _Total)
	{
		PBYTE	_PageEntry = NearPtr();
		_TheNode = (CSQNode *) &_PageEntry[(sizeof(CSQNode) * _Index)];	
	}

	return _TheNode;
}



/*CSQLockedList*/
void CSQLockedList::Init()
{
	_Lock.Init();
	_List.Init();
	
	{
		PBYTE _TheMask = (PBYTE)&_MSK; //.ALC
		_TheMask[0] = 0x2E;
		_TheMask[1] = 0x4C;
		_TheMask[2] = 0x4B;
		_TheMask[3] = 0x4C;
	}	
}

bool CSQLockedList::Check() const
{
	{
		const PBYTE _TheMask = (const PBYTE)&_MSK;
		return (_TheMask[0] == 0x2E) && \
			   (_TheMask[1] == 0x4C) && \
			   (_TheMask[2] == 0x4B) && \
			   (_TheMask[3] == 0x4C);
	}
}

PBYTE CSQLockedList::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}

UINT CSQLockedList::Count() const
{
	return _List.Count();	
}

const CSQNode *CSQLockedList::Get() const
{
    CMQLockGuard< CSQLock > _LGuard(_Lock);
    
    const CSQNode *_TheNode = NULL;

	if(_List.Count() > 0)
    {
        _TheNode = _List.Get();
        if(ISNULL(_TheNode))
        {
			THROW_MQNULLEXCEPTION(_TheNode);
        }
	} 
    
    return (_TheNode);	
}

void CSQLockedList::Put2Head(const CSQNode &_TheNode) const
{
	CMQLockGuard< CSQLock > _LGuard(_Lock);
	_List.Put2Head(_TheNode);	
}

void CSQLockedList::Put2Tail(const CSQNode &_TheNode) const
{
	CMQLockGuard< CSQLock > _LGuard(_Lock);
	_List.Put2Head(_TheNode);		
}


/*CSQPooler*/
void CSQPooler::Init(UINT _Nodes)
{
	_Queue.Init();
	_Array.Init(_Nodes);	
	
	{		
		const CSQNode *_TheNode = NULL;
		for(UINT i=0;i<Nodes();i++)
		{
			_TheNode = _Array[i];
			if(_TheNode)
			{
				_Queue.Put2Tail(*_TheNode);		
			}	
		}
		
	}
	
	if(Nodes()!=UsableNodes())
	{
		THROW_MQEXCEPTION("Init failed!");		
	}
	
	{
		PBYTE _TheMask = (PBYTE)&_MSK; //.ALC
		_TheMask[0] = 0x2E;
		_TheMask[1] = 0x50;
		_TheMask[2] = 0x4C;
		_TheMask[3] = 0x52;
	}
}

bool CSQPooler::Check() const
{
	{
		const PBYTE _TheMask = (const PBYTE)&_MSK;
		return _Queue.Check() && _Array.Check() && \
			   (_TheMask[0] == 0x2E) && \
			   (_TheMask[1] == 0x50) && \
			   (_TheMask[2] == 0x4C) && \
			   (_TheMask[3] == 0x52);
	}
}

PBYTE CSQPooler::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}

UINT CSQPooler::UsableNodes() const
{
	return _Queue.Count();	
}

UINT CSQPooler::Nodes() const
{
	return _Array.Count();	
}

const CSQNode *CSQPooler::Get() const
{   
    return _Queue.Get();
}

void CSQPooler::Put(const CSQNode &_TheNode) const
{
	_Queue.Put2Head(_TheNode);
}


UINT EvaluatePooler(UINT _Nodes)
{
	return sizeof(CSQPooler)+(sizeof(CSQNode) * _Nodes);
}

};

};
