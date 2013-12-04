/* -*- C++ -*- */


// Time-stamp: <2012-07-06 23:05:12 Friday by qingant>

/**
 * @file   CException.cpp
 * @author qingant
 */
//test


#include "Exception.hpp"
#include <stdlib.h>
#if defined(__linux__)
#include <execinfo.h>
#include <cxxabi.h>
#endif

namespace Galaxy
{
   namespace GalaxyRT
{
	using namespace std;

// CException::CException(const std::string& msg) throw()
//     : mMsg(msg),
//       mNow(),
//       mFile("<unknown file>"),
//       mFunc("<unknown func>"),
//       mLine(-1),
//       mErrCode(0),
//       mControl(0),
//       mStackTraceSize(0)
// {
//     this->Now();
// }

	CException::~CException() throw()
	{

	}

	CException::CException(CPSTR file, CPSTR func, INT line, INT errcode, INT control , const std::string& msg) throw()
		:mMsg(msg),mNow(),
		 mFile(file),
		 mFunc(func),
		 mLine(line),
		 mErrCode(errcode),
		 mControl(control),
		 mStackTraceSize(0)
	{
#if defined(__linux__)
		mStackTraceSize = ::CRT_backtrace(mStackTrace, MAX_STACK_TRACE_SIZE);
#endif
		this->Now();
	}

	INT CException::GetErrCode() const
	{
		return mErrCode;
	}
	INT CException::GetControl() const
	{
		return mControl;
	}

	std::string CException::GetClassName() const
	{
		return "CException";
	}

	void CException::Now()
	{
		CHAR            szStamp[64];
		struct	tm 		tme;
		time_t 			now;

		CRT_time(&now);
		CRT_localtime_r(&now, &tme);

		CRT_snprintf((PSTR) szStamp,sizeof(szStamp), "%04d/%02d/%02d %02d:%02d:%02d", \
					 tme.tm_year+1900,tme.tm_mon+1,tme.tm_mday,tme.tm_hour,tme.tm_min, \
					 tme.tm_sec );

		this->mNow = szStamp;
	}

	CPSTR CException::what() const throw()
	{
		return ToString().c_str();
	}

	const std::string& CException::ToString() const
	{
		if (mWhat.empty())
		{
			stringstream sstr("");
			sstr<<"\n-------------------------------------------------------------------------\n";
			if (mLine > 0)
			{
				sstr << "FILE = "<<mFile <<endl;
				sstr << "FUNC = "<<mFunc <<endl;
				sstr << "LINE = "<< mLine <<endl;
			}
			sstr <<  "Class = " << GetClassName() <<endl;
			if (!GetMessage().empty())
			{
				sstr << "DESP = " << GetMessage() <<endl;
			}
			sstr <<"Time = " << mNow << endl;
			sstr << "Stack Trace:\n";
			std::string result =  GetStackTrace();
			sstr << result;
			sstr<<"\n-------------------------------------------------------------------------\n";
			mWhat = sstr.str();
		}
		return mWhat;
	}



