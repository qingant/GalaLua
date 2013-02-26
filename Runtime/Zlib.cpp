/* -*- C++ -*- */

/* Time-stamp: <2012-08-08 17:31:48 星期三 by 骆志辉> */

/**
 * @file   Zlib.cpp
 * @author 骆志辉 <camel_flying@163.com>
 * @date   Fri Aug  3 15:09:39 2012
 *
 * @bref CZlib主要用于打包和压缩数据
 *
 */

#include "Zlib.hpp"

namespace Galaxy   
{
   
   namespace GalaxyRT   
   {

      CCompress::CCompress(const unsigned char* const uncompr, const ULONG uncomprLen, const INT level)
         :_comprLen(compressBound( uncomprLen )), _compr( _comprLen, 0 )
      {
         int err;

         if( level == -1 )
         {
            err = compress( (PBYTE)&_compr[0], &_comprLen, uncompr, uncomprLen);
            if(err != Z_OK)
            {
               char buf[1024];
               CRT_memset(buf, 0x00, sizeof(buf));
               snprintf(buf, sizeof(buf), "compress error: %d\n", err);
               THROW_GENERALERROR(CZlibException, buf);
            }
         }
         else
         {
            err = compress2( (PBYTE)&_compr[0], &_comprLen, uncompr, uncomprLen, level );
            if(err != Z_OK)
            {
               char buf[1024];
               CRT_memset(buf, 0x00, sizeof(buf));
               snprintf(buf, sizeof(buf), "compress2 error: %d\n", err);
               THROW_GENERALERROR(CZlibException, buf);
            }
         }

         _compr.resize( _comprLen );
      }

      const std::string&  CCompress::GetCompr() const
      {
         return _compr;
      }

      const unsigned char* CCompress::GetCompr(ULONG * const comprLen) const
      {
         *comprLen = _comprLen;

         return ( PBYTE )&_compr[ 0 ];
      }

      CUncompress::CUncompress( const unsigned char* const compr, const ULONG comprLen )
         :_uncomprLen(comprLen + 100), _uncompr( _uncomprLen, 0 )
      {
         int err;
         
         do
         {
            err = uncompress((unsigned char* )&_uncompr[0], &_uncomprLen, compr, comprLen);
            if( err == Z_BUF_ERROR)
            {
               _uncomprLen=_uncomprLen*2;
               _uncompr.resize( _uncomprLen );
               continue;
            }
            else if( err == Z_DATA_ERROR )
            {
               THROW_GENERALERROR(CZlibException, "uncompress Z_DATA_ERROR: the input data was corrupted or incomplete.\n");
            }
            else if( err == Z_MEM_ERROR )
            {
               THROW_GENERALERROR(CZlibException, "uncompress Z_MEM_ERROR:  there was not enough memory");
            }
            
            break;
         } while (1);

         _uncompr.resize( _uncomprLen );
      }
      
      const unsigned char * CUncompress::GetUncompr(ULONG* const uncomprLen) const
      {
         *uncomprLen = _uncomprLen;
         
         return ( unsigned char *)&_uncompr[0];
      }
      
