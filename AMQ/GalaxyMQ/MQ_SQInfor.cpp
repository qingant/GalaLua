#include <string.h>
#include "MQ_SQInfor.hpp"
#include "MQ_SQAllocator.hpp"
#include "MQ_SQNodes.hpp"
#include "MQ_SQueue.hpp"
#include "MQException.hpp"

namespace Galaxy
{
namespace AMQ
{

#define MQ_VERSION			(0x36)
#define MQ_MAGIC			(0xDC)
	
const BYTE INF_MAGIC  		= 0;
const BYTE INF_BITWD  		= 1;
const BYTE INF_ALIGN  		= 2;
const BYTE INF_VERSN  		= 3;
const BYTE INF_ENDIAN		= 4;

const BYTE CHK_BGNMAK  		= 0;
const BYTE CHK_PAGE  		= 1;
const BYTE CHK_PGARRAY 		= 2;
const BYTE CHK_ALLOCATOR  	= 3;
const BYTE CHK_NODE			= 4;
const BYTE CHK_NDARRAY		= 5;
const BYTE CHK_POOLER		= 6;
const BYTE CHK_SQSUITE		= 7;
const BYTE CHK_QARRAY		= 8;
const BYTE CHK_INFOR		= 9;
const BYTE CHK_LOCK			= 10;
const BYTE CHK_WAITABLE		= 11;
const BYTE CHK_EVENT		= 12;
const BYTE CHK_DOOR			= 13;
const BYTE CHK_PODTEST		= 14;
const BYTE CHK_ENDMAK  		= 15;

const BYTE POS_QARRAY		= 0;
const BYTE POS_POOLER  		= 1;
const BYTE POS_ALLOCATOR	= 2;

typedef struct
{
   BYTE    _a;
   SHORT   _b;
   UINT    _c;
   LONG    _d;
}STALIGN;

typedef union 
{
	CSQPage			_Page;
	CSQPageArray	_PgArray;
	CSQAllocator	_Alloc;
	CSQNode			_Node;
	CSQNodeArray	_NdArray;
	CSQPooler		_Pooler;
	CSQPrivatePooler _PPL;
	CSQSuite		_Suite;
	CSQSuiteArray	_SQArray;
	CSQInfor		_Infor;
}UN_PODTEST;


static USHORT F_MQCheckSum(PVOID _Buffer, UINT _Length)
{ 
	USHORT *_StepBuffer = (USHORT *)_Buffer;
	UINT cksum=0; 

	while(_Length >1) 
	{ 
		cksum+=*_StepBuffer++;
		_Length -=sizeof(USHORT);
	}
	
	if(_Length)
	{
		cksum += *((PBYTE)_StepBuffer);
	}
	
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >>16);
	return (USHORT)(~cksum);
}


/*CSQInfor*/
void CSQInfor::Init(UINT _Queues,UINT _MaxDepth,USHORT _CatchCount,UINT _Nodes,UINT _Pages,USHORT _PageSize)
{
	{
		_Magic[INF_MAGIC] = MQ_MAGIC;
		_Magic[INF_BITWD] = sizeof(LONG);
		_Magic[INF_ALIGN] = sizeof(STALIGN);
		_Magic[INF_VERSN] = MQ_VERSION;
		
		PUINT _pbed = (PUINT) &(_Magic[INF_ENDIAN]);
		*_pbed = 0x00000001;
		
		_UCHK[CHK_BGNMAK]	=0xDD;
		_UCHK[CHK_PAGE]		=sizeof(CSQPage);
		_UCHK[CHK_PGARRAY]	=sizeof(CSQPageArray);
		_UCHK[CHK_ALLOCATOR]=sizeof(CSQAllocator);
		_UCHK[CHK_NODE]		=sizeof(CSQNode);
		_UCHK[CHK_NDARRAY]	=sizeof(CSQNodeArray);
		_UCHK[CHK_POOLER]	=sizeof(CSQPooler);
		_UCHK[CHK_SQSUITE]	=sizeof(CSQSuite);
		_UCHK[CHK_QARRAY]	=sizeof(CSQSuiteArray);
		_UCHK[CHK_INFOR]	=sizeof(CSQInfor);
		_UCHK[CHK_LOCK]		=sizeof(CSQLock);
		_UCHK[CHK_WAITABLE]	=sizeof(CSQCond);
		_UCHK[CHK_EVENT]	=sizeof(CSQEvent);
		_UCHK[CHK_DOOR]		=sizeof(CSQDoor);
		{
			UN_PODTEST  _PodTest;
			_UCHK[CHK_PODTEST]=sizeof(_PodTest);	
		}
		_UCHK[CHK_ENDMAK]=0xEE;
		
		
		{
			_Env._Queues 	= _Queues;
			_Env._Nodes 	= _Nodes;
			_Env._Pages 	= _Pages;
			_Env._PageSize 	= _PageSize;
		}
		
		{
			_CheckSum = 0xFFFFFFFF;
		}
	
		time(&_CreateDate);

		{
	      	PBYTE _PtrAuthor = (PBYTE)&(_Author);
	      	
	      	_PtrAuthor[7] = (~0x5b) & 0xFF;
	      	_PtrAuthor[6] = (~0xc0) & 0xFF;
	      	_PtrAuthor[5] = (~0xee) & 0xFF;
	   		_PtrAuthor[4] = (~0xd3) & 0xFF;
	      	_PtrAuthor[3] = (~0xee) & 0xFF;
	   		_PtrAuthor[2] = (~0xcf) & 0xFF;
	      	_PtrAuthor[1] = (~0xe8) & 0xFF;
	      	_PtrAuthor[0] = (~0x5d) & 0xFF;
    	}
    	
    	//LIC
    	{
    		memset(_License,0,sizeof(_License));
    	}
    	
    	{
	    	//QUEUE
	    	_Offset[POS_QARRAY]._Offset = sizeof(CSQInfor);
	    	_Offset[POS_QARRAY]._Size = EvaluateSQueue(_Queues);
	    	
	    	//POOLER
	    	_Offset[POS_POOLER]._Offset = (_Offset[POS_QARRAY]._Offset + _Offset[POS_QARRAY]._Size);
	    	_Offset[POS_POOLER]._Size = EvaluatePooler(_Nodes);
	
			//Allocator
	    	_Offset[POS_ALLOCATOR]._Offset = (_Offset[POS_POOLER]._Offset + _Offset[POS_POOLER]._Size);
	    	_Offset[POS_ALLOCATOR]._Size = EvaluateAllocator(_Pages,_PageSize);
	    	
	    	//_Size = sizeof(*this) + _Offset[POS_QARRAY]._Size + _Offset[POS_POOLER]._Size + _Offset[POS_ALLOCATOR]._Size;
	    	_Size = MQEvaluate(_Queues,_Nodes,_Pages,_PageSize);
    	}
    	
    	
    	{
    		USHORT _CheckSumPart = F_MQCheckSum(this,sizeof(*this));	
    		_CheckSum = (_CheckSumPart << 16)|((~_CheckSumPart) & 0xFFFF);	
    	}
	}
	
	CSQAllocator &_TheAllocator = Allocator();
	CSQPooler &_ThePooler = Pooler();
	CSQSuiteArray &_TheQArray = SQArray();
	
	_TheAllocator.Init(_Pages,_PageSize);
	_ThePooler.Init(_Nodes);
	_TheQArray.Init(_Queues,Allocator(),Pooler(),_MaxDepth,_CatchCount);
	
	
	Check();
}

bool CSQInfor::Check() const
{

	if(_Magic[INF_MAGIC] != MQ_MAGIC)
	{
		THROW_MQEXCEPTION("INF_MAGIC not match.");
		return false;	
	}
	
	if(_Magic[INF_BITWD] != sizeof(LONG))
	{
		THROW_MQEXCEPTION("INF_BITWD not match.");
		return false;	
	}
	
	if(_Magic[INF_ALIGN] != sizeof(STALIGN))
	{
		THROW_MQEXCEPTION("INF_ALIGN not match.");
		return false;	
	}
	
	if(_Magic[INF_VERSN] != MQ_VERSION)
	{
		THROW_MQEXCEPTION("INF_VERSN not match.");
		return false;	
	}

	{
		PUINT _pbed = (PUINT) &(_Magic[INF_ENDIAN]);
		if(*_pbed != 0x00000001)
		{
			THROW_MQEXCEPTION("INF_ENDIAN not match.");
			return false;	
		}
	}

	if(_UCHK[CHK_BGNMAK]!=0xDD)
	{
		THROW_MQEXCEPTION("CHK_BGNMAK not match.");
		return false;	
	}
	
	if(_UCHK[CHK_PAGE]!=sizeof(CSQPage))
	{
		THROW_MQEXCEPTION("CHK_PAGE not match.");
		return false;	
	}
	
	if(_UCHK[CHK_PGARRAY]!=sizeof(CSQPageArray))
	{
		THROW_MQEXCEPTION("CHK_PGARRAY not match.");
		return false;	
	}
	
	if(_UCHK[CHK_ALLOCATOR]!=sizeof(CSQAllocator))
	{
		THROW_MQEXCEPTION("CHK_ALLOCATOR not match.");
		return false;	
	}

	if(_UCHK[CHK_NODE]!=sizeof(CSQNode))
	{
		THROW_MQEXCEPTION("CHK_NODE not match.");
		return false;
	}
	
	if(_UCHK[CHK_NDARRAY]!=sizeof(CSQNodeArray))
	{
		THROW_MQEXCEPTION("CHK_NDARRAY not match.");
		return false;
	}
	
	if(_UCHK[CHK_POOLER]!=sizeof(CSQPooler))
	{
		THROW_MQEXCEPTION("CHK_POOLER not match.");
		return false;
	}
	
	if(_UCHK[CHK_SQSUITE]!=sizeof(CSQSuite))
	{
		THROW_MQEXCEPTION("CHK_SQSUITE not match.");
		return false;
	}
	
	if(_UCHK[CHK_QARRAY]!=sizeof(CSQSuiteArray))
	{
		THROW_MQEXCEPTION("CHK_QARRAY not match.");
		return false;
	}
	
	if(_UCHK[CHK_INFOR]!=sizeof(CSQInfor))
	{
		THROW_MQEXCEPTION("CHK_INFOR not match.");
		return false;
	}
	
	if(_UCHK[CHK_LOCK]!=sizeof(CSQLock))
	{
		THROW_MQEXCEPTION("CHK_INFOR not match.");
		return false;
	}
	
	if(_UCHK[CHK_WAITABLE]!=sizeof(CSQCond))
	{
		THROW_MQEXCEPTION("CHK_WAITABLE not match.");
		return false;
	}
	
	if(_UCHK[CHK_EVENT]!=sizeof(CSQEvent))
	{
		THROW_MQEXCEPTION("CHK_EVENT not match.");
		return false;
	}
	
	if(_UCHK[CHK_DOOR]!=sizeof(CSQDoor))
	{
		THROW_MQEXCEPTION("CHK_DOOR not match.");
		return false;
	}
	
	{
		UN_PODTEST  _PodTest;
		if(_UCHK[CHK_PODTEST]!=sizeof(_PodTest))
		{
			THROW_MQEXCEPTION("CHK_PODTEST not match.");
			return false;
		}
	}
	
	if(_UCHK[CHK_ENDMAK]!=0xEE)
	{
		THROW_MQEXCEPTION("CHK_PODTEST not match.");
		return false;
	}

	
	{
      	PBYTE _PtrAuthor = (PBYTE)&(_Author);      	
      	
      	if((_PtrAuthor[7] != (~0x5b & 0xFF))|| \
      		(_PtrAuthor[6] != (~0xc0 & 0xFF))|| \
      		(_PtrAuthor[5] != (~0xee & 0xFF))|| \
   			(_PtrAuthor[4] != (~0xd3 & 0xFF))|| \
      		(_PtrAuthor[3] != (~0xee & 0xFF))|| \
   			(_PtrAuthor[2] != (~0xcf & 0xFF))|| \
      		(_PtrAuthor[1] != (~0xe8 & 0xFF))|| \
      		(_PtrAuthor[0] != (~0x5d & 0xFF)))
      	{
			THROW_MQEXCEPTION("CHK_AUTH not match.");
			return false;
      	}
	}
	
	
	{
		CSQInfor	*_This = (CSQInfor	*)this;
		CSQAllocator &_TheAllocator = _This->Allocator();
		CSQPooler &_ThePooler = _This->Pooler();
		CSQSuiteArray &_TheQArray = _This->SQArray();
		
		
		if(!_TheAllocator.Check())
		{
			THROW_MQEXCEPTION(".ALC check Error.");
			return false;
		}
		
		if(!_ThePooler.Check())
		{
			THROW_MQEXCEPTION(".PLR check Error.");
			return false;
		}

		if(!_TheQArray.Check())
		{
			THROW_MQEXCEPTION(".MQS check Error.");
			return false;
		}
		
		if(_Env._Queues!= _TheQArray.Count())
		{
			THROW_MQEXCEPTION("NUM_QUEUES not match.");
			return false;
		}
		
		if(_Env._Nodes!= _ThePooler.Nodes())
		{
			THROW_MQEXCEPTION("NUM_NODES not match.");
			return false;
		}
		
		if(_Env._Pages!= _TheAllocator.Pages())
		{
			THROW_MQEXCEPTION("NUM_PAGES not match.");
			return false;
		}
		
		if(_Env._PageSize!= _TheAllocator.PageSize())
		{
			THROW_MQEXCEPTION("NUM_PAGESIZE not match.");
			return false;
		}
		
		if(_Offset[POS_QARRAY]._Offset!= (((LONG)&_TheQArray)-((LONG)this)))
		{
			THROW_MQEXCEPTION("OFS_QMPOSIT not match.");
			return false;
		}
		
		if(_Offset[POS_QARRAY]._Size!= EvaluateSQueue(_TheQArray.Count()))
		{
			THROW_MQEXCEPTION("OFS_QMSIZE not match.");
			return false;
		}
		
		
		if(_Offset[POS_POOLER]._Offset!= (((LONG)&_ThePooler)-((LONG)this)))
		{
			THROW_MQEXCEPTION("OFS_PLROSIT not match.");
			return false;
		}
		
		if(_Offset[POS_POOLER]._Size!= EvaluatePooler(_ThePooler.Nodes()))
		{
			THROW_MQEXCEPTION("OFS_PLRSIZE not match.");
			return false;
		}
		
		if(_Offset[POS_ALLOCATOR]._Offset!= (((LONG)&_TheAllocator)-((LONG)this)))
		{
			THROW_MQEXCEPTION("OFS_ALCAPOSIT not match.");
			return false;
		}
		
		if(_Offset[POS_ALLOCATOR]._Size!= EvaluateAllocator(_TheAllocator.Pages(),_TheAllocator.PageSize()))
		{
			THROW_MQEXCEPTION("OFS_ALCASIZE not match.");
			return false;
		}
		
		if(_Size != MQEvaluate(_TheQArray.Count(),_ThePooler.Nodes(),_TheAllocator.Pages(),_TheAllocator.PageSize()))
		{
			THROW_MQEXCEPTION("MEDIA_SIZE not match.");
			return false;
		}
		
	}
	
	{
		CSQInfor  _TheSumInfor;
		memcpy(&_TheSumInfor,this,sizeof(_TheSumInfor));
		_TheSumInfor._CheckSum = 0xFFFFFFFF;
		
		USHORT _CheckSumPart = F_MQCheckSum(&_TheSumInfor,sizeof(_TheSumInfor));	
		UINT _TheCheckSum = (_CheckSumPart << 16)|((~_CheckSumPart) & 0xFFFF);
		if(_TheCheckSum!=_CheckSum)
		{
			THROW_MQEXCEPTION("CHECKSUM not match.");
			return false;	
		}	
	}
	
	return true;	
}

BYTE CSQInfor::GetMagic() const
{
	return 	_Magic[INF_MAGIC];
}

BYTE CSQInfor::GetBitwidth() const
{
	return 	_Magic[INF_BITWD];	
}

CSQSuiteArray &CSQInfor::SQArray()
{
	UINT _TheArrayOffset = (_Offset[POS_QARRAY]._Offset);
	CSQSuiteArray *_TheSQArray = (CSQSuiteArray *)(((PBYTE)this) + _TheArrayOffset);
	return *_TheSQArray;
}

CSQPooler &CSQInfor::Pooler()
{
	CSQPooler *_ThePooler = (CSQPooler *)(((PBYTE)this) + (_Offset[POS_POOLER]._Offset));
	return *_ThePooler;
}

CSQAllocator &CSQInfor::Allocator()
{
	CSQAllocator *_TheAllocator = (CSQAllocator *)(((PBYTE)this) + (_Offset[POS_ALLOCATOR]._Offset));
	return *_TheAllocator;
}

const CSQSuiteArray &CSQInfor::SQArray() const
{
	CSQInfor *_This = (CSQInfor *)this;
	return _This->SQArray();
}




UINT MQEvaluate(UINT _Queues,UINT _Nodes,UINT _Pages,USHORT _PageSize)
{
	return sizeof(CSQInfor)	+ EvaluateSQueue(_Queues) + EvaluatePooler(_Nodes) + EvaluateAllocator(_Pages,_PageSize);
}




};

};
