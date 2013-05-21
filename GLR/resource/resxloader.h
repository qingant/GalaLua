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

extern void resx_openlibs(lua_State * const state);

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
