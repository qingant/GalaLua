/* -*- C++ -*- */

// Time-stamp: <2012-05-21 09:31:21 Monday by OCaml>


/**
 * @file CIOHandle.cpp
 * @author qingant
 */

#include "Handle.hpp"
#include "Exception.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {

       FlagGuard::FlagGuard(int fd,int flag):
           _fd(fd)
       {
           _flags =fcntl(_fd, F_GETFL, 0);

           int rt= fcntl(_fd, F_SETFL, _flags | flag);
           if (rt == -1)
           {
               THROW_SYSERROR( );
           }
       }

       FlagGuard::~FlagGuard()
       {
           fcntl(_fd, F_SETFL, _flags);
       }

#define IOHandle_THROW() (throw CIOError(__FILE__, __PRETTY_FUNCTION__, __LINE__) )


      CIOHandle::CIOHandle()
         :mFd(-1)
      {

      }

      CIOHandle::CIOHandle(INT fd)
         :mFd(fd)
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }
      }

      CIOHandle::~CIOHandle()
      {
         Close();
      }

      ssize_t CIOHandle::read(PVOID Buffer, size_t Length) const
      {
         return CRT_read(GetFD(), Buffer, Length);
      }

      ssize_t CIOHandle::write(CPVOID Buffer, size_t Length) const
      {
         return CRT_write(GetFD(), Buffer,Length);
      }

      ssize_t CIOHandle::readv(const struct iovec *vector, INT count) const
      {
         return CRT_readv(GetFD(),vector,count);
      }

      ssize_t CIOHandle::writev(const struct iovec *vector, INT count) const
      {
         return CRT_writev(GetFD(),vector,count);
      }

      INT CIOHandle::close()
      {
         return CRT_close(GetFD());
      }

      INT CIOHandle::fstat(struct stat *buf)
      {
         return  CRT_fstat(GetFD(),buf);
      }

      INT CIOHandle::ftruncate(off_t length)
      {
         return  CRT_ftruncate(GetFD(),length);
      }


      INT CIOHandle::fcntl(INT cmd)
      {
         return CRT_fcntl(GetFD(),cmd);
      }

      INT CIOHandle::fcntl(INT cmd, long arg)
      {
         return CRT_fcntl(GetFD(),cmd,arg);
      }

      INT CIOHandle::fcntl(INT cmd, struct flock *lock)
      {
         return CRT_fcntl(GetFD(),cmd,lock);
      }

      INT CIOHandle::GetFD() const
      {
         return mFd;
      }

      void CIOHandle::Close()
      {

         if(GetFD()!=-1)
         {
            if(close()==-1)
            {
               IOHandle_THROW();
            }
            else
            {
               mFd = -1;
            }
         }
      }

      INT CIOHandle::flock(INT operation) const
      {
#if  !defined(_AIX) && !defined(__FreeBSD__)
         return CRT_flock(GetFD(), operation);
#else
         IOHandle_THROW();
#endif
      }


      ssize_t CIOHandle::Read(PVOID Buffer, size_t Length) const
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         ssize_t rc = this->read(Buffer,Length);
         if(rc==-1)
         {
            IOHandle_THROW();
         }

         return (rc);
      }

      ssize_t CIOHandle::Write(CPVOID Buffer, size_t Length) const
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         ssize_t rc = this->write(Buffer,Length);
         if(rc==-1)
         {
            IOHandle_THROW();
         }

         return (rc);
      }


      /*CSocketHandle*/
      CSocketHandle::CSocketHandle(SOCKET socket)
         :CIOHandle(socket)
      {

      }

      CSocketHandle::~CSocketHandle()
      {

      }

      ssize_t CSocketHandle::send(const void *buf, size_t len, INT flags) const
      {
         return CRT_send(GetFD(),buf,len,flags);
      }

      ssize_t CSocketHandle::sendmsg(const struct msghdr *msg, INT flags) const
      {
         return CRT_sendmsg(GetFD(),msg,flags);
      }

      ssize_t CSocketHandle::recv(void *buf, size_t len, INT flags) const
      {
         return CRT_recv(GetFD(),buf,len,flags);
      }

      ssize_t CSocketHandle::recvmsg(struct msghdr *msg, INT flags) const
      {
         return ::recvmsg(GetFD(),msg,flags);
      }

      INT CSocketHandle::accept(struct sockaddr *addr, socklen_t *addrlen)
      {
         return CRT_accept(GetFD(),addr,addrlen);
      }

      INT CSocketHandle::connect(const struct sockaddr *serv_addr, socklen_t addrlen)
      {
         return CRT_connect(GetFD(),serv_addr,addrlen);
      }

      INT CSocketHandle::bind(struct sockaddr *my_addr, socklen_t addrlen)
      {
         return CRT_bind(GetFD(),my_addr,addrlen);
      }

      INT CSocketHandle::listen(INT backlog)
      {
         return CRT_listen(GetFD(),backlog);
      }

      INT CSocketHandle::shutdown(INT how)
      {
         return CRT_shutdown(GetFD(),how);
      }

      INT CSocketHandle::setsockopt(INT level, INT optname, const void *optval, socklen_t optlen)
      {
         return CRT_setsockopt(GetFD(),level,optname,optval,optlen);
      }

      INT CSocketHandle::select(fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) const
      {
         return CRT_select(GetFD()+1,readfds,writefds,exceptfds,timeout);
      }

      INT CSocketHandle::sendwait(SHORT timeout) const
      {
         struct pollfd pfd = {GetFD(),POLLOUT,0};

         if(timeout < 0)
         {
            return CRT_poll(&pfd, 1, -1);
         }
         else
         {
            return CRT_poll(&pfd, 1, timeout*1000);
         }
      }

      INT CSocketHandle::recvwait(SHORT timeout) const
      {
         struct pollfd pfd = {GetFD(),POLLIN,0};

         if(timeout < 0)
         {
            return CRT_poll(&pfd, 1, -1);
         }
         else
         {
            return CRT_poll(&pfd, 1, timeout*1000);
         }

      }
      
