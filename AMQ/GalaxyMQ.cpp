
#include "GalaxyMQ.hpp"
#include "MQSuite.hpp"
#include "MQException.hpp"

using namespace Galaxy::AMQ;

/*CGalaxyMQ*/
CGalaxyMQ::CGalaxyMQ(const std::string &_FileName)
    :_MQInstance(new CMQSuite(_FileName))
{
    if(ISNULL(_MQInstance))
    {
        THROW_MQEXCEPTION("Open MQ Error!");    
    }   
}

CGalaxyMQ::CGalaxyMQ(key_t _IPCKey)
    :_MQInstance(new CMQSuite(_IPCKey))
{
    if(ISNULL(_MQInstance))
    {
        THROW_MQEXCEPTION("Open MQ Error!");    
    }   
}

CGalaxyMQ::~CGalaxyMQ()
{
    if(_MQInstance!=NULL)
    {
        delete _MQInstance;
        _MQInstance = NULL;   
    }    
}

const IMQ &CGalaxyMQ::GetInstance() const
{
    if(ISNULL(_MQInstance))
    {
        THROW_MQEXCEPTION("Open MQ Error!");    
    }
    
    return (*_MQInstance);
}

const IMQueueArray &CGalaxyMQ::NQArray() const
{
    return GetInstance().NQArray();    
}

const IMQueueArray &CGalaxyMQ::DQArray() const
{
    return GetInstance().DQArray();     
}


INT  CGalaxyMQ::GetDummyId(GALAXY_MQ_TYPE type, INT id)
{
    if (type == GALAXY_MQ_NQ)
    {
        return (id + DUMMYID);
    }
    else
    {
        return id;
    }
}
	
const IMQueue &CGalaxyMQ::GetQByDummyId(UINT id) const
{
    if (id > DUMMYID)
    {
        return NQArray()[id-DUMMYID];
    }
    else
    {
        return DQArray()[id];
	       
    }
	   
}

const IPoolerView &CGalaxyMQ::PoolerView() const
{
    return GetInstance().PoolerView(); 
}


const IMQueue &CGalaxyMQ::operator[](UINT _Index) const
{
	const IMQueueArray &_NQs = NQArray();
	const IMQueueArray &_DQs = DQArray();
	UINT  _NQTotal = _NQs.Count();
	UINT  _DQTotal = _DQs.Count();
	UINT  _QTotal = (_NQTotal+_DQTotal);
	
	if(_Index >= _QTotal)
	{
		THROW_MQEXCEPTION("_Index Out of Range!");	
	}

	_Index = (_Index % _QTotal);
	
	if(_Index < _NQTotal)
	{
		return _NQs[_Index];	
	}
	else
	{
		return _DQs[(_Index - _NQTotal)];	
	}
}

const IMQ &CGalaxyMQ::InstanceOfSuite() const
{
	return GetInstance();	
}



/*CGalaxyMQCreator*/
CGalaxyMQCreator::CGalaxyMQCreator(const std::string &_FileName,const std::string &_EndUser,UINT _Pages,
                        UINT _PageSize,UINT _NQs,UINT _DQs,UINT _DefaultDepthPeerQ, 
                        UINT _Year,UINT _Month,UINT _Day)
{
    CMQCreator _Creator(_FileName,_EndUser,_Pages,_PageSize,_NQs,_DQs,_DefaultDepthPeerQ,_Year,_Month,_Day);
}

CGalaxyMQCreator::CGalaxyMQCreator(key_t _IPCKey,const std::string &_EndUser,UINT _Pages,
                        UINT _PageSize,UINT _NQs,UINT _DQs,UINT _DefaultDepthPeerQ,
                        UINT _Year,UINT _Month,UINT _Day)
{
    CMQCreator _Creator(_IPCKey,_EndUser,_Pages,_PageSize,_NQs,_DQs,_DefaultDepthPeerQ,_Year,_Month,_Day);
}

CGalaxyMQCreator::~CGalaxyMQCreator()
{
    
}
