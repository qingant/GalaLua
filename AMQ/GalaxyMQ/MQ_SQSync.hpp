#ifndef SQSYNC_HPP_9C3B5956_AE1F_47FB_BB6A_DC4154565288
#define SQSYNC_HPP_9C3B5956_AE1F_47FB_BB6A_DC4154565288


#pragma once

#include <pthread.h>
#include "GalaxyDT.hpp"

namespace Galaxy
{

namespace AMQ
{
	
class CMQNoLock
{
public:
	void Lock() const;
	void UnLock() const;		
};

class CSQLock
{
private:
	__volatile__ UINT		_STA;
	pthread_mutex_t			_MTX;
public:
	void Init();
	pthread_mutex_t *GetMutex() const;
	
	PBYTE NearPtr() const;	
	
	bool Locked() const;
	void Lock() const;
	void UnLock() const;
	bool TryLock() const;
	bool TimedLock(SHORT timeout) const;
		
};

class CSQCond
{
private:
	__volatile__ UINT		_WAT;
	pthread_cond_t			_CND;
	__volatile__ INT		_LCK;
	
	const CSQLock &Locker() const;						
public:
	void Init(CSQLock &_TheLocker);
	PBYTE NearPtr() const;	
	
	UINT Lurkers() const;
	void Wait() const;
	void NotifyOne() const;
	void NotifyAll() const;
	bool TimedWait(SHORT timeout) const;
};

class CSQEvent
{
private:
	CSQCond					_Cond;
	CSQLock					_Lock;
public:
	void Init();
	PBYTE NearPtr() const;

	void Lock() const;
	void UnLock() const;
	bool TryLock() const;
	bool TimedLock(SHORT timeout) const;
	

	UINT Lurkers() const;
	void Wait() const;
	void NotifyOne() const;
	void NotifyAll() const;
	bool TimedWait(SHORT timeout) const;
};

class CSQDoor
{
private:
	CSQCond					_Threshold1;
	CSQCond					_Threshold2;
	CSQLock					_Lock;	
public:	
	void Init();
	PBYTE NearPtr() const;

	void Lock() const;
	void UnLock() const;
	bool TryLock() const;
	bool TimedLock(SHORT timeout) const;
	
	const CSQCond &Threshold1() const;
	const CSQCond &Threshold2() const;
};


};

};

#endif /*SQSYNC_HPP_9C3B5956_AE1F_47FB_BB6A_DC4154565288*/
