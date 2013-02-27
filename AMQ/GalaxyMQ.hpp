#ifndef GALAXYMQ_HPP_6DFE47F5_9476_4454_A558_A2D3C431D377
#define GALAXYMQ_HPP_6DFE47F5_9476_4454_A558_A2D3C431D377

#pragma once

#include "MQI.hpp"

namespace Galaxy
{
   namespace AMQ
   {     
      enum GALAXY_MQ_TYPE
      {
          GALAXY_MQ_NQ,
          GALAXY_MQ_DQ,
      };
      
      const UINT DUMMYID = 8192;

      //MQ й╣ож
      class CGalaxyMQ : public CMQLoneAncestor
      {
      private:
          IMQ  *_MQInstance;
          inline const IMQ &GetInstance() const;
          
		  CGalaxyMQ(const CGalaxyMQ &);
		  const CGalaxyMQ &operator=(const CGalaxyMQ &);
      public:
          explicit CGalaxyMQ(const std::string &_FileName);
          explicit CGalaxyMQ(key_t _IPCKey);
          ~CGalaxyMQ();
      public:
          static INT  GetDummyId(GALAXY_MQ_TYPE type, INT id);
          const IMQueue &GetQByDummyId(UINT id) const;


		  const IPoolerView &PoolerView() const;
          const IMQueueArray &NQArray() const;
          const IMQueueArray &DQArray() const;
          const IMQueue &operator[](UINT _Index) const;
          const IMQ &InstanceOfSuite() const;
      }; 
      
      class CGalaxyMQCreator : public CMQLoneAncestor
      {
      private:
         static void* operator new (size_t);
      public:
         explicit CGalaxyMQCreator(const std::string &_FileName,const std::string &_EndUser,UINT _Pages,
                        UINT _PageSize,UINT _NQs,UINT _DQs,UINT _DefaultDepthPeerQ,UINT _Year,UINT _Month,UINT _Day);
         explicit CGalaxyMQCreator(key_t _IPCKey,const std::string &_EndUser,UINT _Pages,
                        UINT _PageSize,UINT _NQs,UINT _DQs,UINT _DefaultDepthPeerQ,UINT _Year,UINT _Month,UINT _Day);
         ~CGalaxyMQCreator();
      };
    
   } // AMQ namespace
} // Galaxy namespace

#endif /*GALAXYMQ_HPP_6DFE47F5_9476_4454_A558_A2D3C431D377*/
