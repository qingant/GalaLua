/*
 * getexepath.c
 *
 *  Created on: Jul 19, 2013
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
#include <sys/stat.h>

#include <limits.h>
#include <libgen.h>

#if defined(__hpux) || defined(__hpux__)
#include <dlfcn.h>
#include <sys/pstat.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PROCFS
#define PROCFS  "/proc"
#endif

static size_t strlcpy(char * const dest, const char * const src, size_t siz)
{
    const size_t retval = strlen(src);
    if (siz > 0)
    {
        siz = ((retval < siz) ? retval : (siz - 1));
        memcpy(dest, src, siz);
        dest[siz] = '\0';
    }
    return retval; /* does not include '\0' */
}

static size_t strlcat(char * const dest, const char * const src, size_t siz)
{
    const size_t dstlength = strlen(dest);
    const size_t srclength = strlen(src);
    const size_t retval = dstlength + srclength;
    if (siz > dstlength)
    {
        siz = ((retval < siz) ? srclength : (siz - dstlength));
        memcpy(&dest[dstlength], src, siz);
        dest[dstlength + siz] = '\0';
    }
    return retval; /* does not include '\0' */
}

static char *realpathname(const char * const pathname, char * const resolved,
        const size_t siz)
{
    char *dest = resolved;
    const char *src = pathname;
    const char * const sentinel = resolved + siz;
    if ((src[0] == '.')
            && ((src[1] == '/') || ((src[1] == '.') && (src[2] == '/'))))
    {
        if (getcwd(resolved, siz) == NULL)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return NULL;
        }
        dest = strchr(resolved, '\0');
        if (src[1] == '/')
        { /* (src[0] == '.') && (src[1] == '/') */
            src = &src[1];
        }
        else
        { /* (src[0] == '.') && (src[1] == '.') && (src[2] == '/') */
            src = &src[2];
            while ((dest > resolved) && (dest[0] != '/'))
            {
                --dest;
            }
        }
    }
    else if (src[0] != '/')
    {
        resolved[0] = '\0';
        return NULL;
    }
    if (dest < sentinel)
    {
        *dest = '/';
        ++dest;
    }
    const char * const delimiter = &resolved[1];
    while ((src[0] != '\0') && (dest < sentinel))
    {
        if ((src[1] == '.') && ((src[2] == '/') || (src[2] == '\0')))
        {
            src = &src[2];
        }
        else if ((src[1] == '.') && (src[2] == '.')
                && ((src[3] == '/') || (src[3] == '\0')))
        {
            src = &src[3];
            while (dest > delimiter)
            {
                --dest;
                if (dest[-1] == '/')
                {
                    break;
                }
            }
        }
        else if ((src[1] == '/') || (src[1] == '\0'))
        {
            ++src;
        }
        else
        {
            for (++src; (src[0] != '/') && (src[0] != '\0')
                    && (dest < sentinel); ++dest, ++src)
            {
                *dest = *src;
            }
            if (dest < sentinel)
            {
                *dest = '/';
                ++dest;
            }
        }
    }
    if (src[0] != '\0')
    { /* dest >= sentinel */
        resolved[siz - 1] = '\0';
        return NULL;
    }
    else if (dest > delimiter)
    { /* dest < sentinel */
        dest[-1] = '\0';
    }
    else
    { /* dest < sentinel */
        dest[0] = '\0';
    }
    return resolved;
}

static int path_resolving(const char * const pathname, char * const resolved,
        const size_t siz)
{
    const char * const pathenv = getenv("PATH");
    const size_t pathlength = strlen(pathenv);
    char * const buff = (char * const ) malloc(pathlength + 1);
    if (buff == NULL)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n", __FILE__,
                __FUNCTION__, __LINE__);
        return -1;
    }
    memcpy(buff, pathenv, pathlength);
    buff[pathlength] = '\0';

    const char *cursor = buff;
    char * const sentinel = &buff[pathlength];
    char *delimiter = strchr(cursor, ':');
    if (delimiter != NULL)
    {
        delimiter[0] = '\0';
    }
    else
    {
        delimiter = sentinel;
    }
#ifdef PATH_MAX
    char fpath[PATH_MAX + 1] = {[sizeof(fpath) - 1] = '\0'};
