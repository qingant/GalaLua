/* -*- C++ -*- */
// Time-stamp: <2012-11-05 10:57:13 星期一 by matao>
/**
 * @file    Event.cpp
 * @author  Ma Tao <qingant@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 *
 * @section DESCRIPTION
 *
 *
 */

#include "Event.hpp"
#include "prettyprint98.hpp"
#include "cstub.h"

namespace Galaxy
{
   namespace GalaxyRT
   {
#ifdef __linux__
      static uint32_t events2ievents( CBaseSelector::EV_TYPE ev )
      {
         uint32_t iev;

         if (ev&EV_IN)
         {
            iev |= EPOLLIN;
         }
         if (ev&EV_OUT)
         {
            iev |= EPOLLOUT;
         }
#ifdef EPOLLRDHUP
         if (ev&EV_RDHUP)
         {
            iev |= EPOLLRDHUP;
         }
#endif
         if (ev&EV_PRI)
         {
            iev |= EPOLLPRI;
         }
         if (ev&EV_ET)
         {
            iev |= EPOLLET;
         }
         if (ev&EV_ONESHOT)
         {
            iev |= EPOLLONESHOT;
         }

         return ev;
      }

      static CBaseSelector::EV_TYPE ievents2events( uint32_t  iev )
      {
         CBaseSelector::EV_TYPE ev;
         
         if (iev&EPOLLIN)
         {
            ev |= EV_IN;
         }
         if (iev&EPOLLOUT)
         {
            ev |= EV_OUT;
         }
#ifdef EPOLLRDHUP
         if (iev&EPOLLRDHUP)
         {
            ev |= EV_RDHUP;
         }
#endif
         if (iev&EPOLLPRI)
         {
            ev |= EV_PRI;
         }
         if (iev&EPOLLERR)
         {
            ev |= EV_ERR;
         }
         if (iev&EPOLLHUP)
         {
            ev |= EV_HUP;
         }

         return ev;
      }

      /** 
       * events转换成对应的字符串，方便debug
       * 
       * @param ev 需要转换的事件
       * 
       * @return 
       */
      static std::string events2string( CBaseSelector::EV_TYPE ev )
      {
         std::string str;
         
         if (ev&EV_IN)
         {
            str += "EV_IN ";
         }
         if (ev&EV_OUT)
         {
            str += "EV_OUT ";
         }
#ifdef EPOLLRDHUP
         if (ev&EV_RDHUP)
         {
            str += "EV_RDHUP ";
         }
#endif
         if (ev&EV_PRI)
         {
            str += "EV_PRI ";
         }
         if (ev&EV_ET)
         {
            str += "EV_ET ";
         }
         if (ev&EV_ONESHOT)
         {
            str += "EV_ONESHOT ";
         }

         return str;
      }

      /** 
       * ievents转换成对应的字符串，方便debug
       * 
       * @param iev 需要转换的事件
       * 
       * @return 
       */
      static std::string ievents2string( uint32_t iev )
      {
         std::string str;
         
         if (iev&EPOLLIN)
         {
            str += "EPOLLIN ";
         }
         if (iev&EPOLLOUT)
         {
            str += "EPOLLOUT ";
         }
#ifdef EPOLLRDHUP
         if (iev&EPOLLRDHUP)
         {
            str += "EPOLLRDHUP ";
         }
#endif
         
         if (iev&EPOLLPRI)
         {
            str += "EPOLLPRI ";
         }
         if (iev&EPOLLERR)
         {
            str += "EPOLLERR ";
         }
         if (iev&EPOLLHUP)
         {
            str += "EPOLLHUP ";
         }

         return str;
      }
#endif
      CBaseSelector::~CBaseSelector()
      {

      }
      void CBaseSelector::Register(const EV_PAIR &ev)
      {
         Register(ev.first, ev.second);
      }
      void CBaseSelector::Remove(const EV_PAIR &ev)
      {
         Remove(ev.first, ev.second);
      }

      CSelector::CSelector():m_nfds(0)
      {
         FD_ZERO(&m_rset);
         FD_ZERO(&m_wset);
         FD_ZERO(&m_eset);
      }

