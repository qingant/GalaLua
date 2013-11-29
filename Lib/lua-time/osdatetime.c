#ifdef __linux__
#define _XOPEN_SOURCE
#endif
#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

#define loslib_c
#define LUA_LIB

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static void setfield (lua_State *L, const char *key, int value)
{
  lua_pushinteger(L, value);
  lua_setfield(L, -2, key);
}

static void setboolfield (lua_State *L, const char *key, int value)
{
  if (value < 0)
    return;
  lua_pushboolean(L, value);
  lua_setfield(L, -2, key);
}

static int os_strptime(lua_State *L)
{
   const char *date_str = luaL_checkstring(L, 1);
   const char *format = luaL_checkstring(L, 2);
   if(NULL == date_str||!strlen(date_str)|| NULL == format||!strlen(format))
   {
      return luaL_error(L, "must be pass #2 args,type is string");
   }
   struct tm stm;
   if(NULL == strptime(date_str, format, &stm))
   {
      return luaL_error(L, "invalid args");
   }
   /*struct timeval tv;
   gettimeofday(&tv,NULL);*/
   lua_createtable(L, 0, 10);
   setfield(L, "msec", 0);
   setfield(L, "sec", stm.tm_sec);
   setfield(L, "min", stm.tm_min);
   setfield(L, "hour", stm.tm_hour);
   setfield(L, "day", stm.tm_mday);
   setfield(L, "month", stm.tm_mon+1);
   setfield(L, "year", stm.tm_year+1900);
   setfield(L, "wday", stm.tm_wday+1);
   setfield(L, "yday", stm.tm_yday+1);
   setboolfield(L, "isdst", stm.tm_isdst);

   return 1;
}

static int os_gettimeofday(lua_State *L)
{
   struct timeval tv;
   gettimeofday(&tv,NULL);
   lua_createtable(L, 0, 2);
   setfield(L, "msec", tv.tv_usec);
   setfield(L, "sec", tv.tv_sec);

   return 1;
}

static int os_mktime(lua_State *L)
{
   const char *date_str = luaL_checkstring(L, 1);
   const char *format = luaL_checkstring(L, 2);
   if(NULL == date_str||!strlen(date_str)|| NULL == format||!strlen(format))
   {
      return luaL_error(L, "must be pass #2 args,type is string");
   }
   struct tm stm;
   if(NULL == strptime(date_str, format, &stm))
   {
      return luaL_error(L, "invalid args");
   }
   time_t msec = mktime(&stm);
   lua_pushnumber(L,msec);

   return 1;
}

static const luaL_Reg systime[] = {
  {"strptime",      os_strptime},
  {"gettimeofday",  os_gettimeofday},
  {"mktime",        os_mktime},
  {NULL, NULL}
};

LUALIB_API int luaopen_osdatetime (lua_State *L) {
  luaL_register(L, LUA_OSLIBNAME, systime);
  return 1;
}