// INT CSocketHandle::recvwait(SHORT timeout) const
// {
// 	struct timeval		stmo;
// 	fd_set				rfdr;

// 	FD_ZERO(&rfdr);
// 	FD_SET(GetFD(),&rfdr );

// 	if(timeout < 0)
// 	{
// 		return select(&rfdr,NULL,NULL,NULL);
// 	}
// 	else
// 	{
// 		stmo.tv_sec  = timeout;
// 		stmo.tv_usec = 0;

// 		return select(&rfdr,NULL,NULL,&stmo);
// 	}
// }

      void CSocketHandle::SendWait(SHORT timeout) const
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         INT rc = this->sendwait(timeout);
         if(rc==-1)
         {
            THROW_SYSERROR( );
         }
         if (rc == 0)
         {
            char buf[ 1024 ];
            CRT_memset( buf, 0x00, sizeof( buf ) );
            CRT_snprintf( buf, sizeof( buf ), "socket fd = %d SendWait timeout", GetFD( ) );
            THROW_GENERALERROR( CSockTimeOut, buf );
         }
      }


      void CSocketHandle::RecvWait(SHORT timeout) const
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         INT rc = this->recvwait(timeout);
         if(rc==-1)
         {
            THROW_SYSERROR( );
         }
         if (rc == 0)
         {
            char buf[ 1024 ];
            CRT_memset( buf, 0x00, sizeof( buf ) );
            CRT_snprintf( buf, sizeof( buf ), "socket fd = %d RecvWait timeout", GetFD( ) );
            THROW_GENERALERROR( CSockTimeOut, buf );
         }
      }

      ssize_t CSocketHandle::Send(const void *buf, size_t len) const
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         ssize_t rc = this->send(buf,len,0);
         if( rc == -1 )
         {
            if( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK))
            {
               THROW_GENERALERROR( CSockWouldBlock, "the socket is market nonblocking and the send operation would block" );
            }
            
            THROW_SYSERROR( );
         }

         return (rc);
      }

      ssize_t CSocketHandle::Recv(void *buf, size_t len) const
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         ssize_t rc = this->recv(buf,len,0);
         if(rc==-1)
         {
            if( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
            {
               THROW_GENERALERROR( CSockWouldBlock, "the socket is market nonblocking and the recv operation would block" );
            }
            
            THROW_SYSERROR( );
         }
         else if (rc==1 && ((char *)buf)[0] == EOF)
         {
            THROW_GENERALERROR( CSockPeerClose, "socket peer close" );
         }
         else if (rc == 0)
         {
            THROW_GENERALERROR( CSockPeerClose, "recv return value 0, peer has performed and orderly close" );
         }

         return (rc);
      }

      ssize_t CSocketHandle::SendMsg(const struct msghdr *msg) const
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         ssize_t rc = this->sendmsg(msg,0);
         if( rc == -1 )
         {
            if( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
            {
               THROW_GENERALERROR( CSockWouldBlock, "the socket is market nonblocking and the sendmsg operation would block" );
            }
            
            THROW_SYSERROR( );
         }

         return (rc);
      }

      ssize_t CSocketHandle::RecvMsg(struct msghdr *msg) const
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         ssize_t rc = this->recvmsg(msg,0);
         if(rc==-1)
         {
            if(( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
            {
               THROW_GENERALERROR( CSockWouldBlock, "the socket is market nonblocking and the recvmsg operation would block" );
            }
            THROW_SYSERROR( );
         }
         else if (rc == 0)
         {
            THROW_GENERALERROR(CSockPeerClose, "recv return value 0, peer has performed and orderly close" );
         }


         return (rc);
      }

      ssize_t CSocketHandle::Accept(struct sockaddr *addr, socklen_t *addrlen)
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         ssize_t rc = this->accept(addr,addrlen);
         if(rc==-1)
         {
            if(( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
            {
               THROW_GENERALERROR( CSockWouldBlock, "the socket is market nonblocking and the accpet no connect" );
            }
            THROW_SYSERROR( );
         }

         return (rc);
      }


      void CSocketHandle::Connect(const struct sockaddr *serv_addr, socklen_t addrlen,SHORT timeout)
      {

          if(GetFD()==-1)
          {
              IOHandle_THROW();
          }

          FlagGuard f(GetFD(),O_NONBLOCK);
          ssize_t rc = this->connect(serv_addr,addrlen);
          int err=errno;
          if(rc==-1 && err==EINPROGRESS)
          {
              SendWait(timeout);
          }
      }

      bool CSocketHandle::Connect(const struct sockaddr *serv_addr, socklen_t addrlen)
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         ssize_t rc = this->connect(serv_addr,addrlen);
         if(rc==-1)
         {
            THROW_SYSERROR( );
         }

         return (rc!=-1);
      }

      bool CSocketHandle::Bind(struct sockaddr *my_addr, socklen_t addrlen)
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         ssize_t rc = this->bind(my_addr,addrlen);
         if(rc==-1)
         {
            THROW_SYSERROR( );
         }

         return (rc!=-1);
      }

      bool CSocketHandle::Listen(INT backlog)
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         ssize_t rc = this->listen(backlog);
         if(rc==-1)
         {
            THROW_SYSERROR( );
         }

         return (rc!=-1);
      }

      bool CSocketHandle::Shutdown(INT how)
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         ssize_t rc = this->shutdown(how);
         if(rc==-1)
         {
            THROW_SYSERROR( );
         }

         return (rc!=-1);
      }

      bool CSocketHandle::SetSockOpt(INT level, INT optname, const void *optval, socklen_t optlen)
      {
         if(GetFD()==-1)
         {
            IOHandle_THROW();
         }

         ssize_t rc = this->setsockopt(level,optname,optval,optlen);
         if(rc==-1)
         {
            THROW_SYSERROR( );
         }

         return (rc!=-1);
      }

   } /// namespace GalaxyRT
} ///namespace Galaxy
