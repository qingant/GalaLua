#ifndef SQALLOCATOR_HPP_25B2229B_6AE1_4F73_BBA6_9A8AAF1B71EE
#define SQALLOCATOR_HPP_25B2229B_6AE1_4F73_BBA6_9A8AAF1B71EE


#pragma once

#include "MQ_SQSync.hpp"
#include "MQ_SQList.hpp"

namespace Galaxy
{

namespace AMQ
{
	
#pragma pack(1)

class CSQPage
{
private:
	__volatile__ 	INT		_Next;
	__volatile__ 	UINT	_Length; //|~~~PAGESIZE~~~|~~~LENGTH~~~|
	BYTE			_Data[0];	
public:
	void Init(USHORT _PageSize);
	void Next(const CSQPage *_Page) const;
	const CSQPage *Next() const;
	
	USHORT Write(const PSTR _Buffer,USHORT _DataLen) const;
	USHORT Read(PSTR _Buffer,USHORT _MaxLen) const;
	
	void Length(USHORT _TheLength) const;
	USHORT Length() const;
	void PageSize(USHORT _PageSize) const;
	USHORT PageSize() const;
	const PBYTE Data() const;
	PBYTE NearPtr() const;
};

class CSQPageArray
{
private:
	__volatile__ UINT	_Total;
	__volatile__ UINT	_PageSize;
public:
	void Init(UINT _TheTotal,USHORT _ThePageSize);
	PBYTE NearPtr() const; // 这个近指针实际上是PAGE[0]的位置，千万别用错	
	UINT Count() const;
	USHORT PageSize() const;
	const CSQPage *operator[](UINT _Index) const;
};

class CSQAllocator
{
	typedef CSQList<CSQPage> CSQPageList_T;
private:
	CSQLock			_Lock;
	CSQPageList_T	_List;
	CSQPageArray	_Array;
public:	
	void Init(UINT _Pages,USHORT _PageSize);
	PBYTE NearPtr() const; // 这个近指针实际上是PAGE[0]的位置，千万别用错	
	UINT UsablePages() const;
	UINT Pages() const;
	UINT PageSize() const;	

	const CSQPage *Allocate(UINT _Size) const;
	void Free(const CSQPage &_TheMultiPage) const;
	
	//Nursery专用接口，纯粹利用C++特性增加的 等于 Free
	void _NurseryPut(const CSQPage &_TheMultiPage) const;
};

#pragma pack()

UINT EvaluateAllocator(UINT _Pages,USHORT _PageSize);


} // AMQ namespace
} // Galaxy namespace


#endif /*SQALLOCATOR_HPP_25B2229B_6AE1_4F73_BBA6_9A8AAF1B71EE*/
