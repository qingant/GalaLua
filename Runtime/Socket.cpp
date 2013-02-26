/* -*- C++ -*- */

// Time-stamp: <2012-11-27 11:18:37 星期二 by matao>


/**
 * @file Socket.cpp
 * @author john.lee
 */


#include "Socket.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

namespace Galaxy
{
    namespace GalaxyRT
    {

#define MY_THROW() (throw CSocketError(__FILE__, __PRETTY_FUNCTION__, __LINE__) )

/*CSocketAddr*/


#ifdef _AIX
        typedef struct
        {
            struct cmsghdr			cmhdr;
            INT						fd[1];
        }AFASCKCTLDTA,*PAFASCKCTLDTA;



        static INT SendFd(INT sock,PBYTE PTC,UINT PTCLen,INT fd)
        {
            struct msghdr 		send_msg;
            struct iovec 		iov[2];
            BYTE        		Token[8];
            INT					rc;

#ifdef _UNIX95_
            PAFASCKCTLDTA       pMsgData;
            BYTE                szData[sizeof(*pMsgData)+8];
            pMsgData            = (PAFASCKCTLDTA)szData;
#endif

            memset(&send_msg,0,sizeof(send_msg));
            memset(iov,0,sizeof(iov));
            memset(Token,0,sizeof(Token));

            Token[0] 			    = 0xAB;
            Token[1] 			    = 0xCD;
            Token[2] 			    = 0xEF;
            Token[3] 			    = 0xFE;

            memcpy(&Token[4],&PTCLen,sizeof(PTCLen));

            iov[0].iov_base 		= (PSTR)Token;
            iov[0].iov_len 			= sizeof(Token);
            iov[1].iov_base 		= (PSTR)PTC;
            iov[1].iov_len 			= PTCLen;


            send_msg.msg_iov 		    = iov;
            send_msg.msg_iovlen 	    = 2;

#ifndef _UNIX95_
#ifndef  _HPUX_
            send_msg.msg_accrights      = (char *) &fd;
            send_msg.msg_accrightslen   = sizeof(fd);
#else
            send_msg.msg_control      = (char *) &fd;
            send_msg.msg_controllen   = sizeof(fd);
#endif
#else
            memset(pMsgData,0,sizeof(*pMsgData));
            pMsgData->cmhdr.cmsg_level 	= SOL_SOCKET;
            pMsgData->cmhdr.cmsg_type 	= SCM_RIGHTS;
            pMsgData->cmhdr.cmsg_len 	= sizeof(*pMsgData);
            pMsgData->fd[0]				= fd;

            send_msg.msg_control 	    = (char *) pMsgData;
            send_msg.msg_controllen     = pMsgData->cmhdr.cmsg_len;
#endif

            rc = sendmsg(sock, &send_msg, 0);
            if(rc==-1)
            {
                return (__LINE__);
            }
            else if(rc==0)
            {
                return (-1);
            }

            return (0);
        }