      void CSelector::Register(INT fd, CBaseSelector::EV_TYPE type)
      {
         if (type == EV_IN)
         {
            FD_SET(fd, &m_rset);
            m_rfds.push_back(fd);
         }
         else if(type == EV_OUT)
         {
            FD_SET(fd, &m_wset);
            m_wfds.push_back(fd);
         }
         else if(type == EV_ERR)
         {
            FD_SET(fd, &m_eset);
            m_efds.push_back(fd);
         }
         else
         {
            THROW_GENERALERROR(CException, "Event type not supported");
         }
      }


      void CSelector::Remove(INT fd)
      {
         m_rfds.remove(fd);
         m_wfds.remove(fd);
         m_efds.remove(fd);
      }


      void CSelector::Remove(INT fd, CSelector::EV_TYPE type)
      {
         if (type == EV_IN)
         {
            m_rfds.remove(fd);
         }
         else if (type == EV_OUT)
         {
            m_wfds.remove(fd);
         }
         else if (type == EV_ERR)
         {
            m_efds.remove(fd);
         }
         else
         {
            THROW_GENERALERROR(CException, "Event type not supported");
         }

      }

//std::vector<INT, CSelector::EVENTTYPE>
//CSelector::FDPAIRS CSelector::WaitEvent(const struct timeval &timeout)
      CBaseSelector::EV_PAIRS CSelector::WaitEvent(SHORT timeout)
      {
         struct timeval timeval, *pTMO;
         if (timeout < 0 )
         {
            pTMO = NULL;
         }
         else
         {
            timeval.tv_sec = timeout;
            timeval.tv_usec = 0;
            pTMO = &timeval;
         }
         m_nfds = 0;
         for (std::list<INT>::iterator it = m_rfds.begin(); it != m_rfds.end(); ++it)
         {
            FD_SET(*it, &m_rset);
            if (m_nfds < *it)
            {
               m_nfds = *it;
            }
         }

         for (std::list<INT>::iterator it = m_wfds.begin(); it != m_wfds.end(); ++it)
         {
            FD_SET(*it, &m_wset);
            if (m_nfds < *it)
            {
               m_nfds = *it;
            }
         }

         for (std::list<INT>::iterator it = m_efds.begin(); it != m_efds.end(); ++it)
         {
            FD_SET(*it, &m_eset);
            if (m_nfds < *it)
            {
               m_nfds = *it;
            }
         }

         INT rt = CRT_select(m_nfds+1, &m_rset, &m_wset, &m_eset, pTMO);

         CBaseSelector::EV_PAIRS events;
         events.reserve(rt+1);

         if (rt == -1)
         {
            THROW_SYSERROR();
         }


         else if (rt == 0)
         {
            return events;
         }
         else
         {
            for (std::list<INT>::iterator it = m_rfds.begin(); it != m_rfds.end(); ++it)
            {
               if (FD_ISSET(*it, &m_rset))
               {
                  events.push_back(std::make_pair(*it, (CBaseSelector::EV_TYPE)EV_IN));
               }
            }

            for (std::list<INT>::iterator it = m_wfds.begin(); it != m_wfds.end(); ++it)
            {
               if (FD_ISSET(*it, &m_wset))
               {
                  events.push_back(std::make_pair(*it, (CBaseSelector::EV_TYPE)EV_OUT));

               }
            }

            for (std::list<INT>::iterator it = m_efds.begin(); it != m_efds.end(); ++it)
            {
               if (FD_ISSET(*it, &m_eset))
               {
                  events.push_back(std::make_pair(*it, (CBaseSelector::EV_TYPE)EV_ERR));
               }
            }
         }
         return events;
      }


      CSelector::~CSelector()
      {
      }


// CPollSelector

      CPollSelector::CPollSelector(size_t capacity)
         :m_events(capacity)
      {

         POLLEVENT ev = {-1, EV_IN,};
         std::fill(m_events.begin(), m_events.end(),ev);
         m_events[0].fd = _Pair.GetOne()->GetFD();
         m_events[0].events = POLLIN;
         _Pair.GetAnother()->SetNonBlocking();
         // m_events.reserve(capacity);
      }

