/*
 * ozip.h
 *
 *  Created on: 2013-7-18
 *      Author: ocaml
 */

#ifndef OZIP_H_
#define OZIP_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32
        #ifndef __USE_FILE_OFFSET64
                #define __USE_FILE_OFFSET64
        #endif
        #ifndef __USE_LARGEFILE64
                #define __USE_LARGEFILE64
        #endif
        #ifndef _LARGEFILE64_SOURCE
                #define _LARGEFILE64_SOURCE
        #endif
        #ifndef _FILE_OFFSET_BIT
                #define _FILE_OFFSET_BIT 64
        #endif
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#ifdef unix
# include <unistd.h>
# include <utime.h>
#else
# include <direct.h>
# include <io.h>
#endif

#include "zip.h"
#include "unzip.h"

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)

#ifdef _WIN32
#define USEWIN32IOAPI
#include "iowin32.h"
#endif

/* fs zip API */
extern zipFile ozip_zipOpen64 (const void *pathname, int append);
extern zipFile ozip_strzipOpen(void* addr, int size, int limit, int append);

extern int ozip_zipOpenNewFileInZip4_64 (
	zipFile file,
	const char* filename,
	const zip_fileinfo* zipfi,
	const void* extrafield_local,  uint32_t size_extrafield_local,
	const void* extrafield_global, uint32_t size_extrafield_global,
	const char* comment,
	int method,
	int level,
	int raw,
	int windowBits,
	int memLevel,
	int strategy,
	const char* password,
	uint32_t crcForCrypting,
	uint32_t versionMadeBy,
	uint32_t flagBase,
	int zip64);

extern int ozip_zipWriteInFileInZip (zipFile file, const void* buf, unsigned len);
extern int ozip_zipCloseFileInZip (zipFile file);
extern int ozip_zipCloseFileInZipRaw64 (zipFile file, uint64_t uncompressed_size, uint32_t crc32);
extern int ozip_zipRemoveExtraInfoBlock (char* pData, int* dataLen, short sHeader);
extern int ozip_zipClose (zipFile file, const char* global_comment);

/* unzip API */
extern int ozip_unzStringFileNameCompare (const char* fileName1, const char* fileName2, int iCaseSensitivity);
extern zipFile ozip_strunzOpen( void* addr, int size, int limit);
extern unzFile ozip_unzOpen64 (const void *path);
extern int ozip_unzClose (unzFile file);
extern int ozip_unzGetGlobalInfo64 (unzFile file, unz_global_info64 *pglobal_info);
extern int ozip_unzGetGlobalComment (unzFile file, char *szComment, uint32_t uSizeBuf);
extern int ozip_unzGoToFirstFile (unzFile file);
extern int ozip_unzGoToNextFile (unzFile file);
extern int ozip_unzLocateFile (unzFile file, const char *szFileName, int iCaseSensitivity);

extern int ozip_unzGetFilePos64(unzFile file, unz64_file_pos* file_pos);
extern int ozip_unzGoToFilePos64(unzFile file, const unz64_file_pos* file_pos);
extern int ozip_unzGetCurrentFileInfo64 (unzFile file, unz_file_info64 *pfile_info, char *szFileName, uint32_t fileNameBufferSize, void *extraField, uint32_t extraFieldBufferSize, char *szComment, uint32_t commentBufferSize);
extern uint64_t ozip_unzGetCurrentFileZStreamPos64 (unzFile file);
extern int ozip_unzOpenCurrentFile3 (unzFile file, int* method, int* level, int raw, const char* password);
extern int ozip_unzCloseCurrentFile (unzFile file);
extern int ozip_unzReadCurrentFile (unzFile file, void* buf, unsigned len);
extern uint64_t ozip_unztell64 (unzFile file);
extern int ozip_unzeof (unzFile file);
extern int ozip_unzGetLocalExtrafield (unzFile file, void* buf, unsigned len);
extern uint64_t ozip_unzGetOffset64 (unzFile file);
extern int ozip_unzSetOffset64 (unzFile file, uint64_t pos);

#ifdef __cplusplus
}
#endif

#endif /* OZIP_H_ */
