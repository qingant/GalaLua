/* -*- C++ -*- */
// Time-stamp: <2012-08-28 15:36:45 星期二 by tao>
/**
 * @file    File.cpp
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

#include "File.hpp"
#include "Exception.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {

      CFile::CFile(CPSTR name, INT flag, mode_t mode)
         :CIOHandle(CRT_open(name, flag, mode))
      {

      }
      CFile::CFile(INT fd)
         :CIOHandle(fd)
      {

      }
      ssize_t CFile::Readv(const struct iovec *vector, INT count)
      {
         return this->readv(vector, count);
      }
      ssize_t CFile::Writev(const struct iovec *vector, INT count)
      {
         return this->writev(vector, count);
      }
       off_t   CFile::Lseek(off_t offset, int whence)
       {
           return CRT_lseek(GetFD(), offset, whence);
       }

      size_t CFile::Size()
      {
         struct stat buf = {0};
         this->Fstat(&buf);
         return buf.st_size;
      }
      void CFile::Fstat(struct stat *buf)
      {
         if(this->fstat(buf)==-1)
         {
			THROW_SYSERROR();
         }
      }
      void CFile::Ftruncate(off_t length)
      {
         if (this->ftruncate(length) == -1)
         {
			THROW_SYSERROR();
         }
      }

      INT CFile::Fcntl(int cmd)
      {
         return this->fcntl(cmd);
      }
      INT CFile::Fcntl(int cmd, long arg)
      {
         return this->fcntl(cmd, arg);
      }
      INT CFile::Fcntl(int cmd, struct flock *lock)
      {
         return this->fcntl(cmd, lock);
      }


      void CFile::Fsync()
      {
         if (CRT_fsync(GetFD()) == -1)
         {
			THROW_SYSERROR();
         }
      }
      void CFile::Fdatasync()
      {
         if (CRT_fdatasync(GetFD()) == -1)
         {
			THROW_SYSERROR();
         }
      }

      void CFile::Flock(int operation)
      {
#if (!defined(_AIX) && !defined(__FreeBSD__))

         if (flock(operation) == -1)
         {
			THROW_SYSERROR();
         }
#else
         THROW_SYSERROR();
#endif
      }

      CFile::~CFile()
      {

      }

      CFileStream::CFileStream(const std::string &name, const std::string &mode)
         : m_file(fopen(name.c_str(), mode.c_str()))
      {

      }
      CFileStream::CFileStream(INT fd, const std::string &mode)
         :m_file(fdopen(fd, mode.c_str()))
      {

      }
      size_t CFileStream::Fread(void *ptr, size_t size, size_t nmemb) const
      {
         return fread(ptr, size, nmemb, m_file);
      }
      size_t CFileStream::Fwrite(const void *ptr, size_t size, size_t nmemb) const
      {
         return fwrite(ptr, size, nmemb, m_file);
      }
      void CFileStream::Fflush() const        // flush the user-space buffer but not the os cache
      {
         if (fflush(m_file) == -1)
         {
			THROW_SYSERROR();
         }
      }

      INT CFileStream::GetFD() const
      {
         INT ret = fileno(m_file);
         if (ret == -1)
         {
			THROW_SYSERROR();
         }
         return ret;
      }

      CFileStream::~CFileStream()
      {
      }



      CFileMapping::CFileMapping(CFile *file, size_t size, INT prot, bool lock)
         :_pShm(NULL),
          _Size(size),
          _isLocked(lock)
      {
          struct stat st_buf = {0};
          
         file->Fstat(&st_buf);

         if (st_buf.st_size < this->_Size)
         {
			file->Ftruncate(size);
         }

         if (size == 0)
         {
			_Size = st_buf.st_size;
         }
         _pShm = this->Attach(file->GetFD(), prot);
         if (lock)
         {
			// if (CRT_mlock(this->_pShm, this->_Size) == -1)
			// {
			//     THROW_SYSERROR();
			// }

         }

      }
      CFileMapping::~CFileMapping()
      {
         if (this->_isLocked)
         {
			// if (CRT_munlock(this->_pShm, this->_Size) == -1)
			// {
			//     THROW_SYSERROR();
			// }
         }
         Detach(this->_Size);
      }
      void CFileMapping::Detach(size_t len)
      {
         if (_pShm == MAP_FAILED)
         {
			return;
         }
         INT ret = CRT_munmap(_pShm, len);
         if (ret == -1)
         {
			THROW_SYSERROR();
         }
      }

      PVOID CFileMapping::Attach(INT fd, INT prot)
      {
         PVOID pShm = NULL;
         pShm = CRT_mmap(NULL,this->_Size, prot, MAP_SHARED, fd, 0);
         if (pShm == MAP_FAILED)
         {
			THROW_SYSERROR();
         }
         return pShm;

      }


      void CFileMapping::Msync(size_t len, INT flag)
      {
         INT ret = CRT_msync(_pShm, len, flag);
         if (ret == -1)
         {
			THROW_SYSERROR();
         }

      }
      PVOID CFileMapping::Get() const
      {
         if (_pShm == MAP_FAILED)
         {
			THROW_SYSERROR();
         }
         return this->_pShm;
      }
      off_t CFileMapping::GetSize() const
      {
         return _Size;
      }

      void CFileMapping::Msync(INT flag)
      {
         this->Msync(_Size, flag);
      }

   } /// namespace GalaxyRT
} /// namespace Galaxy
