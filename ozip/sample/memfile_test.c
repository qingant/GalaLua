/*
 * memfile_test.c
 *
 *  Created on: 2013-4-23
 *      Author: ocaml
 */

#include <stdio.h>
#include "memfile.h"
#include <string.h>
#include <stdlib.h>

int gc(voidpf strfile) {

	PMEMFILE memfile = (PMEMFILE)strfile;
	if (NULL != memfile->addr) {
		//free(memfile->addr);
	}
	if (NULL != memfile->addr) {
		free(memfile);
	}

	return 0;
}

int main(int argc, char **argv) {

	char block[2048]="12233344444aaabbbccc";
	char buffer[256];
	memset(buffer, 0x00, sizeof(buffer));

	PMEMFILE memfile = memfile_open(NULL, block, sizeof(block));
	fprintf(stderr, "%ld\n", memfile_tell(NULL, memfile));

	memfile_read(NULL, memfile, buffer, 20);
	fprintf(stderr, "%ld\n", memfile_tell(NULL, memfile));

	fprintf(stderr, "%ld\n", memfile_seek(NULL, memfile, 10, SEEK_SET));
	memfile_read(NULL, memfile, buffer, 1);
	fprintf(stderr, "%ld\n", memfile_tell(NULL, memfile));

	memfile_close(NULL, memfile, gc);
	fprintf(stderr, "%s\n", buffer);

	return 0;
}
