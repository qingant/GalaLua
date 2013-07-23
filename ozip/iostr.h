/*
 * iostr.h
 *
 *  Created on: 2013-7-19
 *      Author: ocaml
 */


#include "memfile.h"
#include "ioapi.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void fill_mem_filefunc OF((zlib_filefunc_def* pzlib_filefunc_def));
void fill_mem64_filefunc OF((zlib_filefunc64_def* pzlib_filefunc_def));

#ifdef __cplusplus
}
#endif
