#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include "read_stream.h"


/**
 *class: read_stream
 *function: read_stream
 *parameters[in]:const char, it has the log file name.
 *description:It's the constructor function of class read_stream.
*/

read_stream::read_stream(const char *fn)
{
	struct FileHead fh;
	int fd;
	memset(this->log_file_name, 0, 40);
	strcpy(this->log_file_name, fn);

	// open the file
	fd = open(this->log_file_name, O_RDONLY);
	if(fd <= 0)
	{
		LOG_ERROR("Could not open the file.");
		return;
	}

	// read the file head
	memset(&fh, 0, sizeof(struct FileHead));
	lseek(fd, 0, SEEK_SET);
	read(fd, &fh, sizeof(struct FileHead));
	if(fh.fh_startMagic != MAGIC_F_START || fh.fh_endMagic != MAGIC_F_END)
	{
		//if the file head is damaged
		LOG_ERROR("The head of file is damaged or lost.");
	}

	this->log_current_offset = fh.fh_firstIndexZone + sizeof(struct IndexZoneHead);
	close(fd);
}

/**
 *class: read_stream
 *function: ~read_stream
 *parameters:null.
 *description:It's the destructor function of class read_stream.
*/

read_stream::~read_stream()
{
	/*
	if(this->log_indexes != NULL)
	{
		free(this->log_indexes);
	}
	*/
	;
}




/**
 *class: read_stream
 *function: read_indexes_by_all
 *parameters[in] struct Index*，used to store  indexes
 *parameters[out]:all indexes in a log file, and the number of these indexes.
 *description:It's read all indexes in a log file, then store in the log_indexs.
*/
uint32_t read_stream::read_indexes_by_all(struct Index* &indexes_list)
{
	struct IndexZoneHead izh;
	struct Index i;
	uint32_t offset;
	uint32_t index_num;
	struct FileHead fh;

	int fd;
	
	// open the file
	fd = open(this->log_file_name, O_RDONLY);
	
	// read the file head
	memset(&fh, 0, sizeof(struct FileHead));
	lseek(fd, 0, SEEK_SET);
	read(fd, &fh, sizeof(struct FileHead));	

	index_num = 0;
	offset = fh.fh_firstIndexZone;

	if(indexes_list != NULL) 
	{
		free(indexes_list);
	}
	while(offset > 0)
	{
		// read the IndexZone head
		lseek(fd, offset, SEEK_SET);
		read(fd, &izh, sizeof(struct IndexZoneHead));

		indexes_list = (struct Index*) realloc(indexes_list, sizeof(struct Index)*(index_num+izh.izh_indexNum));

		//read all indexes in the IndexZone
		for(int k = 0; k < (int)izh.izh_indexNum; k++)
		{
			read(fd, &i, sizeof(struct Index));
			indexes_list[index_num].i_timestamp = i.i_timestamp;
			indexes_list[index_num++].i_location = i.i_location;
		}
		offset = izh.izh_nextIndexZone;
	}
	close(fd);
	return index_num;
}

/**
 *class: read_stream
 *function: read_ndexes_by_zone
 *parameters[in]:strunct Index*, used to store indexes in a zone.
 *parameters[out]:all indexes in a log file, and the number of these indexes.
 *description:It's read all indexes in a log file, then store in the log_indexs.
*/
uint32_t read_stream::read_indexes_by_zone(struct  Index* &indexes_list, uint16_t zone_id, uint32_t &item_num, uint32_t &start,  uint32_t &end)
{
	struct IndexZoneHead izh;
	struct Index i;
	uint32_t offset;
	uint32_t index_num;
	uint16_t zone_num;
	struct FileHead fh;
	int fd;
	
	// open the file
	fd = open(this->log_file_name, O_RDONLY);
	
	// read the file head
	memset(&fh, 0, sizeof(struct FileHead));
	lseek(fd, 0, SEEK_SET);
	read(fd, &fh, sizeof(struct FileHead));	
	index_num = 0;
	zone_num = 0;

	offset = fh.fh_firstIndexZone;
	if(indexes_list != NULL) 
	{
		free(indexes_list);
	}
	while(offset > 0)
	{
		// read the IndexZone head
		lseek(fd, offset, SEEK_SET);
		read(fd, &izh, sizeof(struct IndexZoneHead));
		
		zone_num += 1;		
		
        offset = izh.izh_nextIndexZone;

        if(zone_num < zone_id)
        {
        	continue;
        }

		indexes_list = (struct Index*) malloc(sizeof(struct Index)*(izh.izh_indexNum));
		//read all indexes in the IndexZone
		for(int k = 0; k < (int)izh.izh_indexNum; k++)
		{
			read(fd, &i, sizeof(struct Index));
			indexes_list[index_num].i_timestamp = i.i_timestamp;
			indexes_list[index_num++].i_location = i.i_location;
			
		}
		start = izh.izh_startTime;
		end = indexes_list[index_num - 1].i_timestamp;
		item_num = izh.izh_indexNum;
		break;
	}
	close(fd);
	return index_num;
}

uint32_t read_stream::get_index_num()
{
	struct IndexZoneHead izh;
	uint32_t offset;
	uint32_t index_num;
	struct FileHead fh;
	int fd;
	
	// open the file
	fd = open(this->log_file_name, O_RDONLY);
	
	// read the file head
	memset(&fh, 0, sizeof(struct FileHead));
	lseek(fd, 0, SEEK_SET);
	read(fd, &fh, sizeof(struct FileHead));	
	index_num = 0;

	offset =fh.fh_firstIndexZone;
	
	while(offset > 0)
	{
		// read the IndexZone head
		lseek(fd, offset, SEEK_SET);
		read(fd, &izh, sizeof(struct IndexZoneHead));
		index_num += izh.izh_indexNum;
		offset = izh.izh_nextIndexZone;		
	}
	close(fd);
	
	return index_num;
}
uint32_t read_stream::get_item_num()
{
	struct IndexZoneHead izh;
	uint32_t offset;
	uint32_t item_num;
	struct FileHead fh;

	int fd;
	
	// open the file
	fd = open(this->log_file_name, O_RDONLY);
	
	// read the file head
	memset(&fh, 0, sizeof(struct FileHead));
	lseek(fd, 0, SEEK_SET);
	read(fd, &fh, sizeof(struct FileHead));	
	item_num = 0;

	offset = fh.fh_firstIndexZone;

	while(offset > 0)
	{
		// read the IndexZone head
		lseek(fd, offset, SEEK_SET);
		read(fd, &izh, sizeof(struct IndexZoneHead));
		item_num += izh.izh_itemNum;
		offset = izh.izh_nextIndexZone;		
	}
	close(fd);
	return item_num;
}

uint16_t read_stream::get_index_zone_num()
{
	struct IndexZoneHead izh;
	uint32_t offset;
	uint16_t index_zone_num;
	struct FileHead fh;

	int fd;
	
	// open the file
	fd = open(this->log_file_name, O_RDONLY);
	
	// read the file head
	memset(&fh, 0, sizeof(struct FileHead));
	lseek(fd, 0, SEEK_SET);
	read(fd, &fh, sizeof(struct FileHead));	
	index_zone_num = 0;

	offset = fh.fh_firstIndexZone;
	while(offset > 0)
	{

		lseek(fd, offset, SEEK_SET);
		read(fd, &izh, sizeof(struct IndexZoneHead));
		index_zone_num += 1;
		offset = izh.izh_nextIndexZone;		
	}
	close(fd);
	return index_zone_num;
}
void read_stream::get_log_timestamp(uint32_t &start, uint32_t &end)
{
	struct IndexZoneHead izh;
	struct Index i;
	uint32_t offset, offsetB;
	struct FileHead fh;

	int fd;
	

	fd = open(this->log_file_name, O_RDONLY);

	memset(&fh, 0, sizeof(struct FileHead));
	lseek(fd, 0, SEEK_SET);
	read(fd, &fh, sizeof(struct FileHead));	
	

	start = fh.fh_startTime;
	offset = fh.fh_firstIndexZone;

	while(offset > 0)
	{
		// read the IndexZone head
		lseek(fd, offset, SEEK_SET);
		read(fd, &izh, sizeof(struct IndexZoneHead));

		offsetB = offset;
		offset = izh.izh_nextIndexZone;	
	}

	offset = offsetB + sizeof(struct IndexZoneHead) + sizeof(struct Index) * izh.izh_indexNum;
	lseek(fd, offset, SEEK_SET);
	read(fd, &i, sizeof(struct Index));
	end = i.i_timestamp;
	close(fd);
}


