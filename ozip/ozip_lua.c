/*
 * ozip_lua.c
 *
 *  Created on: 2013-7-18
 *      Author: ocaml
 */

#include <lauxlib.h>
#include <lua.h>
#include "ozip.h"
#include <assert.h>
#include <errno.h>
#include <string.h>

#define ZIPARCHIVE_MT      "ozip.zip{archive}"
#define UNZARCHIVE_MT      "ozip.unz{archive}"

#define check_ziparchive(L, narg)                              \
    ((zipFile*)luaL_checkudata((L), (narg), ZIPARCHIVE_MT))

#define check_unzarchive(L, narg)                              \
    ((unzFile*)luaL_checkudata((L), (narg), UNZARCHIVE_MT))

#define absindex(L,i) ((i)>0?(i):lua_gettop(L)+(i)+1)

static void stackdump(lua_State* L) {
	int index;
	int top = lua_gettop(L);

	for (index = -1; index >= -(top); --index) {
		/* repeat for each level */
		int _type = lua_type(L, index);
		switch (_type) {
		case LUA_TSTRING: /* strings */
			fprintf(stderr, "\tindex=%d    string: '%s'\n", index,
					lua_tostring(L, index));
			break;
		case LUA_TBOOLEAN: /* booleans */
			fprintf(stderr, "\tindex=%d    boolean %s\n", index,
					lua_toboolean(L, index) ? "true" : "false");
			break;
		case LUA_TNUMBER: /* numbers */
			fprintf(stderr, "\tindex=%d    number: %g\n", index,
					lua_tonumber(L, index));
			break;
		default: /* other values (TNIL, TTABLE, TUSERDATA, TTHREAD, TFUNTION, Unknow Type)*/
			fprintf(stderr, "\tindex=%d    %s\n", index,
					lua_typename(L, _type));
			break;
		}
	}
}

static int Lua_zipOpen(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	int flags = (lua_gettop(L) < 2) ? 0 : luaL_checkint(L, 2);
	zipFile* ar = (zipFile*) lua_newuserdata(L, sizeof(zipFile));

	*ar = ozip_zipOpen64(path, flags);

	if (!*ar) {
		return luaL_error(L, "failure to zipOpen64\n");
	}

	luaL_getmetatable(L, ZIPARCHIVE_MT);
	assert(!lua_isnil(L, -1));
	lua_setmetatable(L, -2);

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

/* Explicitly close the archive, throwing an error if there are any
 * problems.
 */
static int Lua_zipClose(lua_State* L) {
	zipFile* ar = check_ziparchive(L, 1);
	const char* global_comment = luaL_checkstring(L,-1);
	int err;

	stackdump(L);
	if (!*ar) {
		return 0;
	}

	err = ozip_zipClose(*ar, global_comment);
	if (err != ZIP_OK) {
		return luaL_error(L, "failure to ozip_zipClose: %d\n", err);
	}
	*ar = NULL;

	return 0;
}

static int Lua_zipOpenFile(lua_State* L) {
	zipFile ar = *check_ziparchive(L, 1);
	int err;

	const char* filename;
	zip_fileinfo fileinfo;
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

	assert(lua_istable(L, -1) == 1);

	if (!ar) {
		return luaL_error(L, "userdata is nil\n");
	}

	fileinfo.tmz_date.tm_sec = fileinfo.tmz_date.tm_min =
			fileinfo.tmz_date.tm_hour = fileinfo.tmz_date.tm_mday =
					fileinfo.tmz_date.tm_mon = fileinfo.tmz_date.tm_year = 0;
	fileinfo.dosDate = 0;
	fileinfo.internal_fa = 0;
	fileinfo.external_fa = 0;
//	low api time from parameter
//	filetime("/home/ocaml/Downloads/unzip11/oo.zip", &fileinfo.tmz_date, &fileinfo.dosDate);

	lua_getfield(L, -1, "filename");
	filename = lua_tolstring(L, -1, NULL);
	lua_pop(L, 1);

	lua_getfield(L, -1, "extrafield_local");
	extrafield_local = lua_tolstring(L, -1, NULL);
	lua_pop(L, 1);

	lua_getfield(L, -1, "size_extrafield_local");
	size_extrafield_local = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "extrafield_global");
	extrafield_global = lua_tolstring(L, -1, NULL);
	lua_pop(L, 1);

	lua_getfield(L, -1, "size_extrafield_global");
	size_extrafield_global = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "comment");
	comment = lua_tolstring(L, -1, NULL);
	lua_pop(L, 1);

	lua_getfield(L, -1, "method");
	method = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "level");
	level = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "raw");
	raw = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "windowBits");
	windowBits = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "memLevel");
	memLevel = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "strategy");
	strategy = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "password");
	password = lua_tolstring(L, -1, NULL);
	lua_pop(L, 1);

	lua_getfield(L, -1, "crcForCrypting");
	crcForCrypting = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "versionMadeBy");
	versionMadeBy = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "flagBase");
	flagBase = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "zip64");
	zip64 = lua_tointeger(L, -1);
	lua_pop(L, 1);

	err = ozip_zipOpenNewFileInZip4_64(ar, filename, &fileinfo,
			extrafield_local, size_extrafield_local, extrafield_global,
			size_extrafield_global, comment, method, level, raw, windowBits,
			memLevel, strategy, password, crcForCrypting, versionMadeBy,
			flagBase, zip64);

	if (err != ZIP_OK) {
		luaL_error(L, "failure to NewFileInZip: %d\n", err);
		return 0;
	}
	return 0;
}

