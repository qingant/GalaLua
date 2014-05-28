
#include "GalaxyMQ.hpp"
#include "MQException.hpp"

namespace Galaxy
{
namespace AMQ
{

/*CGalaxyMQ*/
CGalaxyMQ::CGalaxyMQ(const std::string &_FileName)
	:_Media(new CMQFileMemory(_FileName,0)),_Infor(*((CSQInfor *)_Media->Entry()))
{
	_Infor.Check();	
}

CGalaxyMQ::CGalaxyMQ(key_t _IPCKey)
	:_Media(new CSysvSHMemory(_IPCKey,0)),_Infor(*((CSQInfor *)_Media->Entry()))
{
	_Infor.Check();	
}

CGalaxyMQ::~CGalaxyMQ()
{
	if(_Media!=NULL)
	{
		delete _Media;	
	}	
}

UINT CGalaxyMQ::Count() const
{
	return _Infor.SQArray().Count();	
}

const CSQSuite &CGalaxyMQ::operator[](UINT _Index) const
{
	const CSQSuite *_TheSQueue = _Infor.SQArray()[_Index];
	if(ISNULL(_TheSQueue))
	{
		THROW_MQNULLEXCEPTION(_TheSQueue);		
	}
	
	return *_TheSQueue;	
}

/*CGalaxyMQCreator*/
CGalaxyMQCreator::CGalaxyMQCreator(const std::string &_FileName,UINT _Queues,UINT _MaxDepth,USHORT _CatchCount,UINT _Nodes,UINT _Pages,USHORT _PageSize)
{
	if((_Queues * _CatchCount) > _Nodes)
	{
		THROW_MQEXCEPTION("must _Nodes > (_Queues * _CatchCount)");
		return;	
	}
	
	if(_MaxDepth > _Nodes)
	{
		THROW_MQEXCEPTION("must _MaxDepth < _Nodes");
		return;	
	}
	
	if(_Pages < (_Nodes * 2))
	{
		THROW_MQEXCEPTION("must _Pages > (_Nodes * 2)");
		return;	
	}
	
	if(_PageSize < 64)
	{
		THROW_MQEXCEPTION("must _PageSize > 64");
		return;	
	}
	

	CMQFileMemory	_Memory(_FileName,MQEvaluate(_Queues,_Nodes,_Pages,_PageSize));	
	
	CSQInfor *_Infor = (CSQInfor *)_Memory.Entry();
	_Infor->Init(_Queues,_MaxDepth,_CatchCount,_Nodes,_Pages,_PageSize);
}

CGalaxyMQCreator::CGalaxyMQCreator(key_t _IPCKey,UINT _Queues,UINT _MaxDepth,USHORT _CatchCount,UINT _Nodes,UINT _Pages,USHORT _PageSize)
{
	
	if((_Queues * _CatchCount) > _Nodes)
	{
		THROW_MQEXCEPTION("must _Nodes > (_Queues * _CatchCount)");
		return;	
	}
	
	if(_MaxDepth > _Nodes)
	{
		THROW_MQEXCEPTION("must _MaxDepth < _Nodes");
		return;	
	}
	
	if(_Pages < (_Nodes * 2))
	{
		THROW_MQEXCEPTION("must _Pages > (_Nodes * 2)");
		return;	
	}
	
	if(_PageSize < 64)
	{
		THROW_MQEXCEPTION("must _PageSize > 64");
		return;	
	}
	
	CSysvSHMemory	_Memory(_IPCKey,MQEvaluate(_Queues,_Nodes,_Pages,_PageSize));	
	
	CSQInfor *_Infor = (CSQInfor *)_Memory.Entry();
	_Infor->Init(_Queues,_MaxDepth,_CatchCount,_Nodes,_Pages,_PageSize);
}

CGalaxyMQCreator::~CGalaxyMQCreator()
{
	
}
















};
};