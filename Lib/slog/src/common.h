#ifndef __LOG_STORE_COMMON_H__

#define __LOG_STORE_COMMON_H__

#include <iostream>
#include <stdint.h>

//all of types which BSON supports 
#define BSON_REAL 1
#define BSON_STRING 2
#define BSON_DOCUMENT 3
#define BSON_ARRAY 4
#define BSON_BINARY 5
#define BSON_UNDEFINED 6
#define BSON_OBJECTID 7
#define BSON_BOOLEAN 8
#define BSON_DATE 9
#define BSON_NULL 10
#define BSON_REGEX 11
#define BSON_DBPOINTER 12
#define BSON_JSCODE 13
#define BSON_SYMBOL 14
#define BSON_CODEWS 15
#define BSON_INT32 16
#define BSON_TIMESTAMP 17
#define BSON_INT64 18
#define BSON_MINKEY 255
#define BSON_MAXKEY 127
#define	BSON_DEFAULT_CAP	128
#define BSON_SIZE_LENGTH	2

/*
const uint8_t 	MAGIC_F_START[4] = {0x10，0x20, 0x30, 0x40};
const uint8_t 	MAGIC_I_START[4] = {0x50, 0x60, 0x70, 0x80};
const uint8_t	MAGIC_D_START[4] = {0x90, 0xA0, 0xB0, 0xC0};
const uint8_t	MAGIC_F_END[4]	= {0x40, 0x30, 0x20, 0x10};
const uint8_t	MAGIC_I_END[4]	= {0x80, 0x70, 0x60, 0x50};
const uint8_t 	MAGIC_D_END[4]  = {0xC0, 0xB0, 0xA0, 0x90};
*/

#define	MAGIC_F_START	0x10203040
#define	MAGIC_I_START	0x50607080
#define	MAGIC_D_START	0x90A0B0C0
#define	MAGIC_F_END	0x40302010
#define	MAGIC_I_END	0x80706050
#define	MAGIC_D_END	0xC0B0A090

#define MAX_LOG_NUM	10
#define MAX_LOG_FILENAME_LENGTH	20
#define MAX_INDEX_NUM_IN_INDEX_ZONE 10
#define MAX_INDEX_SIZE 8

#define FILE_DEFAULT_INDEX_ZONE_CAP	10
#define INDEX_ZONE_DEFAULT_INDEX_CAP 60

#define INDEX_INTERVAL_TIME		1 //默认单位分钟

//extern "C" int lua_write(lua_State*L);
//extern "C" int lua_read_timeRange(lua_State*L);

#define LOG_ERROR(strerror) (std::cout<<"[ERROR]:"<<strerror<<std::endl)

struct Index
{
	uint32_t i_timestamp;
	uint32_t i_location;
};

struct IndexZone
{
	uint32_t	iz_startTime;
	uint32_t	iz_endTime;
//	uint32_t	iz_nextIndexZone;
	uint16_t	iz_indexsNum;
	uint16_t	iz_indexCap;
	struct Index*	iz_indexs;
	uint32_t	iz_currentIndex;
//	uint16_t	iz_size;
	uint16_t	iz_maxIndexNum;
};

struct File
{
	uint32_t f_startTime;
	int f_fd;
	uint32_t f_endTime;
	//uint32_t f_firstIndexZone;
	uint32_t	f_indexZoneCap;
	struct IndexZone* f_indexZones;
	uint16_t f_indexZoneNum;
	uint16_t f_currentZone;
	uint16_t f_maxIndexZoneNum;
	//uint32_t f_size;
};

struct FileHead
{
	uint32_t fh_startMagic;
	uint32_t fh_startTime;
	uint32_t fh_endTime;
	uint32_t fh_firstIndexZone;
	uint32_t fh_itemNum;
	uint32_t fh_endMagic;

};

struct IndexZoneHead
{
	uint32_t izh_startMagic;
	uint32_t izh_startTime;
	uint32_t izh_endTime;

	uint32_t izh_nextIndexZone;	
	uint32_t izh_maxIndexNum;
	uint32_t izh_indexNum;
	uint32_t izh_dataOffset;
	uint32_t izh_itemNum;
	uint32_t izh_endMagic;

};

static const uint8_t fill[8]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif