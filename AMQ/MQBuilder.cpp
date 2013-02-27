
#include "MQBuilder.hpp"
#include "MQCond.hpp"
#include "MQMutex.hpp"
#include "MQException.hpp"

using namespace Galaxy::AMQ;



/*CMQBuilder*/
CMQBuilder::CMQBuilder(const CAnchor &_Anchor,const CMQLicense &_License,UINT _PreLimit)
{
	UINT _ThePages,_ThePageSize,_TheNQS,_TheDQS;
	
	_ThePages = _License.MaxPages();
	_ThePageSize = _License.MaxPageSize(); 
	_TheNQS = _License.MaxNQs();
	_TheDQS = _License.MaxDQs();


	if((!_License.IsTrial()) && _License.IsDateLimited())
	{
     	THROW_MQEXCEPTION("MQ License Out Of Limit Date!");
     	return; 
	}   
   
   if((_TheNQS % 2)!=0)
   {
		_TheNQS+=1;
   }

   if((_TheDQS % 2)!=0)
   {
		_TheDQS+=1;
   }
   
   if(_PreLimit > _ThePages)
   {
	    _PreLimit = _ThePages;
   }

   //PSQINFO
   {
      PSQINFO  _Info = _Anchor.GetInfoEntry();
        
      //_Magic
      _Info->_Magic[0] = 0xAF;
      _Info->_Magic[1] = sizeof(LONG);  
      _Info->_Magic[2] = sizeof(STALIGN); 
      _Info->_Magic[3] = MQ_VERSION; //Ver
      PUINT _pbed = (PUINT) &(_Info->_Magic[4]);
      *_pbed = 0x00000001;
        
      //_BCHK
      _Info->_BCHK[0] = sizeof(SQINFO);
      _Info->_BCHK[1] = sizeof(SQOFFSET);
      _Info->_BCHK[2] = sizeof(SQNODE);
      _Info->_BCHK[3] = sizeof(SQNODES);
      _Info->_BCHK[4] = sizeof(SQLIST);
      _Info->_BCHK[5] = sizeof(SQNQ);
      _Info->_BCHK[6] = sizeof(SQDQ);
        
      //_UCHK
      _Info->_UCHK[0] = sizeof(SQCOND);
      _Info->_UCHK[1] = sizeof(SQMUTEX);
      _Info->_UCHK[2] = sizeof(SQDEPENDENCE);
      _Info->_UCHK[3] = sizeof(SQNRMLIST);
      _Info->_UCHK[4] = sizeof(SQCHLLIST);
      _Info->_UCHK[5] = sizeof(SQINFO)+sizeof(SQOFFSET)+sizeof(SQDEPENDENCE)+ \
      						sizeof(SQNODE)+sizeof(SQNODES)+sizeof(SQLIST)+ \
      						sizeof(SQNRMLIST)+sizeof(SQCHLLIST)+sizeof(SQNQ)+sizeof(SQDQ);
      
      {
      	CRT_time(&(_Info->_CreateDate));
      	_Info->_CheckSum = 0;
      	PBYTE _PtrAuthor = (PBYTE)&(_Info->_Author);
      	
      	_PtrAuthor[7] = (~0x5b) & 0xFF;
      	_PtrAuthor[6] = (~0xc0) & 0xFF;
      	_PtrAuthor[5] = (~0xee) & 0xFF;
   		_PtrAuthor[4] = (~0xd3) & 0xFF;
      	_PtrAuthor[3] = (~0xee) & 0xFF;
   		_PtrAuthor[2] = (~0xcf) & 0xFF;
      	_PtrAuthor[1] = (~0xe8) & 0xFF;
      	_PtrAuthor[0] = (~0x5d) & 0xFF;
      	
      	_License.Migrate(_Info->_License,sizeof(_Info->_License));
      } 
      //_Size
      _Info->_Size = ::MQEvaluate(_ThePages,_ThePageSize,_TheNQS,_TheDQS);
        
   } 
    
   {
      PSQOFFSET  _OffsetTab  =  _Anchor.GetOffsetTabEntry();  
      
      _OffsetTab->_OFS_BEGIN	 = 0;
      _OffsetTab->_OFS_Depdnc	 = sizeof(SQINFO)+sizeof(SQOFFSET); 
      _OffsetTab->_OFS_Nodes     = _OffsetTab->_OFS_Depdnc + sizeof(SQDEPENDENCE);
      _OffsetTab->_OFS_PageArray = _OffsetTab->_OFS_Nodes +  sizeof(SQNODES) + (sizeof(SQNODE) * _ThePages);
      _OffsetTab->_OFS_PQ        = _OffsetTab->_OFS_PageArray + (_ThePages * _ThePageSize);
      _OffsetTab->_OFS_NQ        = _OffsetTab->_OFS_PQ + sizeof(SQNRMLIST);
      _OffsetTab->_OFS_DQ        = _OffsetTab->_OFS_NQ + sizeof(SQNQ) + (sizeof(SQNRMLIST) * _TheNQS);
      _OffsetTab->_OFS_END       = _OffsetTab->_OFS_DQ + sizeof(SQDQ) + (sizeof(SQCHLLIST) * _TheDQS);
   }
   
   PSQDEPENDENCE _Depdnc = _Anchor.GetDependence();
   
   {  		
   		CConditionCreator 	_Cond(_Depdnc->_CND);
   		CMutexCreator 		_Mtx(_Depdnc->_MTX);	
   }
    
   {
      PSQNODES _Nodes = _Anchor.GetNodesEntry();
        
      _Nodes->_Pages      = _ThePages;
      _Nodes->_PageSize   = _ThePageSize; 
        
      PSQNODE  _Node = NULL;
      PSQNODE  _Prev = NULL;
      for(UINT i=0;i<_Nodes->_Pages;i++,_Prev = _Node)
      {
         _Node =  &(_Nodes->_NodeArray[i]);
         _Node->_Id = (i+1);
         _Node->_Offset = ( sizeof(SQNODE) * (_Nodes->_Pages - i)) + (i * _Nodes->_PageSize); 
         _Node->_Next = 0;
         _Node->_Length = 0;
         _Node->_State = 0xFFFFFFFF;
            
         if(_Prev!=NULL)
         {
            _Prev->_Next = _Node->_Id;    
         }
            
         PBYTE _PageEntry = _Anchor.GetPageEntry();
         _PageEntry = &_PageEntry[(i * _Nodes->_PageSize)];
            
         UINT _Offset = 0;
         if(_PageEntry > (PBYTE)_Node)
         {
            _Offset= (_PageEntry - (PBYTE)_Node);
         }

         if(_Node->_Offset != _Offset)
         {
            CHAR szException[256];
            CRT_snprintf(szException,sizeof(szException),"Node [%d] Data Offset Incorrect!",(i+1));
            THROW_MQEXCEPTION(szException);    
            return;    
         } 
      } 
   }
   
    
   {
      PSQNRMLIST _PoolerQ = _Anchor.GetPoolerQueueEntry();
        
      //List

      _PoolerQ->_Limit 		  = 0;
      _PoolerQ->_LST._Total   = _ThePages;
      _PoolerQ->_LST._Head    = 1;
      _PoolerQ->_LST._Tail    = _ThePages;
      
      CRT_memcpy(&(_PoolerQ->_GLK),&(_Depdnc->_MTX),sizeof(_PoolerQ->_GLK));
      CRT_memcpy(&(_PoolerQ->_EVT._GET),&(_Depdnc->_CND),sizeof(_PoolerQ->_EVT._GET));
      CRT_memcpy(&(_PoolerQ->_EVT._PUT),&(_Depdnc->_CND),sizeof(_PoolerQ->_EVT._PUT)); 
      
      PBYTE _PtrPQType = (PBYTE)&(_PoolerQ->_Type); 
      _PtrPQType[0] = 0x50;
      _PtrPQType[1] = 0x4c;
      _PtrPQType[2] = 0x51;
      _PtrPQType[3] = 0x3a;

   }
    
   {
      PSQNQ   _NQArray = _Anchor.GetNormalQueueEntry();
      _NQArray->_NQ = _TheNQS;
        
      PSQNRMLIST  _NList;
      for(UINT i=0;i<_NQArray->_NQ;i++)
      {
         _NList = &(_NQArray->_Q[i]);    

		 _NList->_Limit		   = _PreLimit;
         _NList->_LST._Total   = 0;
         _NList->_LST._Head    = 0;
         _NList->_LST._Tail    = 0;
          
          
      	 CRT_memcpy(&(_NList->_GLK),&(_Depdnc->_MTX),sizeof(_NList->_GLK));
      	 CRT_memcpy(&(_NList->_EVT._GET),&(_Depdnc->_CND),sizeof(_NList->_EVT._GET));
      	 CRT_memcpy(&(_NList->_EVT._PUT),&(_Depdnc->_CND),sizeof(_NList->_EVT._PUT)); 
      	 
	      PBYTE _PtrNQType = (PBYTE)&(_NList->_Type); 
	      _PtrNQType[0] = 0x4e;
	      _PtrNQType[1] = 0x4d;
	      _PtrNQType[2] = 0x51;
	      _PtrNQType[3] = 0x3a;  
      } 
   }
    
   {
      PSQDQ   _CQArray = _Anchor.GetDistributaryQueueEntry();
      _CQArray->_DQ = _TheDQS;
        
      PSQCHLLIST  _CList;
      for(UINT i=0;i<_CQArray->_DQ;i++)
      {
         _CList = &(_CQArray->_Q[i]); 
         
         _CList->_Limit		   = _PreLimit;
            
         for(UINT j=0;j<MAXPQCHL;j++)
         {
            _CList->_CHL[j]._Total   = 0;
            _CList->_CHL[j]._Head    = 0;
            _CList->_CHL[j]._Tail    = 0;
                

            CRT_memcpy(&(_CList->_EVT._GET[j]),&(_Depdnc->_CND),sizeof(_CList->_EVT._PUT));
         }   
         
         CRT_memcpy(&(_CList->_EVT._PUT),&(_Depdnc->_CND),sizeof(_CList->_EVT._PUT));       
		 CRT_memcpy(&(_CList->_GLK),&(_Depdnc->_MTX),sizeof(_CList->_GLK));
		 
	      PBYTE _PtrDQType = (PBYTE)&(_CList->_Type); 
	      _PtrDQType[0] = 0x44;
	      _PtrDQType[1] = 0x53;
	      _PtrDQType[2] = 0x51;
	      _PtrDQType[3] = 0x3a; 
		  
      } 
   }
   
   //CheckSum
   {
   		PSQINFO  	_Info = _Anchor.GetInfoEntry();
   		PSQOFFSET 	_OffsetTab  =  _Anchor.GetOffsetTabEntry();
   		PSQNODES 	_Nodes = _Anchor.GetNodesEntry();

   		UINT64 _CheckSum0 = ::MQCheckSum(_Info,sizeof(SQINFO));	
   		UINT64 _CheckSum1 = ::MQCheckSum(_OffsetTab,sizeof(SQOFFSET));	
   		UINT64 _CheckSum2 = ::MQCheckSum(_Depdnc,sizeof(SQDEPENDENCE));
   		UINT64 _CheckSum3 = ::MQCheckSum(_Nodes,sizeof(SQNODES));
   			
   		_Info->_CheckSum = (UINT64)((_CheckSum0 << 48)|(_CheckSum1 << 32)|(_CheckSum2 << 16)|(_CheckSum3 & 0xFFFF));
   }
    
   _Anchor.Verify();
}

CMQBuilder::~CMQBuilder()
{
    
}

