#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>
#include <string.h>
#include <imembase.h>

static char *intl_b2hex(char pHex[2], char bVal)
{

    char        HI_BYTE;
    char        LO_BYTE;


    HI_BYTE = (char)(bVal >> 4 ) & (char)0x0F;
    LO_BYTE = (char)(bVal & (char)0x0F);

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

    return (pHex);
}

static char *intl_genhexdump(const char * Src, size_t Len)
{
    char    szHex[128];
    char    szDBL[4];
    char    szIDX[16];
    char *   pOfs;
    char *   pDbl;
    char *   pSep;
    char *   pDsp;
    char *   pSrc;
    int     iLen;
    unsigned int    cLen;
    unsigned int    i,j;

    pOfs    = szHex;
    pDbl    = &szHex[11];
    pSep    = &szHex[59];
    pDsp    = &szHex[61];

    size_t szLen =Len*4;
    char *szTemp = ikmem_malloc(szLen);
    if(NULL == szTemp){
       return NULL;
    }
    int totalSize = ikmem_ptr_size(szTemp);
    szLen = totalSize;
    memset(szTemp,0x00,szLen);
    char *pTemp = szTemp;
    for(i=0,iLen = (int)Len;iLen > 0;(iLen -= 16),i++)
    {
        pSrc = (char *)&Src[i*16];
        cLen = (iLen < 16)? iLen : 16 ;

        memset(szHex,' ',77);
        szHex[77]=0;

        snprintf((char *)szIDX,11,"%08xh:",(i * 16));
        memcpy(pOfs,szIDX,10);

        for(j=0;j<cLen;j++)
        {
            intl_b2hex(szDBL,(char)pSrc[j]);
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
        int strLen = strlen(szHex);
        if(strLen >= szLen){
           szTemp = ikmem_realloc(szTemp,2*totalSize);
           pTemp = szTemp + (totalSize - szLen);
           totalSize = ikmem_ptr_size(szTemp);
           szLen = totalSize - (pTemp - szTemp);
        }
        memcpy(pTemp,szHex,strLen);
        pTemp[strLen] = '\n';
        szLen -= (strLen + 1);
        pTemp += (strLen + 1);
    }
    return szTemp;
}

static int hexdump(lua_State *L)
{
   size_t len;
   const char *str = luaL_checklstring(L,1,&len);
   char *hexStr = intl_genhexdump(str,len);
   if(NULL == hexStr){
      lua_pushnil(L);
      lua_pushstring(L,"invalid string");
      return 2;
   }
   lua_pushstring(L,hexStr);
   return 1;
}

static int destruct(lua_State *L)
{
   ikmem_destroy();
   return 1;
}

LUALIB_API int luaopen_hexdump(lua_State *L)
{
   struct luaL_Reg methods[] = {
		{"dump",hexdump},
		{NULL, NULL},
	};
	ikmem_init(0, 0, 0);
	luaL_register(L,"hexdump",methods);

   luaL_newmetatable(L,"CONVERT TO HEX");
   lua_pushvalue(L, -1);
   lua_setfield(L, -2, "__index");

   lua_pushcfunction(L,destruct);
   lua_setfield(L, -2, "__gc");
   lua_pop(L, 1);
   return 1;
}

