#ifndef MQCARRIER_HPP_09E9805D_10C7_4BC8_B3B8_123D76ECD30C
#define MQCARRIER_HPP_09E9805D_10C7_4BC8_B3B8_123D76ECD30C

#pragma once

#include "MQI.hpp"

namespace Galaxy
{
	namespace AMQ
	{
		class CMQCarrier : public CMQLoneAncestor
		{
		public:
			CMQCarrier(PBYTE _D,CPBYTE _S,UINT _Length);
			~CMQCarrier();
		};
	}
}


#endif /*MQCARRIER_HPP_09E9805D_10C7_4BC8_B3B8_123D76ECD30C*/