static int Lua_zipWriteFile(lua_State* L) {
	zipFile ar = *check_ziparchive(L, 1);
	size_t len;
	const char* buf = lua_tolstring(L, -1, &len);
	int err;

	if (!ar) {
		luaL_error(L, "failure to get userdata\n");
		return 0;
	}

	err = ozip_zipWriteInFileInZip(ar, buf, len);
	if (err != ZIP_OK) {
		luaL_error(L, "failure to ozip_zipWriteInFileInZip: %d\n", err);
		return 1;
	}
	return 0;
}

static int Lua_zipCloseFile(lua_State* L) {
	zipFile ar = *check_ziparchive(L, 1);
	int err;

	if (!ar) {
		luaL_error(L, "userdata is nil\n");
		return 1;
	}
	err = ozip_zipCloseFileInZip(ar);
	if (err != ZIP_OK) {
		luaL_error(L, "failure to ozip_zipCloseFileInZip: %d\n", err);
		return 1;
	}
	return 0;
}

static int Lua_zipCloseFileRaw(lua_State* L) {
	zipFile ar = *check_ziparchive(L, 1);
	uint32_t uncompressed_size = luaL_checknumber(L, 2);
	uint32_t crc32 = luaL_checknumber(L, 3);
	if (!ar) {
		luaL_error(L, "userdata is nil\n");
		return 1;
	}
	ozip_zipCloseFileInZipRaw64(ar, uncompressed_size, crc32);

	return 0;
}

static int Lua_zipRemoveExtraInfoBlock(lua_State* L) {
	zipFile ar = *check_ziparchive(L, 1);
	int err;
	size_t dataLen;
	char* pData = (char*) lua_tolstring(L, -2, &dataLen);
	short int sHeader = luaL_checknumber(L, 3);
	if (!ar) {
		return luaL_error(L, "userdata is nil\n");
	}
	err = ozip_zipRemoveExtraInfoBlock(pData, ((int*) &dataLen), sHeader);
	if (err != ZIP_OK) {
		return luaL_error(L, "ozip_zipRemoveExtraInfoBlock");
	}
	return 0;
}

/******************************************************/

/* unzip API */
//int unzStringFileNameCompare (const char* fileName1, const char* fileName2, int iCaseSensitivity);
static int Lua_unzStringFileNameCompare(lua_State* L) {
	const char* fileName1;
	const char* fileName2;
	int iCaseSensitivity;

	fileName1 = luaL_checkstring(L, 1);
	fileName2 = luaL_checkstring(L,2);
	iCaseSensitivity = luaL_checkinteger(L, 3);

	lua_pushnumber(L,unzStringFileNameCompare(fileName1, fileName2, iCaseSensitivity));

	return 1;
}

static int Lua_unzOpen64(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	unzFile* ar = (unzFile*) lua_newuserdata(L, sizeof(unzFile));
	*ar = ozip_unzOpen64(path);

	if (!*ar) {
		return luaL_error(L, "failure to zipOpen64\n");
	}

	luaL_getmetatable(L, UNZARCHIVE_MT);
	assert(!lua_isnil(L, -1));
	lua_setmetatable(L, -2);

	return 1;
}

