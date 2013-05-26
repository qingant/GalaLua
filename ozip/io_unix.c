/*
 * io_unix.c
 *
 *  Created on: 2013-5-15
 *      Author: ocaml
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zlib.h"
#include "io_api.h"



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

voidpf ZCALLBACK fopen_file_func OF((
   voidpf opaque,
   const char* filename,
   int mode,
   uLong size,
   uLong limit));

uLong ZCALLBACK fread_file_func OF((
   voidpf opaque,
   voidpf stream,
   void* buf,
   uLong size));

uLong ZCALLBACK fwrite_file_func OF((
   voidpf opaque,
   voidpf stream,
   const void* buf,
   uLong size));

long ZCALLBACK ftell_file_func OF((
   voidpf opaque,
   voidpf stream));

long ZCALLBACK fseek_file_func OF((
   voidpf opaque,
   voidpf stream,
   uLong offset,
   int origin));

int ZCALLBACK fclose_file_func OF((
   voidpf opaque,
   voidpf stream));

int ZCALLBACK ferror_file_func OF((
   voidpf opaque,
   voidpf stream));


voidpf ZCALLBACK fopen_file_func (opaque, filename, mode, size, limit)
   voidpf opaque;
   const char* filename;
   int mode;
   uLong size;
   uLong limit;
{
    FILE* file = NULL;
    const char* mode_fopen = NULL;
    if ((mode & OZIP_MODE_READWRITEFILTER)==OZIP_MODE_READ)
        mode_fopen = "rb";
    else
    if (mode & OZIP_MODE_EXISTING)
        mode_fopen = "r+b";
    else
    if (mode & OZIP_MODE_CREATE)
        mode_fopen = "wb";

    if ((filename!=NULL) && (mode_fopen != NULL))
        file = fopen(filename, mode_fopen);
    return file;
}


uLong ZCALLBACK fread_file_func (opaque, stream, buf, size)
   voidpf opaque;
   voidpf stream;
   void* buf;
   uLong size;
{
    uLong ret;
    ret = (uLong)fread(buf, 1, (size_t)size, (FILE *)stream);
    return ret;
}


uLong ZCALLBACK fwrite_file_func (opaque, stream, buf, size)
   voidpf opaque;
   voidpf stream;
   const void* buf;
   uLong size;
{
    uLong ret;
    ret = (uLong)fwrite(buf, 1, (size_t)size, (FILE *)stream);
    return ret;
}

long ZCALLBACK ftell_file_func (opaque, stream)
   voidpf opaque;
   voidpf stream;
{
    long ret;
    ret = ftell((FILE *)stream);
    return ret;
}

long ZCALLBACK fseek_file_func (opaque, stream, offset, origin)
   voidpf opaque;
   voidpf stream;
   uLong offset;
   int origin;
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
    if (fseek((FILE *)stream, offset, fseek_origin) != 0)
        ret = -1;
    return ret;
}

int ZCALLBACK fclose_file_func (opaque, stream)
   voidpf opaque;
   voidpf stream;
{
    int ret;
    ret = fclose((FILE *)stream);
    return ret;
}

int ZCALLBACK ferror_file_func (opaque, stream)
   voidpf opaque;
   voidpf stream;
{
    int ret;
    ret = ferror((FILE *)stream);
    return ret;
}

void fill_fopen_filefunc (filefuncf)
  ozip_filefunc* filefuncf;
{
    filefuncf->zopen_file = fopen_file_func;
    filefuncf->zread_file = fread_file_func;
    filefuncf->zwrite_file = fwrite_file_func;
    filefuncf->ztell_file = ftell_file_func;
    filefuncf->zseek_file = fseek_file_func;
    filefuncf->zclose_file = fclose_file_func;
    filefuncf->zerror_file = ferror_file_func;
    filefuncf->opaque = NULL;
}
