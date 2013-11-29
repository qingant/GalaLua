/* -*- C++ -*- */

/* Time-stamp: <2012-04-20 16:48:25 星期五 by OCaml> */

/**
 * @file
 * @author john.lee,qingant
 */

#ifndef CHANDLE_H
#define CHANDLE_H

#include <cstub.h>

#pragma once

#include "Object.hpp"
#include "Exception.hpp"

namespace Galaxy
{

   namespace GalaxyRT
   {
      class CSockTimeOut: public CException
      {
      public:
         CSockTimeOut(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
			:CException(file, func, line, errcode, control, msg){}
         ~CSockTimeOut() throw(){}
         std::string GetClassName() const
         {
			return std::string("CSockTimeOut");
         }
      };
      
      class CSockPeerClose: public CException
      {
      public:
         CSockPeerClose(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
			:CException(file, func, line, errcode, control, msg){}
         ~CSockPeerClose() throw(){}
         std::string GetClassName() const
         {
			return std::string("CSockPeerClose");
         }
      };

      class CSockWouldBlock: public CException
      {
      public:
         CSockWouldBlock(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
			:CException(file, func, line, errcode, control, msg){}
         ~CSockWouldBlock() throw(){}
         std::string GetClassName() const
         {
			return std::string("CSockWouldBlock");
         }
      };

      class CIOHandle:public CNonCopyAble
      {
      private:
         INT mFd;
      protected:
         ssize_t read(PVOID Buffer, size_t Length) const;
         ssize_t write(CPVOID Buffer, size_t Length) const;

         ssize_t readv(const struct iovec *vector, INT count) const;
         ssize_t writev(const struct iovec *vector, INT count) const;

         INT fstat(struct stat *buf) ;
         INT ftruncate(off_t length);

         INT fcntl(INT cmd) ;
         INT fcntl(INT cmd, long arg) ;
         INT fcntl(INT cmd, struct flock *lock);
         INT flock(INT operation) const;
         INT close();
      public:
         ssize_t Read(PVOID Buffer, size_t Length) const;
         ssize_t Write(CPVOID Buffer, size_t Length) const;
         virtual void Close();
      public:
         CIOHandle();
         CIOHandle(INT fd);
         virtual ~CIOHandle();
         INT GetFD() const;
      };

      class CSocketHandle:public CIOHandle
      {
      public:
         typedef INT SOCKET;
         CSocketHandle(SOCKET socket);
         virtual ~CSocketHandle();
      private:
         ssize_t send(const void *buf, size_t len, INT flags) const;
         ssize_t sendmsg(const struct msghdr *msg, INT flags) const;
         ssize_t recv(void *buf, size_t len, INT flags) const;
         ssize_t recvmsg(struct msghdr *msg, INT flags) const;

         INT accept(struct sockaddr *addr, socklen_t *addrlen);
         INT connect(const struct sockaddr *serv_addr, socklen_t addrlen);
         INT bind(struct sockaddr *my_addr, socklen_t addrlen);
         INT listen(INT backlog);

         INT shutdown(INT how);

         INT setsockopt(INT level, INT optname, const void *optval, socklen_t optlen);

         INT select(fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) const;


         INT sendwait(SHORT timeout) const;
         INT recvwait(SHORT timeout) const;
      public:
         VOID SendWait(SHORT timeout) const;
         VOID RecvWait(SHORT timeout) const;
         ssize_t Send(const void *buf, size_t len) const;
         ssize_t Recv(void *buf, size_t len) const;
         ssize_t SendMsg(const struct msghdr *msg) const;
         ssize_t RecvMsg(struct msghdr *msg) const;

         ssize_t Accept(struct sockaddr *addr, socklen_t *addrlen);
         bool Connect(const struct sockaddr *serv_addr, socklen_t addrlen);
         void Connect(const struct sockaddr *serv_addr, socklen_t addrlen,SHORT timeout);
         bool Bind(struct sockaddr *my_addr, socklen_t addrlen);
         bool Listen(INT backlog);

         bool Shutdown(INT how);
         bool SetSockOpt(INT level, INT optname, const void *optval, socklen_t optlen);
      };

      class FlagGuard
      {
      public:
          FlagGuard(int fd,int flag);
          ~FlagGuard();
      private:
          int _fd;
          int _flags;
      };
/*
  class CFileHandle:public CIOHandle
  {
  
  };
*/

   } /// namespace GalaxyRT
} ///namespace Galaxy

#endif
