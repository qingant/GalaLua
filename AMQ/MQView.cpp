
#include "MQView.hpp"
#include "MQNQ.hpp"
#include "MQDQ.hpp"
#include "MQException.hpp"

using namespace Galaxy::AMQ;

CMQPoolerView::CMQPoolerView(const SQNRMLIST &_ThePQ,const SQNODES &_TheNodes)
    :_PQ(_ThePQ),_Nodes(_TheNodes)
{
    
}

CMQPoolerView::~CMQPoolerView()
{
    
}

UINT CMQPoolerView::Pages() const
{
    return (_Nodes._Pages);    
}

UINT CMQPoolerView::PageSize() const
{
    return (_Nodes._PageSize);
}

UINT CMQPoolerView::Total() const
{
    return (_PQ._LST._Total);
}

UINT CMQPoolerView::Head() const
{
    return (_PQ._LST._Head);    
}

UINT CMQPoolerView::Tail() const
{
    return (_PQ._LST._Tail);   
}

UINT CMQPoolerView::Lurkers() const
{
	return (_PQ._EVT._GET._WAT);  	
}


/*CMQNQView*/
CMQNQView::CMQNQView(const SQNRMLIST &_TheNQ)
    :_NQ(_TheNQ)
{
    
}

CMQNQView::~CMQNQView()
{
    
}

UINT CMQNQView::Total(IMQueue::EMQCHL) const
{
    return (_NQ._LST._Total);    
}

UINT CMQNQView::Head(IMQueue::EMQCHL) const
{
    return (_NQ._LST._Head);     
}

UINT CMQNQView::Tail(IMQueue::EMQCHL) const
{
    return (_NQ._LST._Tail);     
}

UINT CMQNQView::LurkersInGet(IMQueue::EMQCHL) const
{
	return (_NQ._EVT._GET._WAT);  	
}

UINT CMQNQView::LurkersInPut() const
{
	return (_NQ._EVT._PUT._WAT);	
}



/*CMQDQView*/
CMQDQView::CMQDQView(const SQCHLLIST &_TheDQ)
    :_DQ(_TheDQ)
{
    
}

CMQDQView::~CMQDQView()
{
    
}

UINT CMQDQView::Validate(IMQueue::EMQCHL _Chl) const
{
   switch(_Chl)
   {
      case(IMQueue::MQC_RTL):
      {
         return 0; 
      }
      case(IMQueue::MQC_QRY):
      {
         return 1;
      }
      case(IMQueue::MQC_BAT):
      {
         return 2;
      }    
      default:
      {
         THROW_MQEXCEPTION("Unknow Q Channel!");
         return 0; 
      }  
   };  
}

UINT CMQDQView::Total(IMQueue::EMQCHL _Chl) const
{
    return (_DQ._CHL[Validate(_Chl)]._Total);    
}

UINT CMQDQView::Head(IMQueue::EMQCHL _Chl) const
{
    return (_DQ._CHL[Validate(_Chl)]._Head);     
}

UINT CMQDQView::Tail(IMQueue::EMQCHL _Chl) const
{
    return (_DQ._CHL[Validate(_Chl)]._Tail);     
}

UINT CMQDQView::LurkersInGet(IMQueue::EMQCHL _Chl) const
{
	return (_DQ._EVT._GET[Validate(_Chl)]._WAT);  	
}

UINT CMQDQView::LurkersInPut() const
{
	return (_DQ._EVT._PUT._WAT);	
}
