/*
 * ozip_lua.c
 *
 *  Created on: 2013-7-18
 *      Author: ocaml
 */


#include <lua5.1/lauxlib.h>
#include <lua5.1/lua.h>
#include "ozip.h"
#include <assert.h>
#include <errno.h>
#include <string.h>

#define ARCHIVE_MT      "zip{archive}"
#define ARCHIVE_FILE_MT "zip{archive.file}"
#define WEAK_MT         "zip{weak}"

#define check_archive(L, narg)                                   \
    ((zipFile*)luaL_checkudata((L), (narg), ARCHIVE_MT))

#define check_archive_file(L, narg)                              \
    ((zipFile*)luaL_checkudata((L), (narg), ARCHIVE_FILE_MT))

#define absindex(L,i) ((i)>0?(i):lua_gettop(L)+(i)+1)

static void stackdump(lua_State* L)
{
    int index;
    int top = lua_gettop(L);

    for (index = -1; index >= -(top); --index) {
    	/* repeat for each level */
        int _type = lua_type(L, index);
        switch (_type) {
        case LUA_TSTRING:  /* strings */
            fprintf(stderr, "\tindex=%d    string: '%s'\n", index, lua_tostring(L, index));
            break;
        case LUA_TBOOLEAN:  /* booleans */
            fprintf(stderr, "\tindex=%d    boolean %s\n",index, lua_toboolean(L, index) ? "true" : "false");
            break;
        case LUA_TNUMBER:  /* numbers */
            fprintf(stderr, "\tindex=%d    number: %g\n",index, lua_tonumber(L, index));
            break;
        default: /* other values (TNIL, TTABLE, TUSERDATA, TTHREAD, TFUNTION, Unknow Type)*/
            fprintf(stderr, "\tindex=%d    %s\n",index, lua_typename(L, _type));
            break;
        }
    }
}

//#define WRITEBUFFERSIZE (16384)
//#define MAXFILENAME (256)

#ifdef _WIN32
uLong filetime(f, tmzip, dt)
    char *f;                /* name of file to get info on */
    tm_zip *tmzip;             /* return value: access, modific. and creation times */
    uLong *dt;             /* dostime */
{
  int ret = 0;
  {
      FILETIME ftLocal;
      HANDLE hFind;
      WIN32_FIND_DATAA ff32;

      hFind = FindFirstFileA(f,&ff32);
      if (hFind != INVALID_HANDLE_VALUE)
      {
        FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
        FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
        FindClose(hFind);
        ret = 1;
      }
  }
  return ret;
}
#else
#ifdef unix
uLong filetime(f, tmzip, dt)
    char *f;               /* name of file to get info on */
    tm_zip *tmzip;         /* return value: access, modific. and creation times */
    uLong *dt;             /* dostime */
{
  int ret=0;
  struct stat s;        /* results of stat() */
  struct tm* filedate;
  time_t tm_t=0;

  if (strcmp(f,"-")!=0)
  {
    char name[MAXFILENAME+1];
    int len = strlen(f);
    if (len > MAXFILENAME)
      len = MAXFILENAME;

    strncpy(name, f,MAXFILENAME-1);
    /* strncpy doesnt append the trailing NULL, of the string is too long. */
    name[ MAXFILENAME ] = '\0';

    if (name[len - 1] == '/')
      name[len - 1] = '\0';
    /* not all systems allow stat'ing a file with / appended */
    if (stat(name,&s)==0)
    {
      tm_t = s.st_mtime;
      ret = 1;
    }
  }
  filedate = localtime(&tm_t);

  tmzip->tm_sec  = filedate->tm_sec;
  tmzip->tm_min  = filedate->tm_min;
  tmzip->tm_hour = filedate->tm_hour;
  tmzip->tm_mday = filedate->tm_mday;
  tmzip->tm_mon  = filedate->tm_mon ;
  tmzip->tm_year = filedate->tm_year;

  return ret;
}
#else
uLong filetime(f, tmzip, dt)
    char *f;                /* name of file to get info on */
    tm_zip *tmzip;             /* return value: access, modific. and creation times */
    uLong *dt;             /* dostime */
{
    return 0;
}
#endif
#endif


