/* -*- C++ -*- */
// Time-stamp: <2012-04-17 09:59:52 星期二 by OCaml>
/**
 * @file    Utils.cpp
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
#include "Utils.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {

      INT GetHostByName(const std::string &name, struct hostent *phe)
      {
         struct hostent *_phe;
         static CPthreadMutex _mutex;
         CLockGuard guard(&_mutex);
         _phe = CRT_gethostbyname(name.c_str());
         if (_phe == NULL)
         {
			return -1;
         }
         *phe = *_phe;
         return 0;
      }



      void RTrim(std::string& s)
      {

         if (s.empty( ))
			return;

         std::string::iterator p;
         for (p = s.end( ); p != s.begin( ) && std::isspace(*--p););

         if (!std::isspace(*p))
			p++;

         s.erase(p, s.end( ));
      }

// Overloads to make cleaner calling for client code



      std::string LTrim(const std::string& s)
      {
         if (s.empty())
         {
			return std::string();
         }

         size_t i = 0;
         for (; i != s.size() && std::isspace(s[i]); ++i);

         return std::string(&s[i]);
      }




      std::string RLtrim(const std::string& s)
      {
         std::string tmp(s);

         RTrim(tmp);
         return LTrim(tmp);
      }


      UINT MakeConnKey(USHORT device_id, USHORT serial)
      {
         UINT rt = 0;
         rt = device_id<<(sizeof(USHORT)*8);
         rt += serial;

         return rt;
      }

      bool TestConnection(INT fd)
      {
#ifndef _AIX
         struct pollfd fds = {0, 0, 0};
         fds.fd = fd;
         fds.events = POLLIN;
         INT rt = CRT_poll(&fds, 1, 0);

         if (rt == 1)
         {
			if (fds.revents & POLLHUP || fds.revents & POLLERR || fds.revents & POLLNVAL)
			{
               return false;
			}
         }
         else if (rt == -1)
         {
			return false;
         }
         return true;
#else
         std::string buf(8, 0);
         INT flags = ::fcntl(fd, F_GETFL, 0);
         INT rt;
         rt = ::fcntl(fd, F_SETFL, flags|O_NONBLOCK);
         INT rc = ::recv(fd, &buf[0], buf.size(), MSG_PEEK);
         rt = ::fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
         if (rc == 0)
         {
			return false;
         }
         else if (rc == -1 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
         {
			return true;
         }
         else if (rc == -1)
         {
			return false;
         }
         else
         {
			return true;
         }
#endif
      }

      bool BadConnection(INT fd)
      {
         return !TestConnection(fd);
      }

   } /// namespace GalaxyRT
} /// namespace Galaxy
