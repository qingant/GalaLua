#ifndef MQLICENSE_HPP_312768A8_0197_4D15_84BA_81A205F8CDB5
#define MQLICENSE_HPP_312768A8_0197_4D15_84BA_81A205F8CDB5

#pragma once

#include "MQAnchor.hpp"

namespace Galaxy
{
	namespace AMQ
	{
		typedef struct
		{
			BYTE	_Magic[4];
			BYTE	_Provider[24];	//dev.Galaxy.CPP.org
			BYTE	_Consumer[24];	//www.agree.com.cn
			BYTE	_Licensed[16];	//广发银行
			UINT	_MaxPages;
			UINT	_MaxPageSize;
			UINT	_MaxNQs;
			UINT	_MaxDQs; 
			UINT	_DateLimit;		//YEARMMDD	
			UINT	_CheckSum; 
		}MQLINENSE,*PMQLINENSE;


		class CMQLicense : public CMQLoneAncestor
		{
		private:
			MQLINENSE		_Lic;
			
			inline UINT UINTDecoding(UINT _v) const;
		public:
			explicit CMQLicense(CPBYTE _Buffer);
			~CMQLicense();	
		public:		
			void Migrate(PBYTE _Buffer,UINT _BufSize) const;
			
			UINT Size() const;
			CPBYTE Provider() const;
			CPBYTE Consumer() const;
			CPBYTE Licensed() const;
			UINT MaxPages() const;
			UINT MaxPageSize() const;
			UINT MaxNQs() const;
			UINT MaxDQs() const;
			UINT DateLimit() const;
			UINT CheckSum() const;
			
			bool IsTrial() const;
			bool IsDateLimited() const;
		};
		
		
		class CMQLicenseBuilder : public CMQLoneAncestor
		{
		private:
			MQLINENSE		_Lic;
			
			inline UINT UINTEncoding(UINT _V);
		public:
			explicit CMQLicenseBuilder(PBYTE _Provider,PBYTE _Consumer,PBYTE _Licensed,UINT _Pages,UINT _PageSize,UINT _NQs,UINT _DQs,USHORT _Year,BYTE _Month,BYTE _Day);
			~CMQLicenseBuilder();
			
			CPBYTE GetLicense() const;			
		};
		
		
		/*
		class CB2WB : public CLoneAncestor
		{
		public:
			explicit CB2WB(CPBYTE _B,UINT _BLength,PBYTE _WB,UINT _WBSize);
			~CB2WB();		
		};
		
		class CWB2B : public CLoneAncestor
		{
		public:
			explicit CWB2B(CPBYTE _WB,UINT _WBLength,PBYTE _B,UINT _BSize);
			~CWB2B();		
		};
		*/
	}
}

#endif /*MQLICENSE_HPP_312768A8_0197_4D15_84BA_81A205F8CDB5*/
