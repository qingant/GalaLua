/* -*- C++ -*- */

/* Time-stamp: <2012-05-21 15:53:07 Monday by OCaml> */

/**
 * @file   PyGalaxy.h
 * @author OCaml <camel.flying@gmail.com>
 * @date   Mon Apr 23 17:52:22 2012
 *
 * @bref
 *
 */


#ifndef PYGALAXY_H
#define PYGALAXY_H

#pragma once

#include "PyError.cpp"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
initPyGalaxy(void);

#ifdef __cplusplus
}
#endif


#endif /* PYGALAXY_H */