/* If zip_error is non-zero, then push an appropriate error message
 * onto the top of the Lua stack and return zip_error.  Otherwise,
 * just return 0.
 */
/*static int S_push_error(lua_State* L, int zip_error, int sys_error) {
    char buff[1024];
    if ( 0 == zip_error ) return 0;

    int len = zip_error_to_str(buff, sizeof(buff), zip_error, sys_error);
    if ( len >= sizeof(buff) ) len = sizeof(buff)-1;
    lua_pushlstring(L, buff, len);

    return zip_error;
}*/

static int Lua_zipOpen(lua_State* L) {
    const char* path  = luaL_checkstring(L, 1);
    int flags = (lua_gettop(L) < 2) ? 0 : luaL_checkint(L, 2);
    zipFile* ar = (zipFile*)lua_newuserdata(L, sizeof(zipFile));
    int          err   = 0;

    stackdump(L);
    printf("\n ------ path=%s flags=%d ----- \n", path, flags);
    *ar = ozip_zipOpen64(path, flags);

    if ( !*ar ) {
        assert(err);
//        S_push_error(L, err, errno);
//        lua_pushnil(L);
//        lua_insert(L, -2);
        printf("==== failure to create file ====\n");
        luaL_error(L, "failure to zipOpen64\n");
        return 2;
    }

//    luaL_getmetatable(L, ARCHIVE_MT); /* set ARCHIVE_MATE_TABLE */
//    assert(!lua_isnil(L, -1)/* ARCHIVE_MT found? */);

//    lua_setmetatable(L, -2);

    /*
     * Each archive has a weak table of objects that are invalidated
     * when the archive is closed or garbage collected.
     */
//    lua_newtable(L);
//    luaL_getmetatable(L, WEAK_MT);
//    assert(!lua_isnil(L, -1)/* WEAK_MT found? */);

//    lua_setmetatable(L, -2);
//    lua_setfenv(L, -2);

    return 1;
}

/*
 * Invalidate all "weak" references.  This should be done just before
 * zip_close() is called.  Invalidation occurs by calling __gc
 * metamethod.
 */
static void Lua_gc_refs(lua_State* L, int ar_idx) {
    lua_getfenv(L, ar_idx); /* Get the fenv do nothing? */
    assert(lua_istable(L, -1) );/* fenv of archive must exist! */

    /*
     * NULL this archive to prevent infinite recursion
     */
    *((struct zip**)lua_touserdata(L, ar_idx)) = NULL;

    lua_pushnil(L);
    /* -2 it is the table in L stack */
    while ( lua_next(L, -2) != 0 ) {
        /* TODO: Exceptions when calling __gc meta method should be
         * handled, otherwise we get a memory leak.
         */
        if ( luaL_callmeta(L, -2, "__gc") ) {
            lua_pop(L, 2);//must pop luaL_callmeta return value and lua_next value
        } else {
            lua_pop(L, 1);
        }
    }
    // fenv table is shared, but the last owner how to know gc???
    lua_pop(L, 1); /* Pop the fenv */
}

/* Adds a reference from the archive at ar_idx to the object at
 * obj_idx.  This reference will be a "weak" reference if is_weak is
 * true, otherwise it will be a normal reference that prevents the
 * value from being GC'ed.
 *
 * We need to keep these references around so we can assert that the
 * lifetime of "child" objects is always shorter than the lifetime of
 * the "parent" archive object.  We also need to assert that any zip
 * source has a lifetime that lasts at least until the zip_close()
 * function is called.
 *
 */
