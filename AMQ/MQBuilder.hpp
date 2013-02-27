#ifndef MQCREATOR_HPP_FF8A46E3_D7A0_46DC_9F33_D80FBB52D037
#define MQCREATOR_HPP_FF8A46E3_D7A0_46DC_9F33_D80FBB52D037


#pragma once

#include "MQAnchor.hpp"
#include "MQLicense.hpp"

namespace Galaxy
{
	namespace AMQ
	{
		class CMQBuilder : public CMQLoneAncestor
		{
		public:   
		 	explicit CMQBuilder(const CAnchor &_Anchor,const CMQLicense &_License,UINT _PreLimit);
		 	~CMQBuilder();  
		
		};
	}
}










#endif /*MQCREATOR_HPP_FF8A46E3_D7A0_46DC_9F33_D80FBB52D037*/
