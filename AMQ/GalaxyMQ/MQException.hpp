#ifndef MQEXCEPTION_HPP_FD997534_5931_47FD_8B83_F05ECD0A58C1
#define MQEXCEPTION_HPP_FD997534_5931_47FD_8B83_F05ECD0A58C1

#pragma once

#include "GalaxyException.hpp"

namespace Galaxy
{

namespace AMQ
{
	
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
	CHAR	_szMsg[256];
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
	CHAR	_szVar[256];
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
	CHAR	_szAPI[256];
	INT		_Result;
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

#endif /*MQEXCEPTION_HPP_FD997534_5931_47FD_8B83_F05ECD0A58C1*/
