/* -*- C++ -*- */

/* Time-stamp: <2012-11-05 15:17:05 ÐÇÆÚÒ» by matao> */

/**
 * @file   Buffer.cpp
 * @author Luo.ZhiHui <camel.flying@gmail.com>
 * @date   Wed Apr 18 14:30:21 2012
 *
 * @bref
 *
 */

#include "Buffer.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {

      CBuffer::CBuffer(size_t buflen)
      {
         try
         {
			m_buffer.reserve(buflen);
			//m_buffer.resize(buflen);
         }
         catch(std::length_error &e)
         {
			THROW_EXCEPTION_EX(e.what());
         }
      }



      CBuffer::~CBuffer()
      {
          //GALA_DEBUG("~CBuffer");
      }



      BOOL CBuffer::Writeable(size_t nbytes) const
      {
         if(nbytes <= (m_buffer.capacity() - m_buffer.size()))
         {
			return TRUE;
         }

         return FALSE;
      }



      void CBuffer::Write(CPVOID ptr, size_t nbytes) const
      {
         //CLockGuard lock(&m_mutex);
         size_t old_size = m_buffer.size();
         m_buffer.resize(old_size+nbytes);
         CRT_memcpy(&m_buffer[old_size], (CPSTR)ptr, nbytes);
      }



      std::string CBuffer::Read() const
      {
         //CLockGuard lock(&m_mutex);
         std::string tmp(m_buffer);
         m_buffer.clear();

         return tmp;
      }
      
      std::string CBuffer::ResetBufferDepth(size_t depth)
      {
         //CLockGuard lock(&m_mutex);
         std::string tmp(m_buffer);
         this->m_buffer.resize(depth);

         return tmp;
      }

   } /// namespace GalaxyRT
} /// namespace Galaxy
