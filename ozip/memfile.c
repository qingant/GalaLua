/*
 * memfile.c 操作string中zip的底层接口
 *
 *  Created on: 2013-3-24
 *      Author: ocaml
 *
 */

#include "memfile.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#ifndef SEEK_CUR
#define SEEK_CUR    1
#endif

#ifndef SEEK_END
#define SEEK_END    2
#endif

#ifndef SEEK_SET
#define SEEK_SET    0
#endif

/*
 *  opaque: the size of the addr
 */
voidpf memfile_open(voidpf opaque, void* addr, int mode, uLong size, uLong limit) {

//	fprintf(stderr, "size=%lu limit = %lu\n", size, limit);
//	fprintf(stderr, "\nzip file start 0x%02x%02x%02x%2x\n", *((char *)addr+3),
//			*((char *)addr+2), *((char *)addr+1), *((char *)addr));

	PMEMFILE memfile = (PMEMFILE)malloc(sizeof(MEMFILE));
	memfile->addr = addr;
	memfile->pos = 0;
	memfile->size = size;
	memfile->limit = limit;
	memset(memfile->err_msg, 0x00, sizeof(memfile->err_msg));

	return memfile;
}

uLong memfile_read(voidpf opaque, voidpf strfile, void* buffer, uLong size) {

	PMEMFILE memfile = (PMEMFILE)strfile;
	char *src;
//	if((memfile->pos+size)>memfile->size) {
//		return -1;
//	}
	/* 判断是否超出size的范围，超出以后应当返回OZIP_EOF */
	if ( memfile->pos == memfile->size) {
		return -1;
	}

//	fprintf(stderr, "\nzip file start 0x%02x%02x%02x%2x\n", *((char *)memfile->addr+3),
//			*((char *)memfile->addr+2), *((char *)memfile->addr+1), *((char *)memfile->addr));

	//没有足够的ctxt read
	if((memfile->pos+size) > memfile->size){
		size = memfile->size - memfile->pos;
	}
	src = (char*)memfile->addr+memfile->pos;
	memcpy(buffer, src, (size_t)size);
	memfile->pos = memfile->pos+size;

	return size;
}

uLong memfile_write(voidpf opaque, voidpf strfile, const void* buffer, uLong size) {

	PMEMFILE memfile = (PMEMFILE)strfile;
	char *dest = memfile->addr+memfile->pos;

	if ( memfile->pos > memfile->limit) {
		return -1;
	}

	if (memfile->pos+size > memfile->limit){
		size = memfile->limit - memfile->size;
	}

	memcpy(dest, buffer, size);
	memfile->pos = memfile->pos+size;

	return size;
}

long memfile_tell(voidpf opaque, voidpf strfile) {

	PMEMFILE memfile = (PMEMFILE)strfile;
//
//	fprintf(stderr, "\nzip file start 0x%02x%02x%02x%2x\n", *((char *)memfile->addr+3),
//			*((char *)memfile->addr+2), *((char *)memfile->addr+1), *((char *)memfile->addr));

	return memfile->pos;
}

/*
 * SEEK_CUR:设置内存文件的offset为 当前的pos+offset
 * SEEK_END:设置内存文件的offset为 文件的长度length+offset
 * SEEK_SET:设置内存文件的offset为 文件开始+offset
 *
 */
long memfile_seek(voidpf opaque, voidpf strfile, uLong offset, int origin) {

	PMEMFILE memfile = (PMEMFILE)strfile;

//	fprintf(stderr, "\n memfile_seek nzip file start 0x%02x%02x%02x%2x \n",
//			*((char *) memfile->addr + 3), *((char *) memfile->addr + 2),
//			*((char *) memfile->addr + 1), *((char *) memfile->addr));
//
//	fprintf(stderr, "size=%lu limit = %lu pos= %lu\n", memfile->size, memfile->limit, memfile->pos);

    switch (origin)
    {
    case SEEK_CUR :
        memfile->pos = memfile->pos + offset;
        break;
    case SEEK_END :
    	memfile->pos = memfile->size + offset;
        break;
    case SEEK_SET :
    	memfile->pos = offset;
        break;
    default: return -1;
    }

	return 0;
}

int memfile_close(voidpf opaque, voidpf strfile, GC gc) {

	PMEMFILE memfile = (PMEMFILE)strfile;
//
//	fprintf(stderr, "\nzip file start 0x%02x%02x%02x%2x\n", *((char *)memfile->addr+3),
//			*((char *)memfile->addr+2), *((char *)memfile->addr+1), *((char *)memfile->addr));

	if (NULL != gc ) {
		gc(memfile);
	}
	free(memfile);

	return 0;
}

int memfile_error(voidpf opaque, voidpf strfile) {

	PMEMFILE memfile = (PMEMFILE)strfile;
	fprintf(stderr, "%s", memfile->err_msg);
	if(NULL != memfile->err_msg) {
		return 1;
	}

	return 0;
}
