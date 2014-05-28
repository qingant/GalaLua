#ifndef MQT_HPP_1C735308_DB77_441E_B1E2_577AE7A8CCCC
#define MQT_HPP_1C735308_DB77_441E_B1E2_577AE7A8CCCC


#pragma once

#include "GalaxyDT.hpp"

namespace Galaxy
{

namespace AMQ
{

template < class _T1 > bool T_MQEquPtr(const _T1 &_R1,const _T1 &_R2)
{
	return 	(&_R1 == &_R2);
};

/*template*/
template <typename _T2> inline _T2 T_MQPtrCast(PVOID _the_ptr)
{
	return ((_T2)_the_ptr);
};



template < class _T_ > class CMQLockGuard
{
	typedef CMQLockGuard< _T_ > CMQLockGuard_T;
private:
	const _T_ &_Lock;  
 	static void* operator new (size_t); 
 	
 	CMQLockGuard(const CMQLockGuard_T &);
 	const CMQLockGuard_T &operator=(const CMQLockGuard_T &); 
public:
	explicit CMQLockGuard(const _T_ &_TheLock)
		:_Lock(_TheLock)
	{
		_Lock.Lock();	
	};
	
	~CMQLockGuard()
	{
		_Lock.UnLock();	
	};
};


template <class __Container_T,class __Member_T> class CMQNursery
{
private:
	const __Container_T				&_Container;
	const __Member_T				*_Member;
	

	static void* operator new (size_t);
	CMQNursery();
	explicit CMQNursery(const CMQNursery &);
	const CMQNursery &operator=(const CMQNursery &);
public:
	explicit CMQNursery(const __Container_T &_TheContainer)
		:_Container(_TheContainer),_Member(NULL)
	{
		
	};
	
	~CMQNursery()
	{
		if(_Member!=NULL)
		{
			_Container._NurseryPut(*_Member);
			_Member = NULL;
		}		
	};
	
	void Cancel()
	{
		_Member = NULL;
	};
	
	void Set(const __Member_T &_TheMember)
	{
		if(_Member != &_TheMember)
		{		
			if(_Member!=NULL)
			{
				_Container._NurseryPut(*_Member);	
			}
			
			_Member = &_TheMember;	
		}
	} 

	void SwapFrom(const __Container_T &_OtherContainer)
	{
		if((!T_MQEquPtr <__Container_T>(_Container,_OtherContainer)) && (_OtherContainer._Member!=NULL))
		{
			_Member = _OtherContainer._Member;
			_OtherContainer._Member = NULL;
		}			
	}
	
	void SwapTo(const __Container_T &_OtherContainer)
	{
		if( (!T_MQEquPtr <__Container_T>(_Container,_OtherContainer)) && \
			(_Member!=NULL))
		{
			_OtherContainer._Member = _Member;
			_Member = NULL;
		}			
	}
};


};

};

#endif /*MQT_HPP_1C735308_DB77_441E_B1E2_577AE7A8CCCC*/
