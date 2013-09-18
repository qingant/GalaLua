#include "LuaCPP.hpp"
#include "cdtype.h"
#include <string>

class CSTRDump
{
public:
   static std::string Intl_GenHexDump(PSTR Src, UINT Len);
private:
   static void Intl_B2Hex(BYTE pHex[2],BYTE bVal);
};

std::string CSTRDump::Intl_GenHexDump(PSTR Src, UINT Len)
{
    BYTE    szHex[128];
    BYTE    szDBL[4];
    BYTE    szIDX[16];
    PBYTE   pOfs;
    PBYTE   pDbl;
    PBYTE   pSep;
    PBYTE   pDsp;
    PBYTE   pSrc;
    INT     iLen;
    UINT    cLen;
    UINT    i,j;

    pOfs    = szHex;
    pDbl    = &szHex[11];
    pSep    = &szHex[59];
    pDsp    = &szHex[61];

    if(ISNULL(Src)||(Len == 0))
    {
    	return std::string();
    }

    std::string dumpStr;
    for(i=0,iLen = (INT)Len;iLen > 0;(iLen -= 16),i++)
    {
    	pSrc = (PBYTE)&Src[i*16];
    	cLen = (iLen < 16)? iLen : 16 ;

    	memset(szHex,' ',77);
    	szHex[77]=0;

    	snprintf((PSTR)szIDX,11,"%08xh:",(i * 16));
    	memcpy(pOfs,szIDX,10);

    	for(j=0;j<cLen;j++)
    	{
    		Intl_B2Hex(szDBL,(BYTE)pSrc[j]);
    		memcpy(&pDbl[j*3],szDBL,2);

    		pDsp[j] = pSrc[j];
    		if((pDsp[j]=='\r')||(pDsp[j]=='\n')||(pDsp[j]=='\t'))
    		{
    			pDsp[j]=' ';
    		}
    		else if(pDsp[j]==0)
    		{
    			pDsp[j]='.';
    		}
    	}

    	memcpy(pSep,"; ",2);

    	std::string tmpStr((const char *)szHex);
    	tmpStr.append("\n");
    	dumpStr += tmpStr;
    }
    dumpStr.erase(dumpStr.end()-1, dumpStr.end());
    dumpStr[dumpStr.size()] = '\0';
    return dumpStr;
}

void  CSTRDump::Intl_B2Hex(BYTE pHex[2],BYTE bVal)
{
    BYTE        HI_BYTE;
		BYTE        LO_BYTE;


		HI_BYTE = (BYTE)(bVal >> 4 ) & (BYTE)0x0F;
		LO_BYTE = (BYTE)(bVal & (BYTE)0x0F);

		if(HI_BYTE > 0x09)
		{
			pHex[0]=HI_BYTE + 0x37;
		}
		else
		{
			pHex[0]=HI_BYTE + 0x30;
		}

		if(LO_BYTE > 0x09)
		{
			pHex[1]=LO_BYTE + 0x37;
		}
		else
		{
			pHex[1]=LO_BYTE + 0x30;
		}
}

static int hexdump(lua_State *L)
{
   size_t len;
   const char *str = luaL_checklstring(L,1,&len);
   std::string hexStr = CSTRDump::Intl_GenHexDump((PSTR)str,(UINT)len);
   lua_pushstring(L,hexStr.c_str());
   return 1;
}

extern "C"
int luaopen_hexdump(lua_State *L)
{
   struct luaL_Reg methods[] = {
		{"dump",hexdump},
		{NULL, NULL},
	};
	luaL_register(L,"hexdump",methods);

   return 1;
}

