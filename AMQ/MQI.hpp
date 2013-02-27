#ifndef MQI_HPP_6DFE47F5_9476_4454_A558_A2D3C431D377
#define MQI_HPP_6DFE47F5_9476_4454_A558_A2D3C431D377

#pragma once

#include <cstub.h>
#include "stlincl.hpp"

namespace Galaxy
{
   namespace AMQ
   {  
      class CMQAncestor
      {
      protected:
         explicit CMQAncestor() {};
      public:
         virtual ~CMQAncestor() {};
      };


      class CMQLoneAncestor : public CMQAncestor
      {
      private:  // emphasize the following members are private
         CMQLoneAncestor( const CMQLoneAncestor &);
         const CMQLoneAncestor& operator=( const CMQLoneAncestor & );
      protected:
         explicit CMQLoneAncestor(){};
      public:
         virtual ~CMQLoneAncestor() {};
      };
      
      
      class IBlock : public CMQLoneAncestor
      {
      public:
         virtual ~IBlock() {};
         virtual PBYTE RAWEntry() const = 0;
         virtual UINT RAWSize() const = 0;
      };

      
      class IMQView;   
      class IMQueue : public CMQLoneAncestor
      {
      public:
         typedef enum
         {
            // real time ,highest prior
            MQC_RTL=0,
            // query
            MQC_QRY=1,
            // bat
            MQC_BAT=2
         }EMQCHL;

         virtual ~IMQueue() {};
         
         virtual void Reset() const = 0;
         virtual const IMQView &View() const = 0;
         
         virtual UINT GetMaxDepth() const = 0;
         virtual void SetMaxDepth(UINT _Depth) const = 0;

         virtual UINT Get(EMQCHL _Chl,const IBlock &_Block,UINT &_Length) const = 0;
         virtual UINT TimedGet(EMQCHL ,const IBlock &,UINT &, SHORT) const = 0;


         virtual UINT Put(EMQCHL _Chl,const IBlock &_Block,UINT &_Length) const = 0;
         virtual UINT PutEx(EMQCHL _Chl,const IBlock &_Block,UINT &_Length,
                            bool _ExPosHead) const = 0;

         virtual UINT TimedPut(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length, SHORT timeout) const = 0;
         virtual UINT TimedPutEx(IMQueue::EMQCHL _Chl,const IBlock &_Block,UINT &_Length, 
                            bool _ExPosHead, SHORT timeout) const =0;

      };
      
      class IMQueueArray : public CMQLoneAncestor
      {
      public:
         virtual ~IMQueueArray() {};
         virtual UINT Count() const = 0;
         virtual const IMQueue &operator[](UINT _Index) const = 0; 
      };


      class IMQView : public CMQLoneAncestor
      {
      public:
        virtual ~IMQView() {};
        virtual UINT Total(IMQueue::EMQCHL) const =0;
        virtual UINT Head(IMQueue::EMQCHL) const=0;
        virtual UINT Tail(IMQueue::EMQCHL) const=0;
        virtual UINT LurkersInGet(IMQueue::EMQCHL) const = 0;
        virtual UINT LurkersInPut() const = 0;
      };
      
      class IPoolerView : public CMQLoneAncestor
      {
      public:
        virtual ~IPoolerView() {};
        virtual UINT Pages() const =0;
        virtual UINT PageSize() const =0;
        virtual UINT Total() const =0;
        virtual UINT Head() const=0;
        virtual UINT Tail() const=0;
        virtual UINT Lurkers() const = 0;
      };
      
      class IMQ : public CMQLoneAncestor
      {
      public:
         virtual ~IMQ() {};
		 virtual void BKD(UINT _BKDCode,...) const = 0;
         virtual const IMQueueArray &NQArray() const = 0;
         virtual const IMQueueArray &DQArray() const = 0;
         virtual const IPoolerView &PoolerView() const = 0;
      };
   } // AMQ namespace
} // Galaxy namespace

#endif /*MQI_HPP_6DFE47F5_9476_4454_A558_A2D3C431D377*/
