/*==========================================================================*
|                  			    C - RUNTIME.      			         		|
| UNIT: cstub.c                                                             |
| DESP: C���л����ٷ�װ                                                     |
| AUTH: JOHN.LEE															|
| DATE: 2011-04-20  														|
|---------------------------------------------------------------------------|
| INFO: Copyright (c) 2006, AGREE. 											|
|		All rights reserved.        										|
*==========================================================================*/
#include "cstub.h"
#include "sysinc.h"


#define	ENULLARG		1024	/* Function Args is null */

void *CRT_memset(void *s, int c, size_t n)
{
    if(ISNULL(s))
    {
        errno = ENULLARG;
        return (NULL);
    }

    if(n > 0)
    {
        return memset(s,c,n);
    }
    else
    {
        return (s);
    }
}

void *CRT_memcpy(void *dest, const void *src, size_t n)
{
    if(ISNULL(dest)||ISNULL(src))
    {
        errno = ENULLARG;
        return (NULL);
    }

    if(n > 0)
    {
        return memcpy(dest,src,n);
    }
    else
    {
        return (dest);
    }
}


char *CRT_strcpy(char *dest, const char *src)
{
    if(ISNULL(dest)||ISNULL(src))
    {
        errno = ENULLARG;
        return (NULL);
    }

    return strcpy(dest,src);
}

char *CRT_strncpy(char *dest, const char *src, size_t n)
{
    if(ISNULL(dest)||ISNULL(src))
    {
        errno = ENULLARG;
        return (NULL);
    }

    if(n > 0)
    {
        return strncpy(dest,src,n);
    }
    else
    {
        return (dest);
    }
}

void *CRT_memchr(const void *s, int c, size_t n)
{
    if(ISNULL(s))
    {
        errno = ENULLARG;
        return (NULL);
    }

    return memchr(s,c,n);
}

char *CRT_strchr(const char *s, int c)
{
    if(ISNULL(s))
    {
        errno = ENULLARG;
        return (NULL);
    }

    return strchr(s,c);
}

char *CRT_strrchr(char *s, char c)
{
    if(ISNULL(s))
    {
        errno = ENULLARG;
        return (NULL);
    }

    return strrchr(s,c);
}

char *CRT_strstr(const char *haystack, const char *needle)
{
    if(ISNULL(haystack)||ISNULL(needle))
    {
        errno = ENULLARG;
        return (NULL);
    }

    return strstr(haystack,needle);
}

int CRT_sprintf(char *str, const char *format, ...)
{
    int                 rc;
    va_list 			valp;
    if(ISNULL(str)||ISNULL(format))
    {
        errno = ENULLARG;
        return (0);
    }

	va_start( valp, format );
	rc = vsprintf( str, format, valp );
	va_end( valp );

    return (rc);
}

int CRT_snprintf(char *str, size_t size, const char *format, ...)
{
    int                 rc;
    va_list 			valp;
    if(ISNULL(str)||ISNULL(format))
    {
        errno = ENULLARG;
        return (0);
    }
    else if(size == 0)
    {
        return (0);
    }

	va_start( valp, format );
	rc = vsnprintf( str, size ,format, valp );
	va_end( valp );

    return (rc);
}

int CRT_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    int                 rc;

    if(ISNULL(str)||ISNULL(format))
    {
        errno = ENULLARG;
        return (0);
    }
    else if(size == 0)
    {
        return (0);
    }
    rc = vsnprintf( str, size ,format, ap);

    return rc;
}

size_t CRT_strlen(const char *s)
{
    if(ISNULL(s))
    {
        errno = ENULLARG;
        return (0);
    }

    return strlen(s);
}

int CRT_strcmp(const char *s1, const char *s2)
{
    if(ISNULL(s1) && ISNULL(s2))
    {
        return (0);
    }
    else if(ISNULL(s1) || ISNULL(s2))
    {
        errno = ENULLARG;
        return (-1);
    }

    return strcmp(s1,s2);
}
int CRT_strncmp(const char *s1, const char *s2, size_t n)
{
    if(ISNULL(s1) && ISNULL(s2))
    {
        return (0);
    }
    else if(ISNULL(s1) || ISNULL(s2))
    {
        errno = ENULLARG;
        return (-1);
    }

    return strncmp(s1,s2,n);
}

int CRT_memcmp(const void *s1, const void *s2, size_t n)
{
    if(ISNULL(s1) && ISNULL(s2))
    {
        return (0);
    }
    else if(ISNULL(s1) || ISNULL(s2))
    {
        errno = ENULLARG;
        return (-1);
    }

    return memcmp(s1,s2,n);
}

int CRT_open(const char *pathname, int flags, mode_t mode)
{
    if(ISNULL(pathname))
    {
        errno = ENULLARG;
        return (-1);
    }

    return open(pathname,flags,mode);
}

int CRT_creat(const char *pathname, mode_t mode)
{
    if(ISNULL(pathname))
    {
        errno = ENULLARG;
        return (-1);
    }

    return creat(pathname,mode);
}

