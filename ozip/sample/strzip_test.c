
/*
 * test_mini_strzip_api.c
 *
 *  Created on: 2013-5-6
 *      Author: ocaml
 */

/* 头文件顺序对程序的影响 */
#include "ozip.h"

#include <stdio.h>
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

#define filename "1.zip"
#define fname "ggg.txt"

int main() {
	char* buf;
	int iRead;
	char rcBuffer[1024];

	char* ctx="GGGGGGGGGGGGGGHHHKKKLLFFF";
	int size = 4096;
	int ret;
	char* ar_name;
	char* file_name;

//	FILE* file = fopen(filename, "r");
//	if(file == NULL){
//		printf("failure to open fopen\n");
//		exit(-1);
//	}
//	fseek(file, 0, SEEK_END);
//	unsigned long size = ftell(file);

	char* pctx = (char*)malloc(size+(unsigned long)1);
	memset(pctx, 0x00, size+1);
	if (NULL == pctx ) {
		fprintf(stderr, "failure to malloc\n");
//		fclose(file);
		return -1;
	}
	pctx[size] = '\0';
//	fseek(file,0, SEEK_SET);
//	iRead = fread(pctx, size, 1, file);
//	fprintf(stderr, "\nzip file start 0x%02x%02x%02x%2x\n",*(pctx+3),*(pctx+2), *(pctx+1), *(pctx));

	zipFile zf = ozip_strzipOpen(pctx, size, size+1, 0);
	if (NULL == zf) {
		fprintf(stderr, "\n failure to zf_archive_open \n");
		return -1;
	}

	int opt_compress_level =1;
    ret = ozip_zipOpenNewFileInZip4_64(zf, fname, NULL,
                     NULL,0,NULL,0,NULL /* comment*/,
                     (opt_compress_level != 0) ? Z_DEFLATED : 0,
                     opt_compress_level,0,
                     /* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
                     -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                     NULL,0,0,0,0);
    ret = ozip_zipWriteInFileInZip (zf, ctx, strlen(ctx));
    ret = ozip_zipWriteInFileInZip (zf, ctx, strlen(ctx));
    ret = ozip_zipWriteInFileInZip (zf, ctx, strlen(ctx));
    ret = ozip_zipWriteInFileInZip (zf, ctx, strlen(ctx));
    ret = ozip_zipCloseFileInZip (zf);
    ret = ozip_zipClose (zf, NULL);

    printf("\n\n=================================>\n\n");
    unzFile ar = ozip_strunzOpen(pctx, size, size+1);
    if(ar == NULL){
    	printf("failure to call line: %d\n", __LINE__);
    }
    ret = ozip_unzLocateFile(ar, fname, 0);
    if( ret != UNZ_OK){
    	printf("failure to call line: %d\n", __LINE__);
    }
    ret = ozip_unzOpenCurrentFile3(ar, NULL, NULL, 0, NULL);
    if( ret != UNZ_OK){
     	printf("failure to call line: %d\n", __LINE__);
     }


    memset(rcBuffer, 0x00, sizeof(rcBuffer));
    ret = ozip_unzReadCurrentFile(ar, rcBuffer, sizeof(rcBuffer));
	if (ret < 0) {
		printf("failure to call line: %d\n", __LINE__);
    }
    printf("%s\n", rcBuffer);
    ret = ozip_unzCloseCurrentFile(ar);
    if( ret != UNZ_OK){
     	printf("failure to call line: %d\n", __LINE__);
     }
    ret = ozip_unzClose(ar);
    if( ret != UNZ_OK){
     	printf("failure to call line: %d\n", __LINE__);
     }

	free(pctx);

	return 0;
}


