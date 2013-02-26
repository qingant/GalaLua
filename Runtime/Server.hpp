/* -*- C++ -*- */

/* Time-stamp: <2012-04-17 10:00:25 星期二 by OCaml> */

/**
 * @file Server.hpp
 * @author qingant
 */

#ifndef SERVER_H
#define SERVER_H

#include "Socket.hpp"

#pragma once

namespace Galaxy
{
   namespace GalaxyRT
   {

      class CSocketServer:public CNonCopyAble
      {
      public:
         CSocketServer()
         {
         }
         virtual CSocket *Accept() = 0;
         virtual INT      GetFD() = 0;
         virtual CSocket *operator->() = 0;
         virtual ~CSocketServer()
         {
         }
      };

      template <typename ServerType>
      class CSocketServerAdapter:public CSocketServer
      {
      public:
         CSocketServerAdapter(ServerType *server)
			:_Server(server)
         {

         }
         CSocket *Accept()
         {
            return (CSocket*)_Server->AcceptEx();
         }
         INT GetFD()
         {
            return _Server->GetFD();
         }
         CSocket *operator->()
         {
            return (CSocket*)_Server.get();
         }

         virtual ~CSocketServerAdapter()
         {

         }
      private:
         std::auto_ptr<ServerType> _Server;
      };

      typedef CSocketServerAdapter<CTCPSocketServer> CTCPServer;
      typedef CSocketServerAdapter<CUNSocketServer> CUNServer;

   } /// namespace GalaxyRT
} /// namespace Galaxy

#endif /* SERVER_H */
