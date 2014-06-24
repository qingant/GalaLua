#ifndef __WRITE_STREAM_H__
#define __WRITE_STREAM_H__

#include "../common.h"
#include <stdint.h>

class write_stream
{
private:
	int log_file_handle;
	char log_file_name[MAX_LOG_FILENAME_LENGTH];

	uint32_t index_zone_offset;
	uint32_t index_offset;
	uint32_t data_offset;

	uint16_t index_zone_num;
	uint16_t index_num;
	uint32_t item_num;

	uint32_t item_num_in_zone;

	uint32_t index_timestamp;
	uint32_t start_timestamp;
	uint32_t end_timestamp;
	uint16_t index_num_in_zone;


	//uint16_t zone_num_in_file;
	uint16_t max_index_num_in_zone_old;
	uint16_t max_index_num_in_zone_new;
	//uint16_t max_zone_num_in_file;
	//uint32_t max_log_file_size;
	uint8_t  index_interval_time;

public:
	write_stream(const char* fn);
	~write_stream();

	uint32_t get_log_size(void); 
	void set_log_size(uint32_t size);


	uint8_t get_index_interval(void);
	void set_index_interval(uint8_t minutes);

	void set_index_zone_size(uint16_t in);

	uint16_t get_izone_num(void);
	uint32_t get_index_num(void);
	uint32_t get_item_num(void);

	uint32_t get_start_timestamp(void);
	uint32_t get_end_timestamp(void);
	
	void backup(const char* fn);


	uint32_t write_binary(void *ptr, const uint32_t size);
};




#endif