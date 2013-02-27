#ifndef MQMUTEX_HPP_6F6FBB8E_BA35_47EC_ABAB_ED497974F5F2
#define MQMUTEX_HPP_6F6FBB8E_BA35_47EC_ABAB_ED497974F5F2

#pragma once

#include "MQAnchor.hpp"

namespace Galaxy
{
   namespace AMQ
   {

      class CMutexSuite : public CMQLoneAncestor
      {
      private:
         SQMUTEX 				&_Mutex;    
         inline pthread_mutex_t *Entry() const;
      public:
         explicit CMutexSuite(SQMUTEX &_TheMutex);
         ~CMutexSuite();
    
    	 bool Locked() const;
         void Lock() const;
         void UnLock() const;
         bool TryLock() const;
         bool TimedLock(USHORT timeout) const;
    
         const pthread_mutex_t *GetRAWMutex() const;
      };


      class CMutexCreator : public CMQLoneAncestor
      {
      public:
         explicit CMutexCreator(SQMUTEX &_TheMutex);
         ~CMutexCreator();  
      };

      class CMutexLockGuard : public CMQLoneAncestor
      {
      private:
         const CMutexSuite &_Mutex;  
         static void* operator new (size_t); 
      public:
         explicit CMutexLockGuard(const CMutexSuite &_TheMutex);
         ~CMutexLockGuard();
      };


   } // AMQ namespace
} // Galaxy namespace

#endif /*MQMUTEX_HPP_6F6FBB8E_BA35_47EC_ABAB_ED497974F5F2*/