        static INT RecvFd(INT sock,PBYTE PTC,UINT maxPTCLen, PINT fd)
        {
            struct msghdr 		recv_msg;
            struct iovec 		iov[2];
            BYTE        		Token[8];
            UINT                iLen;
            INT					rc;

            /* New _UNIX95_ way*/
#ifdef _UNIX95_
            PAFASCKCTLDTA       pMsgData;
            BYTE                szData[sizeof(*pMsgData)+8];
            pMsgData            = (PAFASCKCTLDTA)szData;
#else
            INT                 rfd;
#endif


            memset(&recv_msg,0,sizeof(recv_msg));
            memset(iov,0,sizeof(iov));
            memset(Token,0,sizeof(Token));

            iov[0].iov_base 	    = (PSTR)Token;
            iov[0].iov_len 			= sizeof(Token);
            iov[1].iov_base 	    = (PSTR)PTC;
            iov[1].iov_len 			= maxPTCLen;

            recv_msg.msg_iov 		    = iov;
            recv_msg.msg_iovlen 	    = 2;

#ifndef _UNIX95_
            rfd = -1;
#ifndef  _HPUX_
            recv_msg.msg_accrights      = (char *) &rfd;
            recv_msg.msg_accrightslen   = sizeof(rfd);
#else
            recv_msg.msg_control      = (char *) &rfd;
            recv_msg.msg_controllen   = sizeof(int);
#endif

#else
            memset(pMsgData,0,sizeof(*pMsgData));
            pMsgData->cmhdr.cmsg_len 	= sizeof(*pMsgData);
            pMsgData->fd[0]				= -1;
            recv_msg.msg_control 	    = (char *) pMsgData;
            recv_msg.msg_controllen     = sizeof(*pMsgData);
#endif
            rc = recvmsg (sock, &recv_msg,0);
            if(rc==-1)
            {
                return (__LINE__);
            }
            else if(rc==0)
            {

                return (-1);
            }

            memcpy(&iLen,&Token[4],sizeof(iLen));
            if((iov[0].iov_len!=sizeof(Token))||(Token[0]!=0xAB)|| \
               (Token[1]!=0xCD)||(Token[2]!=0xEF)||(Token[3]!=0xFE))
            {
                return (__LINE__);
            }

#ifndef _UNIX95_
            fd[0]	  = rfd;
#else
            if((pMsgData->cmhdr.cmsg_level != SOL_SOCKET)||(pMsgData->cmhdr.cmsg_type != SCM_RIGHTS))
            {
                return (__LINE__);
            }
            fd[0]	  = pMsgData->fd[0];
#endif

            return (0);
        }

#endif

        CSocketAddrIn::CSocketAddrIn()
        {
            CRT_memset(&m_addr,0,sizeof(m_addr));
            m_addr.sin_family = AF_INET;
        }

        CSocketAddrIn::CSocketAddrIn(const struct sockaddr_in &addr)
            :m_addr(addr)
        {

        }

        CSocketAddrIn::CSocketAddrIn(std::string host,short port)
        {
            struct hostent          he;
            struct hostent			*phe = &he;

            CRT_memset(&m_addr,0,sizeof(m_addr));

            m_addr.sin_family = AF_INET;
            m_addr.sin_port   = CRT_htons( port );
            // int gethostbyname_r(const char *name,
            //                     struct hostent *ret, char *buf, size_t buflen,
            //                     struct hostent **result, int *h_errnop);

            if ( (GetHostByName(host, phe))!= -1 )
            {
                CRT_memcpy( &(m_addr.sin_addr),phe->h_addr,phe->h_length);
            }
            else if ( ( m_addr.sin_addr.s_addr = inet_addr( (PSTR)host.c_str() ) ) == INADDR_NONE )
            {
                THROW_GENERALERROR( CArgumentError, "INADDR_NONE");
            }
        }

        CSocketAddrIn::CSocketAddrIn(short port)
        {
            CRT_memset(&m_addr,0,sizeof(m_addr));
            m_addr.sin_family       = AF_INET;
            m_addr.sin_port         = CRT_htons( port );
            m_addr.sin_addr.s_addr  = INADDR_ANY;
        }



        CSocketAddrIn::~CSocketAddrIn()
        {

        }

        void CSocketAddrIn::GetPeerName(int Socket)
        {
            socklen_t    iLen;

            if(Socket == -1)
            {
                THROW_GENERALERROR( CArgumentError, "Socket == -1" );
            }

            iLen = sizeof(m_addr);
            CRT_memset(&m_addr,0,sizeof(m_addr));


            if(CRT_getpeername(Socket,(struct sockaddr *) &(m_addr), (socklen_t*)&iLen)==-1)
            {
                THROW_SYSERROR( );
            }
        }

        void CSocketAddrIn::GetSockName(int Socket)
        {
            socklen_t    iLen;

            if(Socket == -1)
            {
                //MY_THROW();
                THROW_GENERALERROR(CArgumentError, "socket == -1");
            }

            iLen = sizeof(m_addr);
            CRT_memset(&m_addr,0,sizeof(m_addr));

            if(CRT_getsockname( Socket, (struct sockaddr *)&(m_addr), (socklen_t*)&iLen ) == -1 )
            {
                THROW_SYSERROR( );
            }
        }

        const struct sockaddr_in *CSocketAddrIn::Get() const
        {
            return &(m_addr);
        }

        UINT CSocketAddrIn::Length() const
        {
            return sizeof(m_addr);
        }

