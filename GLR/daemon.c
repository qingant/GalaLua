/*
 * daemon.c
 *
 *  Created on: Apr 23, 2013
 *      Author: esuomyekcim
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#include "daemon.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PATH_DEVNULL
#define PATH_DEVNULL    "/dev/null"
#endif

#ifndef NUMFD_TO_CLOSE
#define NUMFD_TO_CLOSE  (4096)
#endif

/**
 * 将调用进程转为后台执行的daemon进程
 * @param nochdir 等于零时，改变当前工作目录为根目录
 * @param noclose 等于零时，关闭当前进程已打开的文件描述符，并将标准输入、标准输出和标准错误重定向到/dev/null
 * @return 执行成功，返回0；否则，返回-1.
 */
extern int daemon(const int nochdir, const int noclose)
{
    struct sigaction act;
    if (sigemptyset(&act.sa_mask) != 0)
    {
        return -1;
    }
    act.sa_flags = 0;
    act.sa_handler = SIG_IGN;

    struct sigaction oact;
    const int retval = sigaction(SIGHUP, &act, &oact);

    pid_t pid = fork();
    if (pid > 0)
    { /* parent process */
        _exit(EXIT_SUCCESS);
        return EXIT_SUCCESS; /* senseless, but make Eclipse CDT happy. */
    }
    else if (pid == 0)
    { /* child process */
        const pid_t newgrp = setsid();
        const int oerrno = errno;
        if (newgrp == (pid_t) -1)
        {
            errno = oerrno;
            return -1;
        }
        if ((pid = fork()) > 0)
        { /* child process */
            _exit(EXIT_SUCCESS);
            return EXIT_SUCCESS; /* senseless, but make Eclipse CDT happy. */
        }
        else if (pid == 0)
        { /* grandchild process */
            if (retval == 0)
            {
                sigaction(SIGHUP, &oact, NULL);
            }
            if (nochdir == 0)
            {
                chdir("/");
            }
            umask(0);

            if (noclose == 0)
            {
                long int tmpfd = sysconf(_SC_OPEN_MAX);
                if (tmpfd == -1)
                {
                    tmpfd = NUMFD_TO_CLOSE;
                }
                while ((--tmpfd) > -1)
                {
                    close(tmpfd);
                }
                const int fd = open(PATH_DEVNULL, O_RDWR, 0);
                if (fd == -1)
                {
                    return -1;
                }
                if (dup2(fd, STDIN_FILENO) != STDIN_FILENO)
                {
                    return -1;
                }
                if (dup2(fd, STDOUT_FILENO) != STDOUT_FILENO)
                {
                    return -1;
                }
                if (dup2(fd, STDERR_FILENO) != STDERR_FILENO)
                {
                    return -1;
                }
                if ((fd != STDIN_FILENO) && (fd != STDOUT_FILENO)
                        && (fd != STDERR_FILENO))
                {
                    close(fd);
                }
            }
            return 0; /* grandchild returned */
        }
        else
        { /* second fork() returned -1. */
            return -1;
        }
    }
    else
    { /* first fork() returned -1. */
        return -1;
    }
}

