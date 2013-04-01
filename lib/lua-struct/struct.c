/*
** {======================================================
** Library for packing/unpacking structures.
** $Id: struct.c,v 1.4 2012/07/04 18:54:29 roberto Exp $
** See Copyright Notice at the end of this file
** =======================================================
*/
/*
** Valid formats:
** > - big endian
** < - little endian
** ![num] - alignment  no suport
** x - pading no suport
** b/B - signed/unsigned int8_t
** h/H - signed/unsigned int16_t
** l/Ln - signed/unsigned interger with size `n' (default is size of int64_t)
** T   - size_t no suport
** i/In - signed/unsigned integer with size `n' (default is size of int32_t)
** cn - sequence of `n' chars (from/to a string); when packing, n==0 means
        the whole string; when unpacking, n==0 means use the previous
        read number as the string length
** s - zero-terminated string
** f - float no suport
** d - double no suport
** ' ' - ignored
*/

#include <stdint.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>


#include "lua.h"
#include "lauxlib.h"


#if (LUA_VERSION_NUM >= 502)

#define luaL_register(L,n,f)	luaL_newlib(L,f)

#endif

/* maximum size (in bytes) for integral types */
#define MAXINTSIZE	8

/* is 'x' a power of 2? */
#define isp2(x)		((x) > 0 && ((x) & ((x) - 1)) == 0)

/* dummy structure to get alignment requirements */
struct cD {
  char c;
  double d;
};


#define PADDING		(sizeof(struct cD) - sizeof(double))
#define MAXALIGN  	(PADDING > sizeof(int) ? PADDING : sizeof(int))


/* endian options */
#define BIG	0
#define LITTLE	1


static union {
  int dummy;
  char endian;
} const native = {1};


typedef struct Header {
  int endian;
  int align;
} Header;


static int getnum (const char **fmt, int df) {
  if (!isdigit(**fmt))  /* no number? */
    return df;  /* return default value */
  else {
    int a = 0;
    do {
      a = a*10 + *((*fmt)++) - '0';
    } while (isdigit(**fmt));
    return a;
  }
}


#define defaultoptions(h)	((h)->endian = native.endian, (h)->align = 1)



static size_t optsize (lua_State *L, char opt, const char **fmt) {
  switch (opt) {
    case 'B': case 'b': return sizeof(int8_t);
    case 'H': case 'h': return sizeof(int16_t);
    case 'L': case 'l': {
      int sz = getnum(fmt, sizeof(int64_t));
      if (sz != sizeof(int32_t) && sz != sizeof(int64_t))
        luaL_error(L, "number size is not sizeof(int32_t) or sizeof(int84_t)");
      return sz;
    }
    case 'c': return getnum(fmt,0);
    case 'i': case 'I': {
      int sz = getnum(fmt, sizeof(int32_t));
      if (sz != sizeof(int32_t) && sz != sizeof(int64_t))
        luaL_error(L, "integral size is not sizeof(int32_t) or sizeof(int84_t)");
      return sz;
    }
    default: return 0;  /* other cases do not need alignment */
  }
}

/*
** options to control endianess and alignment
*/
static void controloptions (lua_State *L, int opt,Header *h) {
  switch (opt) {
    case  ' ': return;  /* ignore white spaces */
    case '>': h->endian = BIG; return;
    case '<': h->endian = LITTLE; return;
    default: {
      const char *msg = lua_pushfstring(L, "invalid format option '%c'", opt);
      luaL_argerror(L, 1, msg);
    }
  }
}


static void putinteger (lua_State *L, luaL_Buffer *b, int arg, int endian,
                        int size) {
  lua_Number n = luaL_checknumber(L, arg);
  uint64_t value = 0;
  char buff[MAXINTSIZE] = {0};
  if (n < 0)
    value = (uint64_t)(int64_t)n;
  else
    value = (uint64_t)n;
  if (endian == LITTLE) {
    int i;
    for (i = 0; i < size; i++) {
      buff[i] = (value & 0xff);
      value >>= 8;
    }
  }
  else {
    int i;
    for (i = size - 1; i >= 0; i--) {
      buff[i] = (value & 0xff);
      value >>= 8;
    }
  }
  luaL_addlstring(b, buff, size);
}

