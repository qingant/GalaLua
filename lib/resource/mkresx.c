/*
 * mkresx.c
 *
 *  Created on: Apr 11, 2013
 *      Author: esuomyekcim
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
#include <dirent.h>
#include <libgen.h>

#include <time.h>

#include "bswap.h"
#include "resource.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct location_t
{
    int32_t offset;
    resx_node_t *nodep;
};
typedef struct location_t location_t;

/**
 * 从文件系统中读取文件内容，然后写入到指定的文件
 * @param fin 输入文件流
 * @param fout 输出文件流
 * @param buf 缓冲区指针
 * @param siz 缓冲区字节数
 * @return
 */
static int32_t fstream_sendfile(FILE * const fout, FILE * const fin,
        void * const buf, const size_t siz)
{
    if (fseeko(fin, 0, SEEK_SET) == (off_t) -1)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    int32_t totWritten = 0;
    for (int32_t numRead, numWritten; !feof(fin); totWritten += numWritten)
    {
        if ((numRead = resx_fstream_read(buf, (size_t) 1U, siz, fin)) < siz)
        {
            if (ferror(fin))
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                        __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
        }
        if ((numWritten = resx_fstream_write(buf, (size_t) 1U, numRead, fout))
                < numRead)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }
    return totWritten;
}

static int32_t make_node_generic(const char * const pathname,
        resx_node_t * const nodep, FILE * const stream,
        location_t * const parent);

static int make_node_directory(const char *pathname, FILE * const stream,
        location_t * const parent, char * const buf, const size_t siz)
{
    const size_t length = strlcpy(buf, pathname, siz) + 1;
    if(length > siz)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    buf[length - 1] = '/';

    location_t previous = {.nodep = NULL, .offset = RESOURCE_CHAOS};
    DIR * const dirp = opendir(buf);
    if (dirp == NULL)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    int errcode;
    int32_t retval;
    struct dirent entry, *entryp;
    resx_node_t *nodep;
    do
    {
        if ((errcode = readdir_r(dirp, &entry, &entryp)) != 0)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errcode));
            return -1;
        }
        if ((entryp != NULL) && (entryp->d_name[0] != '.'))
        {
            buf[length] = '\0';
            if (strlcpy(&buf[length], &entryp->d_name[0], siz - length)
                    >= siz - length)
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                        "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
            if ((nodep = (resx_node_t *) malloc(sizeof(resx_node_t))) == NULL)
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d, malloc.\n",
                        __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
            if ((retval = make_node_generic(buf, nodep, stream, parent)) == -1)
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                        __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
            nodep->prev = previous.offset;
            if (previous.nodep != NULL)
            {
                previous.nodep->next = retval;
                // 写入节点
                if (fseeko(stream, previous.offset, SEEK_SET) == -1)
                {
                    fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                            __FILE__, __FUNCTION__, __LINE__, strerror(errno));
                    return -1;
                }
                resxnode_htobe(previous.nodep);
                if (resx_fstream_write(previous.nodep, sizeof(resx_node_t),
                        (size_t) 1U, stream) < (size_t) 1U)
                {
                    fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                            __FILE__, __FUNCTION__, __LINE__);
                    return -1;
                }
                free(previous.nodep);
                previous.nodep = NULL;
            }
            previous.offset = retval;
            previous.nodep = nodep;
        }
    } while (entryp != NULL);
    return 0;
}