static void Lua_add_ref(lua_State* L, int is_weak, int ar_idx, int obj_idx) {
    obj_idx = absindex(L, obj_idx);
    lua_getfenv(L, ar_idx);
    assert(lua_istable(L, -1) /* fenv of archive must exist! */);

    if ( is_weak ) {
        lua_pushvalue(L, obj_idx);
        lua_pushboolean(L, 1);
        lua_rawset(L, -3); // set fenv[stack[obj_index]]=1(boolean)
    } else {
        lua_pushvalue(L, obj_idx);
        lua_rawseti(L, -2, lua_objlen(L, -2)+1); //fenv[lua_objlen(L, -2)+1] = stack[obj_idx]
    }

    lua_pop(L, 1); /* Pop the fenv */
}

/* Explicitly close the archive, throwing an error if there are any
 * problems.
 */
static int Lua_zipClose(lua_State* L) {
//    zipFile  ar  = *check_archive(L, 1);
	zipFile ar = *(char**)lua_touserdata(L, -2);
    const char* global_comment = luaL_checkstring(L,-1);
    int len;
    int err;

    if ( ! ar ) {
    	return 0;
    }

//    S_archive_gc_refs(L, 1);
    err = ozip_zipClose (ar, global_comment);
	if(err != ZIP_OK){
		printf("failure to Lua_zipCloseFile\n");
		luaL_error(L, "failure to ozip_zipClose: %d\n", err);
		return 1;
	}
//    err = zip_close(ar);

//    if ( S_push_error(L, err, errno) ) {
//    	lua_error(L);
//    }

    return 0;
}

/* Try to revert all changes and close the archive since the archive
 * was not explicitly closed.
 */
static int Lua_gc(lua_State* L) {
//    zipFile ar = *check_archive(L, 1);
    zipFile ar = (zipFile*)lua_touserdata(L, -1);
    int err;

    if ( ! ar ) {
    	return 0;
    }

    S_archive_gc_refs(L, 1);

//    zip_unchange_all(ar);
//    zip_close(ar);
    err = ozip_zipClose (ar, NULL);

    return 0;
}

//ozip_zipOpenNewFileInZip4_64
static int Lua_zipOpenFile (lua_State* L){
//	zipFile file = *check_archive(L, 1);
	zipFile ar = *(char**)lua_touserdata(L, -2);
	int err;

	const char* filename ;//= luaL_checkstring(L, 2);
	/* almost parameter can from a table int lua env we can give a default table */
	/* const zip_fileinfo* zipfi; */
	zip_fileinfo zi;
	const void* extrafield_local;
	uint32_t size_extrafield_local;
	const void* extrafield_global;
	uint32_t size_extrafield_global;
	const char* comment;
	int method;
	int level;
	int raw;
	int windowBits;
	int memLevel;
	int strategy;
	const char* password;
	uint32_t crcForCrypting;
	uint32_t versionMadeBy;
	uint32_t flagBase;
	int zip64;

//#define GETSTRING(NAME)                    \
//	lua_getfield(L,-2, #NAME);             \
//	lua_tolstring(L, -1, NULL);\
//	lua_pop(L,1);

	stackdump(L);
	printf("\n==================\n");
	assert(lua_isuserdata(L,-2) == 1);
	assert(lua_istable(L, -1) == 1); /* parameter table */

	if ( ! ar ) {
		luaL_error(L, "userdata is nil\n");
		return 1;
	}

    zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
    zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
    zi.dosDate = 0;
    zi.internal_fa = 0;
    zi.external_fa = 0;
    filetime("/home/ocaml/Downloads/unzip11/oo.zip",&zi.tmz_date,&zi.dosDate);

//	GETSTRING("filename");
	lua_getfield(L,-1,"filename");
	filename = lua_tolstring(L, -1, NULL);
	lua_pop(L,1);

	lua_getfield(L,-1,"extrafield_local");
	extrafield_local = lua_tolstring(L, -1, NULL);
	lua_pop(L,1);

	lua_getfield(L, -1, "size_extrafield_local");
	size_extrafield_local = lua_tointeger(L, -1);
	lua_pop(L,1);

	lua_getfield(L,-1,"extrafield_global");
	extrafield_global = lua_tolstring(L, -1, NULL);
	lua_pop(L,1);

	lua_getfield(L, -1, "size_extrafield_global");
	size_extrafield_global = lua_tointeger(L, -1);
	lua_pop(L,1);

	lua_getfield(L,-1,"comment");
	comment = lua_tolstring(L, -1, NULL);
	lua_pop(L,1);

	lua_getfield(L, -1, "method");
	method = lua_tointeger(L, -1);
	lua_pop(L,1);

	lua_getfield(L, -1, "level");
	level = lua_tointeger(L, -1);
	lua_pop(L,1);

	lua_getfield(L, -1, "raw");
	raw = lua_tointeger(L, -1);
	lua_pop(L,1);

	lua_getfield(L, -1, "windowBits");
	windowBits = lua_tointeger(L, -1);
	lua_pop(L,1);

	lua_getfield(L, -1, "memLevel");
	memLevel = lua_tointeger(L, -1);
	lua_pop(L,1);

	lua_getfield(L, -1, "strategy");
	strategy = lua_tointeger(L, -1);
	lua_pop(L,1);

	lua_getfield(L,-1,"password");
	password = lua_tolstring(L, -1, NULL);
	lua_pop(L,1);

	lua_getfield(L, -1, "crcForCrypting");
	crcForCrypting = lua_tointeger(L, -1);
	lua_pop(L,1);

	lua_getfield(L, -1, "versionMadeBy");
	versionMadeBy = lua_tointeger(L, -1);
	lua_pop(L,1);

	lua_getfield(L, -1, "flagBase");
	flagBase = lua_tointeger(L, -1);
	lua_pop(L,1);

	lua_getfield(L, -1, "zip64");
	zip64 = lua_tointeger(L, -1);
	lua_pop(L,1);

	err = ozip_zipOpenNewFileInZip4_64(ar,filename,&zi,extrafield_local,size_extrafield_local,
		extrafield_global,size_extrafield_global,comment,method,level,raw,windowBits,memLevel,
		strategy,password,crcForCrypting,versionMadeBy,flagBase,zip64);

	if(err != ZIP_OK){
		printf(" ...............failure.........\n");
		luaL_error(L, "failure to NewFileInZip: %d\n", err);
		return 0;
	}
	printf("finish Lua_zipOpenFile ===== \n");
	return 0;
}

