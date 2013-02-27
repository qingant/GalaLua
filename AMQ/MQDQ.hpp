#ifndef MQDQ_HPP_842B9C90_7345_4CBA_907D_F31522CBA99E
#define MQDQ_HPP_842B9C90_7345_4CBA_907D_F31522CBA99E

#pragma once

#include "MQPooler.hpp"
#include "MQView.hpp"

namespace Galaxy
{
   namespace AMQ
   {
/*Distributary Q*/

      class CDsbQListArray;
      class CDsbQList : public CListByNodes
      {
      public:
         explicit CDsbQList(SQLIST &_TheList,const CNodes &_TheNodes);
         ~CDsbQList();
  
         void Push(CNodes::THEPAGEID _PageId) const;
         void Append(CNodes::THEPAGEID _PageId) const;
      };


      class CDsbQSuite;
      class CDsbQListArray : public CMQLoneAncestor
      {
      private:
         std::vector<CDsbQList *>    _Items; 
      public:    
         explicit CDsbQListArray(SQCHLLIST &_TheRAWDQ,const CNodes &_TheNodes);
         ~CDsbQListArray();

         UINT Count() const;
         const CDsbQList &operator[](UINT _Index) const;
      };

      class CDsbQSyncArray : public CMQLoneAncestor
      {
      private:
         CMutexSuite                 	_GLK;
         CConditionSuite				_PUT;
         std::vector<CConditionSuite *>	_GET; 

      public:    
         explicit CDsbQSyncArray(SQCHLLIST &_TheRAWDQ);
         ~CDsbQSyncArray();

         UINT Count() const;
         const CMutexSuite &Locker() const;
         const CConditionSuite &operator[](UINT _Index) const; 
      };


      class CDsbQArray;
      class CDsbQSuite : public IMQueue
      {
      private:
      	 SQCHLLIST			 &_RAWEntry;
         const CPooler       &_Pooler;
         volatile UINT       _Index; 
         

         CDsbQListArray      _ListArray;
         CDsbQSyncArray      _SyncArray;
         CMQDQView           _View;
    
         void Hibernate_GET(IMQueue::EMQCHL _Chl,UINT &_IndexOfList) const;
         bool TimedHibernate_GET(IMQueue::EMQCHL _Chl,SHORT _timeout,UINT &_IndexOfList) const;
         void Wakeup_GET(IMQueue::EMQCHL _Chl,UINT _CurrDepth) const;
         	
         void Hibernate_PUT() const;
         bool TimedHibernate_PUT(SHORT _timeout) const;
         void Wakeup_PUT() const;
                   
      private:
         void _Put(IMQueue::EMQCHL _Chl,CNodes::THEPAGEID _PageId,bool _ExPosHead) const;
		 bool _TimedPut(IMQueue::EMQCHL _Chl,CNodes::THEPAGEID _PageId,bool _ExPosHead,SHORT _timeout) const;

         CNodes::THEPAGEID _Get(IMQueue::EMQCHL _Chl,UINT &_TheIndexOfList) const;
         CNodes::THEPAGEID _TimedGet(IMQueue::EMQCHL _Chl, SHORT timeout,UINT &_TheIndexOfList) const;
      public:
         explicit CDsbQSuite(SQCHLLIST &_TheEntry,UINT _TheIndex,const CPooler &_ThePooler);
         ~CDsbQSuite();
    
         inline PSQCHLLIST RAWEntry() const;
             
         UINT Id() const;
      public:
         const IMQView &View() const;
         
         UINT GetMaxDepth() const;
         void SetMaxDepth(UINT _Depth) const;

         UINT TimedGet(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length, SHORT timeout) const;
         UINT Get(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length) const;
         UINT Put(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length) const;
         UINT TimedPut(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length, SHORT timeout) const;

         UINT PutEx(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length,bool _ExPosHead) const;
         UINT TimedPutEx(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length,bool _ExPosHead, SHORT timeout) const;

		 void Reset() const;
      };

      class CDsbQArray : public IMQueueArray
      {
      private:
         std::vector<CDsbQSuite *>   _Items; 
      public:
         explicit CDsbQArray(const CPooler & _ThePooler);
         ~CDsbQArray();
    
         UINT Count() const;
         const IMQueue &operator[](UINT _Index) const;
      };

   } // AMQ namespace
} // Galaxy namespace
#endif /*CQ_HPP_842B9C90_7345_4CBA_907D_F31522CBA99E*/