int CRT_close(int fd)
{
    if(fd == -1)
    {
        errno = EBADF;
        return (-1);
    }

    return close(fd);
}

ssize_t CRT_read(int fd, void *buf, size_t count)
{
    if(ISNULL(buf))
    {
        errno = ENULLARG;
        return (-1);
    }
    else if(fd == -1)
    {
        errno = EBADF;
        return (0);
    }

    return read(fd,buf,count);
}

ssize_t CRT_write(int fd, const void *buf, size_t count)
{
    if(ISNULL(buf))
    {
        errno = ENULLARG;
        return (-1);
    }
    else if(fd == -1)
    {
        errno = EBADF;
        return (0);
    }

    return write(fd,buf,count);
}

int CRT_fstat(int filedes, struct stat *buf)
{
    if(ISNULL(buf))
    {
        errno = ENULLARG;
        return (-1);
    }
    else if(filedes == -1)
    {
        errno = EBADF;
        return (-1);
    }

    return fstat(filedes,buf);
}

int CRT_access(const char *pathname, int mode)
{
    if(ISNULL(pathname))
    {
        errno = ENULLARG;
        return (-1);
    }

    return access(pathname,mode);
}

ssize_t CRT_readv(int fd, const struct iovec *vector, int count)
{
    if(ISNULL(vector))
    {
        errno = ENULLARG;
        return (-1);
    }
    else if(fd == -1)
    {
        errno = EBADF;
        return (0);
    }

    return readv(fd,vector,count);
}

ssize_t CRT_writev(int fd, const struct iovec *vector, int count)
{
    if(ISNULL(vector))
    {
        errno = ENULLARG;
        return (-1);
    }
    else if(fd == -1)
    {
        errno = EBADF;
        return (0);
    }


    return writev(fd,vector,count);
}

off_t CRT_lseek(int fildes, off_t offset, int whence)
{
    if(fildes == -1)
    {
        errno = EBADF;
        return (off_t)-1;
    }

    return lseek(fildes,offset,whence);
}


FILE *CRT_fopen(const char *path, const char *mode)
{
    if(ISNULL(path)||ISNULL(mode))
    {
        errno = ENULLARG;
        return (NULL);
    }

    return fopen(path,mode);
}

FILE *CRT_fdopen(int fildes, const char *mode)
{
    if(ISNULL(mode))
    {
        errno = ENULLARG;
        return (NULL);
    }
    else if(fildes == -1)
    {
        errno = EBADF;
        return (NULL);
    }

    return fdopen(fildes,mode);
}

int CRT_fclose(FILE *fp)
{
    if(ISNULL(fp))
    {
        errno = ENULLARG;
        return (-1);
    }

    return fclose(fp);
}

int CRT_fflush(FILE *stream)
{
    if(ISNULL(stream))
    {
        errno = ENULLARG;
        return (-1);
    }

    return fflush(stream);
}

int CRT_fprintf(FILE *stream, const char *format, ...)
{

    int                 rc;
    va_list 			valp;

    if(ISNULL(stream)||ISNULL(format))
    {
        errno = ENULLARG;
        return (-1);
    }

	va_start( valp, format );
	rc = vfprintf( stream, format, valp );
	va_end( valp );

    return (rc);
}

size_t CRT_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    if(ISNULL(ptr)||ISNULL(stream))
    {
        errno = ENULLARG;
        return (0);
    }

    return fread(ptr,size,nmemb,stream);
}

size_t CRT_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    if(ISNULL(ptr)||ISNULL(stream))
    {
        errno = ENULLARG;
        return (0);
    }

    return fwrite(ptr,size,nmemb,stream);
}

int CRT_feof(FILE *stream)
{
    if(ISNULL(stream))
    {
        errno = ENULLARG;
        return (-1);
    }

    return feof(stream);
}

int CRT_fileno(FILE *stream)
{
    if(ISNULL(stream))
    {
        errno = ENULLARG;
        return (-1);
    }

    return fileno(stream);
}

char *CRT_fgets(char *s, int size, FILE *stream)
{
    if(ISNULL(stream)||ISNULL(s))
    {
        errno = ENULLARG;
        return (NULL);
    }

    return fgets(s,size,stream);
}

int CRT_fputs(const char *s, FILE *stream)
{
    if(ISNULL(stream)||ISNULL(s))
    {
        errno = ENULLARG;
        return (-1);
    }

    return fputs(s,stream);
}





int CRT_ferror(FILE *stream)
{
    if(ISNULL(stream))
    {
        errno = ENULLARG;
        return (-1);
    }
    return ferror(stream);
}




/*afa3.0*/

int CRT_shmget(key_t key,size_t size,int shmflg)
{
    return shmget(key,size,shmflg);
}

void* CRT_shmat(int shmid, char *shmaddr, int shmflag)
{
    return shmat(shmid,shmaddr,shmflag);
}

