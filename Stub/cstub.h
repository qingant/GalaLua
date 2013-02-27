
/*==========================================================================*
|                  			    C - RUNTIME.      			         		|
| UNIT: cstub.h                                                             |
| DESP: C���л����ٷ�װ                                                     |
| AUTH: JOHN.LEE															|
| DATE: 2011-04-20  														|
|---------------------------------------------------------------------------|
| INFO: Copyright (c) 2006, AGREE. 											|
|		All rights reserved.        										|
*==========================================================================*/
#ifndef __CSTUB_H_INCLUDE_6088DD28_D3BF_4E88_A29D_03591317FECD__
#define __CSTUB_H_INCLUDE_6088DD28_D3BF_4E88_A29D_03591317FECD__

#pragma once

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE  600      // for functions like strerror_r to get more correct and compatable interfaces
#endif


#include <ostype.h>
#include <cdtype.h>
#include <sysinc.h>



#ifdef __cplusplus
extern "C" {
#endif

/*��*/
void *CRT_memset(void *s, int c, size_t n);
void *CRT_memcpy(void *dest, const void *src, size_t n);
char *CRT_strcpy(char *dest, const char *src);
char *CRT_strncpy(char *dest, const char *src, size_t n);
void *CRT_memchr(const void *s, int c, size_t n);
char *CRT_strchr(const char *s, int c);
char *CRT_strrchr(char *str, char c);
char *CRT_strstr(const char *haystack, const char *needle);
int CRT_sprintf(char *str, const char *format, ...);
int CRT_snprintf(char *str, size_t size, const char *format, ...);
int CRT_vsnprintf(char *str, size_t size, const char *format, va_list ap);
size_t CRT_strlen(const char *s);
int CRT_strcmp(const char *s1, const char *s2);
int CRT_strncmp(const char *s1, const char *s2, size_t n);
int CRT_memcmp(const void *s1, const void *s2, size_t n);

/*IO*/
int CRT_open(const char *pathname, int flags, mode_t mode);
int CRT_creat(const char *pathname, mode_t mode);
int CRT_close(int fd);
ssize_t CRT_read(int fd, void *buf, size_t count);
ssize_t CRT_write(int fd, const void *buf, size_t count);
int CRT_fstat(int filedes, struct stat *buf);
int CRT_access(const char *pathname, int mode);
ssize_t CRT_readv(int fd, const struct iovec *vector, int count);
ssize_t CRT_writev(int fd, const struct iovec *vector, int count);
off_t CRT_lseek(int fildes, off_t offset, int whence);

/*stream IO*/
FILE *CRT_fopen(const char *path, const char *mode);
FILE *CRT_fdopen(int fildes, const char *mode);
int CRT_fclose(FILE *fp);
int CRT_fflush(FILE *stream);
int CRT_fprintf(FILE *stream, const char *format, ...);
size_t CRT_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t CRT_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int CRT_feof(FILE *stream);
int CRT_fileno(FILE *stream);
char *CRT_fgets(char *s, int size, FILE *stream);
int CRT_fputs(const char *s, FILE *stream);


int CRT_fcntl(int fd, int cmd, ...);





/*����ȱ��*/
int CRT_ferror(FILE *stream);
char *CRT_strcat(char *dest, const char *src);
char *CRT_strncat(char *dest, const char *src, size_t n);


/*exception*/
#if defined(__linux__)
int CRT_backtrace(void **buffer, int size);
char **CRT_backtrace_symbols(void *const *buffer, int size);
#endif
key_t CRT_ftok(const char *pathname, int proj_id);
void *CRT_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int CRT_munmap(void *addr, size_t length);
int CRT_setvbuf(FILE *stream, char *buf, int mode , size_t size);
int CRT_shm_open(const char *name, int oflag, mode_t mode);
int CRT_shm_unlink(const char *name);
int CRT_unlink(const char *pathname);
int CRT_strerror_r(int errnum, char *buf, size_t buflen);
char *CRT_strerror(int errnum);

sem_t *CRT_sem_open(const char *name, int oflag,mode_t mode, unsigned int value);
int CRT_sem_getvalue(sem_t *sem, int *sval);
int CRT_sem_wait(sem_t *sem);
int CRT_sem_trywait(sem_t *sem);
int CRT_sem_post(sem_t *sem);
int CRT_sem_unlink(const char *name);
int CRT_sem_close(sem_t *sem);

/*afa3.0*/
int CRT_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int CRT_dup(int oldfd);
int CRT_dup2(int oldfd, int newfd);

int CRT_shmget(key_t key, size_t size,int shmflg);
void* CRT_shmat(int shmid, char *shmaddr, int shmflag);
int CRT_shmctl(int shmid, int cmd, struct shmid_ds *buf);
int CRT_shmdt(const void *shmaddr);
int CRT_msgget(key_t key,int msgflg);
int CRT_msgctl(int msgid,int cmd,struct msqid_ds *buf);

int CRT_getpid();
int CRT_atoi(const char *nptr);
double CRT_atof ( const char * str );
char *CRT_getenv(const char *envvar);



void CRT_free(void *ptr);
void *CRT_malloc(size_t size);

typedef void  (*sighandler_t)(int);

sighandler_t CRT_signal(int signum, sighandler_t handler);
int CRT_atexit(void (*function)(void));
void CRT_exit(int status);


int CRT_scanf(const char *format, ...);
int CRT_fscanf(FILE *stream, const char *format, ...);
int CRT_sscanf(const char *str, const char *format, ...);

int CRT_getopt(int argc, char * const argv[], const char *optstring);

DIR *CRT_opendir(const char *name);
struct dirent *CRT_readdir(DIR *dir);
int CRT_closedir(DIR *dir);
int CRT_mkdir(const char *pathname, mode_t mode);
int CRT_mkdir_ex(const char *pathname, mode_t mode);


int CRT_usleep(useconds_t usec);
unsigned int CRT_sleep(unsigned int seconds);
int CRT_ftruncate(int fd, off_t length);
int CRT_fchmod(int fildes, mode_t mode);

int CRT_kill(pid_t pid, int sig);
int CRT_killpg(int pgrp, int sig);
int CRT_system(const char *command);

int CRT_remove(const char *pathname);
int CRT_rename(const char *oldpath, const char *newpath);


int CRT_toupper(int c);
int CRT_tolower(int c);

int CRT_setpgid(pid_t pid, pid_t pgid);

pid_t CRT_fork(void);

pid_t CRT_wait(int *status);
pid_t CRT_waitpid(pid_t pid, int *status, int options);

void *CRT_dlopen(const char *filename, int flag);
const char *CRT_dlerror();
void *CRT_dlsym(void *handle, const char *symbol);
int CRT_dlclose(void *handle);

void CRT_rewind(FILE *stream);

time_t CRT_time(time_t *t);
struct tm *CRT_localtime(const time_t *timep);
struct tm *CRT_localtime_r(const time_t *timep, struct tm *result);
int CRT_ftime(struct timeb *tp);

int CRT_strcasecmp(const char *s1, const char *s2);
int CRT_strncasecmp(const char *s1, const char *s2, size_t n);

int CRT_lstat(const char *path, struct stat *buf);

FILE *CRT_popen(const char *command, const char *type);
int CRT_pclose(FILE *stream);

int CRT_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
ssize_t CRT_msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);