static int Lua_unzClose(lua_State* L) {
	unzFile* ar = check_unzarchive(L, 1);
	int err;

	if (!*ar) {
		return luaL_error(L, "userdata is nil\n");
	}

	err = ozip_unzClose(*ar);
	if (err != UNZ_OK) {
		return luaL_error(L, "failure to Lua_unzClose: %d\n", err);
	}
	*ar = NULL;

	return 0;
}

static int Lua_unzGetGlobalInfo64(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	unz_global_info64 global_info;
	int err;

	if (!ar) {
		return luaL_error(L, "userdata is nil\n");
	}

	err = ozip_unzGetGlobalInfo64(ar, &global_info);

	if (err != UNZ_OK) {
		return luaL_error(L, "failure to ozip_unzGetGlobalInfo64: %d\n", err);
	}
	lua_pushinteger(L, global_info.number_entry);
	lua_pushinteger(L, global_info.size_comment);

	return 2;
}

// set the commet buffer len default value in ozip.lua
static int Lua_unzGetGlobalComment(lua_State* L) {
	int err;
	uint32_t size;
	char* buffer;
	unzFile ar = *check_unzarchive(L, 1);

	if (!ar) {
		return luaL_error(L, "userdata is nil\n");
	}
	size = lua_tointeger(L, -1);

	buffer = (char*) malloc(size);
	err = ozip_unzGetGlobalComment(ar, buffer, size);
	if (err < 0) {
		free(buffer);
		buffer = NULL;
		return luaL_error(L, "failure to unzGetGlobalComment error code: %d\n", err);
	}
	//return value
	lua_pushlstring(L, buffer, err);
	return 1;
}

static int Lua_unzGoToFirstFile(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	int err;

	if (!ar) {
		return luaL_error(L, "userdata is nil\n");
	}
	err = ozip_unzGoToFirstFile(ar);

	lua_pushinteger(L, err);
	return 1;
}

static int Lua_unzGoToNextFile(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	int err;

	if (!ar) {
		return luaL_error(L, "userdata is nil\n");
	}
	err = ozip_unzGoToFirstFile(ar);

	lua_pushinteger(L, err);
	return 1;
}

static int Lua_unzLocateFile(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	const char* szFileName;
	int iCase;
	int index;

	if (!ar) {
		return luaL_error(L, "userdata is nil\n");
	}
	szFileName = luaL_checkstring(L, 2);
	iCase = luaL_checkinteger(L, 3);

	index = ozip_unzLocateFile(ar, szFileName, iCase);
	//handle error code on lua
	lua_pushinteger(L, index);

	return 1;
}

static int Lua_unzGetFilePos64(lua_State* L) {
	unz64_file_pos file_pos;
	unzFile ar = lua_touserdata(L, -1);
	int err;
	if (!ar) {
		return luaL_error(L, "userdata is nil\n");
	}

	err = ozip_unzGetFilePos64(ar, &file_pos);
	if (err != UNZ_OK) {
		return 0;
	}
	//return table can resolve the return value order
	lua_pushinteger(L, file_pos.pos_in_zip_directory);
	lua_pushinteger(L, file_pos.num_of_file);
	return 2;
}

static int Lua_unzGoToFilePos64(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	unz64_file_pos file_pos;
	int err;
	if (!ar) {
		return luaL_error(L, "userdata is nil\n");
	}
	file_pos.pos_in_zip_directory = luaL_checkinteger(L, 2);
	file_pos.num_of_file = luaL_checkinteger(L, 3);

	err = ozip_unzGoToFilePos64(L, &file_pos);

	lua_pushinteger(L, err);

	return 1;
}

