
#include "MQMedia.hpp"
#include "MQException.hpp"

using namespace Galaxy::AMQ;

/*CBinaryFile*/
CBinaryFile::CBinaryFile(const std::string &_FileName,INT _Flag,mode_t _Perm)
   :_FD(Open(_FileName,_Flag,_Perm))
{
   if (_FD == -1)
   {
      THROW_MQEXCEPTION("Invalid Binary File Handle"); 
   }  
}

CBinaryFile::~CBinaryFile()
{
   Close();  
}

INT CBinaryFile::Open(const std::string &_FileName,INT _Flag,mode_t _Perm)
{
   INT rc = CRT_open(_FileName.c_str(),_Flag,_Perm);  
   if(rc < 0)
   {
      THROW_MQEXCEPTION("");     
   } 

   return rc;
}

void CBinaryFile::Close()
{
   if(_FD!=-1)
   {
      INT  rc;
      rc = CRT_close(_FD);   
      if (rc != 0)
      {
         THROW_MQEXCEPTION("");
         return;
      }
        
      _FD = -1;
   }
}

INT CBinaryFile::Handle() const
{
   if (_FD == -1)
   {
      THROW_MQEXCEPTION("Invalid Binary File Handle"); 
   }

   return _FD;   
}

SIZE_T CBinaryFile::Size() const
{
   struct stat _FST;
   if(CRT_fstat(Handle(),&_FST)!=0)
   {
      THROW_MQEXCEPTION("");
      return (0);   
   }
    
   return _FST.st_size;  
}

void CBinaryFile::Truncate(SIZE_T _Length) const
{
   INT rc = CRT_ftruncate(Handle(),_Length);  
   if(rc != 0)
   {
      THROW_MQEXCEPTION(""); 
   }  
}


/*CSHM*/ 
CSHM::CSHM(key_t _Key,UINT _Size, INT _oflag)
    :_Handle(Create(_Key,_Size,_oflag)),_RAWEntry(Attach())
{
    if((_oflag & IPC_CREAT)==IPC_CREAT)
    {
    	Size(_Size);   
    }
}

CSHM::~CSHM()
{
    Detach();
}

INT CSHM::Create(key_t _Key,UINT _Size, INT _oflag)
{
    INT	_Result;
    
    if((_oflag & IPC_CREAT)==IPC_CREAT)
    {
    	_Result = CRT_shmget(_Key,0,(_oflag &~ IPC_CREAT) | IPC_EXCL );
    	if(_Result!=-1)
    	{
		    if(CRT_shmctl(_Result, IPC_RMID, NULL) == -1)
		    {
		        THROW_MQEXCEPTION("CRT_shmctl Failure!");
		    }     
    	}

    	_Result = CRT_shmget(_Key,_Size+sizeof(UINT),_oflag);
    }
    else
    {
    	_Result = CRT_shmget(_Key,0,_oflag); 
    }
    
    if(_Result == -1)
    {
    	THROW_MQEXCEPTION("CRT_shmget failure!");  	
    }
    
    return _Result;
}

PBYTE CSHM::Attach()
{
    union
	{
		PBYTE	pEntry;
		PVOID   pResult;
		LONG	lResult;	
	}_At;
    
    _At.pResult = CRT_shmat(_Handle,NULL,0); 

    if(_At.lResult == -1)
    {
        _At.pResult = NULL;
        THROW_MQEXCEPTION("CRT_shmat Failure!");   
    } 
    
    return _At.pEntry;   
}

void CSHM::Detach()
{
    if(_RAWEntry!=NULL)
    {
        if(CRT_shmdt(_RAWEntry)==-1)
        {
            THROW_MQEXCEPTION("CRT_shmdt Failure!");     
        }
    }
    
    _RAWEntry = NULL; 
}

void CSHM::Size(UINT _Size)
{
    if(ISNULL(_RAWEntry))
    {
        THROW_MQEXCEPTION("_RAWEntry is NULL!");  
    } 
    
    *((UINT *)_RAWEntry) = _Size;
}

PBYTE CSHM::Entry() const
{
    if(ISNULL(_RAWEntry))
    {
        THROW_MQEXCEPTION("_RAWEntry is NULL!");    
    }
    
    return &_RAWEntry[sizeof(UINT)];    
}

UINT CSHM::Size() const
{
    if(ISNULL(_RAWEntry))
    {
        return 0;  
    }
    
    return *((UINT *)_RAWEntry);
}
