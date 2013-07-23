/*
 * strzip.h 操作string中zip的底层接口
 *
 *  Created on: 2013-3-24
 *      Author: ocaml
 *
 */

#ifndef _MEMFILE_H_
#define _MEMFILE_H_

#include <stdio.h>
#include <zlib.h>

/* memory file error code */
enum memfile_err {
    MEMFILE_MEM_ERR=0,
    MEMFILE_PARAM_ERR,
};


//描述string 中文件的存储结构
typedef struct _memfile
{
	void* addr;
	long pos; /*read offset*/
	long size;
	long limit;
	char err_msg[1024];
}MEMFILE, *PMEMFILE;

/* gc the memfile's source such as: addr */
typedef int (*GC)(voidpf strfile);

extern voidpf 	memfile_open(voidpf opaque, void* addr, int mode, uLong size, uLong limit);
extern uLong 	memfile_read(voidpf opaque, voidpf strfile, void* buffer, uLong size);
extern uLong 	memfile_write(voidpf opaque, voidpf strfile, const void* buffer, uLong size);
extern long 	memfile_tell(voidpf opaque, voidpf strfile);
extern long 	memfile_seek(voidpf opaque, voidpf strfile, uLong offset, int origin );
extern int 	memfile_close(voidpf opaque, voidpf strfile, GC gc );
extern int 	memfile_error(voidpf opaque, voidpf strfile );

#endif /*_MEMFILE_H_*/
