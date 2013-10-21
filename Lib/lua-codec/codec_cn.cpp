#include "codec_cn.hpp"
/**
 * 
 * 
 * @author matao (2013/10/19)
 * 
 * @param uStr unicode str
 * @param uLen unicode str length
 * @param str output byte string
 * @param sLen max output size
 * 
 * @return size_t output length
 */
size_t Gb2312Codec::encode(unicode_str uStr, size_t uLen, char *str, size_t sLen)
{
	size_t cursor = 0;
	for (size_t i = 0; i != uLen; ++i)
	{

		if (cursor + 2 < sLen)
		{
			if (uStr[i] < 0x80)
			{
				str[cursor] = uStr[i];
				++cursor;
				continue;
			}
			CPGBKEncode(uStr[i], (DBCHAR *)(str + cursor));
			cursor += 2;
		} else
		{
			break;
		}
	}
	return cursor;
}
/**
 * 
 * 
 * @author matao (2013/10/19)
 * 
 * @param str encoded byte string
 * @param sLen encoded byte string length
 * @param uStr output unicode string
 * @param uLen output max length
 * 
 * @return size_t output length
 */
size_t Gb2312Codec::decode(const char *str, size_t sLen, unicode_str uStr, size_t uLen)
{
	//const DBCHAR *gStr = (const DBCHAR *)str;
	size_t uCursor = 0;
	for (size_t i = 0; i < sLen;)
	{

		if (uCursor <= uLen)
		{
			if ((unsigned char)str[i] < 0x80)
			{
				uStr[uCursor] = str[i];
				++i;
			} else
			{
				CPGBKDecode(str + i, uStr + uCursor);
				i += 2;
			}
			++uCursor;
		} else
		{
			break;
		}
	}
	return uCursor;
}
void Gb2312Codec::CPGBKEncode(ucs4_t c, DBCHAR *s)
{
	DBCHAR chr;
	if (c == 0x2014)
	{
		chr = 0xa1aa;

	} else if (c == 0x2015)
	{
		chr = 0xa844;
	} else if (c == 0x00b7)
	{
		chr = 0xa1a4;
	} else if (c != 0x30fb)
	{
		//TRYMAP_ENC_COND(gbcommon, assi, code));
		const struct unim_index *idx = &gbcommon_encmap[c >> 8];
		ucs4_t val = c & 0xff;
		if (idx->map != NULL && val >= idx->bottom && val <= idx->top)
		{
			chr = idx->map[val - idx->bottom];
			if (chr == NOCHAR)
			{
				//error handling
			}
		}
	}
	char *const out = (char *const)s;
	out[0] = (chr >> 8) | 0x80;
	out[1] = (chr & 0xFF) | 0x80;

}

void Gb2312Codec::CPGBKDecode(const char *gChar, ucs4_t *u)
{
	unsigned char dc1 = gChar[0];
	unsigned char dc2 = gChar[1];
	if ((dc1) == 0xa1 && (dc2) == 0xaa)
	{
		(*u) = 0x2014;
	} else if ((dc1) == 0xa8 && (dc2) == 0x44)
	{
		(*u) = 0x2015;
	} else if ((dc1) == 0xa1 && (dc2) == 0xa4)
	{
		(*u) = 0x00b7;
	} else
	{
		unsigned char c1 = dc1 ^ 0x80;
		unsigned char c2 = dc2 ^ 0x80;
		const struct dbcs_index *index = &gb2312_decmap[c1];

		if (index->map != NULL && c2 >= index->bottom && c2 <= index->top)
		{
			(*u) = index->map[c2 - index->bottom];
		}
		if (*u == UNIINV)
		{
			index = &gbkext_decmap[c1];
			(*u) = index->map[c2 - index->bottom];
		}
	}
}
