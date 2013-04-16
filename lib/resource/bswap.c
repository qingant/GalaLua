/*
 * bswap.c
 *
 *  Created on: Apr 9, 2013
 *      Author: esuomyekcim
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __linux__
#include <endian.h>
#elif defined(__aix__)
#include <sys/machine.h>
#elif defined(__freebsd__)
#include <sys/endian.h>
#endif

#include "bswap.h"

#ifdef __cplusplus
extern "C"
{
#endif

//#if defined(__aix__)
#if BYTE_ORDER == BIG_ENDIAN

inline uint16_t htobe16(const uint16_t h16)
{
    return h16;
}

inline uint16_t htole16(const uint16_t h16)
{
#if defined(__GNUC__) && __GNUC__ > 3 && __GNUC_MINOR__ > 2
    return __builtin_bswap16(h16);
#else
    return bswap_16(h16);
#endif
}

inline uint16_t be16toh(const uint16_t be16)
{
    return be16;
}

inline uint16_t le16toh(const uint16_t le16)
{
#if defined(__GNUC__) && __GNUC__ > 3 && __GNUC_MINOR__ > 2
    return __builtin_bswap16(le16);
#else
    return bswap_16(le16);
#endif
}

inline uint32_t htobe32(const uint32_t h32)
{
    return h32;
}

inline uint32_t htole32(const uint32_t h32)
{
#if defined(__GNUC__) && __GNUC__ > 3 && __GNUC_MINOR__ > 2
    return __builtin_bswap32(h32);
#else
    return bswap_32(h32);
#endif
}

inline uint32_t be32toh(const uint32_t be32)
{
    return be32;
}

inline uint32_t le32toh(const uint32_t le32)
{
#if defined(__GNUC__) && __GNUC__ > 3 && __GNUC_MINOR__ > 2
    return __builtin_bswap32(le32);
#else
    return bswap_32(le32);
#endif
}

inline uint64_t htobe64(const uint64_t h64)
{
    return h64;
}

inline uint64_t htole64(const uint64_t h64)
{
#if defined(__GNUC__) && __GNUC__ > 3 && __GNUC_MINOR__ > 2
    return __builtin_bswap64(h64);
#else
    return bswap_64(h64);
#endif
}

inline uint64_t be64toh(const uint64_t be64)
{
    return be64;
}

inline uint64_t le64toh(const uint64_t le64)
{
#if defined(__GNUC__) && __GNUC__ > 3 && __GNUC_MINOR__ > 2
    return __builtin_bswap64(le64);
#else
    return bswap_64(le64);
#endif
}

#elif BYTE_ORDER == LITTLE_ENDIAN  // end of '#elif BYTE_ORDER == BIG_ENDIAN'

inline uint16_t htobe16(const uint16_t h16)
{
#if defined(__GNUC__) && __GNUC__ > 3 && __GNUC_MINOR__ > 2
    return __builtin_bswap16(h16);
#else
    return bswap_16(h16);
#endif
}

inline uint16_t htole16(const uint16_t h16)
{
    return h16;
}

inline uint16_t be16toh(const uint16_t be16)
{
#if defined(__GNUC__) && __GNUC__ > 3 && __GNUC_MINOR__ > 2
    return __builtin_bswap16(be16);
#else
    return bswap_16(be16);
#endif
}

inline uint16_t le16toh(const uint16_t le16)
{
    return le16;
}

inline uint32_t htobe32(const uint32_t h32)
{
#if defined(__GNUC__) && __GNUC__ > 3 && __GNUC_MINOR__ > 2
    return __builtin_bswap32(h32);
#else
    return bswap_32(h32);
#endif
}

inline uint32_t htole32(const uint32_t h32)
{
    return h32;
}

inline uint32_t be32toh(const uint32_t be32)
{
#if defined(__GNUC__) && __GNUC__ > 3 && __GNUC_MINOR__ > 2
    return __builtin_bswap32(be32);
#else
    return bswap_32(be32);
#endif
}

inline uint32_t le32toh(const uint32_t h32)
{
    return h32;
}

inline uint64_t htobe64(const uint64_t h64)
{
#if defined(__GNUC__) && __GNUC__ > 3 && __GNUC_MINOR__ > 2
    return __builtin_bswap64(h64);
#else
    return bswap_64(h64);
#endif
}

inline uint64_t htole64(const uint64_t h64)
{
    return h64;
}

inline uint64_t be64toh(const uint64_t be64)
{
#if defined(__GNUC__) && __GNUC__ > 3 && __GNUC_MINOR__ > 2
    return __builtin_bswap64(be64);
#else
    return bswap_64(be64);
#endif
}

inline uint64_t le64toh(const uint64_t le64)
{
    return le64;
}

#endif // end of '#elif BYTE_ORDER == LITTLE_ENDIAN'
//#endif

#ifdef __cplusplus
}
#endif