      CPollSelector::~CPollSelector()
      {

      }
      void CPollSelector::_Notify()
      {

         char buf[1] = {'a'};

         try
         {
            _Pair.GetAnother()->Send(buf, sizeof(buf));
         }
         catch (CSockWouldBlock &e)
         {
            GALA_DEBUG("Block");
            return;
         }
      
      }
//std::vector<POLLEVENT>
      CBaseSelector::EV_PAIRS CPollSelector::WaitEvent(SHORT timeout)
      {
         //CLockGuard _Gl(&_Mutex);
 
         CBaseSelector::EV_PAIRS evs;
     
        restart:
         std::vector<POLLEVENT> _events;
         {
            CLockGuard _Gl(&_Mutex);
            _events =  m_events;
         }

      

         //std::cout<<_events;
         //std::cout<<"\n";
         INT rt = CRT_poll((POLLEVENT*)&_events[0], _events.size(), timeout);
         GALA_DEBUG("rt: %d", rt);
         // for (int i = 0; i != _events.size(); ++i)
         // {
         //      printf("fd: %d\n", _events[i].fd);
         // }
         // std::cout<<_events;
         // std::cout<<m_events;
         if (rt == -1)
         {
            THROW_SYSERROR();
         }
         else if (rt == 0)
         {
            return evs;
         }

         else
         {
            if ((_events[0].revents & POLLIN))
            {
               char buf[1024];
               this->_Pair.GetOne()->Recv(buf, sizeof(buf));
               if (rt == 1)
               {
                  goto restart;
               }
            }
            for (size_t i = 1 ; i != _events.size(); ++i)
            {
               SHORT old = _events[i].revents, newer = 0;
               if (old != 0)
               {
                   GALA_DEBUG("%d\n", old);
                  if (old&POLLIN && _events[i].fd != -1)
                  {
                     newer |= EV_IN;
                  }
                  if (old&POLLPRI)
                  {
                     newer |= EV_PRI;
                  }
                  if (old&POLLOUT)
                  {
                     newer |= EV_OUT;
                  }
#if  defined(_GNU_SOURCE) && defined(POLLRDHUP)
                  if (old&POLLRDHUP)
                  {
                     newer |= EV_RDHUP;
                  }
#endif
                  if (old&POLLERR)
                  {
                     newer |= EV_ERR;
                  }
                  if (old&POLLHUP)
                  {
                     newer |= EV_HUP;
                  }
                  if (old&POLLNVAL)
                  {
                     newer |= EV_NVALID;
                  }
                  _events[i].revents = 0;
                  evs.push_back(std::make_pair(_events[i].fd, (CBaseSelector::EV_TYPE) newer));
                
               }
            }
         }

         return evs;
      }

// void CPollSelector::Register(const POLLEVENT &ev)
// {
//      CLockGuard _Gl(&_Mutex);
//      m_events.push_back(ev);
//      char buf[1] = {'a'};
//      _Pair.GetAnother()->Send(buf, sizeof(buf));
// }

      void CPollSelector::Register(INT fd, CBaseSelector::EV_TYPE event)
      {


         GALA_DEBUG("Register fd:%d", fd);

         if (fd < 0)
         {
            THROW_GENERALERROR(CException, "INVALID FD");
         }
         SHORT evtmp = 0;
         if (event&EV_IN)
         {
            evtmp |= POLLIN;
         }
         if (event&EV_PRI)
         {
            evtmp |= POLLPRI;
         }
         if (event&EV_OUT)
         {
            evtmp |= POLLOUT;
         }
#if  defined(_GNU_SOURCE) && defined(POLLRDHUP)
         if (event&EV_RDHUP)
         {
            evtmp |= POLLRDHUP;
         }
#endif
         if (event&EV_ERR)
         {
             evtmp |= POLLERR;
         }
         if (event&EV_HUP)
         {
             evtmp |= POLLHUP;
         }
         if (event&EV_NVALID)
         {
             evtmp |= POLLNVAL;
         }
         POLLEVENT ev = {fd, evtmp,};
     
         CLockGuard _Gl(&_Mutex);

         for(std::vector<POLLEVENT>::iterator i = m_events.begin(); i!= m_events.end(); ++i)
         {
            if ( i->fd == fd)
            {
               //*i = ev;
               i->fd = fd;
               i->events = evtmp|i->events;
               _Notify();
               return;
            }
         }
          
         for(std::vector<POLLEVENT>::iterator i = m_events.begin(); i!= m_events.end(); ++i)
         {
            if (i->fd == -1 )
            {
               //*i = ev;
               i->fd = fd;
               i->events = evtmp|i->events;
               _Notify();
               return;
            }
         }
     
         m_events.push_back(ev);
         _Notify();
      }