static int Lua_zipWriteFile (lua_State* L){
//    zipFile ar = *check_archive(L, 1);
	zipFile ar = *(char**)lua_touserdata(L, -2);
    size_t len;
    const char* buf  = lua_tolstring(L, -1, &len);
//    printf("----------- buf = %s size= %d\n", buf, len);
    int err;

    if ( ! ar ) {
    	printf("===failure to get userdata\n");
    	luaL_error(L, "failure to get userdata\n");
    	return 0;
    }

	err = ozip_zipWriteInFileInZip( ar, buf, len);
	if(err != ZIP_OK){
		luaL_error(L, "failure to ozip_zipWriteInFileInZip: %d\n", err);
		return 1;
	}
	return 0;
}

static int Lua_zipCloseFile (lua_State* L) {
//    zipFile  ar  = *check_archive(L, 1);
    zipFile ar = *(char**)lua_touserdata(L, -1);
    int err;

    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }
    err = ozip_zipCloseFileInZip(ar);
	if(err != ZIP_OK){
		printf("failure to Lua_zipCloseFile\n");
		luaL_error(L, "failure to ozip_zipCloseFileInZip: %d\n", err);
		return 1;
	}
	return 0;
}

static int Lua_zipCloseFileRaw (lua_State* L){
//    zipFile ar = *check_archive(L, 1);
    zipFile ar = *(char**)lua_touserdata(L, -3);
    int err;
    uint32_t uncompressed_size=luaL_checknumber(L, 2);
    uint32_t crc32=luaL_checknumber(L, 3);
    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }
    ozip_zipCloseFileInZipRaw64(ar,uncompressed_size, crc32);

	return 0;
}