int CRT_shmdt(const void *shmaddr)
{
    if(ISNULL(shmaddr))
    {
        errno = ENULLARG;
        return (-1);
    }
    return shmdt(shmaddr);
}
int CRT_shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
    return shmctl(shmid, cmd, buf);
}


int CRT_msgget(key_t key,int msgflg)
{
    return msgget(key,msgflg);
}

int CRT_msgctl(int msqid,int cmd,struct msqid_ds *buf)
{
    return msgctl(msqid,cmd,buf);
}

int CRT_getpid()
{
    return getpid();
}

int CRT_atoi(const char *nptr)
{
    if(ISNULL(nptr))
    {
        errno = ENULLARG;
        return (-1);
    }
    return atoi(nptr);
}
double CRT_atof ( const char * str )
{
    if(ISNULL(str))
    {
        errno = ENULLARG;
        return (-1);
    }
    return atof(str);
}

char *CRT_getenv(const char *envvar)
{
    if(ISNULL(envvar))
    {
        errno = ENULLARG;
        return NULL;
    }
    return getenv(envvar);
}



int CRT_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    return select(nfds,readfds,writefds,exceptfds,timeout);
}

int CRT_dup(int oldfd)
{
    return dup(oldfd);
}

int CRT_dup2(int oldfd, int newfd)
{
    return dup2(oldfd, newfd);
}


void CRT_free(void *ptr)
{
    if(ISNULL(ptr))
    {
        errno = ENULLARG;
    }
    else
    {
        free(ptr);
    }

}

void *CRT_malloc(size_t size)
{
    return malloc(size);
}


char *CRT_strcat(char *dest, const char *src)
{
    if(ISNULL(dest)||ISNULL(src))
    {
        errno = ENULLARG;
        return NULL;
    }
    return strcat(dest,src);
}

char *CRT_strncat(char *dest, const char *src, size_t n)
{
    if(ISNULL(dest)||ISNULL(src))
    {
        errno = ENULLARG;
        return NULL;
    }
    return strncat(dest,src,n);
}

sighandler_t CRT_signal(int signum, sighandler_t handler)
{
    return signal(signum,handler);
}

int CRT_atexit(void (*function)(void))
{
    if(ISNULL(function))
    {
        errno = ENULLARG;
        return (-1);
    }
    return atexit(function);
}

void CRT_exit(int status)
{
    exit(status);
}


int CRT_scanf(const char *format, ...)
{
    int rc;
    va_list valp;
    if(ISNULL(format))
    {
        errno = ENULLARG;
        return (0);
    }
    va_start(valp, format);
    rc = vscanf(format, valp);
    va_end(valp);
    return (rc);
}

int CRT_fscanf(FILE *stream, const char *format, ...)
{
    int rc;
    va_list valp;
    if(ISNULL(stream)||ISNULL(format))
    {
        errno = ENULLARG;
        return (-1);
    }
    va_start(valp,format);
    rc = vfscanf(stream,format,valp);
    va_end(valp);
    return (rc);
}

int CRT_sscanf(const char *str, const char *format, ...)
{
    int rc;
    va_list valp;
    if(ISNULL(str)||ISNULL(format))
    {
        errno = ENULLARG;
        return (0);
    }
    va_start(valp,format);
    rc = vsscanf(str,format,valp);
    va_end(valp);
    return (rc);
}

int CRT_getopt(int argc, char * const argv[], const char *optstring)
{
    return getopt(argc,argv,optstring);
}

DIR *CRT_opendir(const char *name)
{
    if(ISNULL(name))
    {
        errno = ENULLARG;
        return NULL;
    }
    return opendir(name);
}

struct dirent *CRT_readdir(DIR *dir)
{
    if(ISNULL(dir))
    {
        errno = ENULLARG;
        return NULL;
    }
    return readdir(dir);
}

int CRT_closedir(DIR *dir)
{
    if(ISNULL(dir))
    {
        errno = ENULLARG;
        return (-1);
    }
    return closedir(dir);
}

int CRT_mkdir(const char *pathname, mode_t mode)
{
    if(ISNULL(pathname))
    {
        errno = ENULLARG;
        return (-1);
    }
    return mkdir(pathname,mode);
}

int CRT_mkdir_ex(const char *pathname, mode_t mode)
{
    if(ISNULL(pathname))
    {
        errno = ENULLARG;
        return (-1);
    }
    char szTemp[1024];
    CRT_memset(szTemp,0x00,1024);
    CRT_strcpy(szTemp,pathname);
    size_t length = 0;
    length = CRT_strlen(szTemp);
    char *p = (char *)&szTemp[length];
    int rc = 0 ;
    while(length > 0)
    {
        if(*p == '/')
        {
            *p = '0';
            rc = mkdir(szTemp,mode);
            if(rc == -1)
            {
                if(errno == EEXIST )
                    rc = 0;
                break;
            }
        }
        p--;
        length--;
    }
    return rc;
}

