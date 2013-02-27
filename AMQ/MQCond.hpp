#ifndef MQCOND_HPP_17E79D6A_C3F4_4C97_8326_1B1CB6967CF7
#define MQCOND_HPP_17E79D6A_C3F4_4C97_8326_1B1CB6967CF7

#pragma once

#include "MQAnchor.hpp"

namespace Galaxy
{
   namespace AMQ
   {

      class CConditionSuite:public CMQLoneAncestor
      {
      private:
         SQCOND  	     &_Cond;
         SQMUTEX 		 &_Mutex;
    
         inline pthread_cond_t *GetCond() const;
         inline pthread_mutex_t *GetMutex() const;
         
		 CConditionSuite(const CConditionSuite &);
         const CConditionSuite &operator=(const CConditionSuite &);
      public:
         explicit CConditionSuite(SQCOND &_TheCond,SQMUTEX &_TheMutex);
         ~CConditionSuite();

		 UINT Lurkers() const;
         void Wait() const;
         void NotifyOne() const;
         void NotifyAll() const;
         bool TimedWait(SHORT timeout) const;
      };


      class CConditionCreator : public CMQLoneAncestor
      {
      public:
         explicit CConditionCreator(SQCOND &_TheCond);
         ~CConditionCreator();  
      };


   } // AMQ namespace
} // Galaxy namespace

#endif /*MQCOND_HPP_17E79D6A_C3F4_4C97_8326_1B1CB6967CF7*/
