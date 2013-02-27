#include "MQLicense.hpp"
#include "MQException.hpp"

using namespace Galaxy::AMQ;


/*CMQLicense*/
CMQLicense::CMQLicense(CPBYTE _Buffer)
	:_Lic()
{
	PBYTE	_LicPtr = (PBYTE) &_Lic;
	for(UINT i=0;i<Size();i++)
	{
		_LicPtr[(Size()-1)-i] = ((~_Buffer[i]) & 0xFF);	
	}
	
	USHORT _CheckSumPart = ::MQCheckSum(_LicPtr,Size()-sizeof(_Lic._CheckSum));
	UINT _CheckSum = (_CheckSumPart << 16)|((~_CheckSumPart) & 0xFFFF);
	
	if(_CheckSum!=UINTDecoding(_Lic._CheckSum))
	{
		THROW_MQEXCEPTION("License Format Incorrect!");
		return; 	
	}
}

CMQLicense::~CMQLicense()
{
	
}

UINT CMQLicense::UINTDecoding(UINT _v) const
{
	PBYTE	_Ptr = (PBYTE) &_v;
	return ((_Ptr[0] << 24)|(_Ptr[1] << 16)|(_Ptr[2] << 8)|(_Ptr[3] & 0xFF)); 	
}
		
void CMQLicense::Migrate(PBYTE _Buffer,UINT _BufSize) const
{
	if(_BufSize < sizeof(_Lic))
	{
		THROW_MQEXCEPTION("Migrate Buffer small!"); 
		return;	
	}
	
	PBYTE	_LicPtr = (PBYTE) &_Lic;
	for(UINT i=0;i<sizeof(_Lic);i++)
	{
		_Buffer[(sizeof(_Lic)-1)-i] = ((~_LicPtr[i]) & 0xFF);	
	}
}

UINT CMQLicense::Size() const
{
	return sizeof(_Lic);	
}
			
CPBYTE CMQLicense::Provider() const
{
	return _Lic._Provider;	
}

CPBYTE CMQLicense::Consumer() const
{
	return _Lic._Consumer;	
}

CPBYTE CMQLicense::Licensed() const
{
	return _Lic._Licensed;	
}

UINT CMQLicense::MaxPages() const
{
	if(IsTrial())
	{
		return 8;	
	}
	else
	{
		return UINTDecoding(_Lic._MaxPages); 
	}	
}

UINT CMQLicense::MaxPageSize() const
{
	if(IsTrial())
	{
		return 64;	
	}
	else
	{
		return UINTDecoding(_Lic._MaxPageSize); 
	}
}

UINT CMQLicense::MaxDQs() const
{
	if(IsTrial())
	{
		return 2;	
	}
	else
	{		
		return UINTDecoding(_Lic._MaxDQs); 
	}
}

UINT CMQLicense::MaxNQs() const
{
	if(IsTrial())
	{
		return 2;	
	}
	else
	{
		return UINTDecoding(_Lic._MaxNQs);
	}
}

UINT CMQLicense::DateLimit() const
{
	return UINTDecoding(_Lic._DateLimit);		
}

UINT CMQLicense::CheckSum() const
{
	return UINTDecoding(_Lic._CheckSum);	
}


bool CMQLicense::IsTrial() const
{
	return (_Lic._Magic[0]!=0xCA)||(_Lic._Magic[1]!=0xCB)||(_Lic._Magic[2]!=0xCC)||(_Lic._Magic[3]!=0xCD);
}


bool CMQLicense::IsDateLimited() const
{
	struct	tm 		_stme;

	{
		time_t			_time;
		CRT_time(&_time);
		CRT_localtime_r(&_time, &_stme);
	}
	
	UINT _Year[2] = {(_stme.tm_year+1900),((DateLimit() >> 16 ) & 0xFFFF)};
	UINT _Month[2] = {(_stme.tm_mon+1),((DateLimit() >> 8 ) & 0xFF)};
	UINT _Day[2] = {_stme.tm_mday,(DateLimit() & 0xFF )};

	if(_Year[0] < _Year[1])
	{
		return false;	
	}
	else if(_Year[0] == _Year[1])
	{
		if(_Month[0] < _Month[1])
		{
			return false;	
		}
		else if(_Month[0]  == _Month[1])
		{
			if(_Day[0] <= _Day[1])
			{
				return false;		
			}
		}	
	}

	return true;
}

