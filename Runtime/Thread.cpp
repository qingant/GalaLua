
#include "Thread.hpp"
#include "Exception.hpp"

namespace Galaxy
{
    namespace GalaxyRT
    {


/*CRunnable*/
        CRunnable::~CRunnable()
        {

        }

        void CRunnable::Initialize()
        {

        }

        void CRunnable::Finalize()
        {

        }


        CThreadAttr::CThreadAttr(INT _DetachState,UINT _StackSize)
            :_StackEntry(NULL),_Attr()
        {
            new (this) CThreadAttr(_DetachState,PTHREAD_INHERIT_SCHED,_StackSize);
        }

        CThreadAttr::CThreadAttr(INT _DetachState,INT _InheritSched,UINT _StackSize)
            :_StackEntry(NULL),_Attr()
        {
#if !defined(_AIX)
            _StackSize += (_StackSize % 1024) ==0 ? 0: 1024 - (_StackSize%1024) ;
            _StackSize += 1024;
            posix_memalign((void **)&_StackEntry,16,_StackSize);
            if(ISNULL(_StackEntry))
            {
                //THROW_NULLEXCEPTION(_StackEntry);
                THROW_EXCEPTION_EX("");

            }
#endif
            if (CRT_pthread_attr_init(&_Attr) != 0)
            {
                THROW_EXCEPTION_EX("");
            }

            if(CRT_pthread_attr_setdetachstate(&_Attr,_DetachState)!=0)
            {
                THROW_EXCEPTION_EX("");
            }
#ifndef _AIX
            if(pthread_attr_setinheritsched(&_Attr,_InheritSched)!=0)
            {
                THROW_EXCEPTION_EX("");
            }
            if(pthread_attr_setstack(&_Attr,_StackEntry,_StackSize)!=0)
            {
                THROW_EXCEPTION_EX("");
            }
#else
#if 0
            if(pthread_attr_setstackaddr(&_Attr,_StackEntry) !=0)
            {
                THROW_EXCEPTION_EX("");
            }
            if(pthread_attr_setstacksize(&_Attr,_StackSize) != 0)
            {
                THROW_EXCEPTION_EX("");
            }
#endif
#endif
        }

        CThreadAttr::~CThreadAttr()
        {
            if (CRT_pthread_attr_destroy(&_Attr) != 0)
            {
                THROW_EXCEPTION_EX("");
            }

            if(_StackEntry!=NULL)
            {
                CRT_free(_StackEntry);
                _StackEntry = NULL;
            }
        }

        const pthread_attr_t &CThreadAttr::Get() const
        {
            return _Attr;
        }

        pthread_attr_t &CThreadAttr::Get()
        {
            return _Attr;
        }



/*CThread*/
        CThread::CThread(CRunnable &_TheRun,INT _TheIndex,DETACH_STATE _DetachState,UINT _StackSize)
            :_Run(_TheRun),_Index(_TheIndex),_ThreadId(0),_Attr(_DetachState,_StackSize)
        {

        }

        CThread::CThread(CRunnable &_TheRun,INT _TheIndex,DETACH_STATE _DetachState,INT _InheritSched,UINT _StackSize)
            :_Run(_TheRun),_Index(_TheIndex),_ThreadId(0),_Attr(_DetachState,_InheritSched,_StackSize)
        {

        }

        CThread::~CThread()
        {

        }

        pthread_t CThread::ThreadId() const
        {
            if(_ThreadId==0)
            {
                THROW_EXCEPTION_EX("Thread Not Start");
            }
            return _ThreadId;
        }

        INT CThread::Index() const
        {
            return  _Index;
        }

        void CThread::TestCancel() const
        {
            CRT_pthread_testcancel();
        }

        void CThread::Join() const
        {
            PVOID   _Ptr=NULL;
            CThread *_Self = (CThread *)this;
            if(_Self->_ThreadId!=0)
            {
                if(CRT_pthread_join(_Self->_ThreadId,&_Ptr) != 0)
                {
                    THROW_EXCEPTION_EX("");
                }

                _Self->_ThreadId = 0;
            }
        }

        void CThread::Detach() const
        {
            if(CRT_pthread_detach(ThreadId()) != 0)
            {
                THROW_EXCEPTION_EX("");
            }
        }

