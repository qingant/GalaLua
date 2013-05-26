/*
 * otype.h
 *
 *  Created on: 2013-4-17
 *      Author: ocaml
 */

#ifndef _OZIP_OTYPE_H_
#define _OZIP_OTYPE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <zlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef STDC
#  include <stddef.h>
#  include <string.h>
#  include <stdlib.h>
#endif
#ifdef NO_ERRNO_H
    extern int errno;
#else
#   include <errno.h>
#endif

#ifdef HAVE_BZIP2
#include "bzlib.h"
#endif

#define Z_BZIP2ED 12

#if defined(STRICTUNZIP) || defined(STRICTZIPUNZIP)
/* like the STRICT of WIN32, we define a pointer that cannot be converted
 * from (void*) without cast
 */
typedef struct TagzFile__ { int used; } zFile__;
typedef zFile__ *zFile;
typedef zFile__ *File;
#else
typedef voidp zFile;
typedef voidp File;
#endif


#ifndef CASESENSITIVITYDEFAULT_NO
#  if !defined(unix) && !defined(CASESENSITIVITYDEFAULT_YES)
#    define CASESENSITIVITYDEFAULT_NO
#  endif
#endif


#ifndef OZIP_BUFSIZE
#define OZIP_BUFSIZE (16384)
#endif

#ifndef MAXFILENAMEINZIP
#define MAXFILENAMEINZIP (256)
#endif

#ifndef ALLOC
# define ALLOC(size) (malloc(size))
#endif
#ifndef TRYFREE
# define TRYFREE(p) {if (p) free(p);}
#endif

#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)

enum OZIPRet {
	OZIP_OK=0,
	OZIP_END_OF_LIST_OF_FILE = 100,
	OZIP_ERRNO = Z_ERRNO,
	OZIP_EOF = 0,
	OZIP_PARAMERROR = -102,
	OZIP_BADZIPFILE = -103,
	OZIP_INTERNALERROR = -104,
	OZIP_CRCERROR = -105
};

typedef struct {
    /* local file header signature     4 bytes  (0x04034b50) */
    uLong version_needed;			/* version needed to extract       2 bytes */
    uLong flag;						/* general purpose bit flag        2 bytes */
    uLong compression_method;		/* compression method              2 bytes */
    uLong dos_date;					/* last mod file time & date       2 bytes */
    uLong crc32;					/* crc-32                          4 bytes */
    uLong comressed_size;			/* compressed size                 4 bytes */
    uLong uncompressed_size;		/* uncompressed size               4 bytes */
    uLong file_name_length;			/* file name length                2 bytes */
    uLong extra_field_length;		/* extra field length              2 bytes */

    /* file name (variable size) */
    /* extra field (variable size) */
}LocalFileHead;

typedef struct {
    uLong crc32;					/* crc-32                          4 bytes */
    uLong compressed_size;			/* compressed size                 4 bytes */
    uLong uncompressed_size;		/* uncompressed size               4 bytes */
}FileDataDescript;

typedef struct {
    /* central file header signature   4 bytes  (0x02014b50) */
    uLong version;					/* version made by                 2 bytes */
    uLong version_needed;			/* version needed to extract       2 bytes */
    uLong flag;						/* general purpose bit flag        2 bytes */
    uLong comp_method;				/* compression method              2 bytes */
    uLong dos_date;					/* last mod file time & date       4 bytes */
    uLong crc32;					/* crc-32                          4 bytes */
    uLong comp_size;				/* compressed size                 4 bytes */
    uLong uncomp_size;				/* uncompressed size               4 bytes */
    uLong file_name_len;			/* file name length                2 bytes */
    uLong extra_field_len;			/* extra field length              2 bytes */
    uLong comment_len;				/* file comment length             2 bytes */
    uLong disk_number_start;		/* disk number start               2 bytes */
    uLong inter_attr;				/* internal file attributes        2 bytes */
    uLong exter_attr;				/* external file attributes        4 bytes */
    uLong offset_entry;				/* relative offset of local header 4 bytes */

    /* file name (variable size) */
    /* extra field (variable size) */
    /* file comment (variable size) */
}CentralFileHead;

typedef struct {
    /* end of central dir signature    4 bytes  (0x06054b50) */
    uLong num_disk;					/* number of this disk              2 bytes */
    uLong num_disk_with_CD;			/* number of the disk with the
    									start of the central directory  2 bytes */
    uLong sum_entries;				/* total number of entries in the
    									central directory on this disk  2 bytes */
    uLong sum_entries_in_CD;	    /* total number of entries in
    									the central directory           2 bytes */
    uLong size_central_dir;			/* size of the central directory    4 bytes */
    uLong offset_central_dir;		/* offset of start of central
    									directory with respect to
    									the starting disk number        4 bytes */
    uLong comment_len;				/* .zip file comment length         2 bytes */
    /* .zip file comment       (variable size) */
}EndOfCD;

//struct ozip_s;
//typedef ozip_s ozip;
typedef struct ozip_s{
	ozip_filefunc z_filefunc;
	voidpf filestream;
	EndOfCD eof_cd;

	uLong zip_start_pos;
	uLong central_direct_pos;

	int encrypted;
#    ifndef NOUNCRYPT
    unsigned long keys[3];/* keys defining the pseudo-random sequence */
    const unsigned long* pcrc_32_tab;
#    endif
}ozip;

typedef struct {
	char  *read_buffer;         	/* internal buffer for compressed data */
	z_stream stream;
#ifdef HAVE_BZIP2
    bz_stream bstream;          	/* bzLib stream structure for bziped */
#endif
    uLong pos_in_zipfile;       	/* position in byte on the zipfile, for fseek */
    uLong stream_initialised;   	/* flag set if stream structure is initialised */
    uLong sum_crc32;           		/* crc32 we must obtain after decompress all */

    uLong offset_local_extrafield;
    uLong size_local_extrafield;
    uLong pos_local_extrafield;

	int raw;

	CentralFileHead head;
	ozip* zip;
}oentry;


#ifdef __cplusplus
}
#endif

#endif /* _OZIP_OTYPE_H_ */
