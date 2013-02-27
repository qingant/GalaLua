#ifndef MQANCHOR_HPP_A61DB92A_2273_4FB1_8049_3C01D69899F0
#define MQANCHOR_HPP_A61DB92A_2273_4FB1_8049_3C01D69899F0


#pragma once

#include "MQBlock.hpp"
#include "MQMedia.hpp"


//版本，有代码变化一定要升小版本号
#define MQ_VERSION		(0x25)

namespace Galaxy
{
	namespace AMQ
	{
	/*
	  内存映像
	  +-----------------------+
	  |       Check Head      |
	  |-----------------------|
	  |      Offset Table     |
	  |-----------------------|
	  |		  Dependence      |
	  |-----------------------|
	  |         Nodes         |
	  |-----------------------|
	  |         Pages         |
	  |-----------------------|
	  |        Pooler Q       |
	  |-----------------------|
	  |        Normal Q       |
	  |-----------------------|
	  |     Distributary Q    |
	  +-----------------------+
	*/
		typedef struct
		{
		   BYTE    _a;
		   SHORT   _b;
		   UINT    _c;
		   LONG    _d;
		}STALIGN;

		typedef struct
		{
			BYTE                _Magic[8]; 	//0-MAGIC，1-LONG长，2-对齐，3-版本，4~7 字节序
			BYTE                _BCHK[8];   	//0-SQOFFSET size,1-SQNODE size,2-SQNODES size,3-SQLIST SIZE
			USHORT              _UCHK[6];   	//0-SQNRMLIST size , 1-SQCHLLIST size,2-SQNQ size,3-SQCQ size
			UINT                _Size;      	//整体大小

			time_t				_CreateDate;	//创建时间
			UINT64				_CheckSum;		//校验
			UINT64				_Author;		//作者
			BYTE				_License[128];	//许可证
		}SQINFO,*PSQINFO;
		typedef const SQINFO* CPSQINFO;
		
		typedef struct
		{
			UINT		_OFS_BEGIN;
			UINT        _OFS_Depdnc;
			UINT        _OFS_Nodes;
			UINT        _OFS_PageArray;
			UINT        _OFS_PQ;            //Pooler Q
			UINT        _OFS_NQ;
			UINT        _OFS_DQ;
			UINT        _OFS_END;      
		}SQOFFSET,*PSQOFFSET;
		
		
		typedef struct
		{
			UINT			_MSK;
			UINT			_WAT;
			pthread_cond_t	_CND;	
		}SQCOND,*PSQCOND;
		
		typedef struct
		{
			UINT			_MSK;
			UINT			_STA;
			pthread_mutex_t	_MTX;	
			
		}SQMUTEX,*PSQMUTEX;
	
		typedef struct
		{
			SQMUTEX 			_MTX;
			SQCOND				_CND;
		}SQDEPENDENCE,*PSQDEPENDENCE;
		
		typedef struct
		{
			UINT                _Id;        //ID
			UINT                _Offset;    //数据区偏移
			
			UINT                _State;     //状态
			UINT                _Next;      //下一页
			UINT                _Length;    //长度数据   
		}SQNODE,*PSQNODE;
		typedef const SQNODE* CPSQNODE;
		
		typedef struct
		{
			UINT                _Pages;
			UINT                _PageSize;
			SQNODE              _NodeArray[0];  
		}SQNODES,*PSQNODES;
		
		typedef struct
		{
			UINT                _Total;
			UINT	            _Head;
			UINT	            _Tail;
		}SQLIST,*PSQLIST;
	
		typedef struct
		{
			UINT				_Type;					//类型
			UINT            	_Limit;				    //最大深度，受控于PUT
			SQLIST              _LST;					//List
			struct
			{
				SQCOND  		_GET;                   //GET 条件
				SQCOND  		_PUT;                   //PUT 条件
			}					_EVT;					//事件

			SQMUTEX     		_GLK;                   //条件锁
		}SQNRMLIST,*PSQNRMLIST;

		#define MAXPQCHL        (3)
		
		typedef struct
		{
			UINT				_Type;					//类型
			UINT            	_Limit;				    //最大深度，受控于PUT
			SQLIST              _CHL[MAXPQCHL];
			struct
			{
				SQCOND  		_GET[MAXPQCHL];         //GET 条件
				SQCOND  		_PUT;                   //PUT 条件
			}					_EVT;					//事件

			SQMUTEX     		_GLK;                  //条件锁
		}SQCHLLIST,*PSQCHLLIST;

		typedef struct
		{
			UINT                _NQ;                        //普通Q个数
			SQNRMLIST           _Q[0];                      //Q Array;     
		}SQNQ,*PSQNQ;
		
		typedef struct
		{
			UINT                _DQ;                        //通道Q个数
			SQCHLLIST           _Q[0];                      //Q Array;     
		}SQDQ,*PSQDQ;
           
		class CAnchor : public IBlock
		{
		protected:
			explicit CAnchor();
			
		public:
			virtual ~CAnchor();
			void Verify() const;
			PSQINFO GetInfoEntry() const;
			PSQOFFSET GetOffsetTabEntry() const;
			PSQDEPENDENCE GetDependence() const;
			PSQNODES GetNodesEntry() const;
			PBYTE GetPageEntry() const;
			PBYTE GetPagePos(UINT _PageId) const;
			PSQNRMLIST GetPoolerQueueEntry() const;
			PSQNQ GetNormalQueueEntry() const;
			PSQDQ GetDistributaryQueueEntry() const; 
		};
		
		
		class CFileMemoryAnchor : public CAnchor
		{
		private:
			volatile PBYTE _Entry;
			volatile UINT _Size; 
		
		
			inline void Create(const std::string &_FileName,bool _IsCreate,UINT _InitSize);
			inline void Destroy();
			
			CFileMemoryAnchor(const CFileMemoryAnchor &);
			const CFileMemoryAnchor &operator=(const CFileMemoryAnchor &);
		public:
			explicit CFileMemoryAnchor(const std::string &_FileName,bool _IsCreate = false,UINT _InitSize = 0);
			~CFileMemoryAnchor(); 
		
			PBYTE RAWEntry() const;
			UINT RAWSize() const;  
		};
		
		
		class CSHMAnchor : public CAnchor
		{
		private:
			CSHM		_SHM;
		public:
			explicit CSHMAnchor(key_t _IPCKey,bool _IsCreate = false,UINT _InitSize = 0);
			~CSHMAnchor(); 
		
			PBYTE RAWEntry() const;
			UINT RAWSize() const;  	
		};
		

		UINT MQEvaluate(UINT _ThePages,UINT _ThePageSize,UINT _TheNQS,UINT _TheDQS);
		USHORT MQCheckSum(PVOID _Buffer, UINT _Length);
		
		

	} // AMQ namespace
} // Galaxy namespace

#endif /*MQANCHOR_HPP_A61DB92A_2273_4FB1_8049_3C01D69899F0*/
