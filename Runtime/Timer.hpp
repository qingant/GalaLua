/* -*- C++ -*- */

/* Time-stamp: <2012-08-01 01:17:26 星期三 by tao> */

/**
 * @file   Timer.hpp
 * @author Ma Tao <qingant@gmail.com>
 * @date   Thu Jul 19 15:58:16 2012
 *
 * @bref
 *
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#pragma once

#include "Object.hpp"
#include <typeinfo>
#include "Socket.hpp"
#include "Pipe.hpp"
#include "Sync.hpp"
#include "Thread.hpp"
#include <algorithm>
#include <vector>
#include "Event.hpp"
#include "Any.hpp"
#include "Queue.hpp"
#include "Thread.hpp"
namespace Galaxy   
{
    namespace GalaxyRT   
    {


        typedef struct timeval TPType; // time point

        struct TIMEREVENT
        {
            TPType _Val;
            std::string  _Data;

            
            TIMEREVENT(const TPType &val, const std::string &data)
                :_Val(val), _Data(data){
                    
            }
            TIMEREVENT(time_t sec)
                :_Val(),_Data("")
            {
                _Val.tv_sec =  sec;
                _Val.tv_usec = 0;
            }
            bool IsTimeOut(const TPType &t){
                return ((*this)==t) || !((*this) < t);
            }
            bool operator < (const TIMEREVENT &rhs){
                GALA_ERROR("Operator <");
                return ((_Val.tv_sec > rhs._Val.tv_sec) || ((_Val.tv_sec == rhs._Val.tv_sec) && (_Val.tv_usec > rhs._Val.tv_usec)));
                //return (sec>rhs.sec);
            }
            bool operator < (const TPType &rhs){
                GALA_ERROR("Operator <");
                return ((_Val.tv_sec > rhs.tv_sec) || ((_Val.tv_sec == rhs.tv_sec) && (_Val.tv_usec > rhs.tv_usec)));
                //return (sec>rhs.sec);
            }
            bool operator == (const TPType &t){
                return (_Val.tv_sec == t.tv_sec) && (_Val.tv_usec == t.tv_usec);
            }
            INT operator - (const TPType &rhs){
                INT rt = 0;
                rt = ( _Val.tv_sec - rhs.tv_sec)*1000;
                if (_Val.tv_usec > rhs.tv_usec)
                {
                    rt += (_Val.tv_usec - rhs.tv_usec)/(1000*1000);        
                }
                else
                {
                    rt -= (rhs.tv_usec -_Val.tv_usec )/(1000*1000);        
                }
                
                return rt;
            }
            TIMEREVENT operator+ (const TPType &t) const{
                TPType tmp = _Val;
                tmp.tv_sec += t.tv_sec;
                tmp.tv_usec += t.tv_usec;
                return TIMEREVENT(tmp, _Data);
            }
        };


        typedef _TypeDef_FIFO<TIMEREVENT>::CFIFOQueue  TimedQueue;
        typedef void (*TIMERHANDLER)(const TIMEREVENT&);
        class CTimer : public CRunnable
        {
        public:
            CTimer(TIMERHANDLER handle,INT tick = 2000);
            void* Run(const CThread &);
            void SetTimer(const TIMEREVENT &);
            virtual ~CTimer();
        public:
            //TIMEREVENT GetEvent();
            time_t GetTimeStamp() const;
            
        private:
            inline void update_stamp(time_t );
            void wake();
            void eat();
            int  gettimer(TPType);
            std::vector<TIMEREVENT> check_and_pop(TPType);
            //inline void on_timeout(const TIMEREVENT &);
        private:
            TIMERHANDLER _Handle;
            INT         _Tick;
            CSocketPair _Pair;
            std::vector<TIMEREVENT> _Heap;
            CPthreadMutex _Mutex;
            //TimedQueue    _Events;
            time_t        _TimeStamp;
        };
    
    } // namespace GalaxyRT end here
    
} // namespace Galaxy end here


#endif /* _TIMER_H_ */
