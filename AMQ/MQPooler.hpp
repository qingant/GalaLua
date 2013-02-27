#ifndef POOLER_HPP_9CF2C725_6FFF_4A25_A711_F448EDBD68D9
#define POOLER_HPP_9CF2C725_6FFF_4A25_A711_F448EDBD68D9

#pragma once

#include "MQNodes.hpp"
#include "MQMutex.hpp"
#include "MQCond.hpp"
namespace Galaxy
{
   namespace AMQ
   {
      class CPooler;

      class CPoolerList : public CListByNodes
      {  
      public:
         explicit CPoolerList(SQLIST &_TheList,const CNodes &_TheNodes);
         ~CPoolerList();
         void Put(CNodes::THEPAGEID _PageId) const;
      };

      class CPoolerSync : public CMQLoneAncestor
      {
      private:
         CMutexSuite             _GLK;
         CConditionSuite         _GET;
      public:
         explicit CPoolerSync(SQNRMLIST &_TheEntry);
         ~CPoolerSync();
    
         const CMutexSuite &Locker() const;
         const CConditionSuite &Hibernator() const;
      };

      class CPooler : public CMQLoneAncestor
      {
      private:
         const CAnchor       &_Anchor; 
         SQNRMLIST			 &_RAWEntry;
         const CNodes        _Nodes;
         CPoolerList         _List;
         CPoolerSync         _Sync;
    
         void Hibernate() const;
         bool TimedHibernate(SHORT) const;
         void Wakeup() const;
      public:  
         explicit CPooler(const CAnchor &_TheAnchor);
         ~CPooler();
    
         const CAnchor &Anchor() const;
         const CNodes &Nodes() const;
      public:
         void Put(CNodes::THEPAGEID _PageId) const;
         CNodes::THEPAGEID Get(CNode::EMQTYPE _Type,UINT _QID) const;
         CNodes::THEPAGEID TimedGet(CNode::EMQTYPE _Type,UINT _QID, SHORT timeout) const;
         void Reset() const;
      };


      class CPoolerNursery : public CMQLoneAncestor
      {
      private:
         const CPooler                   &_Pooler;
         std::vector<CNodes::THEPAGEID>  _List; 
   
         static void* operator new (size_t);
      public:
         explicit CPoolerNursery(const CPooler &_ThePooler);
         ~CPoolerNursery();
         void Enter(CNodes::THEPAGEID _PageId);    
         void Release(); 
      };

   } // AMQ namespace
} // Galaxy namespace


#endif /*POOLER_HPP_9CF2C725_6FFF_4A25_A711_F448EDBD68D9*/
