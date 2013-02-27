#ifndef MQNQ_HPP_BAC436EF_9B8C_4E37_9451_3A3B5B191021
#define MQNQ_HPP_BAC436EF_9B8C_4E37_9451_3A3B5B191021

#pragma once

#include "MQPooler.hpp"
#include "MQView.hpp"

namespace Galaxy
{
   namespace AMQ
   {
/*Normal Q */
      class CNmlQSuite;
      class CNmlQList : public CListByNodes
      {
      public:
         explicit CNmlQList(SQLIST &_TheList,const CNodes &_TheNodes);
         ~CNmlQList();
  
         void Push(CNodes::THEPAGEID _PageId) const;
         void Append(CNodes::THEPAGEID _PageId) const;
      };

      class CNmlQSync : public CMQLoneAncestor
      {
      private:
         CMutexSuite             _GLK;
         CConditionSuite         _GET;
         CConditionSuite         _PUT;
      public:
         explicit CNmlQSync(SQNRMLIST &_TheEntry);
         ~CNmlQSync();
    
         const CMutexSuite &Locker() const;
         const CConditionSuite &Hibernator_GET() const;
         const CConditionSuite &Hibernator_PUT() const;
      };


      class CNmlQArray;
      class CNmlQSuite : public IMQueue
      {
      private:
         SQNRMLIST			 &_RAWEntry;
         const CPooler       &_Pooler;
         volatile UINT       _Index; 
         
         

         CNmlQList           _List;
         CNmlQSync           _Sync;
         CMQNQView           _View;
         
      private:
         void Hibernate_GET() const;
         bool TimedHibernate_GET(SHORT _timeout) const;
         void Hibernate_PUT() const;
         bool TimedHibernate_PUT(SHORT _timeout) const;
         void Wakeup_GET() const;
         void Wakeup_PUT() const;
         	
         void _Put(CNodes::THEPAGEID _PageId,bool _ExPosHead) const;
         bool _TimedPut(CNodes::THEPAGEID _PageId,bool _ExPosHead,SHORT _timeout) const;
         CNodes::THEPAGEID _Get() const;
         CNodes::THEPAGEID _TimedGet(SHORT _timeout) const;
         
		 inline PSQNRMLIST RAWEntry() const;  
      public:
         explicit CNmlQSuite(SQNRMLIST & _TheEntry,UINT _TheIndex,const CPooler &_ThePooler);
         ~CNmlQSuite();        
  
         UINT Id() const;
      public:
         const IMQView &View() const;
         
         UINT GetMaxDepth() const;
         void SetMaxDepth(UINT _Depth) const;
        
         UINT Get(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length) const;
         UINT TimedGet(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length,
                       SHORT _Timeout) const;
         UINT Put(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length) const;
         UINT TimedPut(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length, SHORT timeout) const;

         UINT PutEx(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length,
                    bool _ExPosHead) const;
         UINT TimedPutEx(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length,
                    bool _ExPosHead, SHORT timeout) const;

		 void Reset() const;
         
      };

      class CNmlQArray : public IMQueueArray
      {
      private:      
         std::vector<CNmlQSuite *>   _Items;

      public:
         explicit CNmlQArray(const CPooler & _ThePooler);
         ~CNmlQArray();

         UINT Count() const;
         const IMQueue &operator[](UINT _Index) const;
      };

   } // AMQ namespace
} // Galaxy namespace
#endif /*MQNQ_HPP_BAC436EF_9B8C_4E37_9451_3A3B5B191021*/
