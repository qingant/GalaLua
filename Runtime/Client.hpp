
/* -*- C++ -*- */

/* Time-stamp: <2012-04-17 08:42:40 星期二 by OCaml> */

/**
 * @file Server.hpp
 * @author qingant
 */

#ifndef CLIENT_H
#define CLIENT_H

#include "Socket.hpp"

#pragma once

namespace Galaxy
{
   namespace GalaxyRT
   {
	
      class CSocketClient:public CNonCopyAble
      {
      public:
         CSocketClient()
         {
         }    
         virtual CSocket *operator->() = 0;
         virtual ~CSocketClient()
         {
         }
      };

      template <typename ClientType>
      class CSocketClienAdapter:public CSocketClient
      {
      public:
         CSocketClienAdapter(ClientType *client)
			:_Client(client)
         {

         }
         CSocket *operator->()
         {
            return (CSocket*)_Client.get();
         }

         virtual ~CSocketClienAdapter()
         {

         }
      private:
         std::auto_ptr<ClientType> _Client;
      };

      typedef CSocketClienAdapter<CTCPSocketClient> CTCPClient;
      typedef CSocketClienAdapter<CUNSocketClient> CUNClient;

   } /// namespace GalaxyRT
} /// namespace Galaxy
#endif /* CLIENT_H */
