/*
 * test_mini_strzip_api.c
 *
 *  Created on: 2013-5-6
 *      Author: ocaml
 */



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

#include "_ozip.h"


#define filename "./haha.zip"

int main(int argc, char **argv) {
	ozip_stat stat;
	char* buf;
	int iRead;

	FILE* file = fopen(filename, "r");
	fseek(file,0, SEEK_END);
	unsigned long size = ftell(file);
//	fprintf(stderr, "size=%lu limit = %lu\n", size, size+1);

	char* pctx = (char*)malloc(size+(unsigned long)1);
	memset(pctx, 0x00, size+1);
	if (NULL == pctx ) {
		fprintf(stderr, "failure to malloc\n");
		fclose(file);
		return -1;
	}
	pctx[size] = '\0';
	fseek(file,0, SEEK_SET);
//	fprintf(stderr, "offset = %lu size = %lu\n", ftell(file), size);
	iRead = fread(pctx, size, 1, file);
//	if ((ret = fread(pctx, size, 1, file)) != (size_t)size) {
//		fprintf(stderr, "bad zip file start : %s (ret = %lu) != (size = %lu) \n", strerror(errno), ret, size);
//		fclose(file);
//	}

//	fprintf(stderr, "\nzip file start 0x%02x%02x%02x%2x\n",*(pctx+3),*(pctx+2), *(pctx+1), *(pctx));

	voidpf zf = zf_archive_open(pctx, NULL, size, size+1);
	if (NULL == zf) {
		fprintf(stderr, "\n failure to zf_archive_open \n");
		return -1;
	}

//	fprintf(stderr, "\nstart zf_archive_open_file\n");
	oentry* entry = zf_archive_open_file(zf, "README.txt",NULL,NULL,NULL,0,0);
	if (NULL == entry) {
		fprintf(stderr, "\n failure to zf_archive_open_file \n");
		return -1;
	}

	zf_file_stat(entry, &stat);
	buf = (char*)malloc(stat.size+1);
	memset(buf,'\0', stat.size+1);
	printf("comp size =%ld uncomp size = %ld\n", entry->head.comp_size, entry->head.uncomp_size);

	iRead = zf_file_read(entry, buf, stat.size);
	if (iRead != stat.size) {
		printf(" Error README.txt size != %lu read from README.txt %d \n", stat.size, iRead);
	}

	printf("README.txt size = %lu read from README.txt context = %s\n", stat.size, buf);
	zf_file_close(entry);


	zf_archive_close(zf);


	fclose(file);
	free(pctx);

	return 0;
}