      void CPollSelector::Remove(INT fd)
      {
         CLockGuard _Gl(&_Mutex);
         for(std::vector<POLLEVENT>::iterator i = m_events.begin(); i!= m_events.end(); ++i)
         {
            if (i->fd == fd)
            {
               // m_events.erase(i);
               i->fd = -1;
               //break;
            }
         }
         _Notify();
      }

      void CPollSelector::Remove(INT fd, CBaseSelector::EV_TYPE event)
      {
         SHORT evtmp = 0;
         if (event&EV_IN)
         {
            evtmp |= POLLIN;
         }
         if (event&EV_PRI)
         {
            evtmp |= POLLPRI;
         }
         if (event&EV_OUT)
         {
            evtmp |= POLLOUT;
         }
#if  defined(_GNU_SOURCE) && defined(POLLRDHUP)
         if (event&EV_RDHUP)
         {
            evtmp |= POLLRDHUP;
         }
#endif
         CLockGuard _Gl(&_Mutex);
         GALA_DEBUG("Remove Event (%d)",evtmp);
         for(std::vector<POLLEVENT>::iterator i = m_events.begin(); i!= m_events.end(); ++i)
         {
             //GALA_DEBUG("Iter %d %d", i->fd, i->events);
            if (i->fd == fd && (i->events & evtmp) != 0)
            {
                //GALA_DEBUG("Iter %d %d", i->fd, i->events);
        

             
               i->events = i->events &~ (SHORT)evtmp;


               //GALA_DEBUG("Iter %d %d", i->fd, i->events);
               //break;
            }
         }
         _Notify();
      }
// void CPollSelector::Remove(const POLLEVENT &ev)
// {
//      Remove(ev.fd, ev.events);
// }


//impl of class CEpollSelector
#if defined(__linux__)

      CEpollSelector::CEpollSelector(INT size, INT bufsize):EpollFd(-1), EventNo(1), EventsBuf(bufsize)
      {
         EpollFd = CRT_epoll_create(size);
         if (EpollFd == -1)
         {
            THROW_SYSERROR();
         }
      }

      void CEpollSelector::Register(EPOLLEVENT &ev)
      {
         CLockGuard _GL( &_Mutex );
         INT rt = CRT_epoll_ctl(EpollFd,EPOLL_CTL_ADD, ev.data.fd, &ev);
         ++EventNo;
         if (rt == -1)
         {
            rt = CRT_epoll_ctl(EpollFd,EPOLL_CTL_MOD, ev.data.fd, &ev);
            if (rt == -1)
               THROW_SYSERROR();
         }
         
         if( _fd2events.find( ev.data.fd ) != _fd2events.end( ) )
         {
            _fd2events[ ev.data.fd ] |= ev.events;   
         }
         else
         {
            _fd2events[ ev.data.fd ] = ev.events;
         }
      }

      void CEpollSelector::Register(INT fd, CBaseSelector::EV_TYPE ev)
      {
         epoll_event event = {0};
         
         if (fd < 0)
         {
            THROW_GENERALERROR(CException, "INVALID FD");
         }
         event.data.fd = fd;

         if (ev&EV_IN)
         {
            event.events |= EPOLLIN;
         }
         if (ev&EV_OUT)
         {
            event.events |= EPOLLOUT;
         }
#ifdef EPOLLRDHUP
         if (ev&EV_RDHUP)
         {
            event.events |= EPOLLRDHUP;
         }
#endif
         if (ev&EV_PRI)
         {
            event.events |= EPOLLPRI;
         }
         if (ev&EV_ET)
         {
            event.events |= EPOLLET;
         }
         if (ev&EV_ONESHOT)
         {
            event.events |= EPOLLONESHOT;
         }
     
         //event.events = ev;
         return Register(event);
      }


