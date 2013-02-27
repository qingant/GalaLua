
#include "MQAnchor.hpp"
#include "MQCond.hpp"
#include "MQMutex.hpp"
#include "MQLicense.hpp"
#include "MQException.hpp"

namespace Galaxy
{
	namespace AMQ
	{
		UINT MQEvaluate(UINT _ThePages,UINT _ThePageSize,UINT _TheNQS,UINT _TheDQS)
		{
		   UINT _NeedSize = 0;
		    
		   _NeedSize+=sizeof(SQINFO) + sizeof(SQOFFSET)+sizeof(SQDEPENDENCE);	/*INFO + OFFSET + DEPENDENCE*/
		   _NeedSize+=sizeof(SQNRMLIST) + sizeof(SQNODES) + (sizeof(SQNODE) * _ThePages) + (_ThePages * _ThePageSize);      /*Pooler,NODES,Pages*/
		   _NeedSize+=sizeof(SQNQ) + (sizeof(SQNRMLIST) * _TheNQS);                /*NQ Array*/
		   _NeedSize+=sizeof(SQDQ) + (sizeof(SQCHLLIST) * _TheDQS);                /*CQ Array*/
		
		   _NeedSize = ((_NeedSize % 1024)==0) ? _NeedSize : (_NeedSize + (1024-(_NeedSize % 1024)));
		
		   return _NeedSize;
		}
		
		USHORT MQCheckSum(PVOID _Buffer, UINT _Length)
		{ 
			USHORT *_StepBuffer = (USHORT *)_Buffer;
			UINT cksum=0; 

			while(_Length >1) 
			{ 
				cksum+=*_StepBuffer++;
				_Length -=sizeof(USHORT);
			}
			
			if(_Length)
			{
				cksum += *((PBYTE)_StepBuffer);
			}
			
			cksum = (cksum >> 16) + (cksum & 0xffff);
			cksum += (cksum >>16);
			return (USHORT)(~cksum);
		}
		
	}
}


using namespace Galaxy::AMQ;


/*CAnchor*/
CAnchor::CAnchor()
{

}

CAnchor::~CAnchor()
{
        
}

static bool MultipleOfFour(UINT _v)
{
   return ((_v % 4)==0);   
}