        std::string CSocketAddrIn::GetHost()
        {
            std::vector<CHAR> tmp(20, 0);
            PSTR szTmp = &tmp[0];
            if (CRT_inet_ntop(AF_INET, (CPVOID)&m_addr.sin_addr, szTmp, tmp.size()) == NULL)
            {
                THROW_SYSERROR();
            }
            return std::string(szTmp);
        }

        short CSocketAddrIn::GetPort()
        {
            return CRT_ntohs(m_addr.sin_port);
        }

        /*CSocketAddrUn*/
        CSocketAddrUn::CSocketAddrUn()
        {
            CRT_memset(&m_addr,0,sizeof(m_addr));
            m_addr.sun_family = AF_UNIX;
        }

        CSocketAddrUn::CSocketAddrUn(sockaddr_un const& addr)
            :m_addr(addr)
        {

        }

        CSocketAddrUn::CSocketAddrUn(std::string token)
        {
            UINT 	            iTlen;

            CRT_memset(&m_addr,0,sizeof(m_addr));
            m_addr.sun_family = AF_UNIX;

            iTlen = token.length();
            iTlen = (iTlen >= sizeof(m_addr.sun_path)) ? (sizeof(m_addr.sun_path)-1) : iTlen;
            CRT_memcpy(m_addr.sun_path,token.c_str(),iTlen);
        }

        UINT CSocketAddrUn::Length() const
        {
            return sizeof(m_addr);
        }

        CSocketAddrUn::~CSocketAddrUn()
        {

        }


        const struct sockaddr_un *CSocketAddrUn::Get() const
        {
            return &(m_addr);
        }



/*CSocket*/
        CSocket::CSocket(SOCKET socket)
            :CSocketHandle(socket)
        {

        }

        CSocket::CSocket(SPF pf,STY type)
            :CSocketHandle(socket(pf,type,0))
        {

        }

        CSocket::~CSocket()
        {
        }

        int CSocket::socket(int domain, int type, int protocol)
        {
            return CRT_socket(domain,type,protocol);
        }

        bool CSocket::SegmentSend(SHORT timeout,CPSTR buf, size_t length,size_t segsize) const
        {
            size_t		sendlen;
            size_t		len;
            size_t		snd;
            time_t      now, justnow;

            if(length == 0)
            {
                return true;
            }


            for(sendlen=0,len=0;sendlen < length;)
            {

                justnow = CRT_time(NULL);
                if (justnow == (time_t)(-1))
                {
                    THROW_SYSERROR( );
                }
                //if(!SendWait(timeout))
                //{
                //   return false;
                //}
                SendWait( timeout );

                snd = (length-sendlen);
                snd = (snd > segsize) ? segsize : snd ;

                len = Send(&buf[sendlen],snd);

                sendlen+=len;
                now = CRT_time(NULL);
                if (now == (time_t)(-1))
                {
                    THROW_SYSERROR( );
                }
                timeout -= (now - justnow);
                if (timeout <= 0)
                {
                    return false;
                }
                //printf("sendlen: %d now: %d justnow:%d timeout:%d\n",sendlen, now, justnow, timeout);
            }

            return true;
        }

        bool CSocket::SegmentRecv(SHORT timeout,PSTR buf, size_t length,size_t segsize) const
        {
            size_t		recvlen;
            size_t		len;
            size_t		rcv;
            time_t      now, justnow;
            if(length == 0)
            {
                return true;
            }

            for(recvlen=0,len=0;recvlen < length;)
            {
                justnow = CRT_time(NULL);
                if (justnow == (time_t)(-1))
                {
                    THROW_SYSERROR( );
                }
                //if(!RecvWait(timeout))
                //{
                //   return false;
                //}
                RecvWait( timeout );

                rcv = (length-recvlen);
                rcv = (rcv > segsize) ? segsize : rcv ;

                len = Recv(&buf[recvlen],rcv);

                recvlen+=len;
                now = CRT_time(NULL);
                if (now == (time_t)(-1) || len == 0) // len==0 indicate the socket closed
                {
                    THROW_SYSERROR( );
                }
                timeout -= (now - justnow);
                if (timeout <= 0)
                {
                    return false;
                }
                //printf("recvlen: %d now: %d justnow:%d timeout:%d\n",recvlen, now, justnow, timeout);

            }

            return (true);
        }


