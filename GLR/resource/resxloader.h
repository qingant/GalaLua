/*
 * resxloader.h
 *
 *  Created on: May 20, 2013
 *      Author: esuomyekcim
 */

#ifndef RESXLOADER_H_
#define RESXLOADER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/types.h>


#ifdef __cplusplus
extern "C"
{
#endif

#include "lua.h"
/* 声明lua的c模块的入口函数 */
extern int luaopen_cjson(lua_State *);
extern int luaopen_struct (lua_State *);
extern int luaopen_iconv(lua_State *);
extern int luaopen_minizip(lua_State *);
extern int luaopen__dir(lua_State *);
extern int luaopen_sigar(lua_State *);
extern int luaopen_xml(lua_State *);
/* extern int luaopen_lxp (lua_State *); */

/* 将lua的c模块的入口函数和模块名加入到lualibrary数组中 */
const luaL_Reg lualibray[] = {
    { "cjson", luaopen_cjson },
    { "struct", luaopen_struct },
    { "iconv", luaopen_iconv },
    { "minizip", luaopen_minizip },
    { "_dir", luaopen__dir },
    { "sigar", luaopen_sigar },
    { "xml", luaopen_xml },
/*    { "lxp", luaopen_lxp }, */
    { (const char *) NULL, (lua_CFunction) NULL }
};

void resx_openlibs(lua_State * const state);

int resx_loader(lua_State * const state);

/**
 * 将资源包中的lua模块载入到lua环境中，效果等同于lua脚本中的require
 * @param pathname 载入的模块名
 * @param state
 * @return
 */
int resx_require(const char * const pathname, lua_State * const state);

/**
 * 将lua模块的loader函数载入到package.loaders列表
 * @param state lua栈
 * @param func loader函数
 */
void resx_setloader(lua_State * const state, int (*func)(lua_State *));

/**
 * 执行模块中的main函数
 * @param state
 * @param name 模块名
 * @return
 */
int resx_main_execute(lua_State * const state, const char *name);

void GLR_initializer(lua_State * state);

#ifdef __cplusplus
}
#endif

#endif /* RESXLOADER_H_ */
