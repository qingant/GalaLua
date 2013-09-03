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
#include <cstdio>
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

/**
 * pidfile.pid文件的内容
 * 1. pid
 * 2. ppid
 * 3. uid
 * 4. create time(time_t)
 * 5. readable time
 */
static ssize_t ExtraInfo(const int fd)
{
    const time_t current = time((time_t *) 0);
    char buff[256];
    const int length = snprintf(&buff[0], sizeof(buff), "%lu\n%lu\n%ld\n%s\n",
            (unsigned long) getppid(), (unsigned long) getuid(),
            (long int) current, ctime(&current));
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
    const ssize_t length = snprintf(&buff[0], sizeof(buff), "0\n0\n%lu\n%ld\n%s\n",
            (unsigned long) getuid(), (long int) current, ctime(&current));
    writen(fd, &buff[0], length);
    close(fd);
}

static int pidcmdline(const pid_t pid, char * const buff, const size_t siz)
{
#if defined(linux) || defined(__linux) || defined(__linux__)
//    snprintf(buff, siz, "/proc/%lu/cmdline", (unsigned long) pid);
//    const int fd = open(&buff[0], O_RDONLY);
//    if (fd == -1)
//    {
//        return -1;
//    }
//    const ssize_t length = readn(fd, buff, siz);
//    if (length < 0)
//    {
//        return -1;
//    }
//    buff[length] = '\0';
//    close(fd);
//    std::cout<<"pid: "<<((unsigned long) pid)<<", cmdline: "<<buff<<std::endl;
//    return 0;
    snprintf(buff, siz, "ps -p %lu -o args", (unsigned long) pid);
    std::cerr<<"pid: "<<((unsigned long) pid)<<", cmdline: ";
    system(buff);
    std::cerr<<std::endl;
    return 0;
#elif defined(_AIX)
    snprintf(buff, siz, "ps -p %lu -o args", (unsigned long) pid);
    std::cerr<<"pid: "<<((unsigned long) pid)<<", cmdline: ";
    system(buff);
    std::cerr<<std::endl;
    return 0;
#endif
}

int DaemonProcTerm(const char * const pathname, const pid_t pidvar)
{
    char buff[512];
    unsigned long pid = pidvar;
    pid_t ppid = 0;
    if (pid == 0)
    {
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
        pid = strtoul(&buff[0], &endptr, 10);
        if ((errno == ERANGE && pid == ULONG_MAX) || (errno != 0 && pid != 0))
        {
            throw(std::string("pid failed"));
        }
        ppid = strtoul(endptr, (char **)NULL, 10);
        if ((errno == ERANGE && pid == ULONG_MAX) || (errno != 0 && pid != 0))
        {
            throw(std::string("pid failed"));
        }
        std::cerr<<"pid = "<<pid<<", ppid = "<<ppid<<std::endl;
    }
    pidcmdline(pid, &buff[0], sizeof(buff));
    std::cerr<<"are you sure to stop this process? (YES or NO)"<<std::endl;
    std::string tmp;
    std::cin>>tmp;
    if (strncmp(tmp.c_str(), "YES", 3) != 0)
    {
        std::cerr<<"pid "<<pid<<" won't be closed"<<std::endl;
        exit(EXIT_SUCCESS);
    }
    std::cout<<"pid "<<pid<<" is to be closed"<<std::endl;
    if ((kill((pid_t) pid, 0) == -1) && (errno == ESRCH))
    {
        std::cerr<<pathname<<": pid not exist"<<std::endl;
        exit(EXIT_SUCCESS);
    }
    kill(ppid, SIGTERM);
    kill((pid_t) pid, SIGTERM);
    std::cerr<<"glr_sl to be stopped"<<std::endl;
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &timeout);
    if (kill((pid_t) pid, 0) == 0)
    {
        kill(ppid, SIGKILL);
        kill((pid_t) pid, SIGKILL);
    }
    std::cerr<<"glr_sl has been stopped"<<std::endl;
    exit(EXIT_SUCCESS);
}

