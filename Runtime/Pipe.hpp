/* -*- C++ -*- */

/* Time-stamp: <2012-04-17 10:00:33 星期二 by OCaml> */

/**
 * @file Pipe.hpp
 * @author qingant
 */

#ifndef PIPE_H
#define PIPE_H
#include "Handle.hpp"
#include "Socket.hpp"
#include "Exception.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {

      class CPipe:public CObject
      {
      public:
         CPipe();
         std::auto_ptr<CIOHandle> GetReader();
         std::auto_ptr<CIOHandle> GetWriter();
         void Close();
         ~CPipe();
      private:
         std::auto_ptr<CIOHandle> m_reader, m_writer;
      };

      class CSocketPair:public CObject
      {
      public:
         CSocketPair();
         CUNSocket *GetOne() ;
         CUNSocket *GetAnother() ;
         CUNSocket *operator [] (int);
         void Close();
         ~CSocketPair();
      private:
         std::auto_ptr<CUNSocket> m_one, m_another;

      };

   } /// namespace GalaxyRT
} /// namespace Galaxy
	
#endif /* PIPE_H */
