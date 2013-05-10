/*
 * test_hex.c
 *
 *  Created on: 2013-5-10
 *      Author: ocaml
 */

#include <stdio.h>

char HEX[16] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'a', 'b',
	'c', 'd', 'e', 'f'
};


char* MD5ToHex(unsigned char* input, size_t length) {
	size_t i;
	char *str = (char*)malloc(length<<1);
	memset(str, '\0', length<<1);
	for ( i = 0; i < length; ++i) {
		int t = input[i];
		int a = t / 16;
		int b = t % 16;
		str[2*i] = HEX[a];
		str[2*i+1] = HEX[b];
		printf("- %c%c -", HEX[a], HEX[b]);
	}

	printf("\nadfadskfjlas ==== i = %lu\n", i);
	return str;
}

int main(int argc, char **argv) {
	unsigned char buf[100];
	buf[0]=0xd0;
	buf[1]=0xe1;
	buf[2]=0xf2;
	MD5ToHex(buf, (size_t)3);
	return 0;
}