static int32_t make_node_generic(const char * const pathname,
        resx_node_t * const nodep, FILE * const stream,
        location_t * const parent)
{
    char name[NAME_MAX + 1] = {[sizeof(name) - 1] = '\0'};
    char path[PATH_MAX + 1] = {[sizeof(path) - 1] = '\0'};

    struct stat statbuf;
    if (stat(pathname, &statbuf) == -1)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    if (strlcpy(&path[0], pathname, sizeof(path)) >= sizeof(path))
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    size_t length = strlcpy(&name[0], basename(&path[0]), sizeof(name));
    if (length >= sizeof(name))
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                "Truncation.\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    ++length; // 需要将字符串的结束符也写入到文件

    // 定位到当前的可写入位置
    if (fseeko(stream, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    const int32_t retval = ftello(stream);
    if (retval == (off_t) -1)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    // 定位到文件名或目录名所在位置
    nodep->name = retval + sizeof(resx_node_t);
    printf("File %s, Function %s, Line %d, %d, name = %d.\n",
                __FILE__, __FUNCTION__, __LINE__, retval, nodep->name);
    if (fseeko(stream, nodep->name, SEEK_SET) == -1)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    if (resx_fstream_write(&name[0], (size_t) 1U, length, stream) < length)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    // 将offset定位到文件内容所在位置
    nodep->offset = nodep->name + length;
    // 节点的父节点
    if (parent != NULL)
    {
        nodep->parent = parent->offset;
        if ((parent->nodep != NULL) &&
                (parent->nodep->children == RESOURCE_CHAOS))
        {
            parent->nodep->children = retval;
        }
    }
    else
    {
        nodep->parent = RESOURCE_CHAOS;
    }
    if (S_ISREG(statbuf.st_mode))
    {
        // 文件的长度
        nodep->length = statbuf.st_size;
        if (fseeko(stream, nodep->offset, SEEK_SET) == -1)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return -1;
        }
        FILE * const fin = fopen(pathname, "rb");
        if (fin == NULL)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return -1;
        }
        if (fstream_sendfile(stream, fin, &path[0], sizeof(path)) == -1)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d.\n", __FILE__,
                    __FUNCTION__, __LINE__);
            return -1;
        }
        fclose(fin);
        nodep->children = RESOURCE_CHAOS;
    }
    else if (S_ISDIR(statbuf.st_mode))
    {
        // 目录的长度
        nodep->length = 0;
        // @TODO 处理目录
        location_t child = {.nodep = nodep, .offset = retval};
        if (make_node_directory(pathname, stream, &child, &path[0],
                sizeof(path)) != 0)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d.\n", __FILE__,
                    __FUNCTION__, __LINE__);
            return -1;
        }
    }
    return retval;
}

static int make_resx_node(FILE * const stream, const int argc,
        char * const argv[], int argind)
{
    location_t previous = {.nodep = NULL, .offset = RESOURCE_CHAOS};
    location_t parent = {.nodep = NULL, .offset = RESOURCE_CHAOS};

    int32_t retval;
    for (resx_node_t *nodep; argind < argc; ++argind)
    {
        printf("Line %d: argv[%d] = %s\n", __LINE__, argind, argv[argind]);
        if ((nodep = (resx_node_t *) malloc(sizeof(resx_node_t))) == NULL)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, malloc.\n",
                    __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
        if ((retval = make_node_generic(argv[argind], nodep, stream,
                &parent)) == -1)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
        nodep->prev = previous.offset;
        if (previous.nodep != NULL)
        {
            printf("File %s, Function %s, Line %d, offset = %d\n",
                    __FILE__, __FUNCTION__, __LINE__, previous.offset);
            previous.nodep->next = retval;
            // 写入节点
            if (fseeko(stream, previous.offset, SEEK_SET) == -1)
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                        __FILE__, __FUNCTION__, __LINE__, strerror(errno));
                return -1;
            }
            resxnode_htobe(previous.nodep);
            if (resx_fstream_write(previous.nodep, sizeof(resx_node_t),
                    (size_t) 1U, stream) < (size_t) 1U)
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                        __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
            free(previous.nodep);
            previous.nodep = NULL;
        }
        previous.offset = retval;
        previous.nodep = nodep;
    }
    // 处理最后一个节点
    if (previous.nodep != NULL)
    {
        previous.nodep->next = RESOURCE_CHAOS;
        // 写入节点
        if (fseeko(stream, previous.offset, SEEK_SET) == -1)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                    __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            return -1;
        }
        resxnode_htobe(previous.nodep);
        if (resx_fstream_write(previous.nodep, sizeof(resx_node_t),
                (size_t) 1U, stream) < (size_t) 1U)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                    __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }

    return 0;
}