int CRT_usleep(useconds_t usec)
{
    return usleep(usec);
}

unsigned int CRT_sleep(unsigned int seconds)
{
    return sleep(seconds);
}

int CRT_ftruncate(int fd, off_t length)
{
    return ftruncate(fd,length);
}

int CRT_fchmod(int fildes, mode_t mode)
{
    return fchmod(fildes,mode);
}

int CRT_kill(pid_t pid, int sig)
{
    return kill(pid,sig);
}

int CRT_killpg(int pgrp, int sig)
{
    return killpg(pgrp,sig);
}

int CRT_system(const char *command)
{
    return system(command);
}

int CRT_fcntl(int fd, int cmd, ...)
{
    va_list valp;
    int rc;
    va_start(valp,cmd);
    rc = fcntl(fd, cmd, valp);
    va_end(valp);
    return (rc);
}

int CRT_remove(const char *pathname)
{
    if(ISNULL(pathname))
    {
        errno = ENULLARG;
        return (-1);
    }
    return remove(pathname);
}
int CRT_rename(const char *oldpath, const char *newpath)
{
    if(ISNULL(oldpath)||ISNULL(newpath))
    {
        errno = ENULLARG;
        return (-1);
    }
    return rename(oldpath,newpath);
}

int CRT_toupper(int c)
{
    return toupper(c);
}

int CRT_tolower(int c)
{
    return tolower(c);
}

int CRT_setpgid(pid_t pid, pid_t pgid)
{
    return setpgid(pid,pgid);
}

pid_t CRT_fork(void)
{
    return fork();
}

pid_t CRT_wait(int *status)
{
    return wait(status);
}

pid_t CRT_waitpid(pid_t pid, int *status, int options)
{
    return waitpid(pid, status, options);
}

void *CRT_dlopen(const char *filename, int flag)
{
    return dlopen(filename,flag);
}
const char *CRT_dlerror()
{
    return dlerror();
}

void *CRT_dlsym(void *handle, const char *symbol)
{
    return dlsym(handle,symbol);
}

int CRT_dlclose(void *handle)
{
    return dlclose(handle);
}

void CRT_rewind(FILE *stream)
{
    if(ISNULL(stream))
    {
        errno = ENULLARG;
    }
    else
    {
        rewind(stream);
    }
}

time_t CRT_time(time_t *t)
{
    return time(t);
}

struct tm *CRT_localtime(const time_t *timep)
{
    if(ISNULL(timep))
    {
        errno = ENULLARG;
        return NULL;
    }
    return localtime(timep);
}

struct tm *CRT_localtime_r(const time_t *timep, struct tm *result)
{
    if(ISNULL(timep))
    {
        errno = ENULLARG;
        return NULL;
    }

    return localtime_r(timep, result);
}

int CRT_ftime(struct timeb *tp)
{
    if(ISNULL(tp))
    {
        errno = ENULLARG;
        return -1;
    }
    return ftime(tp);
}

int CRT_strcasecmp(const char *s1, const char *s2)
{
    return strcasecmp(s1,s2);
}

int CRT_strncasecmp(const char *s1, const char *s2, size_t n)
{
    return strncasecmp(s1,s2,n);
}

int CRT_lstat(const char *path, struct stat *buf)
{
    return lstat(path,buf);
}

FILE *CRT_popen(const char *command, const char *type)
{
     if(ISNULL(command))
    {
        errno = ENULLARG;
        return NULL;
    }
    return popen(command,type);
}


int CRT_pclose(FILE *stream)
{
    if(ISNULL(stream))
    {
        errno = ENULLARG;
        return -1;
    }
    return pclose(stream);
}



int CRT_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
{
    return msgsnd(msqid,msgp,msgsz,msgflg);
}

ssize_t CRT_msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)
{
    return msgrcv(msqid,msgp,msgsz,msgtyp,msgflg);
}

void CRT_bzero(void *s, size_t n)
{
    if(ISNULL(s))
    {
        errno = ENULLARG;
    }
    else
    {
        bzero(s,n);
    }
}

int CRT_semget(key_t key, int nsems, int flag)
{
    return semget(key,nsems,flag);
}



int CRT_listen(int socket, int backlog)
{
    return listen(socket, backlog);
}

int CRT_bind(int socket, const struct sockaddr *address,socklen_t address_len)
{
    return bind(socket, address,address_len);

}

int CRT_connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
    return   connect( socket, address, address_len);
}

int CRT_accept(int socket, struct sockaddr * address, socklen_t * address_len)
{
    return accept( socket,  address, address_len);
}


int CRT_shutdown(int socket, int how)
{
    return  shutdown( socket, how);
}

int CRT_setsockopt(int socket, int level, int option_name,
                   const void *option_value, socklen_t option_len)
{
    return   setsockopt( socket,  level,  option_name, option_value,  option_len);

}

int CRT_socketpair(int domain, int type, int protocol, int socket_vector[2])
{
    return socketpair( domain,  type,  protocol, socket_vector);
}