      CBaseSelector::EV_PAIRS CEpollSelector::WaitEvent(SHORT timeout)
      {
         //epoll_event *events = new epoll_event[EventNo];
         //EventsBuf.resize();
         EPOLLEVENT *events = &EventsBuf[0];
         INT nfds = CRT_epoll_wait(EpollFd, events, EventsBuf.size(), timeout);
         CBaseSelector::EV_PAIRS ev_wait;
         if (nfds == -1)
         {
            THROW_SYSERROR();
         }
         else if (nfds == 0)
         {
            return ev_wait;
         }
         else
         {

            for (INT i = 0; i != nfds; ++i)
            {
               CBaseSelector::EV_TYPE ev = 0, old = events[i].events;
               if (old&EPOLLIN)
               {
                  ev |= EV_IN;
               }
               if (old&EPOLLOUT)
               {
                  ev |= EV_OUT;
               }
#ifdef EPOLLRDHUP
               if (old&EPOLLRDHUP)
               {
                  ev |= EV_RDHUP;
               }
#endif
               if (old&EPOLLPRI)
               {
                  ev |= EV_PRI;
               }
               if (old&EPOLLERR)
               {
                  ev |= EV_ERR;
               }
               if (old&EPOLLHUP)
               {
                  ev |= EV_HUP;
               }

               ev_wait.push_back(std::make_pair((INT)events[i].data.fd, ev));
            }
         }
         
         return ev_wait;
      }

      void CEpollSelector::Remove(INT fd, CBaseSelector::EV_TYPE evi)
      {
         EPOLLEVENT ev = {0};
         
         if (evi&EV_IN)
         {
            ev.events |= EPOLLIN;
         }
         if (evi&EV_OUT)
         {
            ev.events |= EPOLLOUT;
         }
#ifdef EPOLLRDHUP
         if (evi&EV_RDHUP)
         {
            ev.events |= EPOLLRDHUP;
         }
#endif
         if (evi&EV_PRI)
         {
            ev.events |= EPOLLPRI;
         }
         if (evi&EV_ET)
         {
            ev.events |= EPOLLET;
         }
         if (evi&EV_ONESHOT)
         {
            ev.events |= EPOLLONESHOT;
         }

         GALA_DEBUG( "Remove events: %s\n", ievents2string( ev.events ).c_str( ));

         CLockGuard _GL( &_Mutex );
         ev.data.fd = fd;
         UINT old;
         if( _fd2events.find( fd ) != _fd2events.end( ) )
         {
            old = _fd2events[ fd ];
            old &= ~ev.events;
            ev.events = old;
         }
         else
         {
            //throw exception not exist fd
            THROW_GENERALERROR( CEpollNotExistFD, "delete events from fd, but fd is not exist" );
         }
         
         GALA_DEBUG( "delete fd=%d from epoll fd = %d", fd, EpollFd );
         INT rt = CRT_epoll_ctl(EpollFd,EPOLL_CTL_MOD, fd, &ev);
         if (rt == -1)
         {
            THROW_SYSERROR();
         }
         --EventNo;
         //update the events state
         _fd2events[ fd ] = old;
      }

      void CEpollSelector::Remove(INT fd)
      {
         CLockGuard _GL( &_Mutex );
         EPOLLEVENT ev = {0};
         ev.events = EPOLLIN;
         ev.data.fd = fd;
         //ev == NULL maybe bug,system unknow error 1024         
         INT rt = CRT_epoll_ctl(EpollFd, EPOLL_CTL_DEL, fd, &ev);
         --EventNo;

         if (rt == -1)
         {
            //return;
            perror( "failire to epoll_ctl\n");
            THROW_SYSERROR();
         }
         //erase by key
         _fd2events.erase( fd );
      }

      CEpollSelector::~CEpollSelector()
      {
         CRT_close(EpollFd);
      }



#endif

#if defined(_AIX)

      CPollSetSelector::CPollSetSelector(INT size ,INT bufSize):PSFd(-1),EventNo(1),EventsBuf(bufSize)
      {

         PSFd = CRT_pollset_create(size);
         if(PSFd == -1)
         {
            THROW_SYSERROR();
         }

      }

      void CPollSetSelector::Register(POLLSETEVENT *ev ,INT ArrayLen)
      {
         INT rt = CRT_pollset_ctl(PSFd,ev,ArrayLen);
         if (rt == -1)
         {
            THROW_SYSERROR();
         }
         this->EventNo += ArrayLen;
      }

