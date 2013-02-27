#ifndef MQNODES_HPP_8E259A4C_E22E_42B0_8D9C_9D38AD943BE0
#define MQNODES_HPP_8E259A4C_E22E_42B0_8D9C_9D38AD943BE0

#pragma once

#include "MQAnchor.hpp"
namespace Galaxy
{
   namespace AMQ
   {
      class CNodes;
      class CNodeArray;

      class CNode  : public CMQLoneAncestor
      {
      public:
        typedef enum
        {
            QTYP_NQ = 1,
            QTYP_DQ = 2   
        }EMQTYPE;
      private:
      	 SQNODE 			&_NodeEntry;
         const CNodes       &_Nodes;
         

         inline PSQNODE Entry() const;
      public:
         explicit CNode(SQNODE &_TheEntry,const CNodes &_TheNodes);
         ~CNode();
  
         UINT Id() const;

         UINT Offset() const;
         PBYTE PageEntry() const;
    
         UINT Next() const;
         void Next(UINT _next) const;
         UINT Length() const;
         void Length(UINT _length) const;
         
         UINT State() const;
         
         void Occupy(EMQTYPE _Type,UINT _QID) const;
         void Release() const;
         bool IsReleased() const;
         
      };

      class CNodeArray : public CMQLoneAncestor
      {
      private:
      	 const CNodes 		 &_Nodes;
         std::vector<CNode *> _List; 
      public:
         explicit CNodeArray(SQNODES & _TheEntry,const CNodes &_TheNodes);
         ~CNodeArray();
         const CNode *operator[](UINT _Id) const;
      };

      class CNodes : public CMQLoneAncestor
      {
      public:
         typedef UINT THEPAGEID;
      private:
         const CAnchor           &_Anchor;
         SQNODES				 &_RAWNodes; 
         CNodeArray              _Array;

         inline PSQNODES Entry() const;   
      public:
         explicit CNodes(const CAnchor &_TheAnchor);
         ~CNodes();
         const CAnchor & Anchor() const;
         
         UINT Pages() const;
         UINT PageSize() const;
    
         const CNodeArray &Items() const;  
      };

      class CListByNodes : public CMQLoneAncestor
      {
      private:
         SQLIST         &_RAWList;
         const CNodes   &_Nodes;

         inline PSQLIST RAWEntry() const;
      protected:
         const CNodes &Nodes() const;
    
         void Put2Head(CNodes::THEPAGEID _PageId) const;
         void Put2Head(const CNode &_TheNode) const;
         void Put2Tail(CNodes::THEPAGEID _PageId) const;
         inline void Put2Tail(const CNode &_TheNode) const;
      public:
         explicit CListByNodes(SQLIST &_TheEntry,const CNodes &_TheNodes);
         virtual ~CListByNodes();
         UINT Total() const;
         bool Empty() const;
         CNodes::THEPAGEID Get() const;
      };

   } // AMQ namespace
} // Galaxy namespace

#endif /*MQNODES_HPP_8E259A4C_E22E_42B0_8D9C_9D38AD943BE0*/