ssize_t CRT_sendmsg(int socket, const struct msghdr *message, int flags)
{
    return   sendmsg( socket,  message,  flags);

}
ssize_t CRT_recvmsg(int socket, struct msghdr *message, int flags)
{
    return recvmsg( socket, message,  flags);
}

uint16_t CRT_htons(uint16_t hostshort)
{
    return htons(hostshort);
}
in_addr_t CRT_inet_addr(const char *cp)
{
    return inet_addr(cp);
}


int CRT_socket(int domain, int type, int protocol)
{
    return    socket( domain,  type,  protocol);
}

int CRT_getpeername(int socket, struct sockaddr * address, socklen_t * address_len)
{
    return getpeername( socket,  address,address_len);
}

ssize_t CRT_recv(int socket, void *buffer, size_t length, int flags)
{
    return   recv( socket,  buffer,  length,  flags);
}

ssize_t CRT_send(int socket, const void *buffer, size_t length, int flags)
{
    return send(socket, buffer, length, flags);
}

#if defined(__linux__)
int CRT_backtrace(void **buffer, int size)
{
	return backtrace(buffer, size);
}

char **CRT_backtrace_symbols(void *const *buffer, int size)
{
	return backtrace_symbols(buffer, size);
}
#endif

key_t CRT_ftok(const char *pathname, int proj_id)
{
	if(ISNULL(pathname))
	{
		errno = ENULLARG;
		return -1;
	}
	return ftok(pathname,proj_id);
}

void *CRT_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	return mmap(addr, length, prot, flags, fd, offset);
}

int CRT_munmap(void *addr, size_t length)
{
	if(ISNULL(addr))
	{
		errno = ENULLARG;
		return -1;
	}
	return munmap(addr, length);
}

int CRT_setvbuf(FILE *stream, char *buf, int mode, size_t size)
{
	if (ISNULL(stream))
	{
		errno = ENULLARG;
		return (-1);
	}
	return setvbuf(stream, buf, mode, size);
}

int CRT_shm_open(const char *name, int oflag, mode_t mode)
{
	if (ISNULL(name))
	{
		errno = ENULLARG;
		return (-1);
	}
	return shm_open(name, oflag, mode);
}

int CRT_shm_unlink(const char *name)
{
	if (ISNULL(name))
	{
		errno = ENULLARG;
		return (-1);
	}
	return shm_unlink(name);
}

int CRT_unlink(const char *pathname)
{
	if (ISNULL(pathname))
	{
		errno = ENULLARG;
		return (-1);
	}
	return unlink(pathname);
}

sem_t *CRT_sem_open(const char *name, int oflag,
                       mode_t mode, unsigned int value)
{
	if (ISNULL(name))
	{
		errno = ENULLARG;
		return (SEM_FAILED);
	}
	return sem_open(name,oflag,mode,value);
}

int CRT_sem_getvalue(sem_t *sem, int *sval)
{
	return sem_getvalue(sem, sval);
}

int CRT_sem_wait(sem_t *sem)
{
	return sem_wait(sem);
}

int CRT_sem_trywait(sem_t *sem)
{
	return sem_trywait(sem);
}

int CRT_sem_post(sem_t *sem)
{
	return sem_post(sem);
}

int CRT_sem_unlink(const char *name)
{
	if (ISNULL(name))
	{
		errno = ENULLARG;
		return (-1);
	}
	return sem_unlink(name);
}

int CRT_sem_close(sem_t *sem)
{
	return sem_close(sem);
}

int CRT_strerror_r(int errnum, char *buf, size_t buflen)
{
	if (ISNULL(buf))
	{
		errno = ENULLARG;
		return -1;
	}
	return strerror_r(errnum, buf, buflen);
}

char *CRT_strerror(int errnum)
{
	return strerror(errnum);
}


struct hostent *CRT_gethostbyname(const char *name)
{
    if (ISNULL(name))
    {
        errno = ENULLARG;
		return NULL;
    }
    return gethostbyname(name);
}
int CRT_getsockname(int s, struct sockaddr *name, socklen_t *namelen)
{
    if (ISNULL(name))
    {
        errno = ENULLARG;
		return -1;
    }
    return getsockname(s, name, namelen);
}

const char *CRT_inet_ntop(int af, const void *src,
                      char *dst, socklen_t cnt)
{
    if (ISNULL(src) || ISNULL(dst))
    {
        errno = ENULLARG;
		return NULL;
    }
    return inet_ntop(af, src, dst, cnt);
}

uint16_t CRT_ntohs(uint16_t netshort)
{
    return ntohs(netshort);
}

#ifdef __linux__
int CRT_epoll_wait(int epfd, struct epoll_event * events,
                   int maxevents, int timeout)
{
    if (ISNULL(events))
    {
        errno = ENULLARG;
        return -1;
    }
    return epoll_wait(epfd, events, maxevents, timeout);
}
int CRT_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
    if (ISNULL(event))
    {
        errno = ENULLARG;
        return -1;
    }
    return epoll_ctl(epfd, op, fd, event);
}