        void CSocket::SetBlocking(bool flag)
        {
            INT flags = ::fcntl(GetFD(), F_GETFL, 0);
            INT rt;
            if (flag)
            {
                rt = ::fcntl(this->GetFD(), F_SETFL, flags & ~O_NONBLOCK);
            }
            else
            {
                rt = ::fcntl(this->GetFD(), F_SETFL, flags|O_NONBLOCK);
            }
            if (rt == -1)
            {
                THROW_SYSERROR( );
            }
        }
        void CSocket::SetNonBlocking()
        {
            SetBlocking(false);
        }



/*CTCPSocket*/
        CTCPSocket::CTCPSocket(SOCKET socket)
            :CSocket(socket)
        {
            m_remote.GetPeerName(GetFD());
            m_local.GetSockName(GetFD());
        }

        CTCPSocket::CTCPSocket(SOCKET socket,const CSocketAddrIn &Remote)
            :CSocket(socket),m_remote(Remote)
        {

        }

        CTCPSocket::CTCPSocket(const CSocketAddrIn &Remote)
            :CSocket(SPF_INET,STY_STREAM),m_remote(Remote)
        {

        }

        CTCPSocket::CTCPSocket(const CSocketAddrIn &Remote,const CSocketAddrIn &Local)
            :CSocket(SPF_INET,STY_STREAM),m_local(Local),m_remote(Remote)
        {

        }


        CTCPSocket::~CTCPSocket()
        {
        }

        const CSocketAddrIn *CTCPSocket::LocalAddr() const
        {
            return (&m_local);
        }

        const CSocketAddrIn *CTCPSocket::RemoteAddr() const
        {
            return (&m_remote);
        }

/*CTCPSocketClient*/
        CTCPSocketClient::CTCPSocketClient(const CSocketAddrIn &Remote,/*bool ReuseAddr,*/int Linger)
            :CTCPSocket(Remote)
        {
            ConnectToServer(false,/*ReuseAddr,*/Linger);
        }

        CTCPSocketClient::CTCPSocketClient(const CSocketAddrIn &Remote,const CSocketAddrIn &Local,/*bool ReuseAddr,*/int Linger)
            :CTCPSocket(Remote,Local)
        {
            ConnectToServer(true,/*ReuseAddr,*/Linger);
        }

        CTCPSocketClient::CTCPSocketClient(std::string host,short port,/*bool ReuseAddr,*/int Linger)
            :CTCPSocket(CSocketAddrIn(host,port))
        {
            ConnectToServer(false,/*ReuseAddr,*/Linger);
        }

        CTCPSocketClient::~CTCPSocketClient()
        {

        }

        void CTCPSocketClient::ConnectToServer(bool bind,/*bool ReuseAddr,*/int Linger)
        {
            UINT            opt;
            struct linger   lgr;

            opt = 1;
            SetSockOpt(SOL_SOCKET,SO_REUSEADDR,(PSTR)(&opt),sizeof(opt));

            if(Linger >=0)
            {
                CRT_memset(&lgr,0,sizeof(lgr));
                lgr.l_onoff		= 1;
                lgr.l_linger	= Linger;

                SetSockOpt(SOL_SOCKET, SO_LINGER,(PSTR)&lgr, sizeof(lgr));
            }

            if(bind)
            {
                Bind((struct sockaddr *)LocalAddr()->Get(),LocalAddr()->Length());
            }

            Connect((struct sockaddr *)RemoteAddr()->Get(),RemoteAddr()->Length());
        }


// CTCPSocketServer

        void CTCPSocketServer::CreateTCPServer(INT backlog)
        {
            int flag = 1;
            SetSockOpt(SOL_SOCKET,SO_REUSEADDR,(PSTR)(&flag),sizeof(flag));
            Bind((struct sockaddr*)LocalAddr()->Get(), LocalAddr()->Length());
            Listen(backlog);
        }
        CTCPSocketServer::CTCPSocketServer(const CSocketAddrIn &local, int backlog):CTCPSocket(CSocketAddrIn(), local)
        {
            CreateTCPServer(backlog);
        }
        CTCPSocketServer::CTCPSocketServer(short port, int backlog):CTCPSocket(CSocketAddrIn(), CSocketAddrIn("0.0.0.0", port))
        {
            CreateTCPServer(backlog);
        }
        CTCPSocketServer::CTCPSocketServer(const std::string &host, short port, int backlog):CTCPSocket(CSocketAddrIn(), CSocketAddrIn(host, port))
        {
            CreateTCPServer(backlog);
        }