static int Lua_unzGetCurrentFileInfo64(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	unz_file_info64 info;
	uint32_t fileNameBufferSize;
	char* szFileName;
	uint32_t extraFieldBufferSize;
	void *extraField;
	uint32_t commentBufferSize;
	char *szComment;
	int err;

	if (!ar) {
		return luaL_error(L, "userdata is nil\n");
	}
	fileNameBufferSize = luaL_checkinteger(L, 2);
	szFileName = (char*) malloc(fileNameBufferSize);

	extraFieldBufferSize = luaL_checkinteger(L, 3);
	extraField = (char*) malloc(extraFieldBufferSize);

	commentBufferSize = luaL_checkinteger(L, 4);
	szComment = (char*) malloc(commentBufferSize);

	err = ozip_unzGetCurrentFileInfo64(ar, &info, szFileName,
			fileNameBufferSize, extraField, extraFieldBufferSize, szComment,
			commentBufferSize);
	if (err != ZIP_OK) {
		free(szFileName);
		szFileName = NULL;
		free(extraField);
		extraField = NULL;
		free(szComment);
		szComment = NULL;
		return luaL_error(L, "failure to ozip_unzGetCurrentFileInfo64: %d\n",err);
	}

	lua_newtable(L);
	lua_pushinteger(L, info.compressed_size);
	lua_setfield(L, -2, "compressed_size");

	lua_pushinteger(L, info.compression_method);
	lua_setfield(L, -2, "compression_method");

	lua_pushinteger(L, info.crc);
	lua_setfield(L, -2, "crc");

	lua_pushinteger(L, info.disk_num_start);
	lua_setfield(L, -2, "disk_num_start");

	lua_pushinteger(L, info.dosDate);
	lua_setfield(L, -2, "dosDate");

	lua_pushinteger(L, info.external_fa);
	lua_setfield(L, -2, "external_fa");

	lua_pushinteger(L, info.flag);
	lua_setfield(L, -2, "flag");

	lua_pushinteger(L, info.internal_fa);
	lua_setfield(L, -2, "internal_fa");

	lua_pushinteger(L, info.size_file_comment);
	lua_setfield(L, -2, "size_file_comment");

	lua_pushinteger(L, info.size_file_extra);
	lua_setfield(L, -2, "size_file_extra");

	lua_pushinteger(L, info.size_filename);
	lua_setfield(L, -2, "size_filename");

	lua_pushinteger(L, info.uncompressed_size);
	lua_setfield(L, -2, "uncompressed_size");

	lua_pushinteger(L, info.version);
	lua_setfield(L, -2, "version");

	lua_pushinteger(L, info.version_needed);
	lua_setfield(L, -2, "version_needed");

	lua_pushstring(L, szFileName);
	lua_setfield(L, -2, "szFileName");

	lua_pushstring(L, extraField);
	lua_setfield(L, -2, "extraField");

	lua_pushstring(L, szComment);
	lua_setfield(L, -2, "szComment");

	free(szFileName);
	szFileName = NULL;
	free(extraField);
	extraField = NULL;
	free(szComment);
	szComment = NULL;

	return 1;
}

static int Lua_unzGetCurrentFileZStreamPos64(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	uint64_t pos;

	if (!ar) {
		luaL_error(L, "userdata is nil\n");
		return 1;
	}

	pos = ozip_unzGetCurrentFileZStreamPos64(ar);

	lua_pushinteger(L, pos);

	return 1;
}

static int Lua_unzOpenCurrentFile3(lua_State* L) {
	unzFile ar = *(char**)check_unzarchive(L, 1);
	int raw = luaL_checkinteger(L, 2);
	const char* passwd = luaL_checkstring(L, 3);
	int method;
	int level;
	int err;
	if (!ar) {
		luaL_error(L, "userdata is nil\n");
		return 1;
	}
	// add default no passwd
	if (strlen(passwd) == 0) {
		passwd = NULL;
	}
	err = ozip_unzOpenCurrentFile3(ar, &method, &level, raw, NULL);
	if (err != ZIP_OK) {
		return luaL_error(L, "failure to ozip_unzOpenCurrentFile3: %d\n", err);
	}

	lua_pushinteger(L, method);
	lua_pushinteger(L, level);

	return 2;
}

static int Lua_unzCloseCurrentFile(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	int err = ozip_unzCloseCurrentFile(ar);

	lua_pushinteger(L, err);
	return 1;
}

static int Lua_unzReadCurrentFile(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	unsigned int len = luaL_checkinteger(L, 2);

	int err;
	void* buffer = malloc(len + 1);
	err = ozip_unzReadCurrentFile(ar, buffer, len);

	lua_pushinteger(L, err);
	lua_pushlstring(L, buffer, err);
	free(buffer);

	return 2;
}

static int Lua_unztell64(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	uint64_t pos;

	pos = ozip_unztell64(ar);
	lua_pushinteger(L, pos);

	return 1;
}

