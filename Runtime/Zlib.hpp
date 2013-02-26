/* -*- C++ -*- */

/* Time-stamp: <2012-08-11 14:29:44 星期六 by 骆志辉> */

/**
 * @file   Zlib.hpp
 * @author 骆志辉 <camel_flying@163.com>
 * @date   Fri Aug  3 15:10:24 2012
 *
 * @bref CZlib主要用于打包和压缩数据 Archive
 *
 */


#ifndef _RUNTIME_ZLIB_HPP_
#define _RUNTIME_ZLIB_HPP_

#pragma once

#include "Object.hpp"
#include "Exception.hpp"
#include <zlib.h>

namespace Galaxy   
{
   namespace GalaxyRT   
   {

      class CZlibException:public CException
      {
      public:
         CZlibException(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
            :CException(file, func, line,errcode,control,msg)
         {
         }
         ~CZlibException() throw(){}
         std::string GetClassName() const
         {
            return std::string("CZlibException");
         }
      };

      class CCompress: public CNonCopyAble
      {
      public:
         CCompress(const unsigned char* const uncompr, const ULONG uncomprLen, const INT level=-1);
         const unsigned char* GetCompr(ULONG* const comprLen) const;
         const std::string&  GetCompr() const;
      private:
         ULONG _comprLen;
         std::string _compr;
      };


      class CUncompress: public CNonCopyAble
      {
      public:
         CUncompress( const unsigned char * const compr, const ULONG comprLen);
         const unsigned char*  GetUncompr(ULONG* const comprLen) const;
         const std::string& GetUncompr() const;
      private:
         ULONG _uncomprLen;
         std::string _uncompr;
      };

      
//      class CZlibfile: public CNonCopyAble
//      {
//      public:
//         CZlibfile(const char *filename, const char *mode);
//         CZlibfile(int fd, const char *mode);
//         ~CZlibfile();
//         void SetBuffer(unsigned size=8196);
//         void read(void *buffer, unsigned len);
//         void write(const void *buffer, unsigned len);
//         void puts(const char *s);
//         char *gets(char *buf, int len);
// 
//         int getc();
//         void putch(int ch);
// 
//         void flush(int flush=Z_FINISH);
//         void close();
//         const char *error(int *errnum);
//         void clearerr();
//    
//      private:
//         gzFile _file;
//      };

      
   }; // namespace GalaxyRT end here   
   
}; // namespace Galaxy end here

#endif /* _RUNTIME_ZLIB_HPP_ */