static int Lua_zipRemoveExtraInfoBlock (lua_State* L){
//	zipFile ar = *check_archive(L, 1);
	zipFile ar = *(char**)lua_touserdata(L, -3);
	int err;
	size_t dataLen;
	char* pData = (char*)lua_tolstring(L, -2, &dataLen);
	short int sHeader = luaL_checknumber(L, 3);
    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }
	ozip_zipRemoveExtraInfoBlock(pData, ((int*)&dataLen), sHeader);
	return 0;
}

/******************************************************/
/* unzip API */
//int unzStringFileNameCompare (const char* fileName1, const char* fileName2, int iCaseSensitivity);
static int Lua_unzStringFileNameCompare (lua_State* L){
	const char* fileName1;
	const char* fileName2;
	int iCaseSensitivity;

	fileName1 = luaL_checkstring(L, 1);
	fileName2 = luaL_checkstring(L,2);
	iCaseSensitivity = luaL_checkinteger(L,3);

	lua_pushnumber(L, unzStringFileNameCompare(fileName1, fileName2, iCaseSensitivity));

	return 1;
}

//unzFile unzOpen64 (const void *path)
static int Lua_unzOpen64 (lua_State* L){
    const char* path  = luaL_checkstring(L, 1);
    unzFile* ar = (unzFile*)lua_newuserdata(L, sizeof(unzFile));
    int          err   = 0;

    *ar = ozip_unzOpen64(path);

    if ( !*ar ) {
        assert(err);
//        S_push_error(L, err, errno);
//        lua_pushnil(L);
//        lua_insert(L, -2);
        luaL_error(L, "failure to zipOpen64\n");
        return 2;
    }
    return 1;
}

//int unzClose (unzFile file);
static int Lua_unzClose (lua_State* L){
	unzFile ar = *(char**) lua_touserdata(L, -1);
	int err;

    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }

	//    S_archive_gc_refs(L, 1);
	err = ozip_unzClose(ar);
	if (err != UNZ_OK) {
		printf("failure to Lua_zipCloseFile\n");
		luaL_error(L, "failure to ozip_unzClose: %d\n", err);
		return 1;
	}

	//    if ( S_push_error(L, err, errno) ) {
	//    	lua_error(L);
	//    }

	return 0;
}

//int unzGetGlobalInfo64 (unzFile file, unz_global_info64 *pglobal_info);
static int Lua_unzGetGlobalInfo64(lua_State* L) {
	unzFile ar = *(char**) lua_touserdata(L, -1);
	unz_global_info64 global_info;
	int err;

    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }

	err = ozip_unzGetGlobalInfo64(ar, &global_info);

	if(err != UNZ_OK){
		printf("failure to Lua_zipCloseFile\n");
		luaL_error(L, "failure to ozip_unzClose: %d\n", err);
		return 1;
	}
	lua_pushinteger(L, global_info.number_entry);
	lua_pushinteger(L, global_info.size_comment);

	return 2;
}

// int unzGetGlobalComment (unzFile file, char *szComment, uint32_t uSizeBuf);
// set the commet buffer len default value in ozip.lua
static int Lua_unzGetGlobalComment(lua_State* L) {
	int iRet;
	uint32_t size;
	char* buffer;
	unzFile ar = *(char**)lua_touserdata(L, -2);

    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }
	size = lua_tointeger(L, -1);

	buffer=(char*)malloc(size);
	iRet = ozip_unzGetGlobalComment(ar, buffer, size);
	if(iRet<0){
		free(buffer);
		buffer=NULL;
		luaL_error(L, "failure to unzGetGlobalComment error code: %d\n", iRet);
		return 1;
		//error
	}
	//return value
	lua_pushlstring(L,buffer,iRet);
	return 1;
}


//int unzGoToFirstFile (unzFile file)
static int Lua_unzGoToFirstFile (lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -1);
	int err;

    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }
	err = ozip_unzGoToFirstFile(ar);

	lua_pushinteger(L, err);
	return 1;
}

//int unzGoToNextFile (unzFile file);
static  int Lua_unzGoToNextFile (lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -1);
	int err;

    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }
	err = ozip_unzGoToFirstFile(ar);

	lua_pushinteger(L, err);
	return 1;
}