void CAnchor::Verify() const
{
   if(!MultipleOfFour(sizeof(SQINFO)))
   {
      THROW_MQEXCEPTION("Ooh!My God,The <SQINFO> Designer Is Stupid!");
      return;   
   }
   else if(!MultipleOfFour(sizeof(SQOFFSET)))
   {
      THROW_MQEXCEPTION("Ooh!My God,The <SQOFFSET> Designer Is Stupid!");
      return;     
   }
   else if(!MultipleOfFour(sizeof(SQCOND)))
   {
      THROW_MQEXCEPTION("Ooh!My God,The <SQCOND> Designer Is Stupid!");
      return;     
   }
   else if(!MultipleOfFour(sizeof(SQMUTEX)))
   {
      THROW_MQEXCEPTION("Ooh!My God,The <SQMUTEX> Designer Is Stupid!");
      return;     
   }
   else if(!MultipleOfFour(sizeof(SQDEPENDENCE)))
   {
      THROW_MQEXCEPTION("Ooh!My God,The <SQDEPENDENCE> Designer Is Stupid!");
      return;     
   }
   else if(!MultipleOfFour(sizeof(SQNODE)))
   {
      THROW_MQEXCEPTION("Ooh!My God,The <SQNODE> Designer Is Stupid!");
      return;     
   }
   else if(!MultipleOfFour(sizeof(SQNODES)))
   {
      THROW_MQEXCEPTION("Ooh!My God,The <SQNODES> Designer Is Stupid!");
      return;    
   }
   else if(!MultipleOfFour(sizeof(SQLIST)))
   {
      THROW_MQEXCEPTION("Ooh!My God,The <SQLIST> Designer Is Stupid!");
      return;  
   }
   else if(!MultipleOfFour(sizeof(SQNRMLIST)))
   {
      THROW_MQEXCEPTION("Ooh!My God,The <SQNRMLIST> Designer Is Stupid!");
      return;   
   }
   else if(!MultipleOfFour(sizeof(SQCHLLIST)))
   {
      THROW_MQEXCEPTION("Ooh!My God,The <SQCHLLIST> Designer Is Stupid!");
      return;   
   }
   else if(!MultipleOfFour(sizeof(SQNQ)))
   {
      THROW_MQEXCEPTION("Ooh!My God,The <SQNQ> Designer Is Stupid!");
      return;   
   }
   else if(!MultipleOfFour(sizeof(SQDQ)))
   {
      THROW_MQEXCEPTION("Ooh!My God,The <SQCQ> Designer Is Stupid!");
      return;   
   }
    
   //PSQINFO
   {
      PSQINFO  _Info = GetInfoEntry();
        
      //_Magic
      if(_Info->_Magic[0] != 0xAF)
      {
         THROW_MQEXCEPTION("MQ Magic Not Matched!");
         return;    
      }
      else if(_Info->_Magic[1] != sizeof(LONG))
      {  
         THROW_MQEXCEPTION("32/64 bit width not matched!");   
         return; 
      }
      else if(_Info->_Magic[2] != sizeof(STALIGN))
      {
         THROW_MQEXCEPTION("IMP Data Align not matched!"); 
         return;   
      } 
      else if(_Info->_Magic[3] != MQ_VERSION)
      {
         CHAR	szError[1024];
         CRT_snprintf(szError,sizeof(szError),"MQ Version Error! Q:[ v%d.%d ] API:[ v%d.%d ]", \
         			  (_Info->_Magic[3] & 0xF0) >> 4 ,_Info->_Magic[3] & 0x0F, \
         			  (MQ_VERSION & 0xF0) >> 4 ,MQ_VERSION & 0x0F);

         THROW_MQEXCEPTION(szError);
         return;     
      } 
        
      PUINT _pbed = (PUINT) &(_Info->_Magic[4]);
      if((*_pbed) != 0x00000001)
      {
         THROW_MQEXCEPTION("MQ Version Error!");
         return;
      }
      
      
      USHORT _AllStructSize = sizeof(SQINFO)+sizeof(SQOFFSET)+sizeof(SQDEPENDENCE)+ \
      						sizeof(SQNODE)+sizeof(SQNODES)+sizeof(SQLIST)+ \
      						sizeof(SQNRMLIST)+sizeof(SQCHLLIST)+sizeof(SQNQ)+sizeof(SQDQ);
        
      //_BCHK
      if(_Info->_BCHK[0] != sizeof(SQINFO))
      {
         THROW_MQEXCEPTION("BLOCK NO.01 Size Not Matched!");
         return;   
      }
      else if(_Info->_BCHK[1] != sizeof(SQOFFSET))
      {
         THROW_MQEXCEPTION("BLOCK NO.02 Size Not Matched!");
         return;  
      }
      else if(_Info->_BCHK[2] != sizeof(SQNODE))
      {
         THROW_MQEXCEPTION("BLOCK NO.03 Size Not Matched!");
         return;    
      }
      else if(_Info->_BCHK[3] != sizeof(SQNODES))
      {
         THROW_MQEXCEPTION("BLOCK NO.04 Size Not Matched!");
         return;   
      }
      else if(_Info->_BCHK[4] != sizeof(SQLIST))
      {
         THROW_MQEXCEPTION("BLOCK NO.05 Size Not Matched!");
         return;  
      }
      else if(_Info->_BCHK[5] != sizeof(SQNQ))
      {
         THROW_MQEXCEPTION("BLOCK NO.06 Size Not Matched!");
         return;
      }
      else if(_Info->_BCHK[6] != sizeof(SQDQ))
      {
         THROW_MQEXCEPTION("BLOCK NO.07 Size Not Matched!");
         return;
      }
      else if(_Info->_UCHK[0] != sizeof(SQCOND))
      {
         THROW_MQEXCEPTION("BLOCK NO.08 Size Not Matched!");
         return;
      }
      else if(_Info->_UCHK[1] != sizeof(SQMUTEX))
      {
         THROW_MQEXCEPTION("BLOCK NO.09 Size Not Matched!");
         return;
      }
      else if(_Info->_UCHK[2] != sizeof(SQDEPENDENCE))
      {
         THROW_MQEXCEPTION("BLOCK NO.10 Size Not Matched!");
         return;
      }
      else if(_Info->_UCHK[3] != sizeof(SQNRMLIST))
      {
         THROW_MQEXCEPTION("BLOCK NO.11 Size Not Matched!");
         return;
      }
      else if(_Info->_UCHK[4] != sizeof(SQCHLLIST))
      {
         THROW_MQEXCEPTION("BLOCK NO.12 Size Not Matched!");
         return;
      }
      else if(_Info->_UCHK[5] != _AllStructSize)
      {
         THROW_MQEXCEPTION("BLOCK NO.13 Size Not Matched!");
         return;
      }
        
      //_Size
      if(_Info->_Size != ::MQEvaluate(GetNodesEntry()->_Pages,GetNodesEntry()->_PageSize,GetNormalQueueEntry()->_NQ,GetDistributaryQueueEntry()->_DQ))
      {
         THROW_MQEXCEPTION("MQ Size Incorrect!");    
         return;  
      }
   }
    
   {
      PSQOFFSET  _OffsetTab  =  GetOffsetTabEntry();  
       
      if(_OffsetTab->_OFS_Depdnc  != (sizeof(SQINFO)+sizeof(SQOFFSET)))
      {
         THROW_MQEXCEPTION("OFFSET TABLE NO.1 Incorrect!");
         return;
      }
      else if(_OffsetTab->_OFS_Nodes != (_OffsetTab->_OFS_Depdnc + sizeof(SQDEPENDENCE)))
      {
         THROW_MQEXCEPTION("OFFSET TABLE NO.2 Incorrect!");
         return;
      }
      else if(_OffsetTab->_OFS_PageArray != (_OffsetTab->_OFS_Nodes +  sizeof(SQNODES) + (sizeof(SQNODE) * GetNodesEntry()->_Pages)))
      {
         THROW_MQEXCEPTION("OFFSET TABLE NO.3 Incorrect!");
         return;   
      }
      else if(_OffsetTab->_OFS_PQ != (_OffsetTab->_OFS_PageArray + (GetNodesEntry()->_Pages * GetNodesEntry()->_PageSize)))
      {
         THROW_MQEXCEPTION("OFFSET TABLE NO.4 Incorrect!");
         return;    
      }
      else if(_OffsetTab->_OFS_NQ != (_OffsetTab->_OFS_PQ + sizeof(SQNRMLIST)))
      {
         THROW_MQEXCEPTION("OFFSET TABLE NO.5 Incorrect!");
         return;    
      }
      else if(_OffsetTab->_OFS_DQ != (_OffsetTab->_OFS_NQ + sizeof(SQNQ) + (sizeof(SQNRMLIST) * GetNormalQueueEntry()->_NQ)))
      {
         THROW_MQEXCEPTION("OFFSET TABLE NO.6 Incorrect!");
         return; 
      }
      else if(_OffsetTab->_OFS_END != (_OffsetTab->_OFS_DQ + sizeof(SQDQ) + (sizeof(SQCHLLIST) * GetDistributaryQueueEntry()->_DQ)))
      {
         THROW_MQEXCEPTION("OFFSET TABLE NO.7 Incorrect!");
         return;  
      }
   }
   
   //CheckSum
   {
		SQINFO	 _DupInfo;
		PSQINFO  _TheInfo = GetInfoEntry();
   		PSQOFFSET 	_TheOffsetTab  =  GetOffsetTabEntry();
   		PSQDEPENDENCE _TheDepdnc = GetDependence();
   		PSQNODES 	_TheNodes = GetNodesEntry();
   		
   		CRT_memcpy(&_DupInfo,_TheInfo,sizeof(_DupInfo));
   		_DupInfo._CheckSum = 0;
   		
   		
   		UINT64 _CheckSum0 = ::MQCheckSum(&_DupInfo,sizeof(SQINFO));	
   		UINT64 _CheckSum1 = ::MQCheckSum(_TheOffsetTab,sizeof(SQOFFSET));	
   		UINT64 _CheckSum2 = ::MQCheckSum(_TheDepdnc,sizeof(SQDEPENDENCE));
   		UINT64 _CheckSum3 = ::MQCheckSum(_TheNodes,sizeof(SQNODES));
   			
   		if(_TheInfo->_CheckSum != (UINT64)((_CheckSum0 << 48)|(_CheckSum1 << 32)|(_CheckSum2 << 16)|(_CheckSum3 & 0xFFFF)))
   		{
         	THROW_MQEXCEPTION("MQ Checksum Incorrect!");
         	return;  
   		}
   }
   
   CMQLicense	_License(GetInfoEntry()->_License);
   //License
   {
   		if((!_License.IsTrial()) && _License.IsDateLimited())
   		{
         	THROW_MQEXCEPTION("MQ License Out Of Limit Date!");
         	return; 
   		} 

   		if(GetNodesEntry()->_Pages!=_License.MaxPages())
   		{
          	THROW_MQEXCEPTION("Pages Out Of Limite!");
         	return;
   		}
   		else if(GetNodesEntry()->_PageSize!=_License.MaxPageSize())
   		{
          	THROW_MQEXCEPTION("Pagesize Out Of Limite!");
         	return;
   		}
   		else if(GetNormalQueueEntry()->_NQ!=_License.MaxNQs())
   		{
          	THROW_MQEXCEPTION("NQs Out Of Limite!");
         	return;
   		}
   		else if(GetDistributaryQueueEntry()->_DQ!=_License.MaxDQs())
   		{
          	THROW_MQEXCEPTION("DQs Out Of Limite!");
         	return;
   		}
   } 
   
   {
      PSQNRMLIST      _PoolerQ = GetPoolerQueueEntry();
      PSQNODES        _Nodes = GetNodesEntry();
      CMutexSuite     _Mutex(_PoolerQ->_GLK);

      CMutexLockGuard _LG(_Mutex);
        
      UINT _Id,i;

      PBYTE _RAWPageEntry;
      UINT  _RAWOFS;
      for(i=0;i<_Nodes->_Pages;i++)
      {
         PSQNODE _Node = &(_Nodes->_NodeArray[i]);
         if(_Node->_Id!=(i+1))
         {
            THROW_MQEXCEPTION("MQ Pooler Node Id confused!");    
            return;    
         } 
            
         _RAWPageEntry = &(GetPageEntry()[(i * _Nodes->_PageSize)]);
         if(_RAWPageEntry <= (PBYTE)_Node)
         {
            THROW_MQEXCEPTION("MQ Pooler Page Ofsset Error!");    
            return; 
         }
            
         _RAWOFS =  _RAWPageEntry - (PBYTE)_Node;
            
         if(_Node->_Offset!=_RAWOFS)
         {
            THROW_MQEXCEPTION("MQ Pooler Page Ofsset confused!");    
            return; 
         }
      }
        
      SQLIST &_LST = _PoolerQ->_LST;
      for(i=0,_Id=_LST._Head;(i<_LST._Total);i++,_Id = _Nodes->_NodeArray[(_Id-1)]._Next)
      {
         if((_Id < 1) || (_Id > _Nodes->_Pages))
         {
            THROW_MQEXCEPTION("MQ Pooler confused!");    
            return;    
         }
      }   
   }
    
   //NQ校验
   {
      PSQNQ _NQArray  = GetNormalQueueEntry();
      PSQNODES _Nodes = GetNodesEntry();

      for(UINT v=0;v<_NQArray->_NQ;v++)
      {
         SQNRMLIST &_NList = (_NQArray->_Q[v]); 
            
         {
            CMutexSuite     _Mutex(_NList._GLK);
            CMutexLockGuard _LG(_Mutex); 

            SQLIST &_LST = _NList._LST;

            UINT _Id,i;
            for(i=0,_Id=_LST._Head;(i<_LST._Total);i++,_Id = _Nodes->_NodeArray[(_Id-1)]._Next)
            {
               if((_Id < 1) || (_Id > _Nodes->_Pages))
               {
                  THROW_MQEXCEPTION("MQ NQ confused!");    
                  return;    
               }
            } 
            
         }    
      }  
   }
    
   //DQ校验
   {
      PSQDQ _CQArray  = GetDistributaryQueueEntry();
      PSQNODES _Nodes = GetNodesEntry();

      for(UINT v=0;v<_CQArray->_DQ;v++)
      {
         SQCHLLIST &_DList = (_CQArray->_Q[v]); 
            
         {
            CMutexSuite     _Mutex(_DList._GLK);
            CMutexLockGuard _LG(_Mutex); 
            
            for(UINT c=0;c < MAXPQCHL;c++)
            {
                SQLIST &_LST = (_DList._CHL[c]);
                
                UINT _Id,i;
                for(i=0,_Id=_LST._Head;(i<_LST._Total);i++,_Id = _Nodes->_NodeArray[(_Id-1)]._Next)
                {
                   if((_Id < 1) || (_Id > _Nodes->_Pages))
                   {
                      THROW_MQEXCEPTION("MQ CQ confused!");    
                      return;    
                   }
                } 
            }

         }    
      }    
   }  
}

