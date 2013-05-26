/*
 * str_api.c
 *
 *  Created on: 2013-3-24
 *      Author: ocaml
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>
#include "io_api.h"
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

voidpf 	ZCALLBACK fopen_file_func OF(( voidpf opaque, const char* filename, int mode, uLong size, uLong limit ));
uLong 	ZCALLBACK fread_file_func OF(( voidpf opaque, voidpf stream, void* buf, uLong size ));
uLong 	ZCALLBACK fwrite_file_func OF(( voidpf opaque, voidpf stream, const void* buf, uLong size ));
long 	ZCALLBACK ftell_file_func OF(( voidpf opaque,voidpf stream ));
long 	ZCALLBACK fseek_file_func OF(( voidpf opaque, voidpf stream, uLong offset, int origin ));
int 	ZCALLBACK fclose_file_func OF(( voidpf opaque, voidpf stream ));
int 	ZCALLBACK ferror_file_func OF(( voidpf opaque, voidpf stream ));


voidpf ZCALLBACK fopen_file_func (voidpf opaque, const char*  filename, int mode, uLong size, uLong limit)
{
	voidpf file;
	file = memfile_open(opaque, (void*)filename, mode, size, limit);
    return file;
}


uLong ZCALLBACK fread_file_func (voidpf opaque, voidpf stream, void* buf, uLong size)
{
    uLong ret;
    ret = (uLong)memfile_read(opaque, stream, buf, size);
    return ret;
}

uLong ZCALLBACK fwrite_file_func (voidpf opaque, voidpf stream, const void* buf, uLong size)
{
    uLong ret;
    ret = (uLong)memfile_write(opaque, stream, buf, size);
    return ret;
}

long ZCALLBACK ftell_file_func (voidpf opaque, voidpf stream)
{
    long ret;
    ret = memfile_tell(opaque,(FILE *)stream);
    return ret;
}

long ZCALLBACK fseek_file_func (voidpf opaque, voidpf stream, uLong offset, int origin)
{
    int fseek_origin=0;
    long ret;
    switch (origin)
    {
    case OZIP_SEEK_CUR :
        fseek_origin = SEEK_CUR;
        break;
    case OZIP_SEEK_END :
        fseek_origin = SEEK_END;
        break;
    case OZIP_SEEK_SET :
        fseek_origin = SEEK_SET;
        break;
    default: return -1;
    }
    ret = 0;
    if (memfile_seek(opaque, stream, offset, fseek_origin) != 0)
        ret = -1;
    return ret;
}

int ZCALLBACK fclose_file_func (voidpf opaque, voidpf stream)
{
    int ret;
    ret = memfile_close(opaque, (FILE *)stream, NULL);
    return ret;
}

int ZCALLBACK ferror_file_func (voidpf opaque, voidpf stream)
{
    int ret;
    ret = memfile_error(opaque, stream);
    return ret;
}

void fill_fopen_filefunc (filefunc)
  ozip_filefunc* filefunc;
{
    filefunc->zopen_file = fopen_file_func;
    filefunc->zread_file = fread_file_func;
    filefunc->zwrite_file = fwrite_file_func;
    filefunc->ztell_file = ftell_file_func;
    filefunc->zseek_file = fseek_file_func;
    filefunc->zclose_file = fclose_file_func;
    filefunc->zerror_file = ferror_file_func;
    filefunc->opaque = NULL;
}
