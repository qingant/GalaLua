#ifndef MQVIEW_HPP_6E4AA5A2_AB34_4094_ABB9_1E1D38609F92
#define MQVIEW_HPP_6E4AA5A2_AB34_4094_ABB9_1E1D38609F92

#pragma once
#include "MQAnchor.hpp"
#include "MQI.hpp"


namespace Galaxy
{
    namespace AMQ
    {
        class CMQPoolerView : public IPoolerView
        {
        private:
            const SQNRMLIST 	&_PQ;
            const SQNODES 		&_Nodes;
        public:
            explicit CMQPoolerView(const SQNRMLIST &_ThePQ,const SQNODES &_TheNodes);
            ~CMQPoolerView(); 
            
            UINT Pages() const;
            UINT PageSize() const;
            UINT Total() const;
            UINT Head() const;
            UINT Tail() const;
            UINT Lurkers() const;
        };
        
        class CNmlQSuite;
        class CMQNQView : public IMQView
        {
        private:
            const SQNRMLIST &_NQ;
        public:   
            explicit CMQNQView(const SQNRMLIST &_TheNQ);
            ~CMQNQView();
            
            UINT Total(IMQueue::EMQCHL) const;
            UINT Head(IMQueue::EMQCHL) const;
            UINT Tail(IMQueue::EMQCHL) const;
        	UINT LurkersInGet(IMQueue::EMQCHL) const;
        	UINT LurkersInPut() const;
        };
        
        class CDsbQSuite;
        class CMQDQView : public IMQView
        {
        private:
            const SQCHLLIST &_DQ;
            
            UINT Validate(IMQueue::EMQCHL _Chl) const;
        public:   
            explicit CMQDQView(const SQCHLLIST &_TheDQ);
            ~CMQDQView();    
            
            UINT Total(IMQueue::EMQCHL _Chl) const;
            UINT Head(IMQueue::EMQCHL _Chl) const;
            UINT Tail(IMQueue::EMQCHL _Chl) const;
        	UINT LurkersInGet(IMQueue::EMQCHL _Chl) const;
        	UINT LurkersInPut() const;
        };
    }
}


#endif /*MQVIEW_HPP_6E4AA5A2_AB34_4094_ABB9_1E1D38609F92*/