int CRT_epoll_create(int size)
{
    if (size < 0)
    {
        errno = ENULLARG;
        return -1;
    }
    return epoll_create(size);
}
#endif

#ifdef _AIX
pollset_t CRT_pollset_create(int maxfd)
{
    return 	pollset_create(maxfd);
}
int CRT_pollset_destroy(pollset_t ps)
{
	  return pollset_destroy(ps);
}
int CRT_pollset_ctl(pollset_t ps, struct poll_ctl *pollctl_array,int array_length)
{
    if(ISNULL(pollctl_array))
	  {
	  	  errno = ENULLARG;
        return -1;
	  }
	  return pollset_ctl( ps, pollctl_array, array_length);
}
int CRT_pollset_query(pollset_t ps, struct pollfd *pollfd_query)
{
	  if(ISNULL(pollfd_query))
	  {
	  	  errno = ENULLARG;
        return -1;
	  }
	  return pollset_query(ps,pollfd_query);
}
int CRT_pollset_poll(pollset_t ps,struct pollfd *polldata_array,int array_length, int timeout)
{
    if(ISNULL(polldata_array))
	  {
	  	  errno = ENULLARG;
        return -1;
	  }
	  return pollset_poll(ps,polldata_array,array_length,timeout);
}

#endif
#if (!defined(_AIX))&&(!defined(__FreeBSD__))
int CRT_flock(int fd, int operation)
{
    return flock(fd, operation);
}
int CRT_fdatasync(int fd)
{
    return fdatasync(fd);
}

#endif
int CRT_fsync(int fd)
{
    return fsync(fd);
}

int CRT_pipe(int filedes[2])
{
    return pipe(filedes);
}

int CRT_poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    if (ISNULL(fds))
    {
        errno = ENULLARG;
        return -1;
    }
    return poll(fds, nfds, timeout);
}

int CRT_msync(void *start, size_t length, int flags)
{
    if (ISNULL(start))
    {
        errno = ENULLARG;
        return -1;
    }
    return msync(start, length, flags);
}




mode_t CRT_umask(mode_t mask)
{
    return umask(mask);
}
int CRT_getrlimit(int resource, struct rlimit *rlim)
{
    if (ISNULL(rlim))
    {
        errno = ENULLARG;
        return -1;
    }
    return getrlimit(resource, rlim);
}
int CRT_setrlimit(int resource, const struct rlimit *rlim)
{
    if (ISNULL(rlim))
    {
        errno = ENULLARG;
        return -1;
    }
    return setrlimit(resource, rlim);
}
pid_t CRT_getppid(void)
{
    return getppid();
}



int CRT_mlock(const void *addr, size_t len)
{
    if (ISNULL(addr))
    {
        errno = ENULLARG;
        return -1;
    }
    return mlock(addr, len);
}
int CRT_munlock(const void *addr, size_t len)
{
    if (ISNULL(addr))
    {
        errno = ENULLARG;
        return -1;
    }
    return munlock(addr, len);
}





// pthread

int CRT_pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
{
    if(ISNULL(mutex))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_mutex_init(mutex,mutexattr);
}

int CRT_pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    if(ISNULL(mutex))
    {
        errno = ENULLARG;
        return (-1);
    }
    return  pthread_mutex_destroy(mutex);
}

int CRT_pthread_mutex_lock(pthread_mutex_t *mutex)
{
    if(ISNULL(mutex))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_mutex_lock(mutex);
}

int CRT_pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    if(ISNULL(mutex))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_mutex_unlock(mutex);
}

int CRT_pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    if(ISNULL(mutex))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_mutex_trylock(mutex);
}

int CRT_pthread_mutex_timedlock(pthread_mutex_t *mutex,const struct timespec *abs_timeout)
{
    if(ISNULL(mutex)||ISNULL(abs_timeout))
    {
        errno = ENULLARG;
        return (-1);
    }
#ifdef __FreeBSD__
    return pthread_mutex_timedlock(mutex,abs_timeout);
#else
    return pthread_mutex_lock(mutex);
#endif

}


int CRT_pthread_condattr_init(pthread_condattr_t *attr)
{
    if(ISNULL(attr))
    {
        errno = ENULLARG;
        return (-1);
    }

    return pthread_condattr_init(attr);
}

int CRT_pthread_condattr_destroy(pthread_condattr_t *attr)
{
    if(ISNULL(attr))
    {
        errno = ENULLARG;
        return (-1);
    }

    return pthread_condattr_destroy(attr);
}

int CRT_pthread_condattr_setpshared(pthread_condattr_t *attr,int pshared)
{
    if(ISNULL(attr))
    {
        errno = ENULLARG;
        return (-1);
    }

    return pthread_condattr_setpshared(attr,pshared);
}


