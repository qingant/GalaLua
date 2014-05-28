#ifndef SQINFOR_HPP_A0D700B2_E4E8_4AEA_AB55_7F4873392712
#define SQINFOR_HPP_A0D700B2_E4E8_4AEA_AB55_7F4873392712


#pragma once

#include "GalaxyDT.hpp"

namespace Galaxy
{

namespace AMQ
{

class CSQAllocator;
class CSQPooler;
class CSQSuiteArray;
class CSQInfor
{
	typedef struct
	{
		__volatile__ INT	_Offset;
		__volatile__ UINT	_Size;
	}ST_OFFSETTAB;
	
	typedef struct
	{
		__volatile__ UINT _Queues;
		__volatile__ UINT _Nodes;
		__volatile__ UINT _Pages;
		__volatile__ UINT _PageSize;
	}ST_ENV;
private:
	BYTE                _Magic[8]; 		//0-MAGIC，1-LONG长，2-对齐，3-版本，4~7 字节序
	UINT                _Size;      	//整体大小
	USHORT              _UCHK[16];
	
	ST_ENV				_Env;
	UINT				_CheckSum;		//校验
	time_t				_CreateDate;	//创建时间
	UINT64				_Author;		//作者
	BYTE				_License[128];	//许可证
	ST_OFFSETTAB		_Offset[3];	
	

	CSQPooler &Pooler();
	CSQAllocator &Allocator();
	CSQSuiteArray &SQArray();
public:	
	void Init(UINT _Queues,UINT _MaxDepth,USHORT _CatchCount,UINT _Nodes,UINT _Pages,USHORT _PageSize);
	bool Check() const;
	BYTE GetMagic() const;
	BYTE GetBitwidth() const;
	
	const CSQSuiteArray &SQArray() const;
};

UINT MQEvaluate(UINT _Queues,UINT _Nodes,UINT _Pages,USHORT _PageSize);

} // AMQ namespace
} // Galaxy namespace


#endif /*SQINFOR_HPP_A0D700B2_E4E8_4AEA_AB55_7F4873392712*/
