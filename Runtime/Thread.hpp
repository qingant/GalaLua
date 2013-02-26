#ifndef THREAD_H_267C5369_26B9_46F8_AEC0_93F5D30C46B1
#define THREAD_H_267C5369_26B9_46F8_AEC0_93F5D30C46B1

#pragma once

#include "Object.hpp"
#include "Exception.hpp"
namespace Galaxy
{
    namespace GalaxyRT
    {
        

        class CThread;
        class CRunnable :public CNonCopyAble
        {
        public:
            virtual PVOID Run(const CThread &t) = 0;
            //virtual PVOID Run(void*){THROW_EXCEPTION_EX("Not impl");}
            virtual void Clean(const CThread &){};

            virtual void Initialize();
            virtual void Finalize();
            virtual~CRunnable();
        };

        enum DETACH_STATE
        {
            DETACH = PTHREAD_CREATE_DETACHED,
            JOINABLE = PTHREAD_CREATE_JOINABLE,
        };


        class CThreadAttr : public CNonCopyAble
        {
        private:
            PSTR            _StackEntry;
            pthread_attr_t  _Attr;
   
            CThreadAttr();
            CThreadAttr(const CThreadAttr &);
            const CThreadAttr &operator=(const CThreadAttr &); 
        public:
            CThreadAttr(INT _DetachState,UINT _StackSize);
            CThreadAttr(INT _DetachState,INT _InheritSched,UINT _StackSize);
            ~CThreadAttr();    
            const pthread_attr_t &Get() const;
            pthread_attr_t &Get(); 
        };

        class CThread : public CNonCopyAble
        {
        private:
            CRunnable       &_Run;
            INT             _Index;
            pthread_t       _ThreadId;
            CThreadAttr     _Attr;
    
            static void __T_Cleanup(CThread &_TheThread);
            static PVOID __T_Entry(CThread &_TheThread);
        public:
            CThread(CRunnable &_TheRun,INT _TheIndex, DETACH_STATE _DetachState = JOINABLE, UINT _StackSize = 1024*1024*4);
            //CThread(CRunnable &_)
            CThread(CRunnable &_TheRun,INT _TheIndex,DETACH_STATE _DetachState,INT _InheritSched,UINT _StackSize);
            virtual ~CThread();
        public:
            pthread_t ThreadId() const;
            INT Index() const;
    
            void Kill(INT _Signal) const; 
            void TestCancel() const;
            void Join() const;
            void Detach() const;
            void Exit() const;
            void Start() const;
            void Cancel() const;
            bool Test() const;
        };

        class CThreadPooler : public CNonCopyAble
        {
        private:
            bool _Joinable;
            std::vector<CThread *> _Threads;
        public:
            CThreadPooler(CRunnable &_TheRun,UINT _MaxThread,bool _IsJoinable,INT _InheritSched,UINT _StackSize);
            ~CThreadPooler();
            void Start();
            void Cancel(); 
            void Join();  
        };
    }
}

#endif /*THREAD_H_267C5369_26B9_46F8_AEC0_93F5D30C46B1*/