PSQINFO CAnchor::GetInfoEntry() const
{
   return (PSQINFO)RAWEntry();    
}

PSQOFFSET CAnchor::GetOffsetTabEntry() const
{
   return (PSQOFFSET) &(RAWEntry()[sizeof(SQINFO)]);  
}

PSQDEPENDENCE CAnchor::GetDependence() const
{
	return (PSQDEPENDENCE) &(RAWEntry()[GetOffsetTabEntry()->_OFS_Depdnc]);	
}

PSQNRMLIST CAnchor::GetPoolerQueueEntry() const
{
   return (PSQNRMLIST) &(RAWEntry()[GetOffsetTabEntry()->_OFS_PQ]);     
}

PSQNODES CAnchor::GetNodesEntry() const
{
   return (PSQNODES) &(RAWEntry()[GetOffsetTabEntry()->_OFS_Nodes]);     
}


PBYTE CAnchor::GetPageEntry() const
{
   return &(RAWEntry()[GetOffsetTabEntry()->_OFS_PageArray]);     
}

PBYTE CAnchor::GetPagePos(UINT _PageId) const
{
	PSQNODES _Nodes = GetNodesEntry();
	PBYTE	 _PageEntry = GetPageEntry();
	
	if(_PageId >= _Nodes->_Pages)
	{
        THROW_MQEXCEPTION("_PageId out of range!");    
        return NULL;  
	}
	
	return &_PageEntry[(_PageId * _Nodes->_PageSize)];	
}

