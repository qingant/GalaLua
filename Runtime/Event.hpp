/* -*- C++ -*- */

/* Time-stamp: <2012-07-12 22:15:27 Thursday by qingant> */

/**
 * @file CEvent.h
 * @author qingant
 */

#ifndef CEVENT_H
#define CEVENT_H

#include "Object.hpp"
#include "Exception.hpp"
#include "Sync.hpp"
#include "Pipe.hpp"
#include "Thread.hpp"
#include "sysinc.h"

#include <cstub.h>
#include <vector>
#include <string>
#include <map>


#pragma once

//#define THROW_SYSERROR() (throw CSysError(__FILE__, __PRETTY_FUNCTION__, __LINE__) )

namespace Galaxy
{
   namespace GalaxyRT
   {
      enum EVENTTYPE
      {
         EV_IN = 1,                  // available for read
         EV_OUT = (1<<1),                 // available for write
         EV_RDHUP = (1<<2),               // stream socket peer closed connection, or shut down writing half of connection.POLL/EPOLL only.
         EV_PRI = (1<<3),                 // urgent data available for read
         EV_ERR = (1<<4),                 // Error condition happened on the associated file descriptor.
         EV_HUP = (1<<5),                 // Hang up happened on the associated file descriptor. POLL/EPOLL always wait for it.
         EV_ET = (1<<6),                  // Sets  the  Edge Triggered behaviour for the associated file descriptor. EPOLL only
         EV_ONESHOT = (1<<7),              // EPOLL only.
         EV_NVALID = (1<<8),                // fd not valid

         EV_TIMEOUT = (1<<9)               // added for indicate **SELECT/POLL/EPOLL TIMEOUT**
      };
      
      class CEpollNotExistFD: public CException
      {
      public:
         CEpollNotExistFD(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
			:CException(file, func, line, errcode, control, msg){}
         ~CEpollNotExistFD() throw(){}
         std::string GetClassName() const
         {
			return std::string("CEpollNotExistFD");
         }
      };
      
      class CBaseSelector: public CNonCopyAble
      {
      public:

         typedef UINT EV_TYPE;
         typedef std::pair<INT, EV_TYPE> EV_PAIR;
         typedef std::vector<EV_PAIR>  EV_PAIRS;
      public:
         virtual ~CBaseSelector();
         virtual void Register(INT fd, EV_TYPE type)  = 0;
         virtual void Register(const EV_PAIR &ev);
         virtual void Remove(INT fd, EV_TYPE type)  = 0;
         virtual void Remove(const EV_PAIR &ev);
         virtual void Remove(INT fd) = 0;

         virtual EV_PAIRS WaitEvent(SHORT timeout)  = 0;
      };



      class CSelector:public CBaseSelector
      {
      public:
         //enum  EVENTTYPE {SELECT_READ, SELECT_WRITE, SELECT_EXCEPT};
         //typedef std::vector<std::pair<INT, EVENTTYPE> > FDPAIRS;
          using CBaseSelector::Remove;
          using CBaseSelector::Register;

      public:
         CSelector();
         ~CSelector();
         void Register(INT fd, EV_TYPE type) ;
         // void Register(const EV_PAIR &ev) ;

         void Remove(INT fd, EV_TYPE type);
         void Remove(INT);

         EV_PAIRS WaitEvent(SHORT timeout);

      private:
         INT m_nfds;
         fd_set m_rset, m_wset, m_eset;
         std::list<INT> m_rfds, m_wfds, m_efds;
      };


      typedef struct pollfd POLLEVENT;
      class CPollSelector:public CBaseSelector
      {
      public:
         using CBaseSelector::Register;
         using CBaseSelector::Remove;
         CPollSelector(size_t capacity = 8);
         ~CPollSelector();
         void Register(const POLLEVENT &ev);
         void Register(INT fd, EV_TYPE event);
         void Remove(const POLLEVENT &ev);
         void Remove(INT fd, EV_TYPE event);
         void Remove(INT fd);

         CBaseSelector::EV_PAIRS WaitEvent(SHORT timeout);
      private:
         void _Notify();
      private:
         std::vector<POLLEVENT> m_events;
         CPthreadMutex          _Mutex;
         CSocketPair            _Pair;
      };


#if defined(__linux__)
      typedef struct epoll_event EPOLLEVENT;

      class CEpollSelector:public CBaseSelector
      {
      public:
         using CBaseSelector::Register;
         using CBaseSelector::Remove;
         CEpollSelector(INT size = 256, INT bufSize = 500);
         ~CEpollSelector();
         void Register(EPOLLEVENT& ev);
         void Register(INT, EV_TYPE);
         /** 
          * 删除fd上关联的事件ev，如果fd不在epoll的监视中，抛出CEpollNotExistFD exception
          * 
          * @param fd 
          * @param ev  
          */
         void Remove(INT fd, EV_TYPE ev);
         void Remove(INT);
         CBaseSelector::EV_PAIRS WaitEvent(SHORT timeout = -1);
      private:
         void UpdateEvents( EPOLLEVENT ev );
      private:
         INT EpollFd;
         UINT EventNo;
         mutable std::vector<EPOLLEVENT> EventsBuf;
         UINT EventBufferSize;
         CPthreadMutex _Mutex;
         std::map<int, UINT> _fd2events;
      };

#endif

#if defined(_AIX)
//#include <sys/pollset.h>

      typedef struct poll_ctl POLLSETEVENT;
      class CPollSetSelector:public CBaseSelector
      {
      public:
         CPollSetSelector(INT size = 256, INT bufSize = 20);
         ~CPollSetSelector();
         void Register(POLLSETEVENT *ev,INT ArrayLen);
         void Register(INT, EV_TYPE);
         void Remove(INT, EV_TYPE);
         void Remove(INT);
         CBaseSelector::EV_PAIRS WaitEvent(SHORT timeout = -1);
      private:
         pollset_t  PSFd;
         UINT EventNo;
         mutable std::vector<POLLEVENT> EventsBuf;
      };
#endif

      class CPollDispatcher:public CNonCopyAble
      {
      private:
         CPthreadMutex           _MutexWait;
         CPthreadMutex           _MutexRegister;
         std::vector<POLLEVENT>  _Events;
      public:
         CPollDispatcher(size_t capacity = 8);
         virtual ~CPollDispatcher();
         void Register(INT fd, SHORT events);
         bool Remove(INT fd, SHORT events);

         void Register(const POLLEVENT &ev);
         bool Remove(const POLLEVENT &ev);
         void Dispatch(SHORT timeout, CThread * _t = NULL);
      protected:
         virtual void OnEvent(INT fd, SHORT event, CThread*) = 0;

      private:
         SHORT EventCorrective(SHORT events);
         bool WaitEvent(SHORT timeout,POLLEVENT *rtn);
      };
      
   } /// GalaxyRT
} /// Galaxy

#endif /* CEVENT_H */