#else
    char fpath[UINT16_MAX + 1] = {[sizeof(fpath) - 1] = '\0'};
#endif
    size_t length;
    resolved[0] = '\0';
    while (cursor < sentinel)
    {
        if (realpathname(cursor, &fpath[0], sizeof(fpath)) == NULL)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
            break;
        }
        if ((length = strlcat(&fpath[0], "/", sizeof(fpath)))
                >= sizeof(fpath))
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            break;
        }
        if (strlcpy(&fpath[length], pathname, sizeof(fpath) - length)
                >= sizeof(fpath) - length)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            break;
        }
        if (access(&fpath[0], R_OK) == 0)
        {
            if (strlcpy(resolved, &fpath[0], siz) >= siz)
            {
                resolved[0] = '\0';
                fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                        "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            }
            break;
        }

        cursor = ++delimiter;
        if ((delimiter = strchr(cursor, ':')) != NULL)
        {
            delimiter[0] = '\0';
        }
        else
        {
            delimiter = sentinel;
        }
    }
    free(buff);
    if (resolved[0] == '\0')
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, Not found.\n",
                __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}

int realexepath(const char * const pathname, char * const resolved,
        const size_t siz)
{
#ifdef PATH_MAX
    char buff[PATH_MAX + 1] = {[sizeof(buff) - 1] = '\0' };
#else
    char buff[UINT16_MAX + 1] = {[sizeof(buff) - 1] = '\0'};
#endif
    if (realpathname(pathname, &buff[0], sizeof(buff)) == NULL)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    ssize_t length;
    struct stat statbuf;
    do
    {
        if (lstat(&buff[0], &statbuf) == -1)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return -1;
        }
        if (S_ISREG(statbuf.st_mode))
        {
            if (realpathname(&buff[0], resolved, siz) == NULL)
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                        "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
            return 0;
        }
        else if (S_ISLNK(statbuf.st_mode))
        {
            if ((length = readlink(&buff[0], resolved, siz)) == -1)
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                        __FILE__, __FUNCTION__, __LINE__, strerror(errno));
                return -1;
            }
            else if (length < siz)
            {
                resolved[length] = '\0';
                if (resolved[0] == '/')
                {
                    if (realpathname(resolved, &buff[0], sizeof(buff)) == NULL)
                    {
                        fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                                "Truncation.\n", __FILE__, __FUNCTION__,
                                __LINE__);
                        return -1;
                    }
                }
                else
                {
                    const char * const directory = dirname(&buff[0]);
                    strlcpy(&buff[0], directory, sizeof(buff));
                    strlcat(&buff[0], "/", sizeof(buff));
                    if (strlcat(&buff[0], &resolved[0], sizeof(buff))
                            >= sizeof(buff))
                    {
                        fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                                "Truncation.\n", __FILE__, __FUNCTION__,
                                __LINE__);
                        return -1;
                    }
                }
            }
            else
            {
                resolved[siz - 1] = '\0';
                fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                        "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
        }
        else
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, mode %"
                    PRIoMAX ".\n", __FILE__, __FUNCTION__, __LINE__,
                    (uintmax_t) statbuf.st_mode);
            return -1;
        }
    } while (true);
    return -1; /* senseless, make Eclipse CDT happy */
}

#if defined(linux) || defined(__linux) || defined(__linux__)