        void CThread::Exit() const
        {
            pthread_exit(NULL);
        }

        void CThread::Cancel() const
        {
            CThread *_Self = (CThread *)this;
            if(_Self->_ThreadId!=0)
            {
                if(CRT_pthread_cancel(_Self->_ThreadId) != 0)
                {
                    THROW_EXCEPTION_EX("");
                }

                //_Self->_ThreadId = 0;
            }
        }

        void CThread::Kill(INT _Signal) const
        {
            if(pthread_kill(ThreadId(),_Signal)!=0)
            {
                THROW_EXCEPTION_EX("");
            }
        }

        bool CThread::Test() const
        {
            CThread *_Self = (CThread *)this;
            if(_Self->_ThreadId!=0)
            {
                if (pthread_kill(_Self->_ThreadId,0)==0)
                {
                    return true;
                }
                else
                {
                    _Self->_ThreadId = 0;

                }
            }

            return false;
        }

        typedef PVOID (*THREADSTARTROTINE)(PVOID);
        typedef VOID (*THREADCLEANOTINE)(PVOID);

        void CThread::Start() const
        {
            CThread *_Self = (CThread *)this;
            if(!Test())
            {
                if (CRT_pthread_create(&(_Self->_ThreadId), &(_Attr.Get()),(THREADSTARTROTINE)CThread::__T_Entry,_Self)!=0)
                {
                    THROW_EXCEPTION_EX("");
                }
            }
        }

        PVOID CThread::__T_Entry(CThread &_TheThread)
        {
            {
                INT _Old=0;
                if(CRT_pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&_Old)!=0)
                {
                    THROW_EXCEPTION_EX("");
                }
                if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&_Old)!=0)
                {
                    THROW_EXCEPTION_EX("");
                }
            }

            PVOID _ret = NULL;

            pthread_cleanup_push((THREADCLEANOTINE)CThread::__T_Cleanup,(PVOID)&_TheThread);

            _ret =  _TheThread._Run.Run(_TheThread);

            pthread_cleanup_pop(0);


            //_TheThread._ThreadId = 0;   //置线程退出
            _TheThread.Exit();
            return _ret;
        }

        void CThread::__T_Cleanup(CThread &_TheThread)
        {
            _TheThread._Run.Clean(_TheThread);
        //    _TheThread._ThreadId = 0;   //置线程退出
        }

/*CThreadPooler*/
        CThreadPooler::CThreadPooler(CRunnable &_TheRun,UINT _MaxThread,bool _IsJoinable,INT _InheritSched,UINT _StackSize)
            :_Joinable(_IsJoinable),_Threads()
        {
            for (UINT i=0;i<_MaxThread;i++)
            {
                CThread *_Thd = new CThread(_TheRun,i,((_IsJoinable)?JOINABLE:DETACH),_InheritSched,_StackSize);
                if(ISNULL(_Thd))
                {
                    THROW_EXCEPTION_EX("new CThread failure!");
                }

                _Threads.push_back(_Thd);
            }
        }

        CThreadPooler::~CThreadPooler()
        {
            for(UINT i=0;i<_Threads.size();i++)
            {
                if(_Threads[i]!=NULL)
                {
                    delete _Threads[i];
                    _Threads[i] = NULL;
                }
            }

            _Threads.clear();
        }

        void CThreadPooler::Start()
        {
            for (std::vector<CThread *>::iterator it = _Threads.begin(); it!=_Threads.end(); ++it)
            {
                if((*it)!=NULL)
                {
                    (*it)->Start();
                }
            }
        }

        void CThreadPooler::Cancel()
        {
            for (std::vector<CThread *>::iterator it = _Threads.begin(); it!=_Threads.end(); ++it)
            {
                if((*it)!=NULL)
                {
                    (*it)->Cancel();
                }
            }
        }

        void CThreadPooler::Join()
        {
            if (_Joinable)
            {
                for (std::vector<CThread*>::iterator it = _Threads.begin(); it!=_Threads.end(); ++it)
                {
                    if((*it)!=NULL)
                    {
                        (*it)->Join();
                    }
                }
                _Joinable = false;
            }
        }

    }
}
