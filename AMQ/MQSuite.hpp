#ifndef MQSUITE_HPP_9C2DE8E3_5D66_4A55_9C2B_B9EE51149172
#define MQSUITE_HPP_9C2DE8E3_5D66_4A55_9C2B_B9EE51149172

#pragma once

#include "MQAnchor.hpp"
#include "MQView.hpp"
#include "MQNQ.hpp"
#include "MQDQ.hpp"

namespace Galaxy
{
	namespace AMQ
	{
		class CMQSuite : public IMQ
		{
		private:
			CAnchor			*_Anchor;
			CPooler         	_Pooler;
			CNmlQArray      	_NQArray;
			CDsbQArray      	_DQArray;
			CMQPoolerView   	_PoolerView;
			
			
			inline CAnchor *CreateAnchor(const std::string &_FileName);
			inline CAnchor *CreateAnchor(key_t _IPCKey);
			
			CMQSuite(const CMQSuite &);
			const CMQSuite & operator=(const CMQSuite &); 
		public:
			explicit CMQSuite(const std::string &_FileName);
			explicit CMQSuite(key_t _IPCKey);
			~CMQSuite();
			
			
			void BKD(UINT _BKDCode,...) const;
			const IMQueueArray &NQArray() const;
			const IMQueueArray &DQArray() const;
			const IPoolerView &PoolerView() const;
		};
	
		class CMQCreator : public CMQLoneAncestor
		{
		public:
			explicit CMQCreator(const std::string &_FileName,const std::string &_EndUser,UINT _Pages,
			            UINT _PageSize,UINT _NQs,UINT _DQs,UINT _DefaultDepthPeerQ,UINT _Year,UINT _Month,UINT _Day);
			explicit CMQCreator(key_t _IPCKey,const std::string &_EndUser,UINT _Pages,
			            UINT _PageSize,UINT _NQs,UINT _DQs,UINT _DefaultDepthPeerQ,UINT _Year,UINT _Month,UINT _Day);
			~CMQCreator();
		
		};
	} // AMQ namespace
} // Galaxy namespace











#endif /*MQSUITE_HPP_9C2DE8E3_5D66_4A55_9C2B_B9EE51149172*/
