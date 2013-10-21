#include "codec.hpp"

#pragma once

class Utf8Codec : public Codec
{
public:
	static Codec* GetCodec()
	{
		static Codec *codec = NULL;
		if (codec == NULL)
		{
			codec = new Utf8Codec();
		}
		return codec;
	}
private:
	Utf8Codec()
	{
	}
public:
	virtual size_t encode(unicode_str uStr, size_t uLen, char *str, size_t sLen);
	virtual size_t decode(const char *str, size_t sLen, unicode_str uStr, size_t uLen);
private:
	inline int CPUTF8Decode(const char *utf8, ucs4_t *wch, int len = 6)
	{
		//���ַ���Ascii�����0xC0����Ҫ����жϣ����򣬾Ϳ϶��ǵ���ANSI�ַ���
		unsigned char firstCh = utf8[0];
		if (firstCh >= 0xC0)
		{
			//�������ַ��ĸ�λ�ж����Ǽ�����ĸ��UTF8����
			int afters, code;
			if ((firstCh & 0xE0) == 0xC0)
			{
				afters = 2;
				code = firstCh & 0x1F;
			}  else if ((firstCh & 0xF0) == 0xE0)
			{
				afters = 3;
				code = firstCh & 0xF;
			}  else if ((firstCh & 0xF8) == 0xF0)
			{
				afters = 4;
				code = firstCh & 0x7;
			}  else if ((firstCh & 0xFC) == 0xF8)
			{
				afters = 5;
				code = firstCh & 0x3;
			}  else if ((firstCh & 0xFE) == 0xFC)
			{
				afters = 6;
				code = firstCh & 0x1;
			}  else
			{
				*wch = firstCh;
				return 1;
			}
			if (afters > len)
			{
				return 0;
			}
			//֪�����ֽ�����֮�󣬻���Ҫ�����һ�£�������ʧ�ܣ��ͼ򵥵���Ϊ��UTF8���������⣬���߲���UTF8���룬���ǵ���һ��ANSI�����ش���
			for (int k = 1; k < afters; ++k)
			{
				if ((utf8[k] & 0xC0) != 0x80)
				{
					//�ж�ʧ�ܣ�������UTF8����Ĺ���ֱ�ӵ���һ��ANSI�ַ�����
					*wch = firstCh;
					return 1;
				}

				code <<= 6;
				code |= (unsigned char)utf8[k] & 0x3F;
			}

			*wch = code;
			return afters;
		}  else
		{
			*wch = firstCh;
		}

		return 1;
	}
	inline int CPUTF8Encode(ucs4_t wchar, char *utf8)
	{
		int len = 0;
		if (wchar < 0xC0)
		{
			utf8[len++] = (char)wchar;  
		}  else if (wchar < 0x800)
		{
			utf8[len++] = 0xc0 | (wchar >> 6);
			utf8[len++] = 0x80 | (wchar & 0x3f);
		}  else if (wchar < 0x10000)
		{
			utf8[len++] = 0xe0 | (wchar >> 12);
			utf8[len++] = 0x80 | ((wchar >> 6) & 0x3f);
			utf8[len++] = 0x80 | (wchar & 0x3f);
		}  else if (wchar < 0x200000)
		{
			utf8[len++] = 0xf0 | ((int)wchar >> 18);
			utf8[len++] = 0x80 | ((wchar >> 12) & 0x3f);
			utf8[len++] = 0x80 | ((wchar >> 6) & 0x3f);
			utf8[len++] = 0x80 | (wchar & 0x3f);
		}  else if (wchar < 0x4000000)
		{
			utf8[len++] = 0xf8 | ((int)wchar >> 24);
			utf8[len++] = 0x80 | ((wchar >> 18) & 0x3f);
			utf8[len++] = 0x80 | ((wchar >> 12) & 0x3f);
			utf8[len++] = 0x80 | ((wchar >> 6) & 0x3f);
			utf8[len++] = 0x80 | (wchar & 0x3f);
		}  else if (wchar < 0x80000000)
		{
			utf8[len++] = 0xfc | ((int)wchar >> 30);
			utf8[len++] = 0x80 | ((wchar >> 24) & 0x3f);
			utf8[len++] = 0x80 | ((wchar >> 18) & 0x3f);
			utf8[len++] = 0x80 | ((wchar >> 12) & 0x3f);
			utf8[len++] = 0x80 | ((wchar >> 6) & 0x3f);
			utf8[len++] = 0x80 | (wchar & 0x3f);
		}

		return len;
	}
};
