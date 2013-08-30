/*
 * SigleInstantiation.cpp
 *
 *  Created on: Aug 29, 2013
 *      Author: aikon
 */

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <signal.h>
#include <sys/select.h>

#include <ctime>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <exception>
#include "daemon.h"
#include "SigleInstantiation.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

static ssize_t ExtraInfo(const int fd)
{
    const time_t current = time((time_t *) 0);
    char buff[256];
    const int length = snprintf(&buff[0], sizeof(buff), "%lu\n%ld\n%s\n",
            (unsigned long) getuid(), (long int) current, ctime(&current));
    return writen(fd, &buff[0], length);
}


SigleInstantiation::SigleInstantiation(const char *const prog,
        const char *const path):progname(prog), pathname(path)
{
    if ((fd = uniqueness(prog, path, 0)) == -1)
    {
        char buff[512];
        snprintf(&buff[0], sizeof(buff), "progname: %s, pathname: %s, %s",
                prog, path, strerror(errno));
        std::string except(&buff[0]);
        throw(except);
    }
    ExtraInfo(fd);
}

SigleInstantiation::SigleInstantiation(const std::string &prog,
        const std::string &path):progname(prog), pathname(path)
{
    if ((fd = uniqueness(prog.c_str(), path.c_str(), 0)) == -1)
    {
        char buff[512];
        snprintf(&buff[0], sizeof(buff), "progname: %s, pathname: %s, %s",
                prog.c_str(), path.c_str(), strerror(errno));
        std::string except(&buff[0]);
        throw(except);
    }
    ExtraInfo(fd);
}

SigleInstantiation::~SigleInstantiation()
{
    ftruncate(fd, 0);
    char buff[256];
    const time_t current = time((time_t *) 0);
    const ssize_t length = snprintf(&buff[0], sizeof(buff), "0\n%lu\n%ld\n%s\n",
            (unsigned long) getuid(), (long int) current, ctime(&current));
    writen(fd, &buff[0], length);
    close(fd);
}

int DaemonProcTerm(const char * const pathname)
{
    char buff[512];
    const int fd = open(pathname, O_RDONLY);
    if (fd == -1)
    {
        snprintf(&buff[0], sizeof(buff), "pathname: %s, %s", pathname,
                strerror(errno));
        throw(std::string(&buff[0]));
    }
    ssize_t length = readn(fd, &buff[0], sizeof(buff));
    close(fd);
    if (length == -1)
    {
        snprintf(&buff[0], sizeof(buff), "pathname: %s, %s", pathname,
                strerror(errno));
        throw(std::string(&buff[0]));
    }
    char *endptr;
    errno = 0;
    const unsigned long pid = strtoul(&buff[0], &endptr, 10);
    if ((errno == ERANGE && pid == ULONG_MAX) || (errno != 0 && pid != 0))
    {
        throw(std::string("pid failed"));
    }
    if (pid != 0)
    {
        if ((kill((pid_t) pid, 0) == -1) && (errno == ESRCH))
        {
            exit(EXIT_SUCCESS);
        }
        kill((pid_t) pid, SIGTERM);
        std::cerr<<"已发送进程终止信号"<<std::endl;
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &timeout);
        if (kill((pid_t) pid, 0) == 0)
        {
            kill((pid_t) pid, SIGKILL);
        }
        std::cerr<<"进程已终止"<<std::endl;
    }
    exit(EXIT_SUCCESS);
}

