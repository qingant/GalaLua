/* -*- C++ -*- */

// Time-stamp: <2012-04-17 10:00:29 星期二 by OCaml>

/**
 * @file    Pipe.cpp
 * @author  Ma Tao <qingant@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 *
 * @section DESCRIPTION
 *
 *
 */
#include "Pipe.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {

#define MY_THROW() (throw CSysError(__FILE__, __PRETTY_FUNCTION__, __LINE__) )


      CPipe::CPipe()
      {
         INT fd[2];
         if (CRT_pipe(fd) == -1)
         {
			MY_THROW();
         }
         m_reader.reset(new CIOHandle(fd[0]));
         m_writer.reset(new CIOHandle(fd[1]));
      }

      std::auto_ptr<CIOHandle> CPipe::GetReader()
      {
         return m_reader;
      }

      std::auto_ptr<CIOHandle> CPipe::GetWriter()
      {
         return m_writer;
      }

      void CPipe::Close()
      {
         m_reader->Close();
         m_writer->Close();
      }

      CPipe::~CPipe()
      {

      }



      CSocketPair::CSocketPair()
      {
         INT fd[2];

         if (CRT_socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1)
         {
			MY_THROW();
         }
         m_one.reset(new CUNSocket(fd[0]));
         m_another.reset(new CUNSocket(fd[1]));
      }

      CUNSocket *CSocketPair::GetOne()
      {
         return m_one.get();
      }
      CUNSocket *CSocketPair::GetAnother()
      {
         return m_another.get();
      }
      void CSocketPair::Close()
      {
         m_one->Close();
         m_another->Close();
      }
      CSocketPair::~CSocketPair()
      {

      }
   
      CUNSocket *CSocketPair::operator [] (int idx)
      {
         if (idx == 0)
         {
			return this->GetOne();
         }
         else if (idx == 1)
         {
			return this->GetAnother();
         }
         else
         {
			THROW_EXCEPTION(0, "INVALID INDEX");
			return NULL;
         }
      }

   } /// namespace GalaxyRT
} /// namespace Galaxy
