/*
 * resource.c
 *
 *  Created on: Apr 11, 2013
 *      Author: esuomyekcim
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(__linux__) || defined(_AIX)
#include <limits.h>
#endif

#ifndef PATH_MAX
#  define PATH_MAX    (4096)
#endif

#ifndef NAME_MAX
#  define NAME_MAX    (255)
#endif

#include "bswap.h"
#include "resource.h"

#ifdef __cplusplus
extern "C"
{
#endif

void resx_environ_init(resx_environ_t * const resxenvp)
{
    resxenvp->stream = NULL;
    resxenvp->offset = RESOURCE_CHAOS;
    resxenvp->length = 0;
    resxenvp->nodesize = sizeof(resx_node_t);
}

void resx_head_init(resx_head_t * const resxheadp)
{
    resxheadp->creation = (time_t) -1;
    resxheadp->description = RESOURCE_CHAOS;
    resxheadp->length = 0;
    resxheadp->nodesize = sizeof(resx_node_t);
    resxheadp->ident[0] = '\0';
}

void resx_node_init(resx_node_t * const resxnodep)
{
    resxnodep->name = RESOURCE_CHAOS;
    resxnodep->offset = RESOURCE_CHAOS;
    resxnodep->length = 0;
    resxnodep->prev = resxnodep->next = RESOURCE_CHAOS;
    resxnodep->parent = resxnodep->children = RESOURCE_CHAOS;
}

static int resx_environ_open_internal(resx_environ_t * const resxenvp)
{
    // 定位到文件结尾的resx_head_t节点
    if (fseeko(resxenvp->stream, -sizeof(resx_head_t), SEEK_END))
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    // 读取资源文件头
    resx_head_t resxhead;
    if (resx_fstream_read(&resxhead, sizeof(resx_head_t), (size_t) 1U,
            resxenvp->stream) < (size_t) 1U)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    resxhead_betoh(&resxhead);
    // 判断是否存在资源文件
    if (memcmp(&resxhead.ident[0], RESX_MAGIC, RESX_MAGIC_LENGTH) != 0)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                "No resource file.\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    resxenvp->length = resxhead.length;

    if (fseeko(resxenvp->stream, 0, SEEK_END) == -1)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    if ((resxenvp->offset = ftello(resxenvp->stream)) == (off_t) -1)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    resxenvp->offset -= resxenvp->length;
    resxenvp->nodesize = resxhead.nodesize;

//    fprintf(stdout, "File %s, Function %s, Line %d: length = %d\n"
//            "nodesize = %d\n"
//            "offset = %d\n", __FILE__, __FUNCTION__, __LINE__,
//            resxenvp->length, resxenvp->nodesize, resxenvp->offset);

    return 0;
}

#if defined(linux) || defined(__linux) || defined(__linux__)
int resx_environ_open(resx_environ_t * const resxenvp)
{
    if ((resxenvp->stream = fopen(EXECUTION_SELF, "rb")) == NULL)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    if (resx_environ_open_internal(resxenvp) != 0)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}
#else
int resx_environ_open(resx_environ_t * const resxenvp, const char *pathname)
{
    if ((resxenvp->stream = fopen(pathname, "rb")) == NULL)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    if (resx_environ_open_internal(resxenvp) != 0)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}
#endif

void resx_environ_close(resx_environ_t * const resxenvp)
{
    fclose(resxenvp->stream);
    resxenvp->stream = NULL;
    resxenvp->offset = RESOURCE_CHAOS;
    resxenvp->length = 0;
}

/**
 * 获取路径中的第一个目录名
 * @param pathname 目录路径
 * @param buf 存放dirname的缓冲区指针
 * @param siz 缓冲区的字节数
 * @return 返回指向dirname之后的字符的指针。如果返回值为空指针，并且 buf[0] == '\0'，那么路径中不存在
 * dirname；否则@param buf中的dirname发生了截断。
 */
static const char *resx_dirname(const char *pathname, char * const buf,
        const size_t siz)
{
    while ((*pathname != '\0') && (*pathname == '/'))
    {
        ++pathname;
    }
    const char *retval = strchr(pathname, '/');
    if (retval != NULL)
    {
        memcpy(buf, pathname, (retval - pathname < siz ? retval - pathname : siz));
        if (retval - pathname < siz)
        {
            buf[retval - pathname] = '\0';
            return ++retval;
        }
        else
        {
            buf[siz - 1] = '\0';
        }
    }
    else
    {
        buf[0] = '\0';
    }
    return NULL;
}