void CRT_bzero(void *s, size_t n);


int CRT_semget(key_t key, int nsems, int flag);

// socket api
int CRT_listen(int socket, int backlog);
int CRT_bind(int socket, const struct sockaddr *address,socklen_t address_len);
int CRT_connect(int socket, const struct sockaddr *address, socklen_t address_len);
int CRT_accept(int socket, struct sockaddr * address, socklen_t * address_len);
int CRT_shutdown(int socket, int how);
int CRT_setsockopt(int socket, int level, int option_name,
                   const void *option_value, socklen_t option_len);
int CRT_socketpair(int domain, int type, int protocol, int socket_vector[2]);
ssize_t CRT_sendmsg(int socket, const struct msghdr *message, int flags);
ssize_t CRT_recvmsg(int socket, struct msghdr *message, int flags);
int CRT_socket(int domain, int type, int protocol);
int CRT_getpeername(int socket, struct sockaddr * address, socklen_t * address_len);
ssize_t CRT_recv(int socket, void *buffer, size_t length, int flags);
ssize_t CRT_send(int socket, const void *buffer, size_t length, int flags);
uint16_t CRT_htons(uint16_t hostshort);
in_addr_t CRT_inet_addr(const char *cp);
struct hostent *CRT_gethostbyname(const char *name);
int CRT_getsockname(int s, struct sockaddr *name, socklen_t *namelen);