        std::auto_ptr<CTCPSocket> CTCPSocketServer::Accept()
        {
            struct sockaddr_in addrin;
            socklen_t socklen = sizeof(addrin);

            ssize_t fd = CSocketHandle::Accept((struct sockaddr*)(&addrin), &socklen);
            return std::auto_ptr<CTCPSocket>(new CTCPSocket(fd,CSocketAddrIn(addrin)));
        }

        CTCPSocket *CTCPSocketServer::AcceptEx()
        {
            struct sockaddr_in addrin;
            socklen_t socklen = sizeof(addrin);

            ssize_t fd = CSocketHandle::Accept((struct sockaddr*)(&addrin), &socklen);
            return (new CTCPSocket(fd,CSocketAddrIn(addrin)));
        }
        INT CTCPSocketServer::AcceptFD()
        {
            struct sockaddr_in addrin;
            socklen_t socklen = sizeof(addrin);

            ssize_t fd = CSocketHandle::Accept((struct sockaddr*)(&addrin), &socklen);
            if (fd == -1)
            {
                THROW_SYSERROR();
            }
            return fd;
        }

        CTCPSocketServer::~CTCPSocketServer()
        {

        }


// CUNSocket

        CUNSocket::CUNSocket(SOCKET socket)
            :CSocket(socket)
        {

        }
        CUNSocket::CUNSocket(SOCKET socket,const CSocketAddrUn &Addr)
            :CSocket(socket), m_addr(Addr)
        {

        }

        CUNSocket::CUNSocket(const CSocketAddrUn &Addr)
            :CSocket(SPF_UNIX, STY_STREAM), m_addr(Addr)
        {

        }



        bool CUNSocket::SendPTC(CPVOID ptc, size_t nbytes) const
        {
            struct msghdr 		send_msg;
            struct iovec	    iov[2];
            UCHAR               protocol[8];

            if(ISNULL(ptc)||(nbytes==0))
            {
                THROW_GENERALERROR(CArgumentError, "SendPTC");
                return (false);
            }

            CRT_memset(&send_msg,0,sizeof(send_msg));
            CRT_memset(iov,0,sizeof(iov));

            protocol[0]				= 0xCD;
            protocol[1]				= 0xDC;
            CRT_memcpy(&protocol[2],&nbytes,sizeof(nbytes));

            iov[0].iov_base 		= (PSTR)protocol;
            iov[0].iov_len 			= sizeof(nbytes)+2;
            iov[1].iov_base 		= (PSTR)ptc;
            iov[1].iov_len 			= nbytes;

            send_msg.msg_iov 		= iov;
            send_msg.msg_iovlen 	= 2;

            if (SendMsg(&send_msg) == -1)
            {
                THROW_SYSERROR( );
            }

            return true;
        }
        bool CUNSocket::SendPTCEx(CPVOID ptc, size_t nbytes) const
        {
            struct msghdr 		send_msg;
            struct iovec	    iov[1];
            int                 rc = 0;
            if(ISNULL(ptc)||(nbytes==0))
            {
                THROW_GENERALERROR(CArgumentError, "SendPTCEx");
                return (false);
            }

            CRT_memset(&send_msg,0,sizeof(send_msg));
            CRT_memset(iov,0,sizeof(iov));


            iov[0].iov_base 		= (PSTR)ptc;
            iov[0].iov_len 			= nbytes;

            send_msg.msg_iov 		= iov;
            send_msg.msg_iovlen 	= 1;

            if ((rc = SendMsg(&send_msg)) == -1)
            {
                THROW_SYSERROR();
            }
            if ((size_t)rc != nbytes)
            {
                return false;
            }

            return true;
        }

