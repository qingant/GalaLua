/* -*- C++ -*- */

/* Time-stamp: <2012-04-17 08:54:25 星期二 by OCaml> */

/**
 * @file Dll.hpp
 * @author qingant
 */

#ifndef DLL_H
#define DLL_H
#include "Object.hpp"
#include "Exception.hpp"
#pragma once

namespace Galaxy
{
namespace GalaxyRT
{

class CDll: public CObject
{
public:
    CDll(const std::string& path, INT flag=RTLD_LAZY);
    INT Close();
    VOID* Dlsym(std::string) const;
    ~CDll();
private:
    std::string m_dll_path;
    PVOID       m_dll_handle;


};
} /// namespace GalaxyRT
} /// namespace Galaxy
#endif /* DLL_H */