	std::string CException::GetMessage() const
	{
		return mMsg;
	}


#if (defined(__GLIBC__) )
	std::string CException::GetStackTrace() const
	{

		// prINT out all the frames to stderr
		//backtrace_symbols_fd(array, size, 2);
		if (mStackTraceSize == 0)
			return "<No stack trace>\n";
		PSTR* strings = CRT_backtrace_symbols(mStackTrace, mStackTraceSize);
		if (strings == NULL) // Since this is for debug only thus
			// non-critical, don't throw an exception.
			return "<Unknown error: backtrace_symbols returned NULL>\n";
		std::string result;
		// std::cout<<mStackTraceSize<<std::endl;
		for (size_t i = 0; i < mStackTraceSize; ++i)
		{
			std::string mangledName = strings[i];
			std::string::size_type begin = mangledName.find('(');
			std::string::size_type end = mangledName.find('+', begin);
			if (begin == std::string::npos || end == std::string::npos)
			{
				// std::cout<<begin<<"\t"<<end<<"\t"<<std::string::npos<<std::endl;
				// std::cout<<"mangeName: "<<mangledName<<std::endl;
				result += mangledName;
				result += '\n';
				continue;
			}
			++begin;
			INT status;
			PSTR s = abi::__cxa_demangle(mangledName.substr(begin, end-begin).c_str(),
										 NULL, 0, &status);
			if (status != 0)
			{
				// std::cout<<mangledName<<std::endl;

				result += mangledName;
				result += '\n';
				continue;
			}
			std::string demangledName(s);
			// std::cout<<demangledName<<std::endl;
			CRT_free(s);
			// Ignore CException::Init so the top frame is the
			// user's frame where this exception is thrown.
			//
			// Can't just ignore frame#0 because the compiler might
			// inline CException::Init.
			result += mangledName.substr(0, begin);
			result += demangledName;
			result += mangledName.substr(end);
			result += '\n';
		}
		CRT_free(strings);
		return result;
	}

// #elif defined(AIX)
// #include <unistd.h>
// std::string CException::GetStackTrace() const
// {
//     CHAR fileName[30] = "/tmp/stackTrace.XXXXXX";
//     INT fd = CRT_open(fileName, O_RDWR|O_CREAT|O_APPEND, 0666);
//     rc = mt__trce(lcf_fd, 0, 0, 0);
//     CHAR res[1024];
//     CRT_bzero(res, sizeof(res));
//     CRT_read(fd, res, sizeof(res));
//     CRT_close(fd);
//     return std::string(res);
// }
#else
	std::string CException::GetStackTrace() const
	{
		return std::string("<stacktrace not supported on this platform>");
	}
#endif



/*CSysError*/

	CSysError::CSysError(CPSTR file, CPSTR func, INT line, INT errcode, INT control) throw()
		:CException(file,func,line, errcode, control)
	{
		CHAR    sztmp[2048];
		CHAR    szerror[1024];
		//INT     err = errno;
		CRT_strerror_r(errcode, szerror, sizeof(szerror));
		::CRT_snprintf(sztmp,sizeof(sztmp),"SYSERR:(%d) - %s",(INT)errno, szerror);
		this->mMsg = sztmp;
		errno = 0;
	}

	CSysError::~CSysError() throw()
	{

	}

	std::string CSysError::GetClassName() const
	{
		return "CSysError";
	}


/*CIOError*/

	CIOError::CIOError(CPSTR file, CPSTR func, INT line, INT errcode, INT control) throw()
		:CException(file,func,line, errcode, control)
	{
		CHAR    sztmp[2048];
		CHAR    szerror[1024];
		CRT_strerror_r(errcode, szerror, sizeof(szerror));
		::CRT_snprintf(sztmp,sizeof(sztmp),"SYSERR:(%d) - %s",(INT)errno, szerror);
		this->mMsg = sztmp;
		errno = 0;
	}

	CIOError::~CIOError() throw()
	{

	}

	std::string CIOError::GetClassName() const
	{
		return "CIOError";
	}


/*CSocketError*/

	CSocketError::CSocketError(CPSTR file, CPSTR func, INT line, INT errcode, INT control) throw()
		:CException(file,func,line, errcode, control)
	{
		CHAR    sztmp[2048];
		CHAR    szerror[1024];
		CRT_strerror_r(errcode, szerror, sizeof(szerror));
		::CRT_snprintf(sztmp,sizeof(sztmp),"SYSERR:(%d) - %s",(INT)errno, szerror);

		this->mMsg = sztmp;
		errno = 0;
	}

	CSocketError::~CSocketError() throw()
	{

	}

	std::string CSocketError::GetClassName() const
	{
		return "CSocketError";
	}
   
   } /// namespace GalaxyRT
} /// namespace Galaxy
