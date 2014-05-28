#ifndef SQLIST_HPP_7E6E59AB_FB5C_4A9C_9382_268437A6DF77
#define SQLIST_HPP_7E6E59AB_FB5C_4A9C_9382_268437A6DF77


#pragma once

#include "GalaxyDT.hpp"

namespace Galaxy
{

namespace AMQ
{

template < class _T_ > class CSQList
{
	typedef CSQList< _T_ > CSQList_T;
private:
	__volatile__ UINT	_Total;
	__volatile__ INT	_First;
	__volatile__ INT	_Last;
	
	_T_ *First() const
	{
		if(_First == 0L)
		{
			return NULL;	
		}
		else
		{
			return (_T_ *)(((PBYTE)this) + _First);	
		}
	};
	
	void First(const _T_ *_TheNode) const
	{
		CSQList_T *_This = (CSQList_T *)this;
		if(ISNULL(_TheNode))
		{
			_This->_First = 0L;		
		}
		else
		{
			_This->_First = ((PBYTE)_TheNode) - ((PBYTE)this);
		}
	};
	
	_T_ *Last() const
	{
		if(_Last == 0L)
		{
			return NULL;	
		}
		else
		{
			return (_T_ *)(((PBYTE)this) + _Last);	
		}		
	};
	
	void Last(const _T_ *_TheNode) const
	{
		CSQList_T *_This = (CSQList_T *)this;
		if(ISNULL(_TheNode))
		{
			_This->_Last = 0L;		
		}
		else
		{
			_This->_Last = ((PBYTE)_TheNode) - ((PBYTE)this);
		}	
	};
public:
	void Init() const
	{
		CSQList_T *_This = (CSQList_T *)this;	
		_This->_Total = 0U;
		_This->_First = _This->_Last = 0L;	
	};

	UINT Count() const
	{
		return _Total;	
	};
	
	PBYTE NearPtr() const
	{
		return ((PBYTE)this)+ sizeof(*this);	
	};
	
  	const _T_ *Get() const
  	{
		_T_ *_TheNode = NULL;
		
		CSQList_T *_This = (CSQList_T *)this;
		if(_Total > 0)
		{
			_TheNode = First();
			if(_First  ==  _Last)
			{
				_This->_First = _This->_Last = 0L;
			}
			else
			{
				First(_TheNode->Next());	
			}
			
			_TheNode->Next(NULL);
			
			_This->_Total--;
		}
		
		return _TheNode; 	
  	};

   	void Put2Head(const _T_ &_TheNode) const
   	{
	   CSQList_T *_This = (CSQList_T *)this;
	   
	   _TheNode.Next(First());
	    
	   if(_First == 0L)
	   {
	      _This->Last(&_TheNode);
	   }
	   
		_This->First(&_TheNode);
	
	   _This->_Total++; 	
   	};
   	
   	void Put2Tail(const _T_ &_TheNode) const
   	{
		CSQList_T *_This = (CSQList_T *)this;
		_TheNode.Next(NULL);
		
		if(_First == 0L)
		{
			_This->First(&_TheNode);
		}
		
		_T_ *_Tail = Last();
		if(_Tail)
		{
			_Tail->Next(&_TheNode);	
		}
		
		Last(&_TheNode);
		_This->_Total++;	
   	};	
};


};

};







#endif /*SQLIST_HPP_7E6E59AB_FB5C_4A9C_9382_268437A6DF77*/