//int unzLocateFile (unzFile file, const char *szFileName, int iCaseSensitivity);
static int Lua_unzLocateFile(lua_State* L) {
	unzFile ar = *(char**)lua_touserdata(L, -3);
	const char* szFileName;
	int iCase;
	int index;

    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }
	szFileName  = luaL_checkstring(L, 2);
	iCase = luaL_checkinteger(L, 3);

	index = ozip_unzLocateFile(ar, szFileName, iCase);
	//handle error code on lua
	lua_pushinteger(L, index);

	return 1;
}

//int unzGetFilePos64(unzFile file, unz64_file_pos* file_pos)
static int Lua_unzGetFilePos64(lua_State* L){
	unz64_file_pos file_pos;
	unzFile ar = *(char**)lua_touserdata(L, -1);
	int err;
    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }

	err = ozip_unzGetFilePos64(ar, &file_pos);
	if(err != UNZ_OK){
		return 0;
	}
	//return table can resolve the return value order
	lua_pushinteger(L, file_pos.pos_in_zip_directory);
	lua_pushinteger(L, file_pos.num_of_file);
	return 2;
}

//int unzGoToFilePos64(unzFile file, const unz64_file_pos* file_pos);
static int Lua_unzGoToFilePos64(lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -3);
	unz64_file_pos file_pos;
    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }
	file_pos.pos_in_zip_directory = luaL_checkinteger(L,2);
	file_pos.num_of_file = luaL_checkinteger(L,3);

	int err;
	err = ozip_unzGoToFilePos64(L, &file_pos);

	lua_pushinteger(L, err);

	return 1;
}

//int unzGetCurrentFileInfo64 (unzFile file, unz_file_info64 *pfile_info,
//		char *szFileName, uint32_t fileNameBufferSize, void *extraField, uint32_t extraFieldBufferSize,
//		char *szComment, uint32_t commentBufferSize);
static int Lua_unzGetCurrentFileInfo64(lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -4);
	unz_file_info64 info;
	uint32_t fileNameBufferSize;
	char* szFileName;
	uint32_t extraFieldBufferSize;
	void *extraField;
	uint32_t commentBufferSize;
	char *szComment;
	int err;

    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }
	fileNameBufferSize=luaL_checkinteger(L, 2);
	szFileName=(char*)malloc(fileNameBufferSize);

	extraFieldBufferSize = luaL_checkinteger(L, 3);
	extraField = (char*)malloc(extraFieldBufferSize);

	commentBufferSize = luaL_checkinteger(L, 4);
	szComment=(char*)malloc(commentBufferSize);

	err = ozip_unzGetCurrentFileInfo64(ar,&info,szFileName,fileNameBufferSize,
			extraField, extraFieldBufferSize, szComment, commentBufferSize);

	lua_newtable(L);
	lua_pushinteger(L,info.compressed_size);
	lua_setfield(L,-2,"compressed_size");

	lua_pushinteger(L,info.compression_method);
	lua_setfield(L,-2,"compression_method");

	lua_pushinteger(L,info.crc);
	lua_setfield(L,-2,"crc");

	lua_pushinteger(L,info.disk_num_start);
	lua_setfield(L,-2,"disk_num_start");

	lua_pushinteger(L,info.dosDate);
	lua_setfield(L,-2,"dosDate");

	lua_pushinteger(L,info.external_fa);
	lua_setfield(L,-2,"external_fa");

	lua_pushinteger(L,info.flag);
	lua_setfield(L,-2,"flag");

	lua_pushinteger(L,info.internal_fa);
	lua_setfield(L,-2,"internal_fa");

	lua_pushinteger(L,info.size_file_comment);
	lua_setfield(L,-2,"size_file_comment");

	lua_pushinteger(L,info.size_file_extra);
	lua_setfield(L,-2,"size_file_extra");

	lua_pushinteger(L,info.size_filename);
	lua_setfield(L,-2,"size_filename");

	lua_pushinteger(L,info.uncompressed_size);
	lua_setfield(L,-2,"uncompressed_size");

	lua_pushinteger(L,info.version);
	lua_setfield(L,-2,"version");

	lua_pushinteger(L,info.version_needed);
	lua_setfield(L,-2,"version_needed");

	lua_pushstring(L, szFileName);
	lua_setfield(L,-2,"szFileName");

	lua_pushstring(L, extraField);
	lua_setfield(L,-2,"extraField");

	lua_pushstring(L, szComment);
	lua_setfield(L,-2,"szComment");

	free(szFileName);
	szFileName = NULL;
	free(extraField);
	extraField=NULL;
	free(szComment);
	szComment=NULL;

	return 1;
}

