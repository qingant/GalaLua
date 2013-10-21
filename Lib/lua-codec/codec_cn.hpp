#include "codec.hpp"
#include "gb_mappings.h"

class Gb2312Codec : public Codec
{
public:
	static Codec* GetCodec()
	{
		static Codec *codec = NULL;
		if (codec == NULL)
		{
			codec = new Gb2312Codec;
		}
		return codec;
	}
private:
	Gb2312Codec():Codec()
	{
	}
public:
	virtual size_t encode(unicode_str uStr, size_t uLen, char *str, size_t sLen);
	virtual size_t decode(const char *str, size_t sLen, unicode_str uStr, size_t uLen);
private:
	void CPGBKEncode(ucs4_t c, DBCHAR *s); 
	void CPGBKDecode(const char *gChar, ucs4_t *u);
};
