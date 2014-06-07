#include <string>
#include <map>
#include "GalaxyMQ.hpp"
#include "MQException.hpp"
#include "MQ_SQInfor.hpp"
#include "MQT.hpp"




namespace Galaxy
{
namespace AMQ
{

/*CGalaxyMQ*/
CGalaxyMQ::CGalaxyMQ(const std::string &_FileName)
	:_Media(new CMQFileMemory(_FileName,0))
{
	CSQInfor *_TheInfor = T_MQPtrCast<CSQInfor *>(_Media->Entry());
	_TheInfor->Check();	
}

CGalaxyMQ::CGalaxyMQ(key_t _IPCKey)
	:_Media(new CSysvSHMemory(_IPCKey,0))
{
	CSQInfor *_TheInfor = T_MQPtrCast<CSQInfor *>(_Media->Entry());
	_TheInfor->Check();	
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
	return T_MQPtrCast<const CSQInfor *>(_Media->Entry())->SQArray().Count();	
}

const CSQSuite &CGalaxyMQ::operator[](UINT _Index) const
{
	const CSQSuite *_TheSQueue = T_MQPtrCast<const CSQInfor *>(_Media->Entry())->SQArray()[_Index];
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
	_Memory.Zero();
	
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
	
	_Memory.Zero();	
	
	CSQInfor *_Infor = (CSQInfor *)_Memory.Entry();
	_Infor->Init(_Queues,_MaxDepth,_CatchCount,_Nodes,_Pages,_PageSize);
}

CGalaxyMQCreator::~CGalaxyMQCreator()
{
	
}
















};
};