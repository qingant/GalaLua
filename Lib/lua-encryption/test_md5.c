/*
 * test_md5.c
 *
 *  Created on: 2013-5-9
 *      Author: ocaml
 */

#include "md5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char HEX[16] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'a', 'b',
	'c', 'd', 'e', 'f'
};


char* MD5ToHex(unsigned char* input, size_t length) {
	size_t i;
	char *str = (char*)malloc((length<<1) + 1);
	memset(str, '\0', (length<<1) + 1);
	for ( i = 0; i < length; ++i) {
		int t = input[i];
		int a = t / 16;
		int b = t % 16;
		str[2*i] = HEX[a];
		str[2*i+1] = HEX[b];
//		printf("%c%c", HEX[a], HEX[b]);
	}
	return str;
}



char* sumhexa(const char* ctx ) {
	unsigned char buf[16+1];
	char* ptr;

	md5(ctx, strlen(ctx), buf);
	ptr = MD5ToHex(buf, 16);
	return ptr;
}

int main(int argc, char **argv) {

	assert(0 == strcmp(sumhexa(""), "d41d8cd98f00b204e9800998ecf8427e"));
	assert(0 == strcmp(sumhexa("a"), "0cc175b9c0f1b6a831c399e269772661"));
	assert(0 == strcmp(sumhexa("abc"), "900150983cd24fb0d6963f7d28e17f72"));

	printf("\n MD5 test OK\n");

	return 0;
}
