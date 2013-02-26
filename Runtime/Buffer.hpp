/* -*- C++ -*- */

/* Time-stamp: <2012-04-18 14:31:11 ������ by OCaml> */

/**
 * @file   Buffer.hpp
 * @author Luo.ZhiHui <camel.flying@gmail.com>
 * @date   Wed Apr 18 14:30:53 2012
 *
 * @bref CBuffer �������Ĳ�����һ���ļ�������Զ�д
 *
 */

#ifndef BUFFER_H
#define BUFFER_H

#include "Object.hpp"
#include "Utils.hpp"
#include "Sync.hpp"
#include <stdexcept>

#pragma once

namespace Galaxy
{
   namespace GalaxyRT
   {
      
      class CBuffer:public CNonCopyAble
      {
      public:
         CBuffer(size_t buflen);
         virtual ~CBuffer();
         BOOL Writeable(size_t nbytes) const;
         void Write(CPVOID ptr, size_t nbytes) const;
         std::string Read() const;
         /*
          *����ԭ��buffer�е�����
          */
         std::string ResetBufferDepth(size_t depth);

      private:
         mutable std::string         m_buffer;
         CPthreadMutex               m_mutex;
      };
      
   } /// namespace GalaxyRT
} /// namespace Galaxy

#endif /*BUFFER_H*/