/*CMQLicenseBuilder*/
CMQLicenseBuilder::CMQLicenseBuilder(PBYTE _Provider,PBYTE _Consumer,PBYTE _Licensed,UINT _Pages,UINT _PageSize,UINT _NQs,UINT _DQs,USHORT _Year,BYTE _Month,BYTE _Day)
	:_Lic()
{
	if(_Month > 12)
	{
		_Month = 12;
	}
	else if(_Month == 0)
	{
		_Month = 1;	
	}
	
	if(_Day  > 31)
	{
		_Day = 31;	
	}
	else if(_Day == 0)
	{
		_Day = 1;	
	}
	
	if(_Year > 3000)
	{
		_Year = 3000;		
	}
	
	{
		MQLINENSE	_TheLic;
		_TheLic._Magic[0]=0xCA;
		_TheLic._Magic[1]=_TheLic._Magic[0]+1;
		_TheLic._Magic[2]=_TheLic._Magic[1]+1;
		_TheLic._Magic[3]=_TheLic._Magic[2]+1;
		
		CRT_snprintf((PSTR)_TheLic._Provider,sizeof(_TheLic._Provider),"%s",(PSTR)_Provider);
		CRT_snprintf((PSTR)_TheLic._Consumer,sizeof(_TheLic._Consumer),"%s",(PSTR)_Consumer);
		CRT_snprintf((PSTR)_TheLic._Licensed,sizeof(_TheLic._Licensed),"%s",(PSTR)_Licensed);
		
		_TheLic._MaxPages = UINTEncoding(_Pages);
		_TheLic._MaxPageSize = UINTEncoding(_PageSize);
		_TheLic._MaxNQs = UINTEncoding(_NQs);
		_TheLic._MaxDQs = UINTEncoding(_DQs);
		
		_TheLic._DateLimit = UINTEncoding(((_Year << 16)|(_Month << 8)|(_Day & 0xFF)));
		
		USHORT _Chksum = ::MQCheckSum(&_TheLic,sizeof(_TheLic) - sizeof(_TheLic._CheckSum));
		
		_TheLic._CheckSum = UINTEncoding((_Chksum << 16)| ((~_Chksum) & 0xFFFF));
		
		
		PBYTE	_LicPtr0 = (PBYTE) &_TheLic;
		PBYTE	_LicPtr1 = (PBYTE) &_Lic;
		for(UINT i=0;i<sizeof(_Lic);i++)
		{
			_LicPtr1[(sizeof(_Lic)-1)-i] = ((~_LicPtr0[i]) & 0xFF);	
		}
	}

}

CMQLicenseBuilder::~CMQLicenseBuilder()
{
	
}

UINT CMQLicenseBuilder::UINTEncoding(UINT _V)
{
	UINT 				 _R = 0;
	PBYTE _Ptr = (PBYTE) &_R;
	_Ptr[0] = (_V >> 24) & 0xFF;
	_Ptr[1] = (_V >> 16) & 0xFF;
	_Ptr[2] = (_V >> 8) & 0xFF;	
	_Ptr[3] = (_V & 0xFF);
	
	return _R;
}

CPBYTE CMQLicenseBuilder::GetLicense() const
{
	return (CPBYTE)&_Lic;	
}


/*CB2WB*/
/*
CB2WB::CB2WB(CPBYTE _B,UINT _BLength,PBYTE _WB,UINT _WBSize)
{
	UINT		i;
	BYTE		HIBYTE;
	BYTE		LOBYTE;
	
	if(ISNULL(_B)||ISNULL(_WB))
	{
		THROW_MQEXCEPTION("NULL Ptr not allowed!");
		return;
	}
	else if(_WBSize <= (_BLength * 2))
	{
		THROW_MQEXCEPTION("WB Buffer so small!");
		return;
	}
	
	
	for(i=0;i<_BLength;i++)
	{
		HIBYTE = (BYTE)(_B[i] >> 4 ) & 0x0F;
		LOBYTE = (BYTE)(_B[i] & 0x0F);
		
		if(HIBYTE > 0x09)
		{
			_WB[(i*2)]=HIBYTE + 0x37;
		}
		else
		{
			_WB[(i*2)]=HIBYTE + 0x30;
		}

		if(LOBYTE > 0x09)
		{
			_WB[(i*2+1)]=LOBYTE + 0x37;
		}
		else
		{
			_WB[(i*2+1)]=LOBYTE + 0x30;
		}
	}
	_WB[(i*2)]=0;
	_WB[(i*2+1)]=0;
}

CB2WB::~CB2WB()
{
	
}
*/

/*CWB2B*/
/*
CWB2B::CWB2B(CPBYTE _WB,UINT _WBLength,PBYTE _B,UINT _BSize)
{
	UINT		i;
	UINT		j;
	
	BYTE	HIBYTE;
	BYTE	LOBYTE;

	if(ISNULL(_B)||ISNULL(_WB))
	{
		THROW_MQEXCEPTION("NULL Ptr not allowed!");
		return;
	}
	else if((_WBLength%2)!=0)
	{
		THROW_MQEXCEPTION("WB Length Incorrect!");
		return;
	}
	else if(_BSize <= (_WBLength / 2))
	{
		THROW_MQEXCEPTION("B Buffer so small!");
		return;
	}
	

	for(i=0,j=0;(i<_WBLength) && (j<(_WBLength / 2));i+=2,j++)
	{
		HIBYTE = 0;
		LOBYTE = 0;
		
		if(_WB[i] >='a')
		{
			HIBYTE = (_WB[i]-0x57);		
		}
		else if(_WB[i] >='A')
		{
			HIBYTE = (_WB[i]-0x37);		
		}
		else
		{
			HIBYTE = (_WB[i]-0x30);		
		}
		
		if(_WB[i+1] >='a')
		{
			LOBYTE = (_WB[i+1]-0x57);		
		}
		else if(_WB[i+1] >='A')
		{
			LOBYTE = (_WB[i+1]-0x37);		
		}
		else
		{
			LOBYTE = (_WB[i+1]-0x30);	
		}
		
		_B[j]=((HIBYTE << 4) & 0xF0) | (LOBYTE & 0x0F);
	}

	_B[j]=0;

}

CWB2B::~CWB2B()
{
	
}
*/
			