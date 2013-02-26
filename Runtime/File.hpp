/* -*- C++ -*- */

/* Time-stamp: <2012-08-28 14:40:26 星期二 by tao> */

/**
 * @file File.hpp
 * @author qingant
 */

#ifndef FILE_H
#define FILE_H

#include "Handle.hpp"
#pragma once

namespace Galaxy
{
    namespace GalaxyRT
    {

        class CFile:public CIOHandle
        {
        public:
            //ssize_t read(PVOID Buffer, size_t Length) const;
            //ssize_t write(CPVOID Buffer, size_t Length) const;
            CFile(CPSTR name, INT flag, mode_t mode);
            CFile(INT fd);
            ~CFile();
            ssize_t Readv(const struct iovec *vector, INT count);
            ssize_t Writev(const struct iovec *vector, INT count);
            off_t   Lseek(off_t offset, int whence);
            void Fstat(struct stat *buf);
            size_t Size() ;
            void Ftruncate(off_t length);
          
            INT Fcntl(int cmd);
            INT Fcntl(int cmd, long arg);
            INT Fcntl(int cmd, struct flock *lock);
            void Flock(INT operation);
            void Fsync();
            void Fdatasync();
        };

        class CFileStream:public CNonCopyAble
        {
        public:
            CFileStream(const std::string &name, const std::string &mode);
            CFileStream(INT fd, const std::string &mode);
            ~CFileStream();
            size_t Fread(void *ptr, size_t size, size_t nmemb) const;
            size_t Fwrite(const void *ptr, size_t size, size_t nmemb) const;
            void Fflush() const;        // flush the user-space buffer but not the os cache
            INT GetFD() const;
        private:
            FILE *m_file;

        };

        class CFileMapping: CNonCopyAble
        {
        public:
            CFileMapping(CFile *file, size_t size, INT prot = PROT_READ, bool lock = true);
            PVOID Get() const;
            off_t GetSize() const;
            void Msync(size_t len, INT flag = MS_SYNC);
            void Msync(INT flag = MS_SYNC);
            virtual ~CFileMapping();
        private:
            PVOID Attach(INT, INT);
            void  Detach(size_t len);

        private:
            PVOID _pShm;
            off_t _Size;
            bool _isLocked;
        };
   
    } /// namespace GalaxyRT
} /// namespace Galaxy

#endif /* FILE_H */
