
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "MQMedia.hpp"
#include "MQException.hpp"

namespace Galaxy
{

namespace AMQ
{

CMQMemory::CMQMemory(PBYTE _TheEntry,SIZE_T _Size)
	:_Entry(_TheEntry)
{
	if(ISNULL(_Entry))
	{
		THROW_MQNULLEXCEPTION(_Entry);	
	}

	if(_Size!=0)
	{
		*((SIZE_T *)_Entry) = _Size;
	}	
}

CMQMemory::~CMQMemory()
{
	_Entry = NULL;	
}

PBYTE CMQMemory::Entry() const
{
    return &_Entry[sizeof(SIZE_T)];   
}

SIZE_T CMQMemory::Size() const
{    
    return *((SIZE_T *)_Entry);
}

	
/*CBinaryFile*/
class CBinaryFile : public CGalaxyObject
{
private:
	INT     _FD;

	inline INT Open(const std::string &_FileName,INT _Flag,mode_t _Perm);
	inline void Close();
	
	CBinaryFile(const CBinaryFile &);
    const CBinaryFile & operator=(const CBinaryFile &);
public:
	explicit CBinaryFile(const std::string &_FileName,INT _Flag,mode_t _Perm);
	~CBinaryFile();


	INT Handle() const;
	SIZE_T Size() const;
	void Truncate(SIZE_T _Length) const;
};

CBinaryFile::CBinaryFile(const std::string &_FileName,INT _Flag,mode_t _Perm)
   :_FD(Open(_FileName,_Flag,_Perm))
{
 
}

CBinaryFile::~CBinaryFile()
{
   Close();  
}

INT CBinaryFile::Open(const std::string &_FileName,INT _Flag,mode_t _Perm)
{
   INT rc = open(_FileName.c_str(),_Flag,_Perm);  
   if(rc < 0)
   {
      THROW_MQAPIEXCEPTION("open",rc);     
   } 

   return rc;
}

void CBinaryFile::Close()
{
   if(_FD!=-1)
   {
      INT  rc;
      rc = close(_FD);   
      if (rc != 0)
      {
         THROW_MQAPIEXCEPTION("close",rc);  
         return;
      }
        
      _FD = -1;
   }
}

INT CBinaryFile::Handle() const
{
   return _FD;   
}

SIZE_T CBinaryFile::Size() const
{
   struct stat _FST;
   INT _rc;
   if((_rc=fstat(Handle(),&_FST))!=0)
   {
      THROW_MQAPIEXCEPTION("fstat",_rc); 
      return (0);   
   }
    
   return _FST.st_size;  
}

void CBinaryFile::Truncate(SIZE_T _Length) const
{
   INT rc = ftruncate(Handle(),_Length);  
   if(rc != 0)
   {
      THROW_MQAPIEXCEPTION("ftruncate",rc); 
   }  
}

CMQFileMemory::CMQFileMemory(const std::string &_FileName,SIZE_T _Size)
:CMQMemory(Attach(_FileName,_Size),_Size)
{
	
}
	
CMQFileMemory::~CMQFileMemory()
{
	Detach();	
}

inline PBYTE CMQFileMemory::Attach(const std::string &_FileName,SIZE_T &_Size)
{
	INT _Flag = 0;
	
	if(_Size > 0)
	{
		_Size+=sizeof(SIZE_T);

		if(access(_FileName.c_str(),F_OK)!=0)
		{
			_Flag = (O_CREAT|O_RDWR);
		}
		else
		{
			if(access(_FileName.c_str(),R_OK|W_OK)!=0)
			{
			 	throw CMQException(__FILE__,__PRETTY_FUNCTION__,__LINE__,"MQF:%s not be access",_FileName.c_str());
			 	return NULL;
			}
			
			_Flag = (O_EXCL|O_RDWR); 
		}
	}
	else
	{
		if(access(_FileName.c_str(),F_OK)!=0)
		{
			throw CMQException(__FILE__,__PRETTY_FUNCTION__,__LINE__,"MQF:%s not found",_FileName.c_str());
			return NULL;
		}
		
		if(access(_FileName.c_str(),R_OK|W_OK)!=0)
		{
		 	throw CMQException(__FILE__,__PRETTY_FUNCTION__,__LINE__,"MQF:%s not be access",_FileName.c_str());
		 	return NULL;
		}
		
		_Flag = (O_EXCL|O_RDWR);
		_Size = 0;   
	}
    
	CBinaryFile     _File(_FileName,(_Flag),(S_IRUSR|S_IWUSR));

   if(_Size > 0)
   {
      _File.Truncate(_Size);
   }

   {
      SIZE_T _TheSize = (SIZE_T) _File.Size();

#ifndef MAP_POPULATE
#define MAP_POPULATE 0
#endif

      PVOID _TheEntry = mmap(NULL,_TheSize,
                            (PROT_EXEC|PROT_READ|PROT_WRITE),
                            (MAP_SHARED|MAP_POPULATE),
                             _File.Handle(), 0);
        
      if (_TheEntry == MAP_FAILED)
      {
         THROW_MQAPIEXCEPTION("mmap",-1);
         return NULL;
      }
      
      if((_Size > 0) && (_TheSize!= _Size))
      {
		 	throw CMQException(__FILE__,__PRETTY_FUNCTION__,__LINE__,"MQF:%s Media Size not match",_FileName.c_str());
		 	return NULL;
      }
        
      return (PBYTE)_TheEntry;
   }
}

inline void CMQFileMemory::Detach()
{
	munmap(&(Entry()[-sizeof(sizeof(SIZE_T))]),Size()+sizeof(SIZE_T));	
}	


/*CSysvSHMemory*/
CSysvSHMemory::CSysvSHMemory(key_t _Key,SIZE_T _Size)
	:CMQMemory(Attach(_Key,_Size),_Size)
{
	
}

CSysvSHMemory::~CSysvSHMemory()
{
	Detach();
}

inline PBYTE CSysvSHMemory::Attach(key_t _Key,SIZE_T _Size)
{
    INT	_Result;
    
    if(_Size == 0)
    {
    	_Result = shmget(_Key,0,(IPC_EXCL|S_IRWXU)); 	
    }
    else
    {
    	_Result = shmget(_Key,0,(IPC_EXCL|S_IRWXU));
    	if(_Result!=-1)
    	{
		    if(shmctl(_Result, IPC_RMID, NULL) == -1)
		    {
		        THROW_MQAPIEXCEPTION("shmctl",-1);
		    }     
    	}

    	_Result = shmget(_Key,_Size+sizeof(SIZE_T),(IPC_CREAT|S_IRWXU));
    }
    
    if(_Result == -1)
    {
    	THROW_MQAPIEXCEPTION("shmget",_Result);  	
    }
    
    union
	{
		PBYTE	pEntry;
		PVOID   pResult;
		LONG	lResult;	
	}_At;
    
    _At.pResult = shmat(_Result,NULL,0); 

    if(_At.lResult == -1)
    {
        _At.pResult = NULL;
        THROW_MQAPIEXCEPTION("shmat",-1);   
    } 
    
    return _At.pEntry;
}

inline void CSysvSHMemory::Detach()
{
	 shmdt(&(Entry()[-sizeof(sizeof(SIZE_T))]));
}


}; //namespace AMQ
}; //namespace Galaxy

