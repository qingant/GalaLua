#include <string.h>
#include "MQ_SQAllocator.hpp"
#include "MQException.hpp"
#include "MQT.hpp"

namespace Galaxy
{

namespace AMQ
{
	
template < class _PG_,class _LST_,class _LCK_ > void T_Free(const _PG_ &_Pages,const _LST_ &_List,const _LCK_ &_Lock)
{
	const _PG_ *_ThePage = &_Pages;
	const _PG_ *_TheNextPage = NULL;
	
	{
		CMQLockGuard< _LCK_ >	_Guard(_Lock);
		while(_ThePage)
		{
			_TheNextPage = _ThePage->Next();
			
			//清除
			_ThePage->Next(NULL);
			//_ThePage->Length(0); //长度置零为了效率取消
			
			_List.Put2Head(*_ThePage);		
			_ThePage = _TheNextPage; 
		};
	}
};
	

static void __MQCarrier_F(PSTR _Dest,PCSTR _Src,UINT _Length)
{
	if(ISNULL(_Dest)||ISNULL(_Src))
	{
		THROW_MQNULLEXCEPTION("_Dest||_Src");	
		return;
	}

#define _AMQCOPY4_
	
#if defined(_AMQCOPY4_)

	UINT *_iD = (UINT *)_Dest; 
	UINT *_iS = (UINT *)_Src; 
	
	while(_Length > 3)
	{
		*_iD++ = ~(*_iS++);
		_Length -= sizeof(UINT);
		
		__asm__ __volatile__("nop;"); 
	}
	
	USHORT *_SD = (USHORT *)_iD; 
	USHORT *_SS = (USHORT *)_iS; 
	
	if(_Length > 1)
	{
		*_SD++ = (~(*_SS++)) & 0xFFFF;
		_Length -= sizeof(USHORT); 
	}

	if(_Length > 0)
	{
		*((PBYTE)_SD) = (~(*((PBYTE)_SS))) & 0xFF;
	}

#else
	USHORT *_SD = (USHORT *)_Dest; 
	USHORT *_SS = (USHORT *)_Src; 

	while(_Length > 1)
	{
		*_SD++ = (~(*_SS++))&0xFFFF;
		_Length -= sizeof(USHORT); 
		__asm__ __volatile__("nop;");
	}
	
	if(_Length > 0)
	{
		*((PBYTE)_SD) = (~(*((PBYTE)_SS)))&0xFF;
	}
#endif
}


/*CSQPage*/
void CSQPage::Init(USHORT _PageSize)
{
	_Next = 0L;
	_Length = (((UINT)_PageSize) << 16) & 0xFFFF0000;
}

void CSQPage::Next(const CSQPage *_Page) const
{
	CSQPage *_This = (CSQPage *)this;
	if(ISNULL(_Page))
	{
		_This->_Next = 0L;		
	}
	else
	{
		_This->_Next = ((PBYTE)_Page) - ((PBYTE)this);
	}
}

const CSQPage *CSQPage::Next() const
{
	if(_Next == 0L)
	{
		return NULL;	
	}
	else
	{
		return (CSQPage *)(((PBYTE)this) + _Next);
	}
}

inline UINT F_MKLength(UINT _ThePageSize,UINT _TheDataLen)
{
	return (_ThePageSize & 0xFFFF0000) | (_TheDataLen & 0x0000FFFF);	
}

void CSQPage::Length(USHORT _TheLength) const
{
	CSQPage *_This = (CSQPage *)this;
	_This->_Length = F_MKLength(_Length,(UINT)_TheLength);
}

USHORT CSQPage::Length() const
{
	return (USHORT)(_Length & 0x0000FFFF);	
}

void CSQPage::PageSize(USHORT _PageSize) const
{
	CSQPage *_This = (CSQPage *)this;
	_This->_Length = F_MKLength((UINT)_PageSize,_Length);	
}

USHORT CSQPage::PageSize() const
{
	return (USHORT)((_Length >> 16) & 0x0000FFFF);		
}

const PBYTE CSQPage::Data() const
{
	return (const PBYTE)_Data;	
}

PBYTE CSQPage::NearPtr() const
{
	return ((PBYTE)this)+ (sizeof(*this)+PageSize());	
}


USHORT CSQPage::Write(const PSTR _Buffer,UINT _DataLen) const
{
	USHORT _Result = PageSize();
	_Result = (_Result > _DataLen) ? _DataLen : _Result; 
	__MQCarrier_F((PSTR)this->_Data,_Buffer,_Result);
	//memcpy((PSTR)this->_Data,_Buffer,_Result);
	Length(_Result);

	return _Result;
}

USHORT CSQPage::Read(PSTR _Buffer,UINT _MaxLen) const
{
	USHORT _Result = Length();
	_Result = (_Result > _MaxLen) ? _MaxLen : _Result; 
	__MQCarrier_F(_Buffer,(PSTR)this->_Data,_Result);	
	//memcpy(_Buffer,(PSTR)this->_Data,_Result);
	//printf("Read Len = %d\n",_Result);
	return _Result;
}


/*CSQPageArray*/
void CSQPageArray::Init(UINT _TheTotal,USHORT _ThePageSize)
{
	_Total 		= _TheTotal;
	_PageSize 	= _ThePageSize;
	

	{
		CSQPage *_ThePage = NULL;

		for(UINT i=0;i<this->_Total;i++)
		{
			_ThePage = (CSQPage *) this->operator[](i);	

			if(_ThePage!=NULL)
			{
				_ThePage->Init(_PageSize);	
				//printf("%d -- this:0x%X;Total:%d;PageSize:%d;Page:0x%X\n",i,this,_Total,_PageSize,_ThePage);
			}
		}
	}
	
	{
		PBYTE _TheMask = (PBYTE)&_MSK; //.PGS
		_TheMask[0] = 0x2E;
		_TheMask[1] = 0x50;
		_TheMask[2] = 0x47;
		_TheMask[3] = 0x53;
	}	 	
}


bool CSQPageArray::Check() const
{
	{
		const PBYTE _TheMask = (const PBYTE)&_MSK;
		return (_TheMask[0] == 0x2E) && (_TheMask[1] == 0x50) && (_TheMask[2] == 0x47) && (_TheMask[3] == 0x53);
	}
}


PBYTE CSQPageArray::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(CSQPageArray);	
}

UINT CSQPageArray::Count() const
{
	return _Total;	
}

USHORT CSQPageArray::PageSize() const
{
	return (USHORT)(_PageSize & 0x0000FFFF);	
}

const CSQPage *CSQPageArray::operator[](UINT _Index) const
{
	CSQPage *_ThePage = NULL;
	if(_Index < this->_Total)
	{
		PBYTE	_PageEntry = NearPtr();
		_ThePage = (CSQPage *) &_PageEntry[((sizeof(CSQPage)+_PageSize) * _Index)];
	}

	return _ThePage;
}

/*CSQAllocator*/
void CSQAllocator::Init(UINT _Pages,USHORT _PageSize)
{
	
	_Lock.Init();
	_Array.Init(_Pages,_PageSize);	
	_List.Init();
	
	{
		CMQLockGuard< CSQLock >	_Guard(_Lock);
		
		const CSQPage *_ThePage = NULL;
		for(UINT i=0;i<Pages();i++)
		{
			_ThePage = _Array[i];
			if(_ThePage)
			{
				_List.Put2Tail(*_ThePage);		
			}	
		}
		
	}
	
	if(Pages()!=UsablePages())
	{
		THROW_MQEXCEPTION("Init failed!");		
	}
	
	{
		PBYTE _TheMask = (PBYTE)&_MSK; //.ALC
		_TheMask[0] = 0x2E;
		_TheMask[1] = 0x41;
		_TheMask[2] = 0x4C;
		_TheMask[3] = 0x43;
	}
	
}

bool CSQAllocator::Check() const
{
	{
		const PBYTE _TheMask = (const PBYTE)&_MSK;
		return _Array.Check() && (_TheMask[0] == 0x2E) && (_TheMask[1] == 0x41) && (_TheMask[2] == 0x4C) && (_TheMask[3] == 0x43);
	}
}


PBYTE CSQAllocator::NearPtr() const
{
	return ((PBYTE)this)+ sizeof(*this);	
}

UINT CSQAllocator::UsablePages() const
{
	return _List.Count();	
}

UINT CSQAllocator::Pages() const
{
	return _Array.Count();	
}

UINT CSQAllocator::PageSize() const
{
	return _Array.PageSize();	
}

const CSQPage *CSQAllocator::Allocate(UINT _Size) const
{
	const CSQPage *_ThePage = NULL;
	UINT _PageSize = _Array.PageSize();
	
	UINT _nPages = 0;

	{
		_nPages = _Size / _PageSize;
		if(	(_nPages * _PageSize) < _Size)
		{
			_nPages+=1;	
		}
	}
	
	{
		CMQLockGuard< CSQLock >	_Guard(_Lock);	
		if(_List.Count() >= _nPages)
		{
			const CSQPage *_TheNextPage = NULL;
			for(UINT i=0;i<_nPages;++i)
			{
				_ThePage = _List.Get();
				if(_ThePage)
				{
					if(_ThePage->PageSize()!=_PageSize)
					{
						_ThePage->PageSize(_PageSize);	
					}
					
					_ThePage->Next(_TheNextPage);
					_TheNextPage =_ThePage; 
				}
				else //要释放
				{
					CMQNoLock	_NoLock; //为什么要用这个，应为这个函数在锁内，为了适应模板函数，要用假锁
					T_Free< CSQPage,CSQPageList_T,CMQNoLock >(*_TheNextPage,_List,_NoLock);
					_ThePage = NULL;
					break;			
				}	
			}
		}
	}
	
	return _ThePage;
}

void CSQAllocator::Free(const CSQPage &_TheMultiPage) const
{
	T_Free< CSQPage,CSQPageList_T,CSQLock >(_TheMultiPage,_List,_Lock);
}

void CSQAllocator::_NurseryPut(const CSQPage &_TheMultiPage) const
{
	Free(_TheMultiPage);	
}

UINT EvaluateAllocator(UINT _Pages,USHORT _PageSize)
{
	UINT _Result = sizeof(CSQAllocator)+((sizeof(CSQPage)+_PageSize) * _Pages);
	return _Result;
}

};

};