      void CPollSetSelector::Register(INT fd, CBaseSelector::EV_TYPE ev)
      {
         struct poll_ctl event = {0};
         if (fd < 0)
         {
            THROW_GENERALERROR(CException, "INVALID FD");
         }
         event.fd = fd;
         event.cmd = PS_MOD;
         if (ev&EV_IN)
         {
            event.events |= POLLIN;
         }
         if (ev&EV_OUT)
         {
            event.events |= POLLOUT;
         }
#if  defined(POLLRDHUP)
         if (ev&EV_RDHUP)
         {
            event.events |= POLLRDHUP;
         }
#endif
         if (ev&EV_PRI)
         {
            event.events |= POLLPRI;
         }
         if (ev&EV_NVALID)
         {
            event.events |= POLLNVAL;
         }
         return Register(&event,1);
      }
      void CPollSetSelector::Remove(INT fd, CBaseSelector::EV_TYPE ev)
      {

         if (fd < 0)
         {
            THROW_GENERALERROR(CException, "INVALID FD");
         }
         struct pollfd query ={0};
         query.fd = fd ;
         INT rt = CRT_pollset_query(PSFd,&query);
         if(rt == 1)
         {
            struct poll_ctl event = {0};
            event.fd = fd ;
            event.cmd = PS_MOD;
            if (ev&EV_IN)
            {
               event.events = event.events &~POLLIN;
            }
            if (ev&EV_OUT)
            {
               event.events = event.events &~POLLOUT;
            }
#if  defined(POLLRDHUP)
            if (ev&EV_RDHUP)
            {
               event.events = event.events &~POLLRDHUP;
            }
#endif
            if (ev&EV_PRI)
            {
               event.events = event.events &~POLLPRI;
            }
            rt = CRT_pollset_ctl(PSFd,&event,1);

            if (rt == -1)
            {
               THROW_SYSERROR();
            }
         }

      }

      void CPollSetSelector::Remove(INT fd)
      {
         if (fd < 0)
         {
            THROW_GENERALERROR(CException, "INVALID FD");
         }
         struct pollfd query ={0};
         query.fd = fd ;
         INT rt = CRT_pollset_query(PSFd,&query);
         if(rt == 1)
         {
            struct poll_ctl event = {0};
            event.fd = fd ;
            event.cmd = PS_DELETE;
            rt = CRT_pollset_ctl(PSFd,&event,1);
            if (rt == -1)
            {
               return;                 // TODO:
               THROW_SYSERROR();
            }
            --EventNo;
         }
      }

      CBaseSelector::EV_PAIRS CPollSetSelector::WaitEvent(SHORT timeout)
      {

         EventsBuf.resize(EventNo);
         POLLEVENT *events = &EventsBuf[0];
         INT nfds = CRT_pollset_poll(PSFd, events, EventNo, timeout);
         CBaseSelector::EV_PAIRS ev_wait;
         if (nfds == -1)
         {
            THROW_SYSERROR();
         }
         else if (nfds == 0)
         {
            return ev_wait;
         }
         else
         {

            for (INT i = 0; i != nfds; ++i)
            {
               CBaseSelector::EV_TYPE ev = 0, old = events[i].events;
               if (old&POLLIN)
               {
                  ev |= EV_IN;
               }
               if (old&POLLOUT)
               {
                  ev |= EV_OUT;
               }
#if  defined(POLLRDHUP)
               if (old&POLLRDHUP)
               {
                  ev |= EV_RDHUP;
               }
#endif
               if (old&POLLPRI)
               {
                  ev |= EV_PRI;
               }
               if (old&POLLERR)
               {
                  ev |= EV_ERR;
               }
               if (old&POLLHUP)
               {
                  ev |= EV_HUP;
               }

               ev_wait.push_back(std::make_pair((INT)events[i].fd, ev));

            }
         }
         return ev_wait;
      }

      CPollSetSelector::~CPollSetSelector()
      {
         CRT_pollset_destroy(PSFd);
      }

#endif

/*CPollDispatcher*/
      CPollDispatcher::CPollDispatcher(size_t capacity)
      {
         _Events.reserve(capacity);
      }

      CPollDispatcher::~CPollDispatcher()
      {

      }

      SHORT CPollDispatcher::EventCorrective(SHORT events)
      {
         SHORT evtmp = 0;
         if (events&POLLIN)
         {
            evtmp |= POLLIN;
         }
         if (events&POLLPRI)
         {
            evtmp |= POLLPRI;
         }
         if (events&POLLOUT)
         {
            evtmp |= POLLOUT;
         }
#if  defined(_GNU_SOURCE) && defined(POLLRDHUP)
         if (events&POLLRDHUP)
         {
            evtmp |= POLLRDHUP;
         }
#endif

         return (evtmp);
      }


