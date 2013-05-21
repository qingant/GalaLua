/*
 * resource.h
 *
 *  Created on: Apr 11, 2013
 *      Author: esuomyekcim
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RESX_NIDENT
#define RESX_NIDENT     8
#endif

#define RID_MAG0        0       /* File identification byte 0 index */
#define RESXMAG0        0x7f    /* Magic number byte 0 */

#define RID_MAG1        1       /* File identification byte 1 index */
#define RESXMAG1        'R'     /* Magic number byte 1 */

#define RID_MAG2        2       /* File identification byte 2 index */
#define RESXMAG2        'E'     /* Magic number byte 2 */

#define RID_MAG3        3       /* File identification byte 3 index */
#define RESXMAG3        'S'     /* Magic number byte 3 */

#define RID_MAG4        4       /* File identification byte 4 index */
#define RESXMAG4        'X'     /* Magic number byte 4 */

/* Conglomeration of the identification bytes. */
#define RESX_MAGIC      "\177RESX"
#define RESX_MAGIC_LENGTH   5

#define RESX_DATA       5       /* Data encoding byte index */
#define RESXDATANONE    0       /* Invalid data encoding */
#define RESXDATA2LSB    1       /* little endian */
#define RESXDATA2MSB    2       /* big endian */

#define RESX_MAJOR_VERSION      6   /* File major version byte index */
#define RESX_MAJOR_CURRENT      1

#define RESX_VERSION_NONE       0

#define RESX_MINOR_VERSION      7   /* File minor version byte index */
#define RESX_MINOR_CURRENT      0

#if defined(linux) || defined(__linux) || defined(__linux__)
    #ifndef EXECUTION_SELF
        #define EXECUTION_SELF      "/proc/self/exe"
    #endif
#endif

#ifndef RESOURCE_DEFAULT_NAME
#define RESOURCE_DEFAULT_NAME   "resource.resx"
#endif

#ifndef INT32_MIN
#define INT32_MIN      (-2147483647-1)
#endif

#ifndef RESOURCE_CHAOS
#define RESOURCE_CHAOS      (INT32_MIN)
#endif

struct resx_head_t
{
    int32_t creation; // 资源段的创建时间
    int32_t description; // 资源段的描述信息的偏移量
    int32_t length; // 资源段的字节数
    uint16_t nodesize; // 节点头的字节数
    uint16_t padding;
    uint8_t ident[RESX_NIDENT];
}; // 24
typedef struct resx_head_t resx_head_t;

struct resx_node_t
{
    int32_t name; // 文件名或目录名的偏移量
    int32_t offset; // 指向文件内容开始处的偏移量
    uint32_t length; // 文件的字节数
    int32_t prev; // 当前节点的上一个同辈节点的偏移量
    int32_t next; // 当前节点的下一个同辈节点的偏移量
    int32_t parent; // 当前节点的父节点的偏移量
    int32_t children; // 当前节点的子节点的偏移量
}; // 28
typedef struct resx_node_t resx_node_t;

struct resx_environ_t
{
    FILE *stream; // 当前可执行文件流
    int32_t offset; // 资源段在可执行文件中的偏移量
    int32_t length; // 资源段的字节数
    uint16_t nodesize; // 节点头的字节数
};
typedef struct resx_environ_t resx_environ_t;

void resx_environ_init(resx_environ_t * const resxenvp);

void resx_head_init(resx_head_t * const resxheadp);

void resx_node_init(resx_node_t * const resxnodep);

#if defined(linux) || defined(__linux) || defined(__linux__)
int resx_environ_open(resx_environ_t * const resxenvp);
#else
int resx_environ_open(resx_environ_t * const resxenvp, const char *pathname);
#endif

void resx_environ_close(resx_environ_t * const resxenvp);

#ifndef resx_read
#define resx_read(buf, bufsiz, resxenvp)    resx_fstream_read((buf), 1U, (bufsiz), (resxenvp)->stream)
#endif

int32_t resx_environ_read(const resx_environ_t * const resxenvp,
        const char *pathname);

size_t strlcpy(char * const dst, const char * const src, size_t siz);

size_t strlcat(char * const dst, const char * const src, size_t siz);

size_t resx_fstream_read(void * const buf, const size_t siz,
        const size_t nitems, FILE * const stream);

size_t resx_fstream_write(const void * const buf, const size_t siz,
        const size_t nitems, FILE * const stream);

void resxhead_htobe(resx_head_t * const headp);

void resxhead_betoh(resx_head_t * const headp);

void resxnode_htobe(resx_node_t * const nodep);

void resxnode_betoh(resx_node_t * const nodep);

#ifdef __cplusplus
}
#endif

#endif /* RESOURCE_H_ */