static int b_pack (lua_State *L) {
  luaL_Buffer b;
  const char *fmt = luaL_checkstring(L, 1);
  Header h;
  int arg = 2;
  defaultoptions(&h);
  lua_pushnil(L);  /* mark to separate arguments from string buffer */
  luaL_buffinit(L, &b);
  while (*fmt != '\0') {
    int opt = *fmt++;
    size_t size = optsize(L, opt, &fmt);
    switch (opt) {
      case 'b': case 'B': case 'h': case 'H':
      case 'l': case 'L': case 'i': case 'I': {  /* integer types */
        putinteger(L, &b, arg++, h.endian, size);
        break;
      }
      case 'c': case 's': {
        size_t l;
        const char *s = luaL_checklstring(L, arg++, &l);
        if (size == 0) size = l;
        luaL_argcheck(L, l >= (size_t)size, arg, "string too short");
        luaL_addlstring(&b, s, size);
        if (opt == 's') {
          luaL_addchar(&b, '\0');  /* add zero at the end */
          size++;
        }
        break;
      }
      default: controloptions(L, opt,&h);
    }
  }
  luaL_pushresult(&b);
  return 1;
}


static lua_Number getinteger (const char *buff, int endian,
                        int issigned, int size) {
  uint64_t l = 0;
  int i;
  if (endian == BIG) {
    for (i = 0; i < size; i++) {
      l <<= 8;
      l |= (uint64_t)(unsigned char)buff[i];
    }
  }
  else {
    for (i = size - 1; i >= 0; i--) {
      l <<= 8;
      l |= (uint64_t)(unsigned char)buff[i];
    }
  }
  if (!issigned)
    return (lua_Number)l;
  else {  /* signed format */
    uint64_t mask = (uint64_t)(~((uint64_t)0)) << (size*8 - 1);
    if (l & mask)  /* negative value? */
      l |= mask;  /* signal extension */
    return (lua_Number)(int64_t)l;
  }
}


static int b_unpack (lua_State *L) {
  Header h;
  const char *fmt = luaL_checkstring(L, 1);
  size_t ld;
  const char *data = luaL_checklstring(L, 2, &ld);
  size_t pos = luaL_optinteger(L, 3, 1) - 1;
  defaultoptions(&h);
  lua_settop(L, 2);
  while (*fmt) {
    int opt = *fmt++;
    size_t size = optsize(L, opt, &fmt);
    luaL_argcheck(L, pos+size <= ld, 2, "data string too short");
    luaL_checkstack(L, 1, "too many results");
    switch (opt) {
      case 'b': case 'B': case 'h': case 'H':
      case 'l': case 'L': case 'i':  case 'I': {  /* integer types */
        int issigned = islower(opt);
        lua_Number res = getinteger(data+pos, h.endian, issigned, size);
        lua_pushnumber(L, res);
        break;
      }
      case 'c': {
        if (size == 0) {
          if (!lua_isnumber(L, -1))
            luaL_error(L, "format `c0' needs a previous size");
          size = lua_tonumber(L, -1);
          lua_pop(L, 1);
          luaL_argcheck(L, pos+size <= ld, 2, "data string too short");
        }
        lua_pushlstring(L, data+pos, size);
        break;
      }
      case 's': {
        const char *e = (const char *)memchr(data+pos, '\0', ld - pos);
        if (e == NULL)
          luaL_error(L, "unfinished string in data");
        size = (e - (data+pos)) + 1;
        lua_pushlstring(L, data+pos, size - 1);
        break;
      }
      default: controloptions(L, opt,&h);
    }
    pos += size;
  }
  lua_pushinteger(L, pos + 1);
  return lua_gettop(L) - 2;
}


static int b_size (lua_State *L) {
  Header h;
  const char *fmt = luaL_checkstring(L, 1);
  size_t pos = 0;
  defaultoptions(&h);
  while (*fmt) {
    int opt = *fmt++;
    size_t size = optsize(L, opt, &fmt);
    if (opt == 's')
      luaL_argerror(L, 1, "option 's' has no fixed size");
    else if (opt == 'c' && size == 0)
      luaL_argerror(L, 1, "option 'c0' has no fixed size");
    if (!isalnum(opt))
      controloptions(L, opt,&h);
    pos += size;
  }
  lua_pushinteger(L, pos);
  return 1;
}

/* }====================================================== */



static const struct luaL_Reg thislib[] = {
  {"pack", b_pack},
  {"unpack", b_unpack},
  {"size", b_size},
  {NULL, NULL}
};


LUALIB_API int luaopen_struct (lua_State *L);

LUALIB_API int luaopen_struct (lua_State *L) {
  luaL_register(L, "struct", thislib);
  return 1;
}


/******************************************************************************
* Copyright (C) 2010-2012 Lua.org, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

