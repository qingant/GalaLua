/*
 * _ozip.h
 *
 *  Created on: 2013-5-16
 *      Author: ocaml
 */

#ifndef _OZIP__OZIP_H_
#define _OZIP__OZIP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zlib.h>
#include "io_api.h"
#include "otype.h"

typedef struct {
	uLong _magic;				/* the magic about the zf_stat */
    uLong valid;				/* which fields have valid values */
    const char *name;			/* name of the file */
    uLong index;				/* index within archive */
    uLong size;					/* size of file (uncompressed) */
    uLong comp_size;			/* size of file (compressed) */
    time_t mtime;				/* modification time */
    uLong crc;					/* crc of file data */
    uLong comp_method;			/* compression method used */
    uLong encryption_method;	/* encryption method used */
    uLong flags;				/* reserved for future use */
}ozip_stat;

extern zFile zf_archive_open(voidp path, ozip_filefunc* filefunc,uLong size,uLong limit);
extern int zf_archive_close(zFile zf);

extern char* zf_archive_get_comment(zFile file, char *name);
extern int zf_archive_set_comment(zFile file, char *name, char* commet);


extern uLong zf_archive_get_num_files(zFile file);
extern int zf_archive_locate_file(zFile file, const char* pathname, int icase, uLong* pos);
extern int zf_archive_stat_file(zFile file, const char* pathname);
extern File zf_archive_open_file(zFile file, const char* pathname,
		char* passwd, int* level, int* method, int raw, int icase);


extern int zf_file_close(File file);
extern int zf_file_read(oentry* file, char* buffer, uLong len);
extern int zf_file_get_comment(File file, char* name, char* buffer, int len);
extern int zf_file_stat(oentry* file, ozip_stat* stat);


#ifdef __cplusplus
}
#endif

#endif /* _OZIP__OZIP_H_ */
