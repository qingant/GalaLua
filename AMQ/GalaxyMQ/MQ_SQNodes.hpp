#ifndef SQNODES_HPP_649A3730_F697_40CF_AA66_96C80210600F
#define SQNODES_HPP_649A3730_F697_40CF_AA66_96C80210600F


#pragma once

#include "MQ_SQSync.hpp"
#include "MQ_SQList.hpp"

namespace Galaxy
{
namespace AMQ
{

class CSQPage;
class CSQNode
{
private:
	__volatile__ INT	_Next;      //下一个节点
	__volatile__ UINT	_Length;    //长度数据 
	__volatile__ INT	_PageLKTB;  //数据页链表
public:
	void Init();
	PBYTE NearPtr() const;
	
	void Next(const CSQNode *_TheNode) const;
	const CSQNode *Next() const;
	
	void Page(const CSQPage *_ThePage) const;
	const CSQPage *Page() const;
	
	void Length(UINT _TheLength) const;
	UINT Length() const;
};

class CSQNodeArray
{
private:
	UINT	_Total;
public:
	void Init(UINT _TheTotal);
	PBYTE NearPtr() const; // 这个近指针实际上是NODE[0]的位置，千万别用错	
	UINT Count() const;
	const CSQNode *operator[](UINT _Index) const;	
};

class CSQLockedList
{
	typedef CSQList<CSQNode> CSQNodeList_T;
private:
	CSQLock			_Lock;
	CSQNodeList_T	_List;	
public:	
	void Init();
	PBYTE NearPtr() const;
	UINT Count() const;

	//传统方法
	const CSQNode *Get() const;
	void Put2Head(const CSQNode &_TheNode) const;
	void Put2Tail(const CSQNode &_TheNode) const;
};

class CSQPooler
{
private:
	CSQLockedList	_Queue;
	CSQNodeArray	_Array;
public:	
	void Init(UINT _Nodes);
	PBYTE NearPtr() const; // 这个近指针实际上是Node[0]的位置，千万别用错	
	UINT UsableNodes() const;
	UINT Nodes() const;	
	
	const CSQNode *Get() const;
	void Put(const CSQNode &_TheNode) const;
	void Reset() const;
	
};


UINT EvaluatePooler(UINT _Nodes);


};

};


#endif /*SQNODES_HPP_649A3730_F697_40CF_AA66_96C80210600F*/