      void CPollDispatcher::Register(INT fd, SHORT events)
      {
         CLockGuard  _GL(&_MutexRegister);

         if (fd < 0)
         {
            THROW_GENERALERROR(CException, "INVALID FD");
            return;
         }
         else if (events <= 0)
         {
            THROW_GENERALERROR(CException, "INVALID EVENT");
            return;
         }

         POLLEVENT ev = {fd, EventCorrective(events),0};
         for(std::vector<POLLEVENT>::iterator i = _Events.begin(); i!= _Events.end(); ++i)
         {
            if (i->fd == -1)
            {
               *i = ev;
               return;
            }
         }
         _Events.push_back(ev);
      }

      void CPollDispatcher::Register(const POLLEVENT &ev)
      {
         CLockGuard  _GL(&_MutexRegister);

         if(ev.fd < 0)
         {
            THROW_GENERALERROR(CException, "INVALID FD");
            return;
         }
         else if((ev.events <= 0)||(EventCorrective(ev.events)!=ev.events))
         {
            THROW_GENERALERROR(CException, "INVALID EVENT");
            return;
         }

         for(std::vector<POLLEVENT>::iterator i = _Events.begin(); i!= _Events.end(); ++i)
         {
            if (i->fd == -1)
            {
               *i = ev;
               return;
            }
         }

         _Events.push_back(ev);
      }

      bool CPollDispatcher::Remove(const POLLEVENT &ev)
      {
         return Remove(ev.fd, ev.events);
      }


      bool CPollDispatcher::Remove(INT fd, SHORT events)
      {
         CLockGuard  _GL(&_MutexRegister);

         SHORT evtmp = EventCorrective(events);

         for(std::vector<POLLEVENT>::iterator i = _Events.begin(); i!= _Events.end(); ++i)
         {
            if (i->fd == fd && (i->events & events))
            {


               if (i->events == evtmp ) // for debug
               {
                   //printf("Remove Events %d\n", fd);
                  // m_events.erase(i);
                  i->fd = -1;

               }
               else
               {
                  // printf("Remove Events\n");
                  i->events = i->events &~ events;
               }
            }
         }


         return true;
      }

      void CPollDispatcher::Dispatch(SHORT timeout, CThread *_thread)
      {
         POLLEVENT   _plevn={0,0,0};

         if(WaitEvent(timeout,&_plevn))
         {
            if (_plevn.revents & POLLIN)
            {
               OnEvent(_plevn.fd, _plevn.revents, _thread);
            }

            //     if (_plevn.revents&POLLPRI)
            //     {
            //         OnEvent(_plevn.fd,POLLPRI);
            //     }

            //     if (_plevn.revents&POLLOUT)
            //     {
            //         OnEvent(_plevn.fd,POLLOUT);
            //     }
            // #if  defined(_GNU_SOURCE) && defined(POLLRDHUP)
            //     if (_plevn.events&POLLRDHUP)
            //     {
            //         OnEvent(_plevn.fd,POLLRDHUP);
            //     }
            // #endif

            //     if(_plevn.revents&POLLHUP)
            //     {
            //         OnEvent(_plevn.fd, POLLHUP);
            //     }
#ifndef _AIX
            if(_plevn.revents == POLLIN)
            {
               printf("_plevn: %d\n", _plevn.revents);
               Register(_plevn);
            }
#endif
         }
         else
         {
            // printf("No Event\n");
            return;
            OnEvent(-1,0, _thread); //≥¨ ±
         }
      }

      bool CPollDispatcher::WaitEvent(SHORT timeout,POLLEVENT *rtn)
      {
         CLockGuard  _GL(&_MutexWait);

         INT rt = CRT_poll((POLLEVENT*)&_Events[0], _Events.size(), timeout);
         if (rt == -1)
         {
            THROW_SYSERROR();
            return (false);
         }
         else if (rt == 0)
         {
            return (false);
         }
         else
         {
            for (size_t i = 0 ; i != _Events.size(); ++i)
            {
               if (_Events[i].revents != 0 &&_Events[i].fd != -1)
               {
                  if(rtn!=NULL)
                  {
                     CRT_memcpy(rtn,&_Events[i],sizeof(*rtn));
                  }
                  if(!Remove(_Events[i]))
                  {
                     THROW_SYSERROR();
                     return (false);
                  }

                  return true;
               }
            }

            THROW_SYSERROR();
            return (false);
         }
      }


   } /// namespace GalaxyRT
} /// namespace Galaxy

