/*
 * bswap.h
 *
 *  Created on: Apr 10, 2013
 *      Author: esuomyekcim
 */

#ifndef BSWAP_H_
#define BSWAP_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/types.h>

#if defined(__linux__)
#  if __GLIBC__ > 1 && __GLIBC_MINOR__ > 8
#    ifndef _BSD_SOURCE
#      define _BSD_SOURCE
#    endif
#    include <endian.h>
#  else
#    include <byteswap.h>
#  endif
#elif defined(__freebsd__)
   #include <sys/endian.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef bswap16
#  ifdef bswap_16
#    define bswap16(i16)    bswap_16(i16)
#  else
#    define bswap16(i16)    ((uint16_t)((((i16) & 0x00ff) << 8)         \
                                | (((i16) >> 8) & 0x00ff)))
#  endif
#endif

#ifndef bswap32
#  ifdef bswap_32
#    define bswap32(i32)    bswap_32(i32)
#  else
#    define bswap32(i32)    ((uint32_t)((((i32) & 0xff000000u) >> 24)   \
                                | (((i32) & 0x00ff0000u) >> 8)          \
                                | (((i32) & 0x0000ff00u) << 8)          \
                                | (((i32) & 0x000000ffu) << 24)))
#  endif
#endif

#ifndef bswap64
#  ifdef bswap_64
#    define bswap64(i64)    bswap_64(i64)
#  else
#    define bswap64(i64)    ((uint64_t)((((i64) & 0xff00000000000000ull) >> 56) \
                                | (((i64) & 0x00ff000000000000ull) >> 40)       \
                                | (((i64) & 0x0000ff0000000000ull) >> 24)       \
                                | (((i64) & 0x000000ff00000000ull) >> 8)        \
                                | (((i64) & 0x00000000ff000000ull) << 8)        \
                                | (((i64) & 0x0000000000ff0000ull) << 24)       \
                                | (((i64) & 0x000000000000ff00ull) << 40)       \
                                | (((i64) & 0x00000000000000ffull) << 56)))
#  endif
#endif

#ifndef __freebsd__
#  if (defined(__aix__) || (defined(__linux__) && (__GLIBC__ < 2 || __GLIBC_MINOR__ < 9)))
    uint16_t htobe16(uint16_t h16);
    uint16_t htole16(uint16_t h16);
    uint16_t be16toh(uint16_t be16);
    uint16_t le16toh(uint16_t le16);

    uint32_t htobe32(uint32_t h32);
    uint32_t htole32(uint32_t h32);
    uint32_t be32toh(uint32_t be32);
    uint32_t le32toh(uint32_t le32);

    uint64_t htobe64(uint64_t h64);
    uint64_t htole64(uint64_t h64);
    uint64_t be64toh(uint64_t be64);
    uint64_t le64toh(uint64_t le64);
#  endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* BSWAP_H_ */