PSQNQ CAnchor::GetNormalQueueEntry() const
{
   return (PSQNQ) &(RAWEntry()[GetOffsetTabEntry()->_OFS_NQ]);     
}

PSQDQ CAnchor::GetDistributaryQueueEntry() const
{
   return (PSQDQ) &(RAWEntry()[GetOffsetTabEntry()->_OFS_DQ]);    
}



/*CFileMemoryAnchor*/
CFileMemoryAnchor::CFileMemoryAnchor(const std::string &_FileName, bool _IsCreate,UINT _InitSize)
   :CAnchor(),_Entry(NULL),_Size(0)
{
   Create(_FileName,_IsCreate,_InitSize);    
}

CFileMemoryAnchor::~CFileMemoryAnchor()
{
   Destroy();   
}

void CFileMemoryAnchor::Create(const std::string &_FileName,bool _IsCreate,
                         UINT _InitSize)
{
   INT _Flag = 0;
   if(_IsCreate)
   {
      _Flag = (O_CREAT|O_RDWR);
   }
   else
   {
      _Flag = (O_EXCL|O_RDWR);    
   }
    
   CBinaryFile     _File(_FileName,(_Flag|O_RDWR),(S_IRUSR|S_IWUSR));

   if((_IsCreate) && (_InitSize > 0))
   {
      _File.Truncate(_InitSize);
   }

   {
      UINT _TheSize = (UINT) _File.Size();
#ifndef MAP_POPULATE
#define MAP_POPULATE 0
#endif
      PVOID _TheEntry = CRT_mmap(NULL,_TheSize,
                                 (PROT_EXEC|PROT_READ|PROT_WRITE),
                                 (MAP_SHARED|MAP_POPULATE),
                                 _File.Handle(), 0);
        
      if (_TheEntry == MAP_FAILED)
      {
         THROW_MQEXCEPTION("");
         return;
      }
        
      _Entry = (PBYTE)_TheEntry;
      _Size = _TheSize; 
   }
}

void CFileMemoryAnchor::Destroy()
{
   if(_Entry !=NULL)
   {
      INT rc = CRT_munmap(_Entry, _Size);
      if (rc != 0)
      {
         THROW_MQEXCEPTION("");
      }
        
      _Entry = NULL;
      _Size = 0;
   } 
}

PBYTE CFileMemoryAnchor::RAWEntry() const
{
   if(ISNULL(_Entry))
   {
      THROW_MQEXCEPTION("_Entry Is NULL!");   
   }
    
   return _Entry;   
}

UINT CFileMemoryAnchor::RAWSize() const
{
   return _Size;   
}



/*CSHMAnchor*/
CSHMAnchor::CSHMAnchor(key_t _IPCKey,bool _IsCreate,UINT _InitSize)
	:_SHM(_IPCKey,_InitSize,((_IsCreate)?(IPC_CREAT|S_IRWXU):(IPC_EXCL|S_IRWXU)))
{
		
}

CSHMAnchor::~CSHMAnchor()
{
	
}


PBYTE CSHMAnchor::RAWEntry() const
{
	return _SHM.Entry();	
}

	
UINT CSHMAnchor::RAWSize() const
{
	return _SHM.Size();	
}