static int make_resx_head(FILE * const stream)
{
    if (fseeko(stream, 0, SEEK_END) == -1)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    resx_head_t resxhead;
    resx_head_init(&resxhead);
    resxhead.creation = time((time_t *)NULL);
    if ((resxhead.description = ftello(stream)) == (off_t) -1)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    const char * const description = "";
    const size_t length = strlen(description) + 1;
    if (resx_fstream_write(description, (size_t) 1U, length, stream) < length)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    if((resxhead.length = ftello(stream)) == (off_t) -1)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }
    resxhead.length += sizeof(resx_head_t);
    resxhead.nodesize = sizeof(resx_node_t);

    memcpy(&resxhead.ident[0], RESX_MAGIC, RESX_MAGIC_LENGTH);
    resxhead.ident[RESX_DATA] = RESXDATA2MSB;
    resxhead.ident[RESX_MAJOR_VERSION] = RESX_MAJOR_CURRENT;
    resxhead.ident[RESX_MINOR_VERSION] = RESX_MINOR_CURRENT;

    printf("creation = %d\n"
            "length = %d\n"
            "nodesize = %d\n", resxhead.creation,
            resxhead.length, resxhead.nodesize);

    resxhead_htobe(&resxhead);
    if (resx_fstream_write(&resxhead, sizeof(resx_head_t), (size_t) 1U, stream)
            < (size_t) 1U)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n",
                __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}

/**
 *
 * @param argc 参数个数
 * @param argv 参数字符串指针数组，其中最后一个值字符串指针值必须为NULL
 * @param buf
 * @param siz
 * @return 返回第一个非选项参数在@param argv中的索引值。如果出错，返回-1
 * @attention 传入的@param argv参数中，选项参数必须是第一个值
 */
static int parser(const int argc, char * const argv[], char * const buf,
        const size_t siz)
{
    extern char *optarg;
    extern int optind, opterr, optopt;
    optind = 0;
    opterr = 0;
    buf[0] = '\0';
    int retval;
    bool noresxname = true;
    while ((retval = getopt(argc, argv, ":o:")) != -1)
    {
        switch (retval)
        {
            case 'o':
            {
                noresxname = false;
                if (strlcpy(buf, optarg, siz) >= siz)
                {
                    fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                            "Truncation, %s.\n", __FILE__, __FUNCTION__,
                            __LINE__, optarg);
                    return -1;
                }
                break;
            }
            case ':':
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                        "Option -%c requires an operand.\n", __FILE__,
                        __FUNCTION__, __LINE__, optopt);
                return -1;
            }
            case '?':
            {
                fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                        "Unrecognized option -%c.\n", __FILE__,
                        __FUNCTION__, __LINE__, optopt);
                return -1;
            }
        } // end of switch
    } // end of while

    if (noresxname)
    {
        if (strlcpy(buf, RESOURCE_DEFAULT_NAME, siz) >= siz)
        {
            fprintf(stderr, "Error: File %s, Function %s, Line %d, "
                    "Truncation, %s.\n", __FILE__, __FUNCTION__,
                    __LINE__, RESOURCE_DEFAULT_NAME);
            return -1;
        }
    }

    return optind;
}


#ifdef __cplusplus
}
#endif

int main(int argc, char *argv[])
{
    char buf[BUFSIZ + 1] = {[sizeof(buf) - 1] = '\0'};
    char pathname[PATH_MAX + 1] = {[sizeof(pathname) - 1] = '\0'};
    const int retval = parser(argc, argv, &buf[0], sizeof(buf) - 1);
    if (retval == -1)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n", __FILE__,
                __FUNCTION__, __LINE__);
        exit(EXIT_FAILURE);
    }
    FILE * const fout = fopen(&buf[0], "wb");
    if (fout == NULL)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d, %s.\n",
                __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (make_resx_node(fout, argc, argv, retval) != 0)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n", __FILE__,
                __FUNCTION__, __LINE__);
        exit(EXIT_FAILURE);
    }

    if (make_resx_head(fout) != 0)
    {
        fprintf(stderr, "Error: File %s, Function %s, Line %d.\n", __FILE__,
                __FUNCTION__, __LINE__);
        exit(EXIT_FAILURE);
    }
    fclose(fout);

    exit(EXIT_SUCCESS);
}
