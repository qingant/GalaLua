/* -*- C++ -*- */

/* Time-stamp: <2012-07-02 15:36:34 Monday by 骆志辉> */

/**
 * @file   PyConfigure.hpp
 * @author 骆志辉 <camel_flying@163.com>
 * @date   Thu Jun 28 17:42:08 2012
 *
 * @bref
 *
 */

#ifndef _PYCONFIGURE_H_
#define _PYCONFIGURE_H_

#pragma once

#include <Python.h>
#include <structmember.h>
#include "Runtime/Exception.hpp"
#include "Configure.hpp"

typedef struct {
    PyObject_HEAD
    ConfigurePlt *_pConfigurePlt;
} udt_ConfigurePlt;

#endif /* _PYCONFIGURE_H_ */
