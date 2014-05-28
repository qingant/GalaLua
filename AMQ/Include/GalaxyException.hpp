// GalaxyException.h: interface for the CException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GALAXYEXCEPTION_H__748DF752_3BBD_47CC_B4DA_80B70A05A0B2__INCLUDED_)
#define AFX_GALAXYEXCEPTION_H__748DF752_3BBD_47CC_B4DA_80B70A05A0B2__INCLUDED_

#pragma once

#include <stdio.h>
#include "GalaxyObject.hpp"

#define ET_MYEXCEPTION				(1)
#define ET_APIEXCEPTION				(2)

#define ET_MQEXCEPTION				(101)
#define ET_MQNULLEXCEPTION			(102)
#define ET_MQAPIEXCEPTION			(103)
#define ET_MQCTLEXCEPTION			(104)



class IGalaxyException : public CGalaxyObject
{
public:
	virtual UINT ExceptionType() const throw() = 0;
	virtual PCSTR what() const throw() = 0;
	virtual PCSTR File() const throw() = 0;
	virtual PCSTR Func() const throw() = 0;
	virtual UINT Line() const throw() = 0;
	virtual ~IGalaxyException() throw()
	{

	}
};



template <UINT __EXCEPTIONTYPE__> class CGalaxyException : public IGalaxyException
{
	typedef CGalaxyException<__EXCEPTIONTYPE__>	CGalaxyException_T;
private:
	CHAR		_szFile[256];
	CHAR		_szFunc[256];
	UINT		_Line;
	PSTR		_Buffer;
protected:
	explicit CGalaxyException(PCSTR __TheFile__, PCSTR __TheFun__, UINT __TheLine__) throw()
		:_szFile(),_szFunc(),_Line(0),_Buffer(NULL)
	{
		snprintf(_szFile,sizeof(_szFile),"%s",__TheFile__);
		snprintf(_szFunc,sizeof(_szFunc),"%s",__TheFun__);
		_Line = __TheLine__;
	}
	
	PSTR NewBuffer(UINT _Size) const throw()
	{
		CGalaxyException_T *_This = (CGalaxyException_T *)this;
		_This->_Buffer = new CHAR[_Size];	
		return _Buffer;
	}

public:
	CGalaxyException_T &operator=(const CGalaxyException_T &_Other)
	{
		sprintf(_szFile,"%s",_Other._szFile);
		sprintf(_szFunc,"%s",_Other._szFunc);
		_Line = _Other._Line;
		_Buffer = NULL;

		return (*this);
	}

	explicit CGalaxyException(const CGalaxyException_T &_Other)
		:_szFile(),_szFunc(),_Line(0),_Buffer(NULL)
	{
		operator=(_Other);
	}

public:
	virtual ~CGalaxyException() throw()
	{
		if(_Buffer!=NULL)
		{
			delete [] _Buffer;
			_Buffer = NULL;	
		}
	}

	UINT ExceptionType() const throw()
	{
		return __EXCEPTIONTYPE__;
	}

	PCSTR File() const throw()
	{
		return _szFile;
	}
	
	PCSTR Func() const throw()
	{
		return _szFunc;
	}

	UINT Line() const throw()
	{
		return _Line;
	}
};

#endif // !defined(AFX_GALAXYEXCEPTION_H__748DF752_3BBD_47CC_B4DA_80B70A05A0B2__INCLUDED_)