static int Lua_unzeof(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	int eof;

	eof = ozip_unztell64(ar);
	lua_pushinteger(L, eof);

	return 1;
}

static int Lua_unzGetLocalExtrafield(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	int size = 0;
	int len = ozip_unzGetLocalExtrafield(ar, NULL, size);
	void* buf = malloc(len + 1);
	((char*) buf)[len] = '\0';
	int err = ozip_unzGetLocalExtrafield(ar, buf, len);
	if (err != ZIP_OK) {
		return luaL_error(L, "failure to ozip_unzGetLocalExtrafield: %d\n", err);
	}

	lua_pushlstring(L, buf, len);
	free(buf);
	buf = NULL;
	return 1;
}

static int Lua_unzGetOffset64(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
	uint64_t offset;
	offset = ozip_unzGetOffset64(ar);
	lua_pushinteger(L, offset);
	return 1;
}

static int Lua_unzSetOffset64(lua_State* L) {
	unzFile ar = *check_unzarchive(L, 1);
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

	*ar = ozip_strzipOpen((void*) path, 0, 0, flags);
	if (!*ar) {
		return luaL_error(L, "failure to ozip_strzipOpen\n");
	}

	luaL_getmetatable(L, ZIPARCHIVE_MT);
	assert(!lua_isnil(L, -1));
	lua_setmetatable(L, -2);

	return 1;
}

static int Lua_strunzOpen(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	int flags = (lua_gettop(L) < 2) ? 0 : luaL_checkint(L, 2);
	unzFile* ar = (zipFile*) lua_newuserdata(L, sizeof(zipFile));

	*ar = ozip_strunzOpen((void*) path, flags, 0);
	if (!*ar) {
		return luaL_error(L, "failure to ozip_strunzOpen\n");
	}

	luaL_getmetatable(L, UNZARCHIVE_MT);
	assert(!lua_isnil(L, -1));
	lua_setmetatable(L, -2);

	return 1;
}

static int Lua_zipArchiveGC(lua_State* L) {
	zipFile* ar = check_ziparchive(L, 1);
	int err;
	if (!*ar) {
		return 0;
	}

	if ((err = Lua_zipClose(L)) == UNZ_OK) {
		return luaL_error(L, "failure to Lua_zipClose");
	}
	*ar = NULL;

	return 0;
}

static void Lua_zipArchiveRegister(lua_State* L) {
	luaL_newmetatable(L, ZIPARCHIVE_MT);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, Lua_zipArchiveGC);
	lua_setfield(L, -2, "__gc");

	lua_pushcfunction(L, Lua_zipClose);
	lua_setfield(L, -2, "close");

	lua_pushcfunction(L, Lua_zipOpenFile);
	lua_setfield(L, -2, "openFile");

	lua_pushcfunction(L, Lua_zipWriteFile);
	lua_setfield(L, -2, "writeFile");

	lua_pushcfunction(L, Lua_zipCloseFile);
	lua_setfield(L, -2, "closeFile");

	lua_pushcfunction(L, Lua_zipCloseFileRaw);
	lua_setfield(L, -2, "closeFileRaw");

	lua_pushcfunction(L, Lua_zipRemoveExtraInfoBlock);
	lua_setfield(L, -2, "removeExtraInfoBlock");

	lua_pop(L, 1);
}

static int Lua_unzArchiveGC(lua_State* L) {
	unzFile* ar = check_unzarchive(L, 1);
	int err;
	if (!*ar) {
		return 0;
	}

	if ((err = Lua_unzClose(L)) == UNZ_OK) {
		return luaL_error(L, "failure to Lua_unzClose");
	}
	*ar = NULL;
	return 0;
}

