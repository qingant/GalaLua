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
	__volatile__ INT	_Next;      //��һ���ڵ�
	__volatile__ UINT	_Length;    //�������� 
	__volatile__ INT	_PageLKTB;  //����ҳ����
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
	PBYTE NearPtr() const; // �����ָ��ʵ������NODE[0]��λ�ã�ǧ����ô�	
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

	//��ͳ����
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
	PBYTE NearPtr() const; // �����ָ��ʵ������Node[0]��λ�ã�ǧ����ô�	
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