uint32_t read_stream::get_log_size()
{
	struct IndexZoneHead izh;
	uint32_t offset;
	struct FileHead fh;

	int fd;
	

	fd = open(this->log_file_name, O_RDONLY);

	memset(&fh, 0, sizeof(struct FileHead));
	lseek(fd, 0, SEEK_SET);
	read(fd, &fh, sizeof(struct FileHead));	

	
	offset = fh.fh_firstIndexZone;

	while(offset > 0)
	{
		// read the IndexZone head
		lseek(fd, offset, SEEK_SET);
		read(fd, &izh, sizeof(struct IndexZoneHead));
		offset = izh.izh_nextIndexZone;	
	}

	close(fd);
	return izh.izh_dataOffset;
}

void read_stream::read_stream_seek(uint32_t offset)
{
	this->log_current_offset = offset;
}

int read_stream::data_head_invalid(void)
{
	uint32_t data = 0;
	int is0num = 0;
	int fd = 0;
	int isTrue = 1;
	struct IndexZoneHead izh;
	struct FileHead fh;

	//std::cout<<"OFFSET:"<<this->log_current_offset<<std::endl;
	this->log_current_offset -= this->log_current_offset%4;
	//std::cout<<"OFFSET_to:"<<this->log_current_offset<<std::endl;
	
	fd = open(this->log_file_name, O_RDONLY);
	lseek(fd, this->log_current_offset, SEEK_SET);
	read(fd, &data, sizeof(data));
	this->log_current_offset += sizeof(data);
	isTrue = isTrue &&(data != MAGIC_F_START); 
	isTrue = isTrue &&(data != MAGIC_F_END); 
	isTrue = isTrue &&(data != MAGIC_I_START); 
	isTrue = isTrue &&(data != MAGIC_D_START); 
	isTrue = isTrue &&(data != MAGIC_I_END); 
	isTrue = isTrue &&(data != MAGIC_D_END); 
	while( isTrue)
	{
		//std::cout<<"DATA"<<data<<"True:"<<isTrue<<std::endl;
		read(fd, &data, sizeof(data));
		isTrue = isTrue &&(data != MAGIC_F_START); 
		isTrue = isTrue &&(data != MAGIC_F_END); 
		isTrue = isTrue &&(data != MAGIC_I_START); 
		isTrue = isTrue &&(data != MAGIC_D_START); 
		isTrue = isTrue &&(data != MAGIC_I_END); 
		isTrue = isTrue &&(data != MAGIC_D_END); 
		if(data == 0)
		{
			is0num += 1;
			if(is0num > 200)
			{
				break;
			}	
		}
		this->log_current_offset += sizeof(data);

	}
	if(data == MAGIC_I_START)
	{
		
		this->log_current_offset -= sizeof(data);
		lseek(fd, this->log_current_offset, SEEK_SET);
		read(fd, &izh, sizeof(struct IndexZoneHead));
		this->log_current_offset += sizeof(struct IndexZoneHead);
		this->log_current_offset += sizeof(struct Index) * izh.izh_maxIndexNum;
		return 0;
	}
	else if(data == MAGIC_I_END)
	{

		this->log_current_offset -= sizeof(struct IndexZoneHead);
		lseek(fd, this->log_current_offset, SEEK_SET);
		read(fd, &izh, sizeof(struct IndexZoneHead));
		this->log_current_offset += sizeof(struct IndexZoneHead);
		this->log_current_offset += sizeof(struct Index) * izh.izh_maxIndexNum;
		return 0;
	}
	else if(data == MAGIC_D_START)
	{
		//std::cout<<"----"<<std::endl;
		this->log_current_offset -= sizeof(data);
		//std::cout<<"this:offset:"<<this->log_current_offset<<std::endl;
		return 0;
	}
	else if(data == MAGIC_D_END)
	{
		return 0;
	}
	else if(data == MAGIC_F_START)
	{
		this->log_current_offset -= sizeof(data);
		lseek(fd, this->log_current_offset, SEEK_SET);
		read(fd, &fh, sizeof(struct FileHead));
		this->log_current_offset += sizeof(struct FileHead);
		read(fd,&izh, sizeof(struct IndexZoneHead));
		this->log_current_offset += sizeof(struct IndexZoneHead);
		this->log_current_offset += sizeof(struct Index) * izh.izh_maxIndexNum;
		return 0;
	}
	else if(data == MAGIC_F_END)
	{
		lseek(fd, this->log_current_offset, SEEK_SET);
		read(fd,&izh, sizeof(struct IndexZoneHead));
		this->log_current_offset += sizeof(struct IndexZoneHead);
		this->log_current_offset += sizeof(struct Index) * izh.izh_maxIndexNum;
		return 0;
	}
	else
	{
		return -1;
	}
	
}

