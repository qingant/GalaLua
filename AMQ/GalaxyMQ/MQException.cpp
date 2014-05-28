// Exception.cpp: implementation of the CException class.
//
//////////////////////////////////////////////////////////////////////
#include <stdarg.h>
#include "MQException.hpp"

namespace Galaxy
{

namespace AMQ
{

#define MYEXCEPTION_BUFFER_SIZE			(2048)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMQException::CMQException(PCSTR __TheFile__, PCSTR __TheFun__, UINT __TheLine__,PCSTR _Format, ... ) throw()
:CMQException_T(__TheFile__,__TheFun__,__TheLine__)
{
	va_list pArg;
	va_start(pArg, _Format);
	vsnprintf(_szMsg,sizeof(_szMsg),_Format,pArg);
	va_end(pArg);
}

CMQException::~CMQException() throw()
{

}

PCSTR CMQException::what() const throw()
{
	PSTR _TheBuffer = NewBuffer(MYEXCEPTION_BUFFER_SIZE);
	if(_TheBuffer!=NULL)
	{
		snprintf((PSTR)_TheBuffer,MYEXCEPTION_BUFFER_SIZE,"E::CMQException::{FILE=%s;FUNC=%s;LINE=%u;DESC=%s}",File(),Func(),Line(),_szMsg);
	}
	
	return _TheBuffer;
	
}

PCSTR CMQException::Msg() const throw()
{
	return _szMsg;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMQNULLException::CMQNULLException(PCSTR __TheFile__, PCSTR __TheFun__, UINT __TheLine__,PCSTR _TheVar) throw()
:CMQNULLException_T(__TheFile__,__TheFun__,__TheLine__)
{
	snprintf(_szVar,sizeof(_szVar),"%s",_TheVar);
}

CMQNULLException::~CMQNULLException() throw()
{
	
}

PCSTR CMQNULLException::Var() const throw()
{
	return _szVar;	
}

PCSTR CMQNULLException::what() const throw()
{
	PSTR _TheBuffer = NewBuffer(MYEXCEPTION_BUFFER_SIZE);
	if(_TheBuffer!=NULL)
	{
		snprintf((PSTR)_TheBuffer,MYEXCEPTION_BUFFER_SIZE,"E::CMQNULLException::{FILE=%s;FUNC=%s;LINE=%u;{\"%s\" Is NULL}}",File(),Func(),Line(),_szVar);
	}
	
	return _TheBuffer;	
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMQAPIException::CMQAPIException(PCSTR __TheFile__, PCSTR __TheFun__, UINT __TheLine__,PCSTR _TheAPI,INT _TheResult) throw()
:CMQAPIException_T(__TheFile__,__TheFun__,__TheLine__),_Result(_TheResult)
{
	snprintf(_szAPI,sizeof(_szAPI),"%s",_TheAPI);
}

CMQAPIException::~CMQAPIException() throw()
{

}

PCSTR CMQAPIException::what() const throw()
{
	PSTR _TheBuffer = NewBuffer(MYEXCEPTION_BUFFER_SIZE);
	if(_TheBuffer!=NULL)
	{
		snprintf((PSTR)_TheBuffer,MYEXCEPTION_BUFFER_SIZE,"E::CMQAPIException::{FILE=%s;FUNC=%s;LINE=%u;API=%s;RC=%d}",File(),Func(),Line(),_szAPI,_Result);
	}
	
	return _TheBuffer;
	
}

PCSTR CMQAPIException::API() const throw()
{
	return _szAPI;
}

INT CMQAPIException::RC() const throw()
{
	return _Result;	
}















}; //namespace AMQ
}; //namespace Galaxy
