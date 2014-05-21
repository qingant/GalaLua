/*
 * luapopen.cpp
 *
 *  Created on: Nov 29, 2013
 *      Author: aikon
 */

#ifdef __linux__
#define _XOPEN_SOURCE
#endif

#include <cstdio>
#include <string>
#include <exception>

#include <errno.h>
#include <pthread.h>
#include <string.h>

#include "lua.hpp"

namespace LUAPOPEN
{

class CException : public std::exception
{
private:
	std::string _What;

	CException() throw();

	std::string Now() throw();
public:
	explicit CException(const char *__excp__) throw();
	explicit CException(const char * _file_,const char * _func_,int _line_,const char *_msg_) throw();
	~CException() throw();
	const char * what() const throw();
};

#define THROW_EXCEPTION(__excp__) (throw CException(__FILE__,__FUNCTION__,__LINE__,__excp__))


class CMutex
{
private:
    pthread_mutex_t &_mutex;
public:
    CMutex(pthread_mutex_t &_Themutex);
    ~CMutex();
    void Lock();
    void UnLock();
};


class CPopen
{
private:
    FILE *_fp;

    explicit CPopen(const CPopen &);
    const CPopen &operator=(const CPopen &);
public:
    CPopen(const char * const command);
    ~CPopen();
    std::string Read();
};

class CPopenGuard
{
private:
    CMutex  _Mutex;
public:
    CPopenGuard(pthread_mutex_t &_TheMutex);
    ~CPopenGuard();
    std::string Execute(const char * const command);
};

/*CException*/
CException::CException(const char *__excp__) throw()
	:_What(__excp__)
{

}

CException::CException(const char *_file_,const char *_func_, int _line_,const char *_msg_) throw()
	:_What(2048,0)
{
   char    szStrError[256];

   if(_msg_!=NULL)
   {
	  if(errno!=0)
      {
      	strerror_r(errno,szStrError,256);
      	snprintf(&_What[0],2048,"Exception:\n"
                   "    FILE  (%s);\n"
                   "    FUNC  (%s);\n"
                   "    LINE  (%d);\n"
                   "    ERNO  (%d);\n"
                   "    SYER  (%s);\n"
                   "    MESG  (%s);\n"
                   "    TIME  (%s);\n"
                   , \
                   _file_
                   , \
                   _func_
                   , \
                   _line_
                   , \
                   errno
                   , \
                   szStrError
                   , \
                   _msg_
                   , \
                   Now().c_str()
         );

         errno = 0;
      }
      else
      {
      	snprintf(&_What[0],2048,"Exception:\n"
                   "    FILE  (%s);\n"
                   "    FUNC  (%s);\n"
                   "    LINE  (%d);\n"
                   "    MESG  (%s);\n"
                   "    TIME  (%s);\n"
                   , \
                   _file_
                   , \
                   _func_
                   , \
                   _line_
                   , \
                   _msg_
                   , \
                   Now().c_str()
         );
      }

   }
   else
   {
      if(errno!=0)
      {
         strerror_r(errno,szStrError,256);
         snprintf(&_What[0],2048,"Exception:\n"
                      "    FILE  (%s);\n"
                      "    FUNC  (%s);\n"
                      "    LINE  (%d);\n"
                      "    ERNO  (%d);\n"
                      "    SYER  (%s);\n"
                      "    TIME  (%s);\n"
                      , \
                      _file_
                      , \
                      _func_
                      , \
                      _line_
                      , \
                      errno
                      , \
                      szStrError
                      , \
                      Now().c_str()
            );

            errno = 0;
      }
      else
      {
         snprintf(&_What[0],2048,"Exception:\n"
                      "    FILE  (%s);\n"
                      "    FUNC  (%s);\n"
                      "    LINE  (%d);\n"
                      "    TIME  (%s);\n"
                      , \
                      _file_
                      , \
                      _func_
                      , \
                      _line_
                      , \
                      Now().c_str()
            );

      }
   }
}

CException::~CException() throw()
{

}

std::string CException::Now() throw()
{
   char             szStamp[64];
   struct	tm 		tme;
   time_t 			now;

   time(&now);
   localtime_r(&now, &tme);

   snprintf(szStamp,sizeof(szStamp), "%04d/%02d/%02d %02d:%02d:%02d", \
                tme.tm_year+1900,tme.tm_mon+1,tme.tm_mday,tme.tm_hour,tme.tm_min, \
                tme.tm_sec );

   return szStamp;
}

const char *CException::what() const throw()
{
	return  (_What.c_str());
}


/*CMutex*/
CMutex::CMutex(pthread_mutex_t &_Themutex)
    :_mutex(_Themutex)
{

}

CMutex::~CMutex()
{

}

void CMutex::Lock()
{
    const int rc = ::pthread_mutex_lock(&_mutex);
    if (rc != 0)
    {
        THROW_EXCEPTION("call pthread_mutex_lock(...) failure!");
    }
}

void CMutex::UnLock()
{
    const int rc = ::pthread_mutex_unlock(&_mutex);
    if (rc != 0)
    {
		THROW_EXCEPTION("call pthread_mutex_unlock(...) failure!");
    }
}

/*CPopen*/
CPopen::CPopen(const char * const command)
    :_fp(::popen(command, "r"))
{
    if (_fp == NULL)
    {
        const size_t siz = 1024;
        char buff[siz];
        snprintf(&buff[0], siz, "popen: '%s' failed", command);
        THROW_EXCEPTION(buff);
    }
}

CPopen::~CPopen()
{
    if(_fp!=NULL)
    {
        pclose(_fp);
    }
}

std::string CPopen::Read()
{
    std::string _Buffer(819236,0);
    std::string _Result;

    unsigned int _Length = 0;
    _Result.reserve(819200);
	_Result = "";
    while(!feof(_fp))
    {
    	_Length = fread(&_Buffer[0],(size_t) 1U, 81920, _fp);
    	if(_Length!=0)
    	{
	    	_Buffer[_Length]=0;
	    	_Result+=_Buffer;
    	}
    	else
    	{
    		break;
    	}
    }

    return _Result;
}

/*CPopenGuard*/
CPopenGuard::CPopenGuard(pthread_mutex_t &_TheMutex)
    :_Mutex(_TheMutex)
{
    _Mutex.Lock();
}

CPopenGuard::~CPopenGuard()
{
    _Mutex.UnLock();
}

std::string CPopenGuard::Execute(const char * const command)
{
    CPopen _POpenSuite(command);
    return _POpenSuite.Read();
}

}; //namespace


