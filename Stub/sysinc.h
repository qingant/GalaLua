#ifndef __SYSINC_H_INCLUDE__
#define __SYSINC_H_INCLUDE__

#pragma once

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <dlfcn.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdarg.h>
#include <pthread.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/ipc.h>	/* general SysV IPC structures.        */
#include <sys/sem.h>	/* semaphore functions and structs.    */
#include <sys/shm.h>	/* shared memory functions and structs.*/
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/uio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#if defined (__linux__)
#include <execinfo.h>
#endif
#include <semaphore.h>
#include <netdb.h>
#include <sys/file.h>
#if defined (__linux__)
#include <sys/epoll.h>  // need to be preprocessed
#endif
#include <poll.h>
#include <sys/mman.h>

#if defined(_AIX)
#include <sys/pollset.h>
#include <sys/poll.h>
//#include <sys/fcntl.h>
#endif

#endif /*__SYSINC_H_INCLUDE__*/