      const std::string& CUncompress::GetUncompr() const
      {
         return _uncompr;
      }


//      CZlibfile::CZlibfile(const char *filename, const char *mode)
//      {
//         _file = gzopen(filename, mode);
//         if(ISNULL(_file))
//         {
//            char buf[1024];
//            CRT_memset(buf, 0x00, sizeof(buf));
//            snprintf(buf, sizeof(buf), "gzopen ");
//            THROW_GENERALERROR(CZlibException, buf);
//         }
//      }
// 
//      CZlibfile::CZlibfile(int fd, const char *mode)
//      {
//         _file = gzdopen(fd, mode);
//         if(ISNULL(_file))
//         {
//            THROW_GENERALERROR(CZlibException, "failure to gzdopen\n");
//         }
//      }
// 
//      CZlibfile::~CZlibfile()
//      {
//         this->close();
//      }
// 
//      void CZlibfile::SetBuffer(unsigned size)
//      {
//         int iret;
//         iret = gzbuffer(_file, size);
//         if(iret != 0)
//         {
//            char buf[1024];
//            CRT_memset(buf, 0x00, sizeof(buf));
//            snprintf(buf, sizeof(buf), "gzbuffer");
//            THROW_GENERALERROR(CZlibException, buf);
//         }	
//      }
// 
//      void CZlibfile::read(void *buffer, unsigned len)
//      {
//         int iret;
//         iret = gzread(_file, buffer, len);
//         if(iret == -1)
//         {
//            char buf[1024];
//            CRT_memset(buf, 0x00, sizeof(buf));
//            snprintf(buf, sizeof(buf), "gzread");
//            THROW_GENERALERROR(CZlibException, buf);
//         }
//      }
// 
//      void CZlibfile::write(const void *buffer, unsigned len)
//      {
//         int iret;
//         iret = gzwrite(_file, buffer, len);
//         if(iret == 0)
//         {
//            char buf[1024];
//            CRT_memset(buf, 0x00, sizeof(buf));
//            snprintf(buf, sizeof(buf), "gzwrite");
//            THROW_GENERALERROR(CZlibException, buf);
//         }
//      }
// 
//      void CZlibfile::puts(const char *s)
//      {
//         int iret;
//         iret = gzputs(_file, s);
//         if(iret == -1)
//         {
//            char buf[1024];
//            CRT_memset(buf, 0x00, sizeof(buf));
//            snprintf(buf, sizeof(buf), "gzputs");
//            THROW_GENERALERROR(CZlibException, buf);
//         }
//      }
// 
//      char *CZlibfile::gets(char *buf, int len)
//      {
//         char *iret;
//         iret = gzgets(_file, buf, len);
//         if(ISNULL(iret))
//         {
//            char buf[1024];
//            CRT_memset(buf, 0x00, sizeof(buf));
//            snprintf(buf, sizeof(buf), "gzgets");
//            THROW_GENERALERROR(CZlibException, buf);
//         }
// 
//         return iret;
//      }
// 
//      int CZlibfile::getc()
//      {
//         int iret;
//         iret = gzgetc(_file);
//         if(iret == -1)
//         {
//            char buf[1024];
//            CRT_memset(buf, 0x00, sizeof(buf));
//            snprintf(buf, sizeof(buf), "gzgetc");
//            THROW_GENERALERROR(CZlibException, buf);
//         }
// 
//         return iret;			  
//      }
// 
//      void CZlibfile::putch(int ch)
//      {
//         int iret;
//         iret = gzputc(_file, ch);
//         if(iret == -1)
//         {
//            char buf[1024];
//            CRT_memset(buf, 0x00, sizeof(buf));
//            snprintf(buf, sizeof(buf), "gzputs");
//            THROW_GENERALERROR(CZlibException, buf);
//         }
//      }
// 
//      void CZlibfile::flush(int flush)
//      {
//         int iret;
//         iret = gzputc(_file, flush);
//         if(iret == -1)
//         {
//            char buf[1024];
//            CRT_memset(buf, 0x00, sizeof(buf));
//            snprintf(buf, sizeof(buf), "gzputs");
//            THROW_GENERALERROR(CZlibException, buf);
//         }
//      }
// 
// 
//      void CZlibfile::close()
//      {
//         int iret;
//         iret = gzclose(_file);
//         if(iret != Z_OK)
//         {
//            char buf[1024];
//            CRT_memset(buf, 0x00, sizeof(buf));
//            snprintf(buf, sizeof(buf), "gzclose(): %s", error(&iret));
//            THROW_GENERALERROR(CZlibException, buf);
//         }
//      }
// 
//      const char *CZlibfile::error(int *errnum)
//      {
//         return gzerror(_file, errnum);
//      }
// 
//      void CZlibfile::clearerr()
//      {
//         gzclearerr(_file);
//      }
      
   }; // namespace GalaxyRT end here
   
}; // namespace Galaxy end here

