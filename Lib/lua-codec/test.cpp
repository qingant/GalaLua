#include <string>
#include <iostream>
#include "codec.hpp"
#include "codec_cn.hpp"
#include "codec_utf8.hpp"
#include <cstdio>
void Print(ucs4_t *str, size_t len)
{
	printf("USC4:\n");
	for (size_t i = 0; i != len;++i) 
	{
		printf("%X ", str[i]);
	}
	printf("\n");
}
int main()
{

	ucs4_t uStr[10] = { };
	ucs4_t outU[10] = { };
	uStr[0] = 0x597d; 
	uStr[1] = 0x5b69;
	uStr[2] = 0x5b50;
	uStr[3] = 'c';
	uStr[4] = 0x597d;
 	Codec *codec = Gb2312Codec::GetCodec();
	Codec *utf8 = Utf8Codec::GetCodec();
	char rrr[20] = { };

	size_t len = utf8->encode(uStr, 5, rrr, sizeof(rrr));
    printf("utf8->encode:%d\n",len);
	printf("%s\n", rrr);

	size_t o_len=utf8->decode(rrr, len, outU, sizeof(outU));
    printf("o_len:%d  \n",o_len);
	Print(outU, 5);

	char result[20] = { }; 
    size_t len1 = codec->encode(uStr, 5, result, sizeof(result));
    printf("len1:%d\n", len1);
	printf("CXXXXXXXX:%d\n", sizeof(outU));
	size_t outlen = codec->decode(result, len1, outU, sizeof(outU));
	Print(outU, outlen);

	printf("%X\n", outU[0]);
	printf("%s\n", result);
	return 0;
}
