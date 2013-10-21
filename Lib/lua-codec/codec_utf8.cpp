#include "codec_utf8.hpp"

size_t Utf8Codec::encode(unicode_str uStr, size_t uLen, char *str, size_t sLen)
{
	size_t cursor = 0;
	for (size_t i = 0; i != uLen;++i) 
	{
		if (sLen - cursor < 6)
		{
			break;
		} else
		{
			int len = CPUTF8Encode(uStr[i], str+cursor);
			cursor += len;
		}
	}
	return cursor;
}
size_t Utf8Codec::decode(const char *str, size_t sLen, unicode_str uStr, size_t uLen)
{
	size_t sCursor = 0;
	size_t uCursor = 0;
	while (uCursor != uLen && sCursor != sLen)
	{
		int len = CPUTF8Decode(str+sCursor, uStr + uCursor, sLen - sCursor);
		if (len != 0)
		{
			sCursor += len;
			++uCursor;
		} else
		{
			break;
		}
	}
	return uCursor;
}
