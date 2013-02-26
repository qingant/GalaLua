/* -*- C++ -*- */

/* Time-stamp: <2012-04-17 10:00:22 星期二 by OCaml> */

/**
 * @file
 * @author john.lee
 */

#ifndef CSOCKET_H
#define CSOCKET_H
#include <vector>
#include <utility>

#include "Handle.hpp"
#include "Exception.hpp"
#include <sys/un.h>

#pragma once

namespace Galaxy
{
   namespace GalaxyRT
   {
      class CUNSockProtocolError: public CException
      {
      public:
         CUNSockProtocolError(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
			:CException(file, func, line, errcode, control, msg){}
         ~CUNSockProtocolError() throw(){}
         std::string GetClassName() const
         {
			return std::string("CUNSockProtocolError");
         }
      };
   
      class CUNSockCmsgError: public CException
      {
      public:
         CUNSockCmsgError(CPSTR file, CPSTR func, INT line, INT errcode, INT control, const std::string &msg) throw()
			:CException(file, func, line, errcode, control, msg){}
         ~CUNSockCmsgError() throw(){}
         std::string GetClassName() const
         {
			return std::string("CUNSockCmsgError");
         }
      };

   
      typedef std::pair<UCHAR, INT> FDPAIR;
      typedef std::vector<FDPAIR >  FDTYPE;

      class CSocketAddrIn
      {
      private:
         struct sockaddr_in  m_addr;
      public:
         const struct sockaddr_in *Get() const;
         UINT Length() const;
         void GetPeerName(int Socket);
         void GetSockName(int Socket);
      public:
         CSocketAddrIn();
         CSocketAddrIn(const struct sockaddr_in &addr);
         CSocketAddrIn(std::string host,short port);
         CSocketAddrIn(short port);
         ~CSocketAddrIn();

         std::string GetHost();
         short GetPort();
      };


      class CSocketAddrUn
      {
      private:
         struct sockaddr_un  m_addr;
      public:
         const struct sockaddr_un *Get() const;
      public:
         CSocketAddrUn();
         inline UINT Length() const;
         CSocketAddrUn(const struct sockaddr_un &addr);
         CSocketAddrUn(std::string token);
         ~CSocketAddrUn();
      };

      class CSocket:public CSocketHandle
      {
      public:
         enum SPF { SPF_UNIX = PF_UNIX,SPF_INET = PF_INET };
         enum STY { STY_STREAM = SOCK_STREAM,STY_SEQPACKET = SOCK_SEQPACKET,STY_RAW = SOCK_RAW };
      private:
         int socket(int domain, int type, int protocol);
      public:
         bool SegmentSend(SHORT timeout,CPSTR buf, size_t length,size_t segsize=1024) const;
         bool SegmentRecv(SHORT timeout,PSTR buf, size_t length,size_t segsize=1024) const;
      public:
         void SetBlocking(bool flag = true);
         void SetNonBlocking();
      public:
         CSocket(SOCKET socket);
         CSocket(SPF pf,STY type);
         virtual ~CSocket();

      };


//TCPSocket
      class CTCPSocket:public CSocket
      {
      private:
         CSocketAddrIn   m_local;
         CSocketAddrIn   m_remote;
      public:
         //CTCPSocket();
         CTCPSocket(SOCKET socket);
         CTCPSocket(SOCKET socket,const CSocketAddrIn &Remote);
         CTCPSocket(const CSocketAddrIn &Remote);
         CTCPSocket(const CSocketAddrIn &Remote,const CSocketAddrIn &Local);
         virtual ~CTCPSocket();

         const CSocketAddrIn *LocalAddr() const;
         const CSocketAddrIn *RemoteAddr() const;
// public:     /*published*/
//     bool SendWait(SHORT timeout);
//     bool RecvWait(SHORT timeout);
//     ssize_t Send(const void *buf, size_t len);
//     ssize_t Recv(void *buf, size_t len);
//     Shutdown()
      };

      class CTCPSocketClient:public CTCPSocket
      {
      private:
         void ConnectToServer(bool bind,/*bool ReuseAddr,*/int Linger);
      public:
         CTCPSocketClient(const CSocketAddrIn &Remote,/*bool ReuseAddr=true,*/int Linger=10);
         CTCPSocketClient(const CSocketAddrIn &Remote,const CSocketAddrIn &Local,/*bool ReuseAddr=true,*/int Linger=10);
         CTCPSocketClient(std::string host,short port,/*bool ReuseAddr=true,*/int Linger=10);
         virtual ~CTCPSocketClient();
      };

      class CTCPSocketServer:public CTCPSocket
      {
      private :
         void CreateTCPServer(INT backlog);
      public :
         CTCPSocketServer(const CSocketAddrIn &Local, int backlog = 50);
         CTCPSocketServer(short port, int backlog = 50);
         CTCPSocketServer(const std::string &host, short port, int backlog = 50);

         std::auto_ptr<CTCPSocket> Accept();
         CTCPSocket                *AcceptEx();
         INT                       AcceptFD();
         virtual ~CTCPSocketServer();
      };
// test


// 
      class CUNSocket:public CSocket
      {
      public:
         enum {MAXFDNUM = 255};
         enum {REGULARFILE, TCPSOCKET, UNSOCKET, PIPE, INVALID};
      public:
         CUNSocket(SOCKET socket);
         CUNSocket(SOCKET socket,const CSocketAddrUn &addr);
         CUNSocket(const CSocketAddrUn &addr);
         bool SendFD(const FDTYPE &sendfds, CPVOID ptr, size_t nbytes ) const;
         FDTYPE RecvFD(PVOID ptr, size_t nbytes) const;
         bool SendPTC(CPVOID ptc, size_t nbytes) const;
         bool SendPTCEx(CPVOID ptc, size_t nbytes) const;

         bool RecvPTC(PVOID ptc, size_t nbytes) const;
         bool RecvPTCEx(PVOID ptc, size_t nbytes) const;


         virtual ~CUNSocket();
      protected:
         const CSocketAddrUn *GetAddr() const;
      private:
         CSocketAddrUn m_addr;
      };


      class CUNSocketClient:public CUNSocket
      {
      public:
         CUNSocketClient(const CSocketAddrUn &addr,/*bool ReuseAddr=true,*/int Linger=10);
         CUNSocketClient(std::string path,/*bool ReuseAddr=true,*/int Linger=10);
         virtual ~CUNSocketClient();
      private:
         void ConnectToServer(/*bool ReuseAddr,*/int Linger);
      };

      class CUNSocketServer:public CUNSocket
      {
      public:
         CUNSocketServer(const CSocketAddrUn &addr, int backlog = 50);
         CUNSocketServer(const std::string &path, int backlog = 50);
         ~CUNSocketServer();
      public:
         std::auto_ptr<CUNSocket> Accept();
         CUNSocket                *AcceptEx();
         INT                       AcceptFD();

      private:
         void CreateUNServer(INT backlog);
      };

   } /// namespace GalaxyRT
} /// namespace Galaxy
   
	
#endif
