/* -*- C++ -*- */

/* Time-stamp: <2012-07-19 10:51:05 Thursday by qingant> */

/**
 * @file   Bridge.hpp
 * @author Ma Tao <qingant@gmail.com>
 * @date   Wed May 30 16:07:01 2012
 *
 * @bref
 *
 */

#ifndef _HEADDESP_H_
#define _HEADDESP_H_

#pragma once
#include "cdtype.h"
#include <stddef.h>

#pragma pack(1)

typedef struct{
    BYTE    Magic[4];
    UINT    WRKStatus ;
    UINT    Coding;
    UINT    Size;
    UINT    UsedLength;
    UINT    MappingKey;
    time_t  Stamp;
    UINT    CRC32;
}GalaHeadDesp,*PGalaHeadDesp;

#pragma pack()

#endif /* _HEADDESP_H_ */