        bool CUNSocket::RecvPTCEx(PVOID ptc, size_t nbytes) const
        {
            struct msghdr 		recv_msg;
            struct iovec 		iov[1];
            int                 rc;
            if(ISNULL(ptc)||(nbytes==0))
            {
                THROW_GENERALERROR(CArgumentError, "RecvPTCEx parameter error");
                return (false);
            }

            memset(&recv_msg,0,sizeof(recv_msg));
            memset(iov,0,sizeof(iov));


            iov[0].iov_base 		= (PSTR)ptc;
            iov[0].iov_len 			= nbytes;

            recv_msg.msg_iov 		= iov;
            recv_msg.msg_iovlen 	= 1;

            if ((rc = RecvMsg(&recv_msg)) == -1)
            {
                THROW_SYSERROR();
                return (false);
            }
            if (rc == 0)
            {
                return false;
            }



            //判断一下发送协议
            return true;

        }

        bool CUNSocket::RecvPTC(PVOID ptc, size_t nbytes) const
        {
            struct msghdr 		recv_msg;
            struct iovec 		iov[2];
            UCHAR				protocol[8];
            UINT                iLen;

            if(ISNULL(ptc)||(nbytes==0))
            {
                THROW_GENERALERROR(CArgumentError, "RecvPTC");
                return (false);
            }

            memset(&recv_msg,0,sizeof(recv_msg));
            memset(iov,0,sizeof(iov));

            iov[0].iov_base 		= (PSTR)protocol;
            iov[0].iov_len 			= sizeof(protocol);
            iov[1].iov_base 		= (PSTR)ptc;
            iov[1].iov_len 			= nbytes;

            recv_msg.msg_iov 		= iov;
            recv_msg.msg_iovlen 	= 2;

            if (RecvMsg(&recv_msg) == -1)
            {
                THROW_SYSERROR( );
                return (false);
            }

            memcpy(&iLen,&protocol[2],sizeof(iLen));

            //判断一下发送协议
            if((protocol[0]!=0xCD)||(protocol[1]!=0xDC)||(iLen>nbytes))
            {
                THROW_GENERALERROR( CUNSockProtocolError, "CUNSocket Protocol Error");
                return (false);
            }

            return true;

        }

#ifdef _AIX
        FDTYPE CUNSocket::RecvFD(PVOID ptr, size_t nbytes) const
        {
            INT fd = -1;
            if (Galaxy::GalaxyRT::RecvFd(GetFD(), (PBYTE)ptr, nbytes, &fd) != 0)
            {
                THROW_SYSERROR( );
            }
            FDTYPE fds;
            fds.push_back(std::make_pair((UCHAR)0, fd));
            return fds;
        }

