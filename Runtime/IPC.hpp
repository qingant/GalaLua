/* -*- C++ -*- */

// Time-stamp: <2012-04-17 10:00:57 星期二 by OCaml>


/**
 * @file CIPC.h
 * @author qingant
 */
#ifndef CIPC_H
#define CIPC_H
#include <cstub.h>

#include "Object.hpp"
#include "Exception.hpp"
#include "Handle.hpp"
#include "File.hpp"


#pragma once

namespace Galaxy
{
   namespace GalaxyRT
   {

#define URW_PERM  (0400|0200)

      class CIPC : public CNonCopyAble
      {
      public:
         virtual ~CIPC();
      };


      class CMsgQ : public CIPC
      {
         // FINAL: this class is not designed to be derieved
         // it's implemented with System V message queue
      public:
         CMsgQ(const std::string &name, INT oflags = IPC_CREAT|S_IRWXU);

         INT Remove();
         // INT GetAttr();
         // INT SetAttr();
         INT Send(CPVOID buf, size_t len, INT flag = 0) const;
         INT SendNoWait(const VOID *buf, size_t len) const;
         INT Recv(PVOID buf, size_t len, LONG type = 0, INT flag = 0) const;
         INT RecvNoWait(PVOID buf, size_t len, LONG type = 0) const;
         ~CMsgQ();
      private:
         INT MsgID;
         std::string Name;
         key_t Key;
      };

      class CPosixShmObj:public CIOHandle
      {
      public:
         CPosixShmObj(CPSTR name, INT oflag, mode_t mode = S_IRWXU);
         ~CPosixShmObj();
         const std::string &GetName() const;
         void Close();
      private:
         std::string m_name;
      };




// struct ShmStramp
// {
//     char magic[4];
//     INT  size;
//     char timeStramp[30];
// }
      class CSHM:public CIPC
      {
         // This class is not designed to be derieved
         // implemented with POSIX shared memory
         // TODO: add header
         //
      public:
         CSHM(std::auto_ptr<CIOHandle> handle, size_t size);
         void Unlink();
         PVOID Attach();
         PVOID Attach(INT);
         void Msync(size_t len, INT flag = MS_SYNC);
         void Msync(INT flag = MS_SYNC);
         void   Detach();
         virtual ~CSHM();
      private:
         void   Detach(size_t);
      private:
         std::auto_ptr<CIOHandle> m_handle;
         PVOID                    m_pShm;
         size_t                   m_size;
      };




      class CSystemVSHM:public CIPC
      {
      public:
         CSystemVSHM(key_t key, size_t size, INT oflag = IPC_CREAT|S_IRWXU);
         PVOID GetSHM() const;
         void  Detach();
         void  Remove();
         ~CSystemVSHM();
      private:
         key_t  m_key;
         INT    m_identifier;
         PVOID  m_pshm;
      };

   } /// namespace GalaxyRT
} /// namespace Galaxy

#endif
