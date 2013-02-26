
/* -*- C++ -*- */

/* Time-stamp: <2012-07-06 23:05:28 Friday by qingant> */

/**
 * @file   Exception.hpp
 * @author qingant
 */


#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <cstub.h>
#include <exception>
#include <string>
#include <iostream>
#include <sstream>

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#pragma once

#include "Object.hpp"

namespace Galaxy
{
   namespace GalaxyRT
{

#define THROW_SYSTIMEOUT() (throw CSysTimeOut(__FILE__, __PRETTY_FUNCTION__, __LINE__, errno) )
#define THROW_SYSERROR() (throw CSysError(__FILE__, __PRETTY_FUNCTION__, __LINE__, errno) )
#define THROW_CONTROL(control) (throw CSysError(__FILE__, __PRETTY_FUNCTION__, __LINE__, errno, control) )
#define THROW_EXCEPTION(control, msg) (throw CException(__FILE__, __PRETTY_FUNCTION__, __LINE__, errno, control, msg) )
#define THROW_EXCEPTION_EX(msg) (throw Galaxy::GalaxyRT::CException(__FILE__, __PRETTY_FUNCTION__, __LINE__, errno, 0, msg) )
#define THROW_GENERALERROR(ExClass, args...)							\
    do																	\
    {																	\
        ExClass e(__FILE__, __PRETTY_FUNCTION__, __LINE__,errno, 0,  args); \
        throw e;														\
    }																	\
    while (false)

#define THROW_ERROR(ExClass, args...)			\
    do											\
    {											\
		ExClass e(__FILE__, __PRETTY_FUNCTION__, __LINE__, "##args##");  \
		throw e;								\
    }											\
    while (false)

// macro used to define an derieved Exception
// sample: GALA_DEFINE_EXCEPTION(CRWLockTimeOut, CException)
#define GALA_DEFINE_EXCEPTION(ExClass, Base)							\
    ExClass(const char *fname, const char *funcname, size_t line, const std::string& msg = "") throw() \
        : Base(fname,funcname,line,errno, 0,  msg)                     \
    {}																	\
	                                                                    \
    ~ExClass() throw() {}												\
	                                                                    \
    std::string GetClassName() const									\
    {																	\
       return #ExClass;                                                 \
    }


	class CException : public std::exception, public CObject
	{
	public:
		CException(CPSTR file, CPSTR func, INT line, INT errcode=0, INT control=0, const std::string& msg="") throw();
		virtual ~CException() throw();
		virtual std::string GetClassName() const;
		virtual std::string GetMessage() const;
		CPSTR what() const throw();
		const std::string& ToString() const;
		std::string GetStackTrace() const;
		INT GetErrCode() const;
		INT GetControl() const;
	protected:
		std::string mMsg;
		std::string mNow;
        std::string mFile;
        std::string mFunc;
		INT mLine;
		INT mErrCode;
		INT mControl;

		void Now();
	private:
		enum { MAX_STACK_TRACE_SIZE = 64 };
		VOID* mStackTrace[MAX_STACK_TRACE_SIZE];
		size_t mStackTraceSize;
		mutable std::string mWhat;
	};


	class CSysError:public CException
	{
	public:
		CSysError(CPSTR file, CPSTR func, INT line, INT errcode = 0, INT control = 0) throw();
		~CSysError() throw();
		std::string GetClassName() const;
	};


	class CIOError:public CException
	{
	public:
		CIOError(CPSTR file, CPSTR func, INT line, INT errcode = 0, INT control = 0) throw();
		~CIOError() throw();
		std::string GetClassName() const;
	};

	class CSocketError:public CException
	{
	public:
		CSocketError(CPSTR file, CPSTR func, INT line, INT errcode = 0, INT control = 0) throw();
		~CSocketError() throw();
		std::string GetClassName() const;
	};

   class CArgumentError: public CException
   {
   public:
      CArgumentError(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
         :CException(file, func, line, errcode, control, msg){}
      ~CArgumentError() throw(){}
      std::string GetClassName() const
      {
         return std::string("CArgumentError");
      }
   };

// class CSysTimeOut:public CException
// {
// public:
//     CSysTimeOut(CPSTR file, CPSTR func, INT line, INT errcode = 0, INT control = 0)throw();
//     ~CSysTimeOut()throw();
//     std::string GetClassName() const;
// };

/*
  class CDllError:public CException
  {
  public:
  CDllError(CPSTR file, CPSTR func, INT line) throw();
  ~CDllError() throw();
  std::string CDllError() const;
  };
*/

   } /// namespace GalaxyRT
} /// namespace Galaxy

#endif
