#ifndef MQ_SQUEUE_HPP_93EC098D_0D23_4037_803C_2F7232BB95C1
#define MQ_SQUEUE_HPP_93EC098D_0D23_4037_803C_2F7232BB95C1

#pragma once

#include "MQ_SQSync.hpp"
#include "MQ_SQList.hpp"

namespace Galaxy
{

namespace AMQ
{

class CSQNode;
class CSQPooler;

class CSQPrivatePooler
{
	typedef CSQList<CSQNode> CSQNodeList_T;
private:
	__volatile__ INT	_Pooler;
	__volatile__ UINT	_Catch;
	CSQEvent			_Lock;
	CSQNodeList_T		_List;
	

	const CSQPooler &Pooler() const;
	void Pooler(const CSQPooler &_ThePooler);

	void Hibernate() const;
	bool TimedHibernate(SHORT timeout) const;
	void Wakeup() const;
public:
	void Init(UINT _TheCatchCount,const CSQPooler &_ThePooler);
	PBYTE NearPtr() const;
	UINT Count() const;
	
	const CSQNode *Get() const;
	const CSQNode *TimedGet(SHORT timeout) const;
	void Put(const CSQNode &_TheNode) const;
	void Reset() const;
	
	//Nursery专用接口，纯粹利用C++特性增加的 等于 Put
	void _NurseryPut(const CSQNode &_TheNode) const;
};

class CSQDoorQueue
{
	typedef CSQList<CSQNode> CSQNodeList_T;
private:
	__volatile__ UINT	_MaxDepth;
	CSQDoor				_Door;
	CSQNodeList_T		_List;
	
	void GET_Hibernate() const;
	bool GET_TimedHibernate(SHORT timeout) const;
	void GET_Wakeup() const;
	
	void PUT_Hibernate() const;
	bool PUT_TimedHibernate(SHORT timeout) const;
	void PUT_Wakeup() const;
public:
	void Init(UINT _TheDepth);
	PBYTE NearPtr() const;
	UINT Count() const;
	
	UINT MaxDepth() const;
	void MaxDepth(UINT _TheDepth) const;
	
	const CSQNode *Get() const;
	const CSQNode *TimedGet(SHORT timeout) const;
	void Put(const CSQNode &_TheNode) const;
	bool TimedPut(const CSQNode &_TheNode,SHORT timeout) const;
	void Reset() const;
	
	//Nursery专用接口，纯粹利用C++特性增加的 等于 Put
	void _NurseryPut(const CSQNode &_TheNode) const;
};


typedef PSTR (*RTN_RECVCALLBACK)(UINT _Length,PVOID _Object);

class CSQAllocator;
class CSQSuite
{
	
private:
	__volatile__ INT	_Allocator;
	__volatile__ INT	_Pooler;
	CSQDoorQueue		_Queue;
	CSQPrivatePooler	_PPL;
	
	void Pooler(const CSQPooler &_ThePooler);
	void Allocator(const CSQAllocator &_TheAllocator);
	const CSQAllocator &Allocator() const;
	const CSQPooler &Pooler() const;
public:	
	void Init(const CSQAllocator &_TheAllocator,const CSQPooler &_ThePooler,UINT _MaxDepth,UINT _CatchCount);
	PBYTE NearPtr() const;

	UINT Get(PSTR _Buffer,UINT _MaxLength) const;
	UINT TimedGet(PSTR _Buffer,UINT _MaxLength,SHORT _Timeout) const;
	
	UINT GetCB(RTN_RECVCALLBACK _Callback,PVOID _Object) const;
	UINT TimedGetCB(SHORT _Timeout,RTN_RECVCALLBACK _Callback,PVOID _Object) const;
	
	void Put(PCSTR _Buffer,UINT _Length) const;
	bool TimedPut(PCSTR _Buffer,UINT _Length,SHORT _Timeout) const;
};


class CSQSuiteArray
{
private:
	__volatile__ UINT	_Total;
public:
	void Init(UINT _TheTotal,const CSQAllocator &_TheAllocator,const CSQPooler &_ThePooler,UINT _MaxDepth,UINT _CatchCount);
	PBYTE NearPtr() const; // 这个近指针实际上是SQ[0]的位置，千万别用错	
	UINT Count() const;
	const CSQSuite *operator[](UINT _Index) const;
};

UINT EvaluateSQueue(UINT _Count);




};

};

#endif /*MQ_SQUEUE_HPP_93EC098D_0D23_4037_803C_2F7232BB95C1*/
