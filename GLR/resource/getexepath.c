/*
 * getexepath.c
 *
 *  Created on: Jul 15, 2013
 *      Author: aikon
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/types.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <limits.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PROCFS
#define PROCFS  "/proc"
#endif

#ifndef PATH_MAX
#define PATH_MAX    4096
#endif

static int path_resolving(const char *const pathname, char *const resolved,
        const size_t siz)
{
    const char * const path = getenv("PATH");
    size_t length = strlen(path);
    char * const buf = (char * const) malloc(length + 1);
    if (buf == NULL)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, malloc.\n",
                __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    char * const sentinel = &buf[length];
    strncpy(buf, path, length);
    buf[length] = '\0';
    if (strlen(pathname) + 1 + 1 < siz)
    { // 预留字符串结尾符以及路径与文件名之间的‘/’分隔符所需要的存储空间
        length = siz - strlen(pathname) - 1 - 1;
    }
    else
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
        free(buf);
        return -1;
    }
    char *cur = buf;
    char *dlm = strchr(buf, ':');
    if (dlm == NULL)
    {
        dlm = sentinel;
    }
    while (cur < sentinel)
    {
        if (dlm - cur < length)
        {
            memcpy(resolved, cur, dlm - cur);
            resolved[dlm - cur] = '/';
            strncpy(&resolved[dlm - cur + 1], pathname, siz - (dlm - cur + 1));
            resolved[siz - 1] = '\0';
//            fprintf(stdout, "File %s, Function %s, Line %d, %s\n",
//                    __FILE__, __FUNCTION__, __LINE__, resolved);
            if (access(resolved, R_OK) == 0)
            {
                break;
            }
            resolved[0] = '\0';
        }
        else
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            free(buf);
            return -1;
        }
        cur = ++dlm;
        if ((dlm = strchr(cur, ':')) == NULL)
        {
            dlm = sentinel;
        }
    }
    free(buf);
    if (resolved[0] == '\0')
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}

#if defined(linux) || defined(__linux) || defined(__linux__)

extern int getexepath(const char * const pathname, char * const resolved,
        const size_t siz)
{
    const pid_t pid = getpid();
    char buf[PATH_MAX + 1] = {[sizeof(buf) - 1] = '\0'};
    if ((pathname != NULL) && (access(pathname, R_OK) == 0))
    {
        if (realpath(pathname, &buf[0]) == NULL)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return -1;
        }
        if (strlen(&buf[0]) < siz)
        {
            strncpy(resolved, &buf[0], siz);
            return 0;
        }
        else
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }
    if ((pathname != NULL) && (path_resolving(pathname, resolved, siz) == 0))
    {
        if (realpath(resolved, &buf[0]) == NULL)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return -1;
        }
        if (strlen(&buf[0]) < siz)
        {
            strncpy(resolved, &buf[0], siz);
            return 0;
        }
        else
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }
    if (access(PROCFS, R_OK) == 0)
    {
        if (access(PROCFS "/self", R_OK) == 0)
        { // "/proc/self" exists.
            if (snprintf(&buf[0], sizeof(buf), PROCFS "/self/exe") >= sizeof(buf))
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                        "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
        }
        else if (errno == ENOTDIR)
        { // "/proc/self" doesn't exist.
            if (snprintf(&buf[0], sizeof(buf), PROCFS "/%" PRIuMAX "/exe",
                    (uintmax_t) pid) >= sizeof(buf))
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                        "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
        }
        else
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return -1;
        }
        const ssize_t retval = readlink(&buf[0], resolved, siz);
        if ((retval > 0) && (retval < siz))
        {
            resolved[retval] = '\0';
            if (realpath(resolved, &buf[0]) == NULL)
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                        __FILE__, __FUNCTION__, __LINE__, strerror(errno));
                return -1;
            }
            if (strlen(&buf[0]) < siz)
            {
                strncpy(resolved, &buf[0], siz);
                return 0;
            }
            else
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                        "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
        }
        else if ((retval > 0) && (retval >= siz))
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
        else
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return -1;
        }
    }
    else
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
}

#elif defined(_AIX)

extern int getexepath(const char * const pathname, char * const resolved,
        const size_t siz)
{
    const pid_t pid = getpid();
    char buf[PATH_MAX + 1] = {[sizeof(buf) - 1] = '\0'};
    if ((pathname != NULL) && (access(pathname, R_OK) == 0))
    {
        if (realpath(pathname, &buf[0]) == NULL)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return -1;
        }
        if (strlen(&buf[0]) < siz)
        {
            strncpy(resolved, &buf[0], siz);
            return 0;
        }
        else
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }
    if ((pathname != NULL) && (path_resolving(pathname, resolved, siz) == 0))
    {
        if (realpath(resolved, &buf[0]) == NULL)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return -1;
        }
        if (strlen(&buf[0]) < siz)
        {
            strncpy(resolved, &buf[0], siz);
            return 0;
        }
        else
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }
    if (access(PROCFS, R_OK) == 0)
    {
        if (snprintf(resolved, siz, PROCFS "/%" PRIuMAX "/object/a.out",
                (uintmax_t) pid) >= siz)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
        if (access(resolved, R_OK) == 0)
        {
            return 0;
        }
    }
    else
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
}

#endif

#ifdef __cplusplus
}
#endif

