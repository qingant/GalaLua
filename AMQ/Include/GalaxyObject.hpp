// GalaxyObject.h: interface for the CGalaxyObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GALAXYOBJECT_H__01721757_8554_4BC0_A005_CCDCBEA7A4A1__INCLUDED_)
#define AFX_GALAXYOBJECT_H__01721757_8554_4BC0_A005_CCDCBEA7A4A1__INCLUDED_

#pragma once

#include<GalaxyDT.hpp>


class CGalaxyObject
{
public:
	virtual ~CGalaxyObject()
	{

	}
};


template <class __T__> class CNursery : public CGalaxyObject
{
private:
	__T__ *_TheBaby;

	static void* operator new (size_t);
	CNursery( const CNursery<__T__> &);
    const CNursery<__T__>& operator=( const CNursery<__T__>&);
	CNursery();
public:
	explicit CNursery(__T__ *_Baby)
		:_TheBaby(_Baby)
	{

	}
	
	~CNursery()
	{
		if(_TheBaby!=NULL)
		{
			delete _TheBaby;
			_TheBaby = NULL;
		}
		
	}

	void Detach()
	{
		_TheBaby = NULL;
	}
 };

#endif // !defined(AFX_GALAXYOBJECT_H__01721757_8554_4BC0_A005_CCDCBEA7A4A1__INCLUDED_)
