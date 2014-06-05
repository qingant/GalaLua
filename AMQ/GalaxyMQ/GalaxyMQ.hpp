#ifndef GALAXYMQ_HPP_6DFE47F5_9476_4454_A558_A2D3C431D377
#define GALAXYMQ_HPP_6DFE47F5_9476_4454_A558_A2D3C431D377

#pragma once


#include "MQMedia.hpp"
#include "MQ_SQueue.hpp"

namespace Galaxy
{
namespace AMQ
{
	class CGalaxyMQ : public CGalaxyObject
	{
	private:
		CMQMemory		*_Media;
	
		CGalaxyMQ(const CGalaxyMQ &);
		const CGalaxyMQ &operator=(const CGalaxyMQ &);
	public:
		explicit CGalaxyMQ(const std::string &_FileName);
		explicit CGalaxyMQ(key_t _IPCKey);
		~CGalaxyMQ();
	public:
		UINT Count() const;
		const CSQSuite &operator[](UINT _Index) const;
		//void Backup(const std::string &_FileName) const;
	}; 
	
	class CGalaxyMQCreator : public CGalaxyObject
	{
	private:
		static void* operator new (size_t);
	public:
		explicit CGalaxyMQCreator(const std::string &_FileName, \
				UINT _Queues,UINT _MaxDepth,USHORT _CatchCount,UINT _Nodes,UINT _Pages,USHORT _PageSize);
		explicit CGalaxyMQCreator(key_t _IPCKey, \
				UINT _Queues,UINT _MaxDepth,USHORT _CatchCount,UINT _Nodes,UINT _Pages,USHORT _PageSize);
		~CGalaxyMQCreator();
	};
	
	typedef CGalaxyMQ *  MQMHANDLE;
	typedef CSQSuite *	 MQHANDLE;
	
    
} // AMQ namespace
} // Galaxy namespace

#endif /*GALAXYMQ_HPP_6DFE47F5_9476_4454_A558_A2D3C431D377*/
