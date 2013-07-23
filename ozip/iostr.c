/*
 * iostr.c
 *
 *  Created on: 2013-3-24
 *      Author: ocaml
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>
#include "iostr.h"
#include "ioapi.h"
#include "memfile.h"



/* I've found an old Unix (a SunOS 4.1.3_U1) without all SEEK_* defined.... */
#ifndef SEEK_CUR
#define SEEK_CUR    1
#endif

#ifndef SEEK_END
#define SEEK_END    2
#endif

#ifndef SEEK_SET
#define SEEK_SET    0
#endif


//voidpf 	ZCALLBACK mem_fopen_file_func OF(( voidpf opaque, const char* filename, int mode, uLong size, uLong limit ));
static voidpf 	ZCALLBACK mem_fopen_file_func OF((voidpf opaque, const void* filename, int mode));
static uLong 	ZCALLBACK mem_fread_file_func OF(( voidpf opaque, voidpf stream, void* buf, uLong size ));
static uLong 	ZCALLBACK mem_fwrite_file_func OF(( voidpf opaque, voidpf stream, const void* buf, uLong size ));
static ZPOS64_T 	ZCALLBACK mem_ftell_file_func OF(( voidpf opaque,voidpf stream ));
static long 	ZCALLBACK mem_fseek_file_func OF(( voidpf opaque, voidpf stream, ZPOS64_T offset, int origin ));
static int 	ZCALLBACK mem_fclose_file_func OF(( voidpf opaque, voidpf stream ));
static int 	ZCALLBACK mem_ferror_file_func OF(( voidpf opaque, voidpf stream ));

//voidpf ZCALLBACK mem_fopen_file_func (voidpf opaque, const char*  filename, int mode, uLong size, uLong limit)
static voidpf 	ZCALLBACK mem_fopen_file_func OF((voidpf opaque, const void* addr, int mode))
{
	voidpf file=NULL;
	PMEMFILE mf = (PMEMFILE)addr;
	file = memfile_open(opaque, mf->addr, mode, mf->size, mf->limit);

    return file;
}


static uLong ZCALLBACK mem_fread_file_func (voidpf opaque, voidpf stream, void* buf, uLong size)
{
    uLong ret;
    ret = (uLong)memfile_read(opaque, stream, buf, size);
    return ret;
}

static uLong ZCALLBACK mem_fwrite_file_func (voidpf opaque, voidpf stream, const void* buf, uLong size)
{
    uLong ret;
    ret = (uLong)memfile_write(opaque, stream, buf, size);
    return ret;
}

static ZPOS64_T ZCALLBACK mem_ftell_file_func (voidpf opaque, voidpf stream)
{
    long ret;
    ret = memfile_tell(opaque,(FILE *)stream);
    return ret;
}

static long ZCALLBACK mem_fseek_file_func (voidpf opaque, voidpf stream, ZPOS64_T offset, int origin)
{
    int fseek_origin=0;
    long ret;
    switch (origin)
    {
    case ZLIB_FILEFUNC_SEEK_CUR :
        fseek_origin = SEEK_CUR;
        break;
    case ZLIB_FILEFUNC_SEEK_END :
        fseek_origin = SEEK_END;
        break;
    case ZLIB_FILEFUNC_SEEK_SET :
        fseek_origin = SEEK_SET;
        break;
    default: return -1;
    }
    ret = 0;
    if (memfile_seek(opaque, stream, offset, fseek_origin) != 0)
        ret = -1;
    return ret;
}

static int ZCALLBACK mem_fclose_file_func (voidpf opaque, voidpf stream)
{
    int ret;
    ret = memfile_close(opaque, (FILE *)stream, NULL);
    return ret;
}

static int ZCALLBACK mem_ferror_file_func (voidpf opaque, voidpf stream)
{
    int ret;
    ret = memfile_error(opaque, stream);
    return ret;
}

//void fill_fopen_filefunc (filefunc)
void fill_mem_filefunc (filefunc)
zlib_filefunc_def* filefunc;
{
//    filefunc->zopen_file = mem_fopen_file_func;
//    filefunc->zread_file = mem_fread_file_func;
//    filefunc->zwrite_file = mem_fwrite_file_func;
//    filefunc->ztell_file = mem_ftell_file_func;
//    filefunc->zseek_file = mem_fseek_file_func;
//    filefunc->zclose_file = mem_fclose_file_func;
//    filefunc->zerror_file = mem_ferror_file_func;
    filefunc->opaque = NULL;
}

void fill_mem64_filefunc OF((zlib_filefunc64_def* pzlib_filefunc_def)){
    pzlib_filefunc_def->zopen64_file = mem_fopen_file_func;
    pzlib_filefunc_def->zread_file = mem_fread_file_func;
    pzlib_filefunc_def->zwrite_file = mem_fwrite_file_func;
    pzlib_filefunc_def->ztell64_file = mem_ftell_file_func;
    pzlib_filefunc_def->zseek64_file = mem_fseek_file_func;
    pzlib_filefunc_def->zclose_file = mem_fclose_file_func;
    pzlib_filefunc_def->zerror_file = mem_ferror_file_func;
    pzlib_filefunc_def->opaque = NULL;
}