//uint64_t unzGetCurrentFileZStreamPos64 (unzFile file);
static int Lua_unzGetCurrentFileZStreamPos64 (lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -1);
	uint64_t pos;

    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }

	pos = ozip_unzGetCurrentFileZStreamPos64(ar);

	lua_pushinteger(L, pos);

	return 1;
}

//int unzOpenCurrentFile3 (unzFile file, int* method, int* level, int raw, const char* password);
static int Lua_unzOpenCurrentFile3 (lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -3);
	int raw = luaL_checkinteger(L, 2);
	const char* passwd = luaL_checkstring(L, 3);
	int method;
	int level;
	int err;
    if ( ! ar ) {
    	luaL_error(L, "userdata is nil\n");
    	return 1;
    }
    // add default no passwd
	err = ozip_unzOpenCurrentFile3(ar, &method, &level, raw, NULL);
	//do err != UNZ_OK

	lua_pushinteger(L, method);
	lua_pushinteger(L, level);

	return 2;
}

//int unzCloseCurrentFile (unzFile file);
static int Lua_unzCloseCurrentFile (lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -1);
	int err = ozip_unzCloseCurrentFile(ar);

	lua_pushinteger(L, err);
	return 1;
}

//int unzReadCurrentFile (unzFile file, void* buf, unsigned len);
static int Lua_unzReadCurrentFile (lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -2);
	unsigned int len = luaL_checkinteger(L,2);

	int err;
	void* buffer = malloc(len+1);
	err = ozip_unzReadCurrentFile(ar,buffer,len);
//	printf(" == read ctx: == \n %s\n\n", (char*)buffer);

	lua_pushinteger(L,err);
	lua_pushlstring(L, buffer, err);
	free(buffer);

	return 2;
}

//uint64_t unztell64 (unzFile file);
static int Lua_unztell64(lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -1);
	uint64_t pos;

	pos = ozip_unztell64(ar);

	lua_pushinteger(L, pos);

	return 1;
}

//int unzeof (unzFile file);
static int Lua_unzeof (lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -1);
	int eof;

	eof = ozip_unztell64(ar);

	lua_pushinteger(L, eof);

	return 1;
}

//int unzGetLocalExtrafield (unzFile file, void* buf, unsigned len);
static int Lua_unzGetLocalExtrafield (lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -1);
	int size;
	int len = ozip_unzGetLocalExtrafield(ar,NULL,size);
	void* buf = malloc(len+1);
	((char*)buf)[len]='\0';
	int err = ozip_unzGetLocalExtrafield(ar,buf,len);

	lua_pushlstring(L, buf, len);
	free(buf);
	buf = NULL;
	return 1;
}

//uint64_t unzGetOffset64 (unzFile file){
static int Lua_unzGetOffset64 (lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -1);
	uint64_t offset;
	offset = ozip_unzGetOffset64(ar);
	lua_pushinteger(L, offset);
	return 1;
}

//int unzSetOffset64 (unzFile file, uint64_t pos);
static int Lua_unzSetOffset64 (lua_State* L){
	unzFile ar = *(char**)lua_touserdata(L, -2);
	uint64_t offset = luaL_checkinteger(L, 2);
	int err;
	err = ozip_unzSetOffset64(ar, offset);
	lua_pushinteger(L, err);
	return 1;
}