/*
int main(int argc, char *argv[])
{
 	try
    {
        static pthread_mutex_t __popen_mutex = PTHREAD_MUTEX_INITIALIZER;
        {
            LUAPOPEN::CPopenGuard _Guard(__popen_mutex);
            const std::string retval = _Guard.Execute("ps -ef");
            printf("\n%s\n",retval.c_str());
            fflush(stdout);
        }

        return 1;
    }
    catch(LUAPOPEN::CException &except)
    {
        printf("\n%s\n",except.what());
        fflush(stdout);
    }
}
*/

static int Ppopen(lua_State * const L)
{
    const char * const command = luaL_checkstring(L, 1);
    try
    {
        static pthread_mutex_t __popen_mutex = PTHREAD_MUTEX_INITIALIZER;
        {
            LUAPOPEN::CPopenGuard _Guard(__popen_mutex);
            const std::string retval = _Guard.Execute(command);
            lua_pushstring(L, retval.c_str());
        }

        return 1;
    }
    catch (LUAPOPEN::CException &except)
    {
        lua_pushnil(L);
        lua_pushstring(L, except.what());
        return 2;
    }
}

#ifdef __cplusplus
extern "C"
{
#endif

LUALIB_API int luaopen_aimagtpls(lua_State * const L)
    {
    const luaL_Reg library[] = {
                {"popen", Ppopen},
                {(const char *) NULL, (lua_CFunction) NULL}
    };
#if LUA_VERSION_NUM == 501
    luaL_register(L, "aimagtpls", &library[0]);
#elif LUA_VERSION_NUM == 502
    luaL_newlibtable(L, library);
    luaL_setfuncs(L, &library[0], 0);
#endif
    return 1;
}

#ifdef __cplusplus
}
#endif

