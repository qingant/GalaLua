/* -*- C++ -*- */

/* Time-stamp: <2013-02-21 16:24:54 星期四 by matao> */

/**
 * @file   Timer.cpp
 * @author Ma Tao <qingant@gmail.com>
 * @date   Mon Jul 23 11:21:10 2012
 *
 * @bref
 *
 */


#include "Timer.hpp"


using namespace Galaxy::GalaxyRT;


CTimer::CTimer(TIMERHANDLER handle, INT tick)
    :_Handle(handle),
     _Tick(tick) 
{
    gettimeofday(&_TimeStamp, NULL);
    std::make_heap(_Heap.begin(), _Heap.end());

    _Pair.GetAnother()->SetNonBlocking();
    _Pair.GetOne()->SetNonBlocking();
}
CTimer::~CTimer()
{
    
}

void CTimer::SetTimer(const TIMEREVENT &time)
{
    CLockGuard _Gl(&_Mutex);
    TPType now;
    gettimeofday(&now, NULL);
    
    _Heap.push_back(TIMEREVENT(time+now));
    std::push_heap(_Heap.begin(), _Heap.end());
    wake();
}



int CTimer::gettimer(TPType now)
{
    CLockGuard _Gl(&_Mutex);
    eat();
    if (_Heap.size() == 0)
    {
        return (-1);
    }

    // std::pop_heap(_Heap.begin(), _Heap.end());
    // _Heap.pop_back();

    int rt =  (_Heap.front() - now);
    // if (rt == 0)
    // {
    //     return (-1);
    // }
    return rt;
}

void CTimer::update_stamp(TPType now)
{
    _TimeStamp.tv_sec = now.tv_sec;
    _TimeStamp.tv_usec = now.tv_usec;
}
TPType CTimer::GetTimeStamp() const
{
    return _TimeStamp;
}

std::vector<TIMEREVENT> CTimer::check_and_pop(TPType now)
{
    CLockGuard _Gl(&_Mutex);
    std::vector<TIMEREVENT> vec;
  
    //GALA_DEBUG("Heap Size : %ld", _Heap.size());
    while ((_Heap.size() != 0) &&  _Heap.front().IsTimeOut(now))
    {
        vec.push_back(_Heap.front());
        std::pop_heap(_Heap.begin(), _Heap.end());
        _Heap.pop_back();
    }
    return vec;
}


//
//void CTimer::on_timeout(const TIMEREVENT &t)
//{
//    _Events.Put(t);
//}
//TIMEREVENT CTimer::GetEvent()
//{
//    return _Events.Get();
//}


void *CTimer::Run(const CThread &)
{
    struct pollfd ev;
    ev.fd = _Pair.GetOne()->GetFD();
    ev.events = POLLIN;
    TPType now;
    gettimeofday(&now, NULL);
    while (true)
    {

        int timeval = gettimer(now);
        update_stamp(now); // 更新时间戳
        timeval = (timeval < _Tick) && (timeval != -1)?timeval:_Tick; // 睡眠一个Tick和timeval中较小的值
        //GALA_ERROR("TIMEVAL: [%d]", timeval);
        if (timeval != 0)
        {
            int rt = poll(&ev, 1, timeval);
            assert(rt!=0xffffff);
        }
        

        //GALA_DEBUG("Poll Result : %d",rt);
        
        gettimeofday(&now, NULL);

        std::vector<TIMEREVENT> vec = check_and_pop(now);
        for (size_t i = 0; i != vec.size(); ++i)
        {
            _Handle(vec[i]);
        }
    }
}

void CTimer::wake()
{
    try
    {
        _Pair.GetAnother()->Send(" ",1);
    }
    catch (...)
    {

    }
}

void CTimer::eat()
{
    char buf[1024];
    try
    {
        _Pair.GetOne()->Recv(buf, sizeof(buf));    
    }
    catch (...)
    {
            
    }
    
}