extern int getexepath(const char * const pathname, char * const resolved,
        const size_t siz)
{
    if (access(PROCFS, R_OK) == 0)
    {
        ssize_t retval = -1;
        if ((retval = readlink(PROCFS "/self/exe", resolved, siz)) == -1)
        {
            resolved[0] = '\0';
            if (errno == ENOTDIR)
            { // "/proc/self" doesn't exist.
#ifdef PATH_MAX
                char buff[PATH_MAX + 1] = {[sizeof(buff) - 1] = '\0'};
#else
                char buff[UINT16_MAX + 1] = {[sizeof(buff) - 1] = '\0'};
#endif
                const pid_t pid = getpid();
                if (snprintf(&buff[0], siz, PROCFS "/%" PRIuMAX "/exe",
                        (uintmax_t) pid) < siz)
                {
                    if ((retval = readlink(&buff[0], resolved, siz)) == -1)
                    {
                        resolved[0] = '\0';
                    }
                }
            }
        }
        if (retval > 0)
        {
            if (retval < siz)
            {
                resolved[retval] = '\0';
                return 0;
            }
            else
            {
                resolved[siz - 1] = '\0';
                fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                        "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
        }
    }
    if (access(pathname, R_OK) == 0)
    {
#ifdef PATH_MAX
        char buff[PATH_MAX + 1] = {[sizeof(buff) - 1] = '\0' };
#else
        char buff[UINT16_MAX + 1] = {[sizeof(buff) - 1] = '\0'};;
#endif
        if (realpathname(pathname, &buff[0], sizeof(buff)) == NULL)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
        if (strlcpy(resolved, &buff[0], siz) >= siz)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }
    else if (path_resolving(pathname, resolved, siz) != 0)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}

#elif defined(_AIX)

extern int getexepath(const char * const pathname, char * const resolved,
        const size_t siz)
{
    if (access(PROCFS, R_OK) == 0)
    {
        const pid_t pid = getpid();
        if (snprintf(resolved, siz, PROCFS "/%" PRIuMAX "/object/a.out",
                (uintmax_t) pid) < siz)
        {
            if (access(resolved, R_OK) == 0)
            {
                return 0;
            }
        }
    }
    if (access(pathname, R_OK) == 0)
    {
#ifdef PATH_MAX
        char buff[PATH_MAX + 1] = {[sizeof(buff) - 1] = '\0'};
#else
        char buff[UINT16_MAX + 1] = {[sizeof(buff) - 1] = '\0'};;
#endif
        if (realpathname(pathname, &buff[0], sizeof(buff)) == NULL)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return -1;
        }
        if (strlcpy(resolved, &buff[0], siz) >= siz)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }
    else if (path_resolving(pathname, resolved, siz) != 0)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}

#elif defined(__hpux) || defined(__hpux__)

extern int getexepath(const char * const pathname, char * const resolved,
        const size_t siz)
{
    struct load_module_desc desc;
    const void * const handle = dlget(-2, &desc, sizeof(struct load_module_desc));
    if (handle != NULL)
    {
        const char * const name = dlgetname(&desc, sizeof(struct load_module_desc),
                (void *(*)(void *, unsigned long long, size_t, int)) NULL, 0, 0);
        if (dlclose(handle) != 0)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, dlerror());
        }
        if (name != NULL)
        {
            if (strlcpy(resolved, name, siz) < siz)
            {
                return 0;
            }
            else
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                        "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
        }
    }
    if (access(pathname, R_OK) == 0)
    {
#ifdef PATH_MAX
        char buff[PATH_MAX + 1] = {[sizeof(buff) - 1] = '\0'};
#else
        char buff[UINT16_MAX + 1] = {[sizeof(buff) - 1] = '\0'};
#endif
        if (realpathname(pathname, &buff[0], sizeof(buff)) == NULL)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return -1;
        }
        if (strlcpy(resolved, &buff[0], siz) >= siz)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }
    else if (path_resolving(pathname, resolved, siz) != 0)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}

#endif

#ifdef __cplusplus
}
#endif

#ifdef TEST
int main(int argc, char *argv[])
{
#ifdef PATH_MAX
    char buff[PATH_MAX + 1] = {[sizeof(buff) - 1] = '\0'};
#else
    char buff[UINT16_MAX + 1] = {[sizeof(buff) - 1] = '\0'};;
#endif
    if (argc > 1) {
        if (realpathname(argv[1], &buff[0], sizeof(buff)) != NULL)
        {
            fprintf(stdout, "realpathname = %s\n", &buff[0]);
        }
        else
        {
            fprintf(stdout, "realpathname failed\n");
        }

        if (realexepath(argv[1], &buff[0], sizeof(buff)) == 0)
        {
            fprintf(stdout, "realexepath = %s\n", &buff[0]);
        }
        else
        {
            fprintf(stdout, "realexepath failed\n");
        }
    }
    if (getexepath(argv[0], &buff[0], sizeof(buff)) == 0)
    {
        printf("argv[0] = %s, buff = %s\n", argv[0], &buff[0]);
    }
    return 0;
}
#endif
