#ifndef MQBLOCK_HPP_20B04A05_ABFB_4E89_852E_F064112B19C0
#define MQBLOCK_HPP_20B04A05_ABFB_4E89_852E_F064112B19C0

#pragma once

#include "MQI.hpp"

namespace Galaxy
{
   namespace AMQ
   {		
		class CMQRefBlock : public IBlock
		{
		private:
			PBYTE    _Buffer;
			UINT	 _Size;
			
			CMQRefBlock(const CMQRefBlock &);
			const CMQRefBlock & operator=(const CMQRefBlock &);
		public:
			CMQRefBlock(PBYTE _TheBuffer,UINT _TheSize);
			~CMQRefBlock();
			PBYTE RAWEntry() const;
			UINT RAWSize() const;
		};

		
		
		// CStrBlock
		// an adapter to std::string
		// only used to convert interface from IBlock to std::string
		class CStrBlock:public IBlock
		{
		public:
			CStrBlock( std::string & );
			CStrBlock( const CStrBlock & );
			virtual ~CStrBlock();
			PBYTE RAWEntry() const;
			UINT RAWSize() const;
		private:
			CStrBlock();
		private:
			std::string &_Block;
		};

      
   } // AMQ namespace
} // Galaxy namespace


#endif /*MQBLOCK_HPP_20B04A05_ABFB_4E89_852E_F064112B19C0*/
