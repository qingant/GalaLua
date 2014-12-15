#ifndef AMQ_HPP_6DFE47F5_9476_4454_A558_A2D3C431D377
#define AMQ_HPP_6DFE47F5_9476_4454_A558_A2D3C431D377

#pragma once

#include <string>

#include <GalaxyObject.hpp>
#include <GalaxyException.hpp>

namespace Galaxy
{
namespace AMQ
{

typedef PSTR (*RTN_RECVCALLBACK)(UINT _Length,PVOID _Object);

//接口类，请别初始化成任何对像	
class CSQSuite : public CGalaxyObject
{
private:
	static void* operator new (size_t);
public:	
	PBYTE NearPtr() const;

	UINT Get(PSTR _Buffer,UINT _MaxLength) const;
	UINT TimedGet(PSTR _Buffer,UINT _MaxLength,SHORT _Timeout) const;
	
	void Put(PCSTR _Buffer,UINT _Length) const;
	bool TimedPut(PCSTR _Buffer,UINT _Length,SHORT _Timeout) const;
	
	UINT GetCB(RTN_RECVCALLBACK _Callback,PVOID _Object) const;
	UINT TimedGetCB(SHORT _Timeout,RTN_RECVCALLBACK _Callback,PVOID _Object) const;
    UINT Count() const;
	
};

class CGalaxyMQ : public CGalaxyObject
{
private:
    PVOID  _Media;
public:
	explicit CGalaxyMQ(const std::string &_FileName);
	explicit CGalaxyMQ(key_t _IPCKey);
	~CGalaxyMQ();
public:
	UINT Count() const;
	const CSQSuite &operator[](UINT _Index) const;
	//void Backup(const std::string &_FileName) const;
}; 

class CGalaxyMQCreator : public CGalaxyObject
{
private:
	static void* operator new (size_t);
public:
	explicit CGalaxyMQCreator(const std::string &_FileName, \
			UINT _Queues,UINT _MaxDepth,USHORT _CatchCount,UINT _Nodes,UINT _Pages,USHORT _PageSize);
	explicit CGalaxyMQCreator(key_t _IPCKey, \
			UINT _Queues,UINT _MaxDepth,USHORT _CatchCount,UINT _Nodes,UINT _Pages,USHORT _PageSize);
	~CGalaxyMQCreator();
};

#if defined(__xlC__)
#define __PRETTY_FUNCTION__		__FUNCTION__
#endif

#if !defined(__PRETTY_FUNCTION__)
#define __PRETTY_FUNCTION__		__FUNCTION__
#endif

typedef CGalaxyException<ET_MQEXCEPTION> CMQException_T;
		
class CMQException : public CMQException_T
{
private:
	static void* operator new (size_t);
public:
	explicit CMQException(PCSTR __TheFile__, PCSTR __TheFun__, UINT __TheLine__,PCSTR _Format, ...) throw();
	~CMQException() throw();
	
	PCSTR what() const throw();
	PCSTR Msg() const throw();
};

typedef CGalaxyException<ET_MQNULLEXCEPTION> CMQNULLException_T;
class CMQNULLException : public CMQNULLException_T
{
private:
	static void* operator new (size_t);
public:
	explicit CMQNULLException(PCSTR __TheFile__, PCSTR __TheFun__, UINT __TheLine__,PCSTR _TheVar) throw();
	~CMQNULLException() throw();
	PCSTR Var() const throw();
	PCSTR what() const throw();
};

typedef CGalaxyException<ET_MQAPIEXCEPTION> CMQAPIException_T;
class CMQAPIException : public CMQAPIException_T
{
private:
	static void* operator new (size_t);
public:
	explicit CMQAPIException(PCSTR __TheFile__, PCSTR __TheFun__, UINT __TheLine__,PCSTR _TheAPI,INT _TheResult) throw();
	~CMQAPIException() throw();
	
	PCSTR what() const throw();
	PCSTR API() const throw();
	INT RC() const throw();
};


#define THROW_MQEXCEPTION(__excp__) (throw CMQException(__FILE__,__PRETTY_FUNCTION__,__LINE__,"%s",__excp__))
#define THROW_MQNULLEXCEPTION(__var__) (throw CMQNULLException(__FILE__,__PRETTY_FUNCTION__,__LINE__,#__var__))		
#define THROW_MQAPIEXCEPTION(__api__,__result__) (throw CMQAPIException(__FILE__,__PRETTY_FUNCTION__,__LINE__,__api__,__result__))		


} // AMQ namespace
} // Galaxy namespace

#endif /*AMQ_HPP_6DFE47F5_9476_4454_A558_A2D3C431D377*/
