
#include "MQNodes.hpp"
#include "MQException.hpp"

using namespace Galaxy::AMQ;    
/*CNode*/
CNode::CNode(SQNODE &_TheEntry,const CNodes &_TheNodes)
   :_NodeEntry(_TheEntry),_Nodes(_TheNodes)
{   
   /*
   if(Id()!=_Id)
   {
      THROW_MQEXCEPTION("_Node Id Incorrect!");    
   }

   if(ISNULL(_NodeEntry))
   {
      THROW_MQEXCEPTION("_NodeEntry Is NULL!");
   }
   else 
   */   
}

CNode::~CNode()
{
  
}

PSQNODE CNode::Entry() const
{
   return &_NodeEntry;    
}

UINT CNode::Id() const
{
   return Entry()->_Id;    
}

UINT CNode::Offset() const
{
   return Entry()->_Offset;     
}

PBYTE CNode::PageEntry() const
{
   PBYTE _Ety = (PBYTE)Entry();  
   return &_Ety[Offset()];   
}

UINT CNode::Next() const
{
   return Entry()->_Next;     
}

void CNode::Next(UINT _next) const
{
   if(_next > _Nodes.Pages())
   {
      THROW_MQEXCEPTION("Next Id out of Range!");
      return;  
   }
        
   Entry()->_Next = _next;    
}

UINT CNode::Length() const
{
   return Entry()->_Length; 
}

void CNode::Length(UINT _length) const
{
   Entry()->_Length = _length;     
}

UINT CNode::State() const
{
    return Entry()->_State;     
}

void CNode::Occupy(EMQTYPE _Type,UINT _QID) const
{
    Entry()->_State = (((UINT)_Type) << 28) | (_QID & 0x0FFFFFFF);
}

void CNode::Release() const
{
    Entry()->_State = 0xFFFFFFFF;   
}

bool CNode::IsReleased() const
{
    return ((Entry()->_State & 0xF0000000) == 0xF0000000);  
}



/*CNodeArray*/
CNodeArray::CNodeArray(SQNODES & _TheEntry,const CNodes &_TheNodes)
   :_Nodes(_TheNodes),_List()
{
   for(UINT i=0;i<_Nodes.Pages();i++)
   {
      SQNODE & _RAWNode = _TheEntry._NodeArray[i];
	  if(_RAWNode._Id != (i+1))
	  {
	      THROW_MQEXCEPTION("_Node Id Incorrect!");    
	  }
        
      CNode *_Node = new CNode(_RAWNode,_TheNodes);
      if(ISNULL(_Node))
      {
         THROW_MQEXCEPTION("new _Node failure!");
         return;
      }
        
      _List.push_back(_Node);  
   }    
}

CNodeArray::~CNodeArray()
{
   for(UINT i=0;i<_List.size();i++)
   {
      if(_List[i]!=NULL)
      {
         delete _List[i];
         _List[i] = NULL;
      }
   }
   _List.clear();
}

const CNode *CNodeArray::operator[](UINT _Id) const
{
   if((_Id < 1)||(_Id > _Nodes.Pages()))
   {
      THROW_MQEXCEPTION("Node Id out of range in NodeArray!");
      return NULL;
   }
    
   CNode *_Node = _List[(_Id-1)];
    
   if(ISNULL(_Node))
   {
      THROW_MQEXCEPTION("_Node Is NULL!");
      return NULL;
   }
    
   if(_Node->Id()!=_Id)
   {
      THROW_MQEXCEPTION("_Node Id Incorrect!");
      return NULL; 
   }
    
    
   return _Node;
}

/*CNodes*/
CNodes::CNodes(const CAnchor &_TheAnchor)
   :_Anchor(_TheAnchor),_RAWNodes(*(_Anchor.GetNodesEntry())), \
   	_Array(_RAWNodes,*this)
{
    
}

CNodes::~CNodes()
{
    
}

PSQNODES CNodes::Entry() const
{
    return &_RAWNodes; 
}

UINT CNodes::Pages() const
{
   return Entry()->_Pages;
}

UINT CNodes::PageSize() const
{
   return Entry()->_PageSize;    
}

const CNodeArray &CNodes::Items() const
{
   return _Array;    
}

const CAnchor & CNodes::Anchor() const
{
	return _Anchor;	
}

/*CListByNodes*/
CListByNodes::CListByNodes(SQLIST &_TheEntry,const CNodes &_TheNodes)
    :_RAWList(_TheEntry),_Nodes(_TheNodes)
{

}

CListByNodes::~CListByNodes()
{
    
}

PSQLIST CListByNodes::RAWEntry() const
{
    return &_RAWList;    
}

const CNodes &CListByNodes::Nodes() const
{
    return _Nodes;   
}

UINT CListByNodes::Total() const
{
   return RAWEntry()->_Total;   
}

bool CListByNodes::Empty() const
{
   return (Total()==0);    
}

void CListByNodes::Put2Head(CNodes::THEPAGEID _PageId) const
{
   const CNode *_Node = Nodes().Items()[_PageId];
   Put2Head(*_Node); 
}

void CListByNodes::Put2Head(const CNode &_TheNode) const
{
   CNodes::THEPAGEID _PageId = _TheNode.Id();
   _TheNode.Next(RAWEntry()->_Head);
    
   if(RAWEntry()->_Head == 0)
   {
      RAWEntry()->_Head =  RAWEntry()->_Tail = _PageId;
   }
   else
   {
      RAWEntry()->_Head = _PageId;
   }
    
   RAWEntry()->_Total++;  
}

void CListByNodes::Put2Tail(CNodes::THEPAGEID _PageId) const
{
   const CNode *_Node = Nodes().Items()[_PageId];
   Put2Tail(*_Node);   
}

void CListByNodes::Put2Tail(const CNode &_TheNode) const
{
    CNodes::THEPAGEID _PageId = _TheNode.Id();
   _TheNode.Next(0);

   if(RAWEntry()->_Head == 0)
   {
      RAWEntry()->_Head =  RAWEntry()->_Tail = _PageId;
   }
   else
   {
      const CNode *_Tail = Nodes().Items()[RAWEntry()->_Tail];
      /*
      if(_Tail->Id()!=RAWEntry()->_Tail)
      {
         THROW_MQEXCEPTION("Tail Page Id Confused!");
         return;    
      }
      */
      _Tail->Next(_PageId);
      RAWEntry()->_Tail      = _PageId;
   }
    
   RAWEntry()->_Total++; 
}

CNodes::THEPAGEID CListByNodes::Get() const
{
   CNodes::THEPAGEID _Result = 0;

   if(!Empty())
   {
      const CNode *_Node = Nodes().Items()[RAWEntry()->_Head];
      if(RAWEntry()->_Head  ==  RAWEntry()->_Tail)
      {
         RAWEntry()->_Head = RAWEntry()->_Tail = 0;
      }
      else
      {
         RAWEntry()->_Head = _Node->Next();
      }

      RAWEntry()->_Total--;

      _Node->Next(0);

      _Result = _Node->Id(); 
   }
    
   return _Result;  
}