static int Lua_strzipOpen(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	int flags = (lua_gettop(L) < 2) ? 0 : luaL_checkint(L, 2);
	zipFile* ar = (zipFile*) lua_newuserdata(L, sizeof(zipFile));
	int err = 0;

	stackdump(L);
	*ar = ozip_strzipOpen(path, 0, 0, 0);

	if (!*ar) {
		assert(err);
		luaL_error(L, "failure to zipOpen64\n");
		return 2;
	}
	return 1;
}

static int Lua_strunzOpen(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	int flags = (lua_gettop(L) < 2) ? 0 : luaL_checkint(L, 2);
	zipFile* ar = (zipFile*) lua_newuserdata(L, sizeof(zipFile));
	int err = 0;

	stackdump(L);
	*ar = ozip_strunzOpen(path, flags, 0);

	if (!*ar) {
		assert(err);
		luaL_error(L, "failure to zipOpen64\n");
		return 2;
	}
	return 1;
}


static int Lua_ozipOR(lua_State* L) {
    int result = 0;
    int top = lua_gettop(L);
    while ( top ) {
        result |= luaL_checkint(L, top--);
    }
    lua_pushinteger(L, result);
    return 1;
}

LUALIB_API int luaopen__ozip(lua_State* L) {
    static luaL_reg fns[] = {
    	{ "OR",       Lua_ozipOR },

        { "fs_zipOpen",  Lua_zipOpen },
        { "fs_zipClose",  Lua_zipClose },
        { "fs_zipOpenFile",  Lua_zipOpenFile },
        { "fs_zipWriteFile",  Lua_zipWriteFile },
        { "fs_zipCloseFile",  Lua_zipCloseFile },
        { "fs_zipCloseFileRaw",  Lua_zipCloseFileRaw },
        { "fs_zipRemoveExtraInfoBlock",  Lua_zipRemoveExtraInfoBlock },

        { "fs_unzStringFileNameCompare",  Lua_unzStringFileNameCompare },
        { "fs_unzOpen",  Lua_unzOpen64 },
        { "fs_unzClose",  Lua_unzClose },
        { "fs_unzGetGlobalInfo",  Lua_unzGetGlobalInfo64 },
        { "fs_unzGetGlobalComment",  Lua_unzGetGlobalComment },
        { "fs_unzGoToFirstFile",  Lua_unzGoToFirstFile },
        { "fs_unzGoToNextFile",  Lua_unzGoToNextFile },
        { "fs_unzLocateFile",  Lua_unzLocateFile },
        { "fs_unzGetFilePos",  Lua_unzGetFilePos64 },
        { "fs_unzGoToFilePos",  Lua_unzGoToFilePos64 },
        { "fs_unzGetCurrentFileInfo",  Lua_unzGetCurrentFileInfo64 },
        { "fs_unzGetCurrentFileZStreamPos",  Lua_unzGetCurrentFileZStreamPos64 },
        { "fs_unzOpenCurrentFile",  Lua_unzOpenCurrentFile3 },
        { "fs_unzCloseCurrentFile",  Lua_unzCloseCurrentFile },
        { "fs_unzReadCurrentFile",  Lua_unzReadCurrentFile },
        { "fs_unztell",  Lua_unztell64 },
        { "fs_unzeof",  Lua_unzeof },
        { "fs_unzGetLocalExtrafield",  Lua_unzGetLocalExtrafield },
        { "fs_unzGetOffset",  Lua_unzGetOffset64 },
        { "fs_unzSetOffset",  Lua_unzSetOffset64 },

        { "str_zipOpen",  Lua_strzipOpen },
        { "str_unzOpen",  Lua_strunzOpen },

        { NULL, NULL }
    };

    lua_newtable(L);
    luaL_register(L, NULL, fns);

//#define UNZ_OK 100

#define EXPORT_CONSTANT(NAME)    \
    lua_pushnumber(L, UNZ_##NAME);\
    lua_setfield(L, -2, #NAME);

    EXPORT_CONSTANT(OK);

    return 1;
}


