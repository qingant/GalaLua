/*
 * ioaip.h
 *
 *  Created on: 2013-3-15
 *      Author: ocaml
 */

#ifndef _OZIP_IO_API_H_
#define _OZIP_IO_API_H_

//#include "otype.h"
#include <zlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OZIP_SEEK_CUR (1)
#define OZIP_SEEK_END (2)
#define OZIP_SEEK_SET (0)

#define OZIP_MODE_READ      		(1)
#define OZIP_MODE_WRITE     		(2)
#define OZIP_MODE_READWRITEFILTER  (3)

#define OZIP_MODE_EXISTING 		(4)
#define OZIP_MODE_CREATE   		(8)


#ifndef ZCALLBACK

#if (defined(WIN32) || defined (WINDOWS) || defined (_WINDOWS)) && defined(CALLBACK) && defined (USEWINDOWS_CALLBACK)
#define ZCALLBACK CALLBACK
#else
#define ZCALLBACK
#endif
#endif


typedef voidpf	(ZCALLBACK *open_file_func) 		OF((voidpf opaque, const char* filename, int mode, uLong size, uLong limit));
typedef uLong	(ZCALLBACK *read_file_func) 		OF((voidpf opaque, voidpf stream, void* buf, uLong size));
typedef uLong	(ZCALLBACK *write_file_func) 		OF((voidpf opaque, voidpf stream, const void* buf, uLong size));
typedef long	(ZCALLBACK *tell_file_func) 		OF((voidpf opaque, voidpf stream));
typedef long	(ZCALLBACK *seek_file_func) 		OF((voidpf opaque, voidpf stream, uLong offset, int origin));
typedef int	(ZCALLBACK *close_file_func) 		OF((voidpf opaque, voidpf stream));
typedef int	(ZCALLBACK *testerror_file_func) 	OF((voidpf opaque, voidpf stream));

typedef struct ozip_filefunc_def_s {
	open_file_func		zopen_file;
	read_file_func		zread_file;
	write_file_func 	zwrite_file;
	tell_file_func 		ztell_file;
	seek_file_func 		zseek_file;
	close_file_func 	zclose_file;
	testerror_file_func zerror_file;
	voidpf 				opaque;
} ozip_filefunc;



extern void fill_fopen_filefunc(ozip_filefunc* filefunc);

#define ZREAD(filefunc,filestream,buf,size) ((*((filefunc).zread_file))((filefunc).opaque,filestream,buf,size))
#define ZWRITE(filefunc,filestream,buf,size) ((*((filefunc).zwrite_file))((filefunc).opaque,filestream,buf,size))
#define ZTELL(filefunc,filestream) ((*((filefunc).ztell_file))((filefunc).opaque,filestream))
#define ZSEEK(filefunc,filestream,pos,mode) ((*((filefunc).zseek_file))((filefunc).opaque,filestream,pos,mode))
#define ZCLOSE(filefunc,filestream) ((*((filefunc).zclose_file))((filefunc).opaque,filestream))
#define ZERROR(filefunc,filestream) ((*((filefunc).zerror_file))((filefunc).opaque,filestream))


#ifdef __cplusplus
}
#endif

#endif /* _OZIP_IOAPI_H_ */
