/* -*- C++ -*- */
// Time-stamp: <2012-04-17 09:02:19 星期二 by OCaml>
/**
 * @file    Dll.cpp
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
#include "Dll.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {

      CDll::CDll(const std::string &path, INT flag):m_dll_path(path),m_dll_handle(NULL)
      {
         CPSTR szPath = path.c_str();
         if (m_dll_path[0] == '\x00')
         {
			szPath = NULL;
         }
         m_dll_handle = ::dlopen(szPath, flag);
         if (m_dll_handle == NULL)
         {
			const CHAR *errmsg = CRT_dlerror();
			if (errmsg == NULL)
			{
               printf("error\n");
               THROW_EXCEPTION(0, "dlopen error");

			}
			printf("%s\n", errmsg);
			THROW_EXCEPTION(0, errmsg);
         }
      }
      CDll::~CDll()
      {
         Close();
      }

      INT CDll::Close()
      {
         return CRT_dlclose(m_dll_handle);
      }

      VOID* CDll::Dlsym(const std::string name) const
      {
         VOID *handle = CRT_dlsym(m_dll_handle, name.c_str());
         if (handle == NULL)
         {
			const CHAR *errmsg = CRT_dlerror();
			THROW_GENERALERROR(CException, errmsg);
         }
         return handle;
      }
   } /// namespace GalaxyRT
} /// namespace Galaxy

