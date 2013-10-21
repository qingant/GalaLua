/**
 * File: codec.hpp 
 * author: Ma Tao 
 */
#pragma once
#include <cstdlib>
#include <cstdio>



typedef unsigned int ucs4_t;
typedef unsigned short DBCHAR;
typedef ucs4_t *unicode_str;
struct dbcs_index {
	const ucs4_t *map;
	unsigned char bottom, top;
};
struct unim_index {
	const DBCHAR *map;
	unsigned char bottom, top;
};
/* a unicode "undefined" codepoint */
#define UNIINV  0xFFFE

/* internal-use DBCS codepoints which aren't used by any charsets */
#define NOCHAR  0xFFFF
#define MULTIC  0xFFFE
#define DBCINV  0xFFFD

/* shorter macros to save source size of mapping tables */
#define U UNIINV
#define N NOCHAR
#define M MULTIC
#define D DBCINV

class Codec
{
public:
    virtual size_t encode(unicode_str uStr, size_t uLen, char *str, size_t sLen) = 0;
    virtual size_t decode(const char *str, size_t sLen, unicode_str uStr, size_t uLen) = 0;
};