static void Lua_unzArchiveRegister(lua_State* L) {
	luaL_newmetatable(L, UNZARCHIVE_MT);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, Lua_unzArchiveGC);
	lua_setfield(L, -2, "__gc");

	lua_pushcfunction(L, Lua_unzStringFileNameCompare);
	lua_setfield(L, -2, "strFileNameCmp");

	lua_pushcfunction(L, Lua_unzClose);
	lua_setfield(L, -2, "close");

	lua_pushcfunction(L, Lua_unzGetGlobalInfo64);
	lua_setfield(L, -2, "getGlobalInfo");

	lua_pushcfunction(L, Lua_unzGetGlobalComment);
	lua_setfield(L, -2, "getGlobalComment");

	lua_pushcfunction(L, Lua_unzGoToFirstFile);
	lua_setfield(L, -2, "goToFirstFile");

	lua_pushcfunction(L, Lua_unzGoToNextFile);
	lua_setfield(L, -2, "goToNextFile");

	lua_pushcfunction(L, Lua_unzLocateFile);
	lua_setfield(L, -2, "locateFile");

	lua_pushcfunction(L, Lua_unzGetFilePos64);
	lua_setfield(L, -2, "getFilePos");

	lua_pushcfunction(L, Lua_unzGoToFilePos64);
	lua_setfield(L, -2, "gotoFilePos");

	lua_pushcfunction(L, Lua_unzGetCurrentFileInfo64);
	lua_setfield(L, -2, "getCurrentFileInfo");

	lua_pushcfunction(L, Lua_unzGetCurrentFileZStreamPos64);
	lua_setfield(L, -2, "getCurrendFileInfoZStreamPos");

	lua_pushcfunction(L, Lua_unzOpenCurrentFile3);
	lua_setfield(L, -2, "openFile");

	lua_pushcfunction(L, Lua_unzCloseCurrentFile);
	lua_setfield(L, -2, "closeFile");

	lua_pushcfunction(L, Lua_unzReadCurrentFile);
	lua_setfield(L, -2, "readFile");

	lua_pushcfunction(L, Lua_unztell64);
	lua_setfield(L, -2, "tell");

	lua_pushcfunction(L, Lua_unzeof);
	lua_setfield(L, -2, "eof");

	lua_pushcfunction(L, Lua_unzGetLocalExtrafield);
	lua_setfield(L, -2, "getLocalExtraField");

	lua_pushcfunction(L, Lua_unzGetOffset64);
	lua_setfield(L, -2, "getOffset");

	lua_pushcfunction(L, Lua_unzSetOffset64);
	lua_setfield(L, -2, "setOffset");

	lua_pop(L, 1);
}

static int Lua_ozipOR(lua_State* L) {
	int result = 0;
	int top = lua_gettop(L);

	while (top) {
		result |= luaL_checkint(L, top--);
	}
	lua_pushinteger(L, result);

	return 1;
}

static const luaL_Reg ozip_fun_list[] = {
		{ "OR", Lua_ozipOR },

		{ "fs_zipOpen",Lua_zipOpen },
		{ "fs_unzOpen", Lua_unzOpen64 },

		{ "str_zipOpen",Lua_strzipOpen },
		{ "str_unzOpen", Lua_strunzOpen },
		{ NULL, NULL }
};

#define EXPORT_CONSTANT(NAME)		\
    lua_pushnumber(L, NAME);		\
    lua_setfield(L, -2, #NAME);

LUALIB_API int luaopen__ozip(lua_State* L) {

	lua_newtable(L);
	luaL_register(L, NULL, ozip_fun_list);

	EXPORT_CONSTANT(Z_DEFLATED);
	EXPORT_CONSTANT(Z_DEFAULT_COMPRESSION);
	EXPORT_CONSTANT(Z_MAX_WBITS);
	EXPORT_CONSTANT(Z_DEFAULT_STRATEGY);
	EXPORT_CONSTANT(DEF_MEM_LEVEL);

	EXPORT_CONSTANT(ZIP_OK);
	EXPORT_CONSTANT(ZIP_EOF);
	EXPORT_CONSTANT(ZIP_ERRNO);
	EXPORT_CONSTANT(ZIP_PARAMERROR);
	EXPORT_CONSTANT(ZIP_BADZIPFILE);
	EXPORT_CONSTANT(ZIP_INTERNALERROR);

	EXPORT_CONSTANT(UNZ_OK);
	EXPORT_CONSTANT(UNZ_END_OF_LIST_OF_FILE);
	EXPORT_CONSTANT(UNZ_ERRNO);
	EXPORT_CONSTANT(UNZ_EOF);
	EXPORT_CONSTANT(UNZ_PARAMERROR);
	EXPORT_CONSTANT(UNZ_BADZIPFILE);
	EXPORT_CONSTANT(UNZ_INTERNALERROR);
	EXPORT_CONSTANT(UNZ_CRCERROR);

	Lua_zipArchiveRegister(L);
	Lua_unzArchiveRegister(L);

	return 1;
}