int CRT_pthread_cond_init(pthread_cond_t *cond,const pthread_condattr_t *cond_attr)
{
    if(ISNULL(cond))
    {
        errno = ENULLARG;
        return (-1);
    }

    return pthread_cond_init(cond,cond_attr);
}

int CRT_pthread_cond_destroy(pthread_cond_t *cond)
{
    if(ISNULL(cond))
    {
        errno = ENULLARG;
        return (-1);
    }

    return pthread_cond_destroy(cond);
}

int CRT_pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    if(ISNULL(cond)||ISNULL(mutex))
    {
        errno = ENULLARG;
        return (-1);
    }

    return pthread_cond_wait(cond,mutex);
}

int CRT_pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
{
    if(ISNULL(cond)||ISNULL(mutex)||ISNULL(abstime))
    {
        errno = ENULLARG;
        return (-1);
    }

    return pthread_cond_timedwait(cond,mutex,abstime);
}

int CRT_pthread_cond_signal(pthread_cond_t *cond)
{
    if(ISNULL(cond))
    {
        errno = ENULLARG;
        return (-1);
    }

    return pthread_cond_signal(cond);
}

int CRT_pthread_cond_broadcast(pthread_cond_t *cond)
{
    if(ISNULL(cond))
    {
        errno = ENULLARG;
        return (-1);
    }

    return pthread_cond_broadcast(cond);
}



int CRT_pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    if(ISNULL(attr))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_mutexattr_destroy(attr);
}

int CRT_pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    if(ISNULL(attr))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_mutexattr_init(attr);
}

int CRT_pthread_mutexattr_gettype(const  pthread_mutexattr_t  *attr, int *type)
{
    if(ISNULL(attr)||ISNULL(type))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_mutexattr_gettype(attr,type);
}

int CRT_pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
    if(ISNULL(attr))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_mutexattr_settype(attr,type);
}

int CRT_pthread_mutexattr_setpshared(pthread_mutexattr_t *attr,int pshared)
{
    if(ISNULL(attr))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_mutexattr_setpshared(attr,pshared);
}

void CRT_pthread_testcancel()
{
    pthread_testcancel();
}

int CRT_pthread_setcancelstate(int state, int *oldstate)
{
    return pthread_setcancelstate(state,oldstate);
}

int CRT_pthread_attr_init(pthread_attr_t *attr)
{
    if(ISNULL(attr))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_attr_init(attr);
}

int CRT_pthread_attr_destroy(pthread_attr_t *attr)
{
    if(ISNULL(attr))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_attr_destroy(attr);
}

int CRT_pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    if(ISNULL(attr))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_attr_setstacksize(attr,stacksize);
}

int CRT_pthread_attr_getstacksize(const pthread_attr_t * attr, size_t * stacksize)
{
    return pthread_attr_getstacksize(attr,stacksize);
}

int CRT_pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
    if(ISNULL(attr))
    {
        errno = ENULLARG;
        return (-1);
    }
    return pthread_attr_setdetachstate(attr,detachstate);
}

int CRT_pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate)
{
    return pthread_attr_getdetachstate(attr,detachstate);
}

int CRT_pthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void*), void * arg)
{
    return pthread_create(thread,attr,start_routine,arg);
}

int CRT_pthread_cancel(pthread_t thread)
{
    return pthread_cancel(thread);
}

int CRT_pthread_join(pthread_t thread, void **value_ptr)
{
    return pthread_join(thread,value_ptr);
}

int CRT_pthread_detach(pthread_t thread)
{
    return pthread_detach(thread);
}

pthread_t CRT_pthread_self(void)
{
    return pthread_self();
}



/* typedef struct */
/* { */
/* 	struct cmsghdr			cmhdr; */
/* 	INT						fd[1];	 */
/* }AFASCKCTLDTA,*PAFASCKCTLDTA; */



/* INT SendFd(INT sock,PBYTE PTC,UINT PTCLen,INT fd) */
/* { */
/* 	struct msghdr 		send_msg; */
/* 	struct iovec 		iov[2]; */
/* 	BYTE        		Token[8]; */
/* 	INT					rc; */

/* 	#ifdef _UNIX95_ */
/* 	PAFASCKCTLDTA       pMsgData; */
/*     BYTE                szData[sizeof(*pMsgData)+8]; */
/*     pMsgData            = (PAFASCKCTLDTA)szData; */
/*     #endif */




/* 	memset(&send_msg,0,sizeof(send_msg)); */
/* 	memset(iov,0,sizeof(iov)); */
/* 	memset(Token,0,sizeof(Token)); */

/* 	Token[0] 			    = 0xAB; */
/* 	Token[1] 			    = 0xCD; */
/* 	Token[2] 			    = 0xEF; */
/* 	Token[3] 			    = 0xFE; */

/* 	memcpy(&Token[4],&PTCLen,sizeof(PTCLen)); */

/* 	iov[0].iov_base 		= (PSTR)Token; */
/* 	iov[0].iov_len 			= sizeof(Token); */
/* 	iov[1].iov_base 		= (PSTR)PTC; */
/* 	iov[1].iov_len 			= PTCLen; */