extern int lockfsegment(const int fd, const int type, const int whence,
        const int start, const int len)
{
    struct flock buff = {
            .l_type = type, .l_whence = whence,
            .l_start = start, .l_len = len
    };
    if (fcntl(fd, F_SETLK, &buff) == -1)
    {
        fprintf(stderr, "File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    return 0;
}

extern int lockfsegmentw(const int fd, const int type, const int whence,
        const int start, const int len)
{
    struct flock buff = {
            .l_type = type, .l_whence = whence,
            .l_start = start, .l_len = len
    };
    if (fcntl(fd, F_SETLKW, &buff) == -1)
    {
        fprintf(stderr, "File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    return 0;
}

extern pid_t lockfsegmentif(const int fd, const int type, const int whence,
        const int start, const int len)
{
    struct flock buff = {
            .l_type = type, .l_whence = whence,
            .l_start = start, .l_len = len
    };
    if (fcntl(fd, F_GETLK, &buff) == -1)
    {
        fprintf(stderr, "File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return (pid_t) -1;
    }
    return ((buff.l_type == F_UNLCK) ? (pid_t) 0 : buff.l_pid);
}

extern ssize_t writen(const int fd, const void * const buf, const size_t siz)
{
    ssize_t numWritten;
    size_t totWritten = 0;
    const char *buff = (const char *) buf;
    while (totWritten < siz)
    {
        if((numWritten = write(fd, buff, siz - totWritten)) > 0)
        {
            buff += numWritten;
            totWritten += numWritten;
        }
        else if (numWritten == 0)
        {
            return totWritten;
        }
        else if (errno != EINTR)
        {
            if (totWritten == 0)
            {
                fprintf(stderr, "File %s, Function %s, Line %d, %s.\n",
                        __FILE__, __FUNCTION__, __LINE__, strerror(errno));
                return -1;
            }
            return totWritten;
        }
    }
    return totWritten; /* 如果writen从此处返回，totWritten的值为siz */
}

extern ssize_t readn(const int fd, void * const buf, const size_t siz)
{
    ssize_t numRead;
    size_t totRead = 0;
    char *buff = (char *) buf;
    while (totRead < siz)
    {
        if((numRead = read(fd, buff, siz - totRead)) > 0)
        {
            buff += numRead;
            totRead += numRead;
        }
        else if (numRead == 0)
        { /* 已读到文件结尾. */
            return totRead; /* 如果这是第一次调用read函数，totRead的值可能为0 */
        }
        else if (errno != EINTR)
        {
            if (totRead == 0)
            {
                fprintf(stderr, "File %s, Function %s, Line %d, %s.\n",
                        __FILE__, __FUNCTION__, __LINE__, strerror(errno));
                return -1;
            }
            return totRead;
        }
    }
    return totRead; /* 如果readn从此处返回，totRead的值为siz */
}

/**
 *
 * @param progname 程序名
 * @param fpath progname.pid文件的路径
 * @param flags 指定FD_CLOEXEC标志
 * @return 成功，返回progname.pid文件的描述符；否则，返回-1.
 */
extern int uniqueness(const char * const progname, const char * const fpath,
        int flags)
{
    const int fd = open(fpath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        fprintf(stderr, "File %s, Function %s, Line %d, %s, "
                "Could not open PID file %s.\n", __FILE__, __FUNCTION__,
                __LINE__, strerror(errno), fpath);
        return -1;
    }
    do {
        if (flags & FD_CLOEXEC)
        {
            if ((flags = fcntl(fd, F_GETFD)) == -1)
            {
                fprintf(stderr, "File %s, Function %s, Line %d, %s, "
                        "Could not get flags for PID file %s.\n", __FILE__,
                        __FUNCTION__, __LINE__, strerror(errno), fpath);
                break;
            }
            flags |= FD_CLOEXEC;
            if (fcntl(fd, F_SETFD, flags) == -1)
            {
                fprintf(stderr, "File %s, Function %s, Line %d, %s, "
                        "Could not set flags for PID file %s.\n", __FILE__,
                        __FUNCTION__, __LINE__, strerror(errno), fpath);
                break;
            }
        }
        if (lockfsegment(fd, F_WRLCK, SEEK_SET, 0, 0) == -1)
        {
            if ((errno == EAGAIN) || (errno == EACCES))
            {
                fprintf(stderr, "File %s, Function %s, Line %d, %s, "
                        "PID file '%s' is locked, probably '%s' is already "
                        "running.\n", __FILE__, __FUNCTION__, __LINE__,
                        strerror(errno), fpath, progname);
            }
            else
            {
                fprintf(stderr, "File %s, Function %s, Line %d, %s, "
                        "Unable to lock PID file '%s'.\n", __FILE__,
                        __FUNCTION__, __LINE__, strerror(errno), fpath);
            }
            break;
        }
        if (ftruncate(fd, 0) == -1)
        {
            fprintf(stderr, "File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            break;
        }
        char buff[64];
        const int length = snprintf(&buff[0], sizeof(buff), "%" PRIuMAX "\n",
                (uintmax_t) getpid());
        if (length >= (int) sizeof(buff))
        {
            fprintf(stderr, "File %s, Function %s, Line %d, Truncation.\n",
                    __FILE__, __FUNCTION__, __LINE__);
        }
        /**
         * 需要保证buff中的内容全写入文件中，也许需要writen函数。
         */
        if (writen(fd, &buff[0], length) == -1)
        {
            fprintf(stderr, "File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            break;
        }
        return fd;
    } while (false);
    while ((close(fd) == -1) && (errno == EINTR))
    {
        /* null sentence */
    }
    return -1;
}

#ifdef __cplusplus
}
#endif