int read_stream::data_tail_invalid(void)
{
	uint32_t size = 0;
	return size;
}

uint32_t read_stream::read_next(void * &ptr)
{
	int fd;
	uint32_t start;
	uint32_t end;
	uint32_t size = 0;
	uint32_t ws = 0;

	start = 0;
	fd = open(this->log_file_name, O_RDONLY);
	//std::cout<<"Data Offset: "<<this->log_current_offset<<std::endl;
	lseek(fd, this->log_current_offset, SEEK_SET);
	read(fd, &start, sizeof(start));
	this->log_current_offset += sizeof(start);
	//std::cout<<"start: "<<start<<std::endl;
	//std::cout<<"Data Offset: "<<this->log_current_offset<<std::endl;
	if(start != MAGIC_D_START)
	{
		//std::cout<<"------------------"<<std::endl;
		if(start == MAGIC_I_START)
		{
			//LOG_ERROR("data is lost.12");
			struct IndexZoneHead izh;
			this->log_current_offset -= sizeof(start);
			lseek(fd, this->log_current_offset,SEEK_SET);
			read(fd, &izh,  sizeof(IndexZoneHead));

			this->log_current_offset += sizeof(struct IndexZoneHead);
			this->log_current_offset += sizeof(struct Index) * izh.izh_maxIndexNum;

		}
		else 
		{
			//LOG_ERROR("data is lost.");
			this->log_current_offset -= sizeof(start);
			size = this->data_head_invalid();
			if(size == 0)
			{
				start = 0;
				lseek(fd, this->log_current_offset, SEEK_SET);
				read(fd, &start, sizeof(start));
				this->log_current_offset += sizeof(start);
			}
			else
			{
				//LOG_ERROR("haha it.");
				return 0;
			}
		}		
	}


	read(fd,&size, sizeof(size));
	//std::cout<<"size:"<<size<<std::endl;
	this->log_current_offset += sizeof(size);
	read(fd,&ws, sizeof(ws));
	this->log_current_offset += sizeof(ws);
	//std::cout<<"file size:"<<(int)ws <<std::endl;
	ptr = (uint8_t*) malloc(size + ws);
	read(fd, ptr, size + ws);
	this->log_current_offset += size+ws;
	read(fd, &end, sizeof(end));
	this->log_current_offset += sizeof(end);
	//std::cout<<"SIZE:"<<size<<std::endl;
	close(fd);
	return size;
}