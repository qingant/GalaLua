/* -*- C++ -*- */

/* Time-stamp: <2012-04-17 09:42:48 星期二 by OCaml> */

/**
 * @file Dll.hpp
 * @author qingant
 */


#include "Object.hpp"
#include "Exception.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {
      class NotInTransaction:public CException
      {
      public:
         GALA_DEFINE_EXCEPTION
      };
      class OperationNotCompleted:public CException
      {
      public:
         GALA_DEFINE_EXCEPTION
      };
      template <typename _Resource>
      class Transaction:public CNonCopyAble
      {
      public:
         typedef void (*PROCESS)(_Resource);
      public:
         Transaction()
			:_IsInTransaction(false){
			
         }
         virtual void BeginTransaction(){
			_Log = Log();
         }
         virtual void Do(PROCESS func){
			if (!IsInTransaction()){
               THROW_ERROR(NotInTransaction)
                  }
			try{
               func(_Log);
			}
			catch (const CException &e){
               _IsInTransaction = false;
               throw e;
			}
         }
         virtual void Commit() = 0;
         virtual void RollBack(){
			
         }
         virtual ~Transaction(){
			
         }
      protected:
         virtual _Resource Log() = 0;
         virtual bool IsInTransaction() const{
			return _IsInTransaction;
         }
         virtual _Resource GetNewCopy() const = 0;
      protected:
         _Resource _Log;
         bool      _IsInTransaction;
      };
     
   } /// namespace GalaxyRT
} /// namespace Galaxy


