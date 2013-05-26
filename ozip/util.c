/*
 * util.c
 *
 *  Created on: 2013-5-15
 *      Author: ocaml
 */

#include "util.h"

int get_byte(filefunc,filestream,pi)
    const ozip_filefunc* filefunc;
    voidpf filestream;
    int *pi;
{
    unsigned char c;
    int err = (int)ZREAD(*filefunc,filestream,&c,1);
    if (err==1)
    {
        *pi = (int)c;
        return OZIP_OK;
    }
    else
    {
        if (ZERROR(*filefunc,filestream))
        {
        	return OZIP_ERRNO;
        }
        else
        {
        	return OZIP_EOF;
        }
    }
}


int get_short (filefunc,filestream,pX)
    const ozip_filefunc* filefunc;
    voidpf filestream;
    uLong *pX;
{
    uLong x ;
    int i = 0;
    int err;

    err = get_byte(filefunc,filestream,&i);
    x = (uLong)i;

    if (err==OZIP_OK)
    {
    	err = get_byte(filefunc,filestream,&i);
    }
    x += ((uLong)i)<<8;

    if (err==OZIP_OK)
    {
    	*pX = x;
    }
    else
    {
    	*pX = 0;
    }
    return err;
}


int get_long (filefunc,filestream,pX)
    const ozip_filefunc* filefunc;
    voidpf filestream;
    uLong *pX;
{
    uLong x ;
    int i = 0;
    int err;

    err = get_byte(filefunc,filestream,&i);
    x = (uLong)i;

    if (err==OZIP_OK)
        err = get_byte(filefunc,filestream,&i);
    x += ((uLong)i)<<8;

    if (err==OZIP_OK)
        err = get_byte(filefunc,filestream,&i);
    x += ((uLong)i)<<16;

    if (err==OZIP_OK)
        err = get_byte(filefunc,filestream,&i);
    x += ((uLong)i)<<24;

    if (err==OZIP_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
}


/* My own strcmpi / strcasecmp */
 int strcmp_case_nosensitive (fileName1,fileName2)
    const char* fileName1;
    const char* fileName2;
{
    for (;;)
    {
        char c1=*(fileName1++);
        char c2=*(fileName2++);
        if ((c1>='a') && (c1<='z'))
            c1 -= 0x20;
        if ((c2>='a') && (c2<='z'))
            c2 -= 0x20;
        if (c1=='\0')
            return ((c2=='\0') ? 0 : -1);
        if (c2=='\0')
            return 1;
        if (c1<c2)
            return -1;
        if (c1>c2)
            return 1;
    }
    return 1;
}


#ifdef  CASESENSITIVITYDEFAULT_NO
#define CASESENSITIVITYDEFAULTVALUE 2
#else
#define CASESENSITIVITYDEFAULTVALUE 1
#endif

#ifndef STRCMPCASENOSENTIVEFUNCTION
#define STRCMPCASENOSENTIVEFUNCTION strcmp_case_nosensitive
#endif

int string_file_name_compare (fileName1,fileName2,iCaseSensitivity)
    const char* fileName1;
    const char* fileName2;
    int iCaseSensitivity;
{
    if (iCaseSensitivity==0)
        iCaseSensitivity=CASESENSITIVITYDEFAULTVALUE;

    if (iCaseSensitivity==1)
        return strcmp(fileName1,fileName2);

    return STRCMPCASENOSENTIVEFUNCTION(fileName1,fileName2);
}

#ifndef BUFREADCOMMENT
#define BUFREADCOMMENT (0x400)
#endif
