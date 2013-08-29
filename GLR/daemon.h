/*
 * daemon.h
 *
 *  Created on: Aug 29, 2013
 *      Author: aikon
 */

#ifndef DAEMON_H_
#define DAEMON_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 将调用进程转为后台执行的daemon进程
 * @param nochdir 等于零时，改变当前工作目录为根目录
 * @param noclose 等于零时，关闭当前进程已打开的文件描述符，并将标准输入、标准输出和标准错误重定向到/dev/null
 * @return 执行成功，返回0；否则，返回-1.
 */
extern int daemon(const int nochdir, const int noclose);

extern int lockfsegment(const int fd, const int type, const int whence,
        const int start, const int len);

extern int lockfsegmentw(const int fd, const int type, const int whence,
        const int start, const int len);

extern pid_t lockfsegmentif(const int fd, const int type, const int whence,
        const int start, const int len);

extern ssize_t writen(const int fd, const void * const buf, const size_t siz);

extern ssize_t readn(const int fd, void * const buf, const size_t siz);

/**
 *
 * @param progname 程序名
 * @param fpath progname.pid文件的路径
 * @param flags 指定FD_CLOEXEC标志
 * @return 成功，返回progname.pid文件的描述符；否则，返回-1.
 */
extern int uniqueness(const char * const progname, const char * const fpath,
        int flags);

#ifdef __cplusplus
}
#endif

#endif /* DAEMON_H_ */