/* 	send_msg.msg_iov 		    = iov; */
/* 	send_msg.msg_iovlen 	    = 2; */

/* 	#ifndef _UNIX95_ */
/*     	#ifndef  _HPUX_ */
/*             send_msg.msg_accrights      = (char *) &fd; */
/*             send_msg.msg_accrightslen   = sizeof(fd); */
/*         #else */
/*             send_msg.msg_control      = (char *) &fd; */
/*             send_msg.msg_controllen   = sizeof(fd); */
/*         #endif */
/*     #else */
/*     memset(pMsgData,0,sizeof(*pMsgData)); */
/* 	pMsgData->cmhdr.cmsg_level 	= SOL_SOCKET; */
/* 	pMsgData->cmhdr.cmsg_type 	= SCM_RIGHTS; */
/* 	pMsgData->cmhdr.cmsg_len 	= sizeof(*pMsgData); */
/* 	pMsgData->fd[0]				= fd; */

/* 	send_msg.msg_control 	    = (char *) pMsgData; */
/* 	send_msg.msg_controllen     = pMsgData->cmhdr.cmsg_len; */
/*     #endif */

/* 	rc = sendmsg(sock, &send_msg, 0); */
/* 	if(rc==-1) */
/* 	{ */
/* 	    return (__LINE__);	 */
/* 	} */
/* 	else if(rc==0) */
/* 	{ */
/* 		return (-1); 	 */
/* 	} */

/* 	return (0);  */
/* } */

/* INT RecvFd(INT sock,PBYTE PTC,UINT maxPTCLen, PINT fd) */
/* { */
/* 	struct msghdr 		recv_msg; */
/* 	struct iovec 		iov[2]; */
/* 	BYTE        		Token[8]; */
/* 	UINT                iLen; */
/* 	INT					rc; */

/* 	/\* New _UNIX95_ way*\/ */
/* 	#ifdef _UNIX95_ */
/* 	PAFASCKCTLDTA       pMsgData; */
/*     BYTE                szData[sizeof(*pMsgData)+8]; */
/*     pMsgData            = (PAFASCKCTLDTA)szData; */
/*     #else */
/*     INT                 rfd; */
/*     #endif */


/* 	memset(&recv_msg,0,sizeof(recv_msg)); */
/* 	memset(iov,0,sizeof(iov)); */
/* 	memset(Token,0,sizeof(Token)); */

/* 	iov[0].iov_base 	    = (PSTR)Token; */
/* 	iov[0].iov_len 			= sizeof(Token); */
/* 	iov[1].iov_base 	    = (PSTR)PTC; */
/* 	iov[1].iov_len 			= maxPTCLen; */

/* 	recv_msg.msg_iov 		    = iov; */
/* 	recv_msg.msg_iovlen 	    = 2;	 */

/* 	#ifndef _UNIX95_ */
/* 	rfd = -1; */
/*     	#ifndef  _HPUX_ */
/*             recv_msg.msg_accrights      = (char *) &rfd; */
/*             recv_msg.msg_accrightslen   = sizeof(rfd); */
/*         #else */
/*             recv_msg.msg_control      = (char *) &rfd; */
/*             recv_msg.msg_controllen   = sizeof(int); */
/*         #endif	 */

/*     #else */
/*     memset(pMsgData,0,sizeof(*pMsgData)); */
/* 	pMsgData->cmhdr.cmsg_len 	= sizeof(*pMsgData); */
/* 	pMsgData->fd[0]				= -1; */
/* 	recv_msg.msg_control 	    = (char *) pMsgData; */
/* 	recv_msg.msg_controllen     = sizeof(*pMsgData); */
/*     #endif */
/* 	rc = recvmsg (sock, &recv_msg,0); */
/* 	if(rc==-1) */
/* 	{ */
/* 	    return (__LINE__);	 */
/* 	} */
/* 	else if(rc==0) */
/* 	{ */

/* 		return (-1); 	 */
/* 	} */

/* 	memcpy(&iLen,&Token[4],sizeof(iLen)); */
/* 	if((iov[0].iov_len!=sizeof(Token))||(Token[0]!=0xAB)|| \ */
/* 		(Token[1]!=0xCD)||(Token[2]!=0xEF)||(Token[3]!=0xFE)) */
/* 	{ */
/* 	    return (__LINE__);	 */
/* 	} */

/* 	#ifndef _UNIX95_ */
/* 	fd[0]	  = rfd; */
/* 	#else */
/* 	if((pMsgData->cmhdr.cmsg_level != SOL_SOCKET)||(pMsgData->cmhdr.cmsg_type != SCM_RIGHTS)) */
/* 	{ */
/* 	    return (__LINE__);	   */
/* 	} */
/* 	fd[0]	  = pMsgData->fd[0]; */
/* 	#endif */

/* 	return (0);   */
/* } */