        bool CUNSocket::SendFD(const FDTYPE &sendfds, CPVOID ptr, size_t nbytes) const
        {
            if (sendfds.size() != 1)
            {
                THROW_EXCEPTION_EX("This platform does not support muti fd deliever");
            }

            if (Galaxy::GalaxyRT::SendFd(GetFD(), (PBYTE)ptr, nbytes, sendfds[0].second) != 0)
            {
                THROW_EXCEPTION_EX("SendFd Failure");
            }
            return true;

        }
#else
        bool CUNSocket::SendFD(const FDTYPE &sendfds, CPVOID ptr, size_t nbytes) const
        {
            struct msghdr	msg = {0,0,0,0,0,0,0};
            struct iovec	iov[3];
            UCHAR   protocol[2];
            //int i;
            if (sendfds.size() > 255)
            {
                THROW_GENERALERROR(CArgumentError, "sendfds.size( ) > 255)");
            }
            if (ISNULL(ptr) && (nbytes != 0))
            {
                THROW_GENERALERROR(CArgumentError, "SendFD");
            }
            size_t fdarraylen = sendfds.size();
            std::vector<UCHAR> types(256, 0);


// #ifndef	HAVE_MSGHDR_MSG_CONTROL
//     MY_THROW();
//     return false;
// #endif


#ifndef _UNIX95_
            std::vector<INT> fds(sendfds.size(), -1);
            for (size_t i=0; i!= sendfds.size();++i)
            {
                fds[i] = sendfds.at(i).second;
            }
#ifndef  _HPUX_
            send_msg.msg_accrights      = (char *) &(fds[0]);
            send_msg.msg_accrightslen   = sizeof(int)*fdarraylen;
#else
            send_msg.msg_control      = (char *) &(fds[0]);
            send_msg.msg_controllen   = sizeof(int)*fdarraylen;
#endif
#endif
            union {
                struct cmsghdr	cm;
                CHAR				control[CMSG_SPACE(sizeof(int)*CUNSocket::MAXFDNUM)];
            } control_un;


            struct cmsghdr	*cmptr;
            msg.msg_control = control_un.control;
            msg.msg_controllen = sizeof(control_un.control);
            msg.msg_controllen = CMSG_LEN(sizeof(int)*fdarraylen);

            cmptr = CMSG_FIRSTHDR(&msg);
            cmptr->cmsg_len  = CMSG_LEN( sizeof(int)*fdarraylen );
            cmptr->cmsg_level = SOL_SOCKET;
            cmptr->cmsg_type = SCM_RIGHTS;

            for(size_t i=0;i != fdarraylen;i++)
            {
                types[i] = sendfds[i].first;
                *( (int *) (CMSG_DATA(cmptr)+sizeof(int)*i) ) = sendfds.at(i).second; // 找对正确的偏移量!!!
            }


            msg.msg_name = NULL;
            msg.msg_namelen = 0;

            protocol[0] = 0xAB;
            protocol[1] = sendfds.size();
            iov[0].iov_base = protocol;
            iov[0].iov_len = sizeof(protocol);

            iov[1].iov_base = (PVOID)(&types[0]);
            iov[1].iov_len = sizeof(UCHAR)*types.size();

            iov[2].iov_base = (PVOID)ptr;
            // printf("SendFd : %s   %d\n", (char*)ptr, nbytes);
            iov[2].iov_len = nbytes;

            msg.msg_iov = iov;
            msg.msg_iovlen = 3;
            if (SendMsg(&msg) == -1)
            {
                THROW_SYSERROR( );
            }
            return true;
        }




