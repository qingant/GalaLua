/*
 * ozip.c
 *
 *  Created on: 2013-7-14
 *      Author: ocaml
 */

#ifndef OZIP_C_
#define OZIP_C_

#include "ozip.h"
#include <stdio.h>

/* fs zip API */
zipFile ozip_zipOpen64 (const void *pathname, int append){
	return zipOpen64 (pathname, append);
}

//extern zipFile ozip_strzipOpen(void* addr, int size, int limit, int append);
zipFile ozip_strzipOpen(void* addr, int size, int limit, int append){
	return strzipOpen(addr, size, limit, append);
}

int ozip_zipOpenNewFileInZip4_64 (
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
	int zip64){
	return zipOpenNewFileInZip4_64 (file,filename,zipfi,extrafield_local,size_extrafield_local,
		extrafield_global, size_extrafield_global,comment,method,level,raw,windowBits,
		memLevel,strategy,password,crcForCrypting,versionMadeBy,flagBase,zip64);
}

int ozip_zipWriteInFileInZip (zipFile file, const void* buf, unsigned len){
	return zipWriteInFileInZip(file, buf, len);
}
int ozip_zipCloseFileInZip (zipFile file){
	return zipCloseFileInZip(file);
}
int ozip_zipCloseFileInZipRaw64 (zipFile file, uint64_t uncompressed_size, uint32_t crc32){
	return zipCloseFileInZipRaw64 (file, uncompressed_size, crc32);
}
int ozip_zipClose (zipFile file, const char* global_comment){
	return zipClose(file, global_comment);
}

int ozip_zipRemoveExtraInfoBlock (char* pData, int* dataLen, short sHeader){
	return zipRemoveExtraInfoBlock( pData, dataLen, sHeader);
}

/* unzip API */
int ozip_unzStringFileNameCompare (const char* fileName1, const char* fileName2, int iCaseSensitivity){
	return unzStringFileNameCompare( fileName1, fileName2, iCaseSensitivity);
}

unzFile ozip_strunzOpen( void* addr, int size, int limit){
	return strunzOpen(addr, size, limit);
}

unzFile ozip_unzOpen64 (const void *path) {
	return unzOpen64 (path);
}

int ozip_unzClose (unzFile file){
	return unzClose (file);
}

int ozip_unzGetGlobalInfo64 (unzFile file, unz_global_info64 *pglobal_info){
	return unzGetGlobalInfo64 (file, pglobal_info);
}

int ozip_unzGetGlobalComment (unzFile file, char *szComment, uint32_t uSizeBuf){
	return unzGetGlobalComment ( file,szComment, uSizeBuf);
}

int ozip_unzGoToFirstFile (unzFile file) {
	return unzGoToFirstFile (file);
}

int ozip_unzGoToNextFile (unzFile file){
	return unzGoToNextFile (file);
}

int ozip_unzLocateFile (unzFile file, const char *szFileName, int iCaseSensitivity){
	return unzLocateFile (file, szFileName, iCaseSensitivity);
}

int ozip_unzGetFilePos64(unzFile file, unz64_file_pos* file_pos){
	return unzGetFilePos64( file,  file_pos);
}

int ozip_unzGoToFilePos64(unzFile file, const unz64_file_pos* file_pos){
	return unzGoToFilePos64( file, file_pos);
}

int ozip_unzGetCurrentFileInfo64 (unzFile file, unz_file_info64 *pfile_info, char *szFileName, uint32_t fileNameBufferSize,
		void *extraField, uint32_t extraFieldBufferSize, char *szComment, uint32_t commentBufferSize){
	return unzGetCurrentFileInfo64 (file, pfile_info, szFileName, fileNameBufferSize,
			extraField, extraFieldBufferSize, szComment, commentBufferSize);
}

uint64_t ozip_unzGetCurrentFileZStreamPos64 (unzFile file){
	return unzGetCurrentFileZStreamPos64 (file);
}

int ozip_unzOpenCurrentFile3 (unzFile file, int* method, int* level, int raw, const char* password){
	return unzOpenCurrentFile3 (file,method,level,raw,password);
}

int ozip_unzCloseCurrentFile (unzFile file){
	return unzCloseCurrentFile (file);
}

int ozip_unzReadCurrentFile (unzFile file, void* buf, unsigned len){
	return unzReadCurrentFile (file,buf,len);
}

uint64_t ozip_unztell64 (unzFile file){
	return unztell64 (file);
}

int ozip_unzeof (unzFile file){
	return unzeof (file);
}

int ozip_unzGetLocalExtrafield (unzFile file, void* buf, unsigned len){
	return unzGetLocalExtrafield (file, buf, len);
}

uint64_t ozip_unzGetOffset64 (unzFile file){
	return unzGetOffset64 (file);
}

int ozip_unzSetOffset64 (unzFile file, uint64_t pos){
	return unzSetOffset64 (file, pos);
}

#endif /* OZIP_C_ */