const char *CRT_inet_ntop(int af, const void *src,
                      char *dst, socklen_t cnt);
#if (defined(_AIX)&&(!defined(_ALL_SOURCE)))
const char *inet_ntop (int af,const void *src,char *dst,size_t size);
#endif
uint16_t CRT_htons(uint16_t hostshort);
uint16_t CRT_ntohs(uint16_t netshort);

#if defined(__linux__)
int CRT_epoll_wait(int epfd, struct epoll_event * events,
                   int maxevents, int timeout);
int CRT_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int CRT_epoll_create(int size);
#elif defined(_AIX)
pollset_t CRT_pollset_create(int maxfd);
int CRT_pollset_destroy(pollset_t ps);
int CRT_pollset_ctl(pollset_t ps, struct poll_ctl *pollctl_array,int array_length);
int CRT_pollset_query(pollset_t ps, struct pollfd *pollfd_query);
int CRT_pollset_poll(pollset_t ps,struct pollfd *polldata_array,int array_length, int timeout);

#endif
#if (!defined(_AIX))&&(!defined(__FreeBSD__))
int CRT_flock(int fd, int operation);
int CRT_fdatasync(int fd);
#else
#define CRT_fdatasync CRT_fsync
#endif
int CRT_fsync(int fd);
int CRT_pipe(int filedes[2]);
int CRT_poll(struct pollfd *fds, nfds_t nfds, int timeout);
int CRT_msync(void *start, size_t length, int flags);





mode_t CRT_umask(mode_t mask);
int CRT_getrlimit(int resource, struct rlimit *rlim);
int CRT_setrlimit(int resource, const struct rlimit *rlim);
pid_t CRT_getppid(void);
int CRT_mlock(const void *addr, size_t len);
int CRT_munlock(const void *addr, size_t len);



// pthread

int CRT_pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
int CRT_pthread_mutex_destroy(pthread_mutex_t *mutex);
int CRT_pthread_mutex_lock(pthread_mutex_t *mutex);
int CRT_pthread_mutex_unlock(pthread_mutex_t *mutex);
int CRT_pthread_mutex_trylock(pthread_mutex_t *mutex);
int CRT_pthread_mutex_timedlock(pthread_mutex_t *mutex,const struct timespec *abs_timeout);

void CRT_pthread_testcancel();
int CRT_pthread_setcancelstate(int state, int *oldstate);
int CRT_pthread_attr_init(pthread_attr_t *attr);
int CRT_pthread_attr_destroy(pthread_attr_t *attr);
int CRT_pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int CRT_pthread_mutexattr_setpshared(pthread_mutexattr_t *attr,int pshared);

int CRT_pthread_attr_getstacksize(const pthread_attr_t * attr, size_t * stacksize);

int CRT_pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int CRT_pthread_mutexattr_init(pthread_mutexattr_t *attr);
int CRT_pthread_mutexattr_gettype(const  pthread_mutexattr_t  *attr, int *type);
int CRT_pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);




int CRT_pthread_condattr_init(pthread_condattr_t *attr);
int CRT_pthread_condattr_destroy(pthread_condattr_t *attr);
int CRT_pthread_condattr_setpshared(pthread_condattr_t *attr,int pshared);


int CRT_pthread_cond_init(pthread_cond_t *cond,const pthread_condattr_t *cond_attr);
int CRT_pthread_cond_destroy(pthread_cond_t *cond);
int CRT_pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int CRT_pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
int CRT_pthread_cond_signal(pthread_cond_t *cond);
int CRT_pthread_cond_broadcast(pthread_cond_t *cond);

int CRT_pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int CRT_pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
int CRT_pthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void*), void * arg);
int CRT_pthread_cancel(pthread_t thread);
int CRT_pthread_join(pthread_t thread, void **value_ptr);
int CRT_pthread_detach(pthread_t thread);
pthread_t CRT_pthread_self();

#ifdef __cplusplus
}
#endif

#endif /*__CRTL1_H_INCLUDE_6088DD28_D3BF_4E88_A29D_03591317FECD__*/