        FDTYPE CUNSocket::RecvFD(PVOID ptr, size_t nbytes) const
        {
            struct msghdr	msg;
            struct iovec	iov[3];
            UCHAR protocol[2];
            FDTYPE fds;
            UCHAR types[256];

            if (ISNULL(ptr) && (nbytes != 0))
            {
                THROW_GENERALERROR(CArgumentError, "RecvFD");
            }
            CRT_memset(types, 0, sizeof(types));
// #ifndef	HAVE_MSGHDR_MSG_CONTROL
//     MY_THROW();
//     return std::vector<INT>();
// #endif
            union {
                struct cmsghdr	cm;
                CHAR				control[CMSG_SPACE(sizeof(int)*CUNSocket::MAXFDNUM)];
            } control_un;
            struct cmsghdr	*cmptr;

            msg.msg_name = NULL;
            msg.msg_namelen = 0;

            iov[0].iov_base = protocol;
            iov[0].iov_len = sizeof(protocol);

            iov[1].iov_base = types;
            iov[1].iov_len = sizeof(types);
            iov[2].iov_base = ptr;
            iov[2].iov_len = nbytes;
            msg.msg_iov = iov;
            msg.msg_iovlen = 3;
#ifndef _UNIX95_
            std::vector<INT> fds(CUNSocket::MAXFDNUM, -1);
#ifndef  _HPUX_
            recv_msg.msg_accrights      = (char *) &fds[0];
            recv_msg.msg_accrightslen   = sizeof(int)*CUNSocket::MAXFDNUM;
#else
            recv_msg.msg_control      = (char *) &fds[0];
            recv_msg.msg_controllen   = sizeof(int);
#endif

            if (RecvMsg(&msg) == -1)
            {
                THROW_SYSERROR( );
            }
            if (protocol[0] != 0xAB)
            {
                THROW_GENERALERROR( CUNSockProtocolError, "protocol[0] != 0xAB");
            }
            size_t fdarraylen = protocol[1];
            fds.resize(fdarraylen);
            return fds;

#endif
            msg.msg_control = control_un.control;
            msg.msg_controllen = sizeof(control_un.control);
            if (RecvMsg(&msg) == -1)
            {
                THROW_SYSERROR( );
            }

            if ( (cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&  cmptr->cmsg_len > 0)
            {
                if (cmptr->cmsg_level != SOL_SOCKET)
                {
                    THROW_GENERALERROR( CUNSockCmsgError, "cmsg_level != SOL_SOCKET");
                }
                if (cmptr->cmsg_type != SCM_RIGHTS){
                    THROW_GENERALERROR( CUNSockCmsgError, "cmsg_type != SCM_RIGHTS");
                }
                if (protocol[0] != 0xAB)
                {
                    THROW_GENERALERROR( CUNSockProtocolError, "protocol[0] != 0xAB");
                }
                size_t fdarraylen = protocol[1];

                for (size_t i=0; i != fdarraylen; ++i)
                {
                    INT fd = *( (INT *) (CMSG_DATA(cmptr)+sizeof(INT)*i));
                    fds.push_back(std::pair<UCHAR, INT>(types[i], fd));
                }

            }
            else
            {
                THROW_GENERALERROR( CUNSockCmsgError, "cmsghdr Error");
            }


            return fds;
        }
#endif


        const CSocketAddrUn *CUNSocket::GetAddr() const
        {
            return &m_addr;
        }
        CUNSocket::~CUNSocket()
        {

        }

        CUNSocketClient::CUNSocketClient(const CSocketAddrUn &addr,/* bool ReuseAddr,*/ int Linger)
            :CUNSocket(addr)
        {
            ConnectToServer(/*ReuseAddr,*/ Linger);
        }

        CUNSocketClient::CUNSocketClient(std::string path,/*bool ReuseAddr,*/int Linger)
            :CUNSocket(CSocketAddrUn(path))
        {
            ConnectToServer(/*ReuseAddr,*/ Linger);
        }

        CUNSocketClient::~CUNSocketClient()
        {

        }

        void CUNSocketClient::ConnectToServer(/*bool ReuseAddr,*/int Linger)
        {
            UINT            opt;
            struct linger   lgr;

            opt = 1;
            SetSockOpt(SOL_SOCKET,SO_REUSEADDR,(PSTR)(&opt),sizeof(opt));

            if(Linger >=0)
            {
                CRT_memset(&lgr,0,sizeof(lgr));
                lgr.l_onoff		= 1;
                lgr.l_linger	= Linger;

                SetSockOpt(SOL_SOCKET, SO_LINGER,(PSTR)&lgr, sizeof(lgr));
            }

            Connect((struct sockaddr *)GetAddr()->Get(),GetAddr()->Length());
        }




        CUNSocketServer::CUNSocketServer(const CSocketAddrUn &addr, int backlog)
            :CUNSocket(addr)
        {
            CreateUNServer(backlog);
        }

        CUNSocketServer::CUNSocketServer(const std::string &path, int backlog)
            :CUNSocket(CSocketAddrUn(path))
        {
            CreateUNServer(backlog);
        }

        CUNSocketServer::~CUNSocketServer()
        {
        }

        void CUNSocketServer::CreateUNServer(INT backlog)
        {
            CRT_unlink(GetAddr()->Get()->sun_path);
            Bind((struct sockaddr*)GetAddr()->Get(), GetAddr()->Length());
            Listen(backlog);

        }

        std::auto_ptr<CUNSocket> CUNSocketServer::Accept()
        {
            struct sockaddr_un addrin;
            socklen_t socklen = sizeof(addrin);
            ssize_t fd = CSocketHandle::Accept((struct sockaddr*)(&addrin), &socklen);
            return std::auto_ptr<CUNSocket>(new CUNSocket(fd,CSocketAddrUn(addrin)));
        }

        CUNSocket *CUNSocketServer::AcceptEx()
        {
            struct sockaddr_un addrin;
            socklen_t socklen = sizeof(addrin);
            ssize_t fd = CSocketHandle::Accept((struct sockaddr*)(&addrin), &socklen);
            return (new CUNSocket(fd,CSocketAddrUn(addrin)));
        }

        INT CUNSocketServer::AcceptFD()
        {
            struct sockaddr_un addrin;
            socklen_t socklen = sizeof(addrin);
            ssize_t fd = CSocketHandle::Accept((struct sockaddr*)(&addrin), &socklen);
            if (fd == -1)
            {
                THROW_SYSERROR();
            }
            return fd;
        }

    }/// namespace GalaxyRT
}/// namespace Galaxy
