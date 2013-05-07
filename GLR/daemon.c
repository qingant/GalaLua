/*
 * daemon.c
 *
 *  Created on: Apr 23, 2013
 *      Author: esuomyekcim
 */

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <stdbool.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define PATH_DEVNULL    "/dev/null"

/**
 * 将调用进程转为后台执行的daemon进程
 * @param nochdir 等于零时，改变当前工作目录为根目录
 * @param noclose 等于零时，关闭当前进程已打开的文件描述符，并将标准输入、标准输出和标准错误重定向到/dev/null
 * @return 执行成功，返回0；否则，返回-1.
 */
int daemonize(const int nochdir, const int noclose)
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
        return EXIT_SUCCESS; /* non sense, but make Eclipse CDT happy. */
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
            return EXIT_SUCCESS; /* non sense, but make Eclipse CDT happy. */
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
                long int maxfd;
                if ((maxfd = sysconf(_SC_OPEN_MAX)) == -1)
                {
                    maxfd = 1024;
                }
                for (int fd = 0; fd < maxfd; ++ fd)
                {
                    close(fd);
                }
                const int fd = open(PATH_DEVNULL, O_RDWR, 0);
                if (fd != -1)
                {
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
                    if (fd > 2)
                    {
                        close(fd);
                    }
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

#ifdef __cplusplus
}
#endif
