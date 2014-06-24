#ifndef __READ_STREAM_H__
#define __READ_STREAM_H__

/**
 *author:Zhou Xiaopeng
 *date:2014/06/18
 *description: define class read_stream reading indexes from a log file.
*/


#include <stdint.h>
#include "../common.h"

class read_stream
{
private:
	//int 	 log_file_handle; 
	char 	 log_file_name[40]; 
	uint32_t log_current_offset;
	//uint32_t log_first_izone_offset;

	//uint32_t log_index_num;
	//uint16_t log_izone_num;
	//uint32_t log_item_num;
	//uint32_t log_current_index;
	//struct  Index* log_indexes;
private:
	//uint16_t read_all_indexes();
	int data_head_invalid();
	int data_tail_invalid();
public:
	read_stream(const char* fn);
	~read_stream();	
	uint32_t read_indexes_by_all(struct Index* &);
	uint32_t read_indexes_by_zone(struct Index* &, uint16_t zone_id, uint32_t &item_num, uint32_t &start, uint32_t &end);

	uint32_t get_index_num();
	uint32_t get_item_num();
	uint16_t get_index_zone_num();
	uint32_t get_log_size();
	void get_log_timestamp(uint32_t &start, uint32_t &end);

	void read_stream_seek(uint32_t offset);
	uint32_t read_next(void * &ptr);

};

#endif