/**
 * 获取路径中的第一个目录名
 * @param pathname 目录路径
 * @param buf 存放basename的缓冲区指针
 * @param siz 缓冲区的字节数
 * @return 返回指向basename的第一个字符的指针。如果返回值为空指针，并且buf[0] == '\0', 那么路径中
 * 不存在basename；否则@param buf中的basename发生了截断。
 */
static const char *resx_basename(const char *pathname, char * const buf,
        const size_t siz)
{
    const char *sentinel = pathname + strlen(pathname);
    while ((sentinel > pathname) && (sentinel[-1] == '/'))
    {
        --sentinel;
    }
    const char *retval = sentinel;
    while ((retval > pathname) && (retval[-1] != '/'))
    {
        --retval;
    }
    if (retval < sentinel)
    {
        memcpy(buf, retval, (sentinel - retval < siz ? sentinel - retval : siz));
        if (sentinel - retval < siz)
        {
            buf[sentinel - retval] = '\0';
            return retval;
        }
        else
        {
            buf[siz - 1] = '\0';
        }
    }
    else
    {
        buf[0] = '\0';
    }
    return NULL;
}

static int resx_read_internal(const off_t offset, void * const buf,
        const size_t siz, FILE * const stream)
{
    if (fseeko(stream, offset, SEEK_SET) != 0)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    if (resx_fstream_read(buf, (size_t) 1U, siz, stream) < siz)
    {
        if (ferror(stream))
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }
    return 0;
}

/**
 *
 * @param resxenvp
 * @param pathname
 * @return
 * -2, 没有找到资源文件
 */
int32_t resx_environ_read(const resx_environ_t * const resxenvp,
        const char *pathname)
{
    const char *retval;
    char name[NAME_MAX + 1] = {[sizeof(name) - 1] = '\0'};
    char buf[PATH_MAX + 1] = {[sizeof(buf) - 1] = '\0'};

    int32_t next = 0;
    resx_node_t resxnode;
    while (true)
    {
        if (resx_read_internal(resxenvp->offset + next, &resxnode,
                sizeof(resx_node_t), resxenvp->stream) != 0)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
        resxnode_betoh(&resxnode);
        if (resx_read_internal(resxenvp->offset + resxnode.name, &name[0],
                sizeof(name) - 1, resxenvp->stream) != 0)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
        if ((retval = resx_dirname(pathname, &buf[0], sizeof(buf) - 1)) != NULL)
        { // 当前路径pathname仍然存在‘/’分隔符
//            fprintf(stdout, "File %s, Function %s, Line %d, pathname = %s, "
//                    "buf = %s, name = %s, retval = %s\n", __FILE__,
//                    __FUNCTION__, __LINE__, pathname, buf, &name[0], retval);
            if (strncmp(&name[0], &buf[0], sizeof(name) - 1) != 0)
            { // 遍历同辈节点，寻找指定名字的目录
                next = resxnode.next;
            }
            else
            { // 进入子目录，在子目录中继续查找文件
                next = resxnode.children;
                pathname = retval;
            }
//            fprintf(stdout, "File %s, Function %s, Line %d, resxnode.next = %d, "
//                    "resxnode.children = %d\n", __FILE__, __FUNCTION__,
//                    __LINE__, resxnode.next, resxnode.children);
            if (next == RESOURCE_CHAOS)
            {
                fprintf(stderr, "Debug: File %s, Function %s, Line %d, "
                        "No such directory, '%s'.\n", __FILE__, __FUNCTION__,
                        __LINE__, &name[0]);
                return -2; // 没有找到相应的资源文件
            }
        }
        else if (buf[0] == '\0')
        { // pathname中已不存在目录名（也就是说，不存在以‘/’分隔的字段
//            fprintf(stdout, "File %s, Function %s, Line %d, pathname = %s\n",
//                    __FILE__, __FUNCTION__, __LINE__, pathname);
            if ((retval = resx_basename(pathname, &buf[0], sizeof(buf) - 1)) != NULL)
            {
//                fprintf(stdout, "File %s, Function %s, Line %d, pathname = %s, "
//                        "buf = %s, retval = %s, name = %s\n", __FILE__,
//                        __FUNCTION__, __LINE__, pathname, buf, retval, &name[0]);
                if (strncmp(&name[0], &buf[0], sizeof(name) - 1) != 0)
                { // 遍历同辈节点，寻找指定名字的目录
                    next = resxnode.next;
                    if (next == RESOURCE_CHAOS)
                    {
                        fprintf(stderr, "Debug: File %s, Function %s, Line %d, "
                                "No such directory, '%s'.\n", __FILE__, __FUNCTION__,
                                __LINE__, &name[0]);
                        return -2; // 没有找到相应的资源文件
                    }
                }
                else
                { // 找到了指定的资源文件
                    if (fseeko(resxenvp->stream,
                            resxenvp->offset + resxnode.offset, SEEK_SET) == -1)
                    {
                        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
                        return -1;
                    }
                    return resxnode.length;
                }
            }
            else if (buf[0] == '\0')
            {
                fprintf(stderr, "Debug: File %s, Function %s, Line %d, "
                        "No such file, '%s'.\n", __FILE__, __FUNCTION__,
                        __LINE__, &name[0]);
                return -2; // 没有找到相应的资源文件
            }
            else
            { // basename中存在过长的文件名，导致缓冲区不足，发生截断
                fprintf(stderr, "Error: File %s, Function %s, Line %d, Truncation, "
                        "'%s'.\n", __FILE__, __FUNCTION__, __LINE__, pathname);
                return -1;
            }
        }
        else
        { // pathname中存在过长的目录名，导致缓冲区不足，发生截断
            fprintf(stderr, "Error: File %s, Function %s, Line %d, Truncation, "
                    "'%s'.\n", __FILE__, __FUNCTION__, __LINE__, pathname);
            return -1;
        }
    }
    return 0;
}

size_t strlcpy(char * const dst, const char * const src, size_t siz)
{
    const size_t retval = strlen(src);

    if (siz > 0)
    {
        siz = ((retval < siz) ? retval : (siz - 1));
        memcpy(dst, src, siz);
        dst[siz] = '\0';
    }
    return retval; /* does not include NUL */
}

size_t strlcat(char * const dst, const char * const src, size_t siz)
{
    const size_t dstlength = strlen(dst);
    const size_t srclength = strlen(src);
    const size_t retval = dstlength + srclength;
    if (siz > dstlength)
    {
        siz = ((retval < siz) ? srclength : (siz - dstlength));
        memcpy(&dst[dstlength], src, siz);
        dst[dstlength + siz] = '\0';
    }
    return retval; /* does not include NUL */
}

size_t resx_fstream_read(void * const buf, const size_t siz,
        const size_t nitems, FILE * const stream)
{
    size_t numRead = 0, totRead = 0;
    char (* const array)[siz] = (char (*)[siz]) buf;
    while ((totRead += fread(&array[numRead][0], siz,
            nitems - numRead, stream)) < nitems)
    {
        if (feof(stream))
        {
            break;
        }
        else
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return 0;
        }
        numRead = totRead;
    }
    return totRead;
}

size_t resx_fstream_write(const void * const buf, const size_t siz,
        const size_t nitems, FILE * const stream)
{
    size_t numWritten = 0, totWritten = 0;
    const char (* const array)[siz] = (const char (*)[siz]) buf;
    while ((totWritten += fwrite(&array[numWritten][0], siz,
            nitems - numWritten, stream)) < nitems)
    {
        if (ferror(stream) != 0)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
            return 0;
        }
        numWritten = totWritten;
    }
    return totWritten;
}

void resxhead_htobe(resx_head_t * const headp)
{
    headp->creation = htobe32(headp->creation);
    headp->description = htobe32(headp->description);
    headp->length = htobe32(headp->length);
    headp->nodesize = htobe16(headp->nodesize);
}

void resxhead_betoh(resx_head_t * const headp)
{
    headp->creation = be32toh(headp->creation);
    headp->description = be32toh(headp->description);
    headp->length = be32toh(headp->length);
    headp->nodesize = be16toh(headp->nodesize);
}

void resxnode_htobe(resx_node_t * const nodep)
{
    nodep->name = htobe32(nodep->name);
    nodep->offset = htobe32(nodep->offset);
    nodep->length = htobe32(nodep->length);
    nodep->prev = htobe32(nodep->prev);
    nodep->next = htobe32(nodep->next);
    nodep->parent = htobe32(nodep->parent);
    nodep->children = htobe32(nodep->children);
}

void resxnode_betoh(resx_node_t * const nodep)
{
    nodep->name = be32toh(nodep->name);
    nodep->offset = be32toh(nodep->offset);
    nodep->length = be32toh(nodep->length);
    nodep->prev = be32toh(nodep->prev);
    nodep->next = be32toh(nodep->next);
    nodep->parent = be32toh(nodep->parent);
    nodep->children = be32toh(nodep->children);
}


#ifdef __cplusplus
}
#endif
