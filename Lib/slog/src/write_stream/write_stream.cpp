#include <fcntl.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include "write_stream.h"


static inline void addIndexZone(struct IndexZoneHead &iz,int fd, uint32_t indexZoneOffset)
{
	lseek(fd, indexZoneOffset, SEEK_SET);
	write(fd, &iz,sizeof(struct IndexZoneHead));
}

static inline void addIndex(struct Index &i,int fd, uint32_t indexOffset)
{
	lseek(fd, indexOffset, SEEK_SET);
	write(fd, &i, sizeof(struct Index));
}

static inline size_t addData(const void *ptr, size_t size, int fd, uint32_t dataOffset)
{
	size_t s;
	lseek(fd, dataOffset, SEEK_SET);
	s = write(fd, ptr, size);
	return s;
}

static inline void fillFileNull(struct File &file)
{
	file.f_startTime = 0;
	file.f_endTime = 0;
	file.f_indexZoneNum = 0;
	file.f_indexZoneCap = 0;
	//file.f_size = 0;
	file.f_currentZone = 0;
	file.f_indexZones = NULL;
}

write_stream::write_stream(const char* fn)
{
	
	//std::cout<<"I'm write_stream constructor function."<<std::endl;

	struct FileHead fh;
	struct IndexZoneHead izh;
	struct Index i;
	uint32_t offset = 0;
	int count = 0;

	memset(&fh, 0, sizeof(struct FileHead));
	memset(&izh, 0, sizeof(struct IndexZoneHead));
	memset(&i, 0 ,sizeof(struct Index));
	memset(this->log_file_name, 0, sizeof(this->log_file_name));
	//std::cout<<"FD:"<<this->log_fd<<std::endl;

	strcpy(this->log_file_name, fn);

	this->log_file_handle = open(this->log_file_name, O_RDWR | O_CREAT);
	truncate(this->log_file_name, 1 * 1024 * 1024);

	this->index_offset = 0;
	this->index_zone_offset = 0;
	this->data_offset = 0;

	this->index_zone_num = 0;
	this->index_num = 0;
	this->item_num = 0;
	this->item_num_in_zone = 0;

	this->max_index_num_in_zone_old = 200;	
	this->max_index_num_in_zone_new = 200;	
	this->index_interval_time = 1;
	this->start_timestamp = 0;
	this->end_timestamp = 0;
	offset = 0;
	lseek(this->log_file_handle, offset ,SEEK_SET);
	read(this->log_file_handle, (char*)&fh, sizeof(struct FileHead));
	//std::cout<<"SIZE:"<<s<<(unsigned long)fh.fh_startMagic<<std::endl;

	if(fh.fh_startMagic == 0)
	{
		
		fh.fh_startMagic = MAGIC_F_START;

		fh.fh_endMagic = MAGIC_F_END;
		fh.fh_itemNum = 0;
		fh.fh_firstIndexZone = sizeof(struct FileHead);
		fh.fh_startTime = time(NULL);
		fh.fh_endTime = time(NULL);

		this->start_timestamp = fh.fh_startTime;
		this->end_timestamp = fh.fh_endTime;

		offset = 0;
		lseek(this->log_file_handle, offset ,SEEK_SET);
		write(this->log_file_handle, (char*)&fh, sizeof(struct FileHead));
	}

	if(fh.fh_startMagic != MAGIC_F_START)
	{
		LOG_ERROR("LOG File destory..!");
		this->index_timestamp = 0;
		return ;
	}
	else
	{
		this->item_num  = fh.fh_itemNum;
		offset = fh.fh_firstIndexZone;		
		this->index_zone_offset = offset;
		this->start_timestamp = fh.fh_startTime;
	
		while(offset > 0)
		{
			memset(&izh, 0, sizeof(struct IndexZoneHead));
			this->index_zone_offset = offset;
			this->index_zone_num += 1;

			lseek(this->log_file_handle, offset, SEEK_SET);
			read(this->log_file_handle, &izh, sizeof(struct IndexZoneHead));
			offset = izh.izh_nextIndexZone;
			this->index_num += izh.izh_indexNum;
		}
		this->data_offset = izh.izh_dataOffset;
		this->max_index_num_in_zone_old = izh.izh_maxIndexNum;
		count = izh.izh_indexNum;
		this->index_num_in_zone = izh.izh_indexNum;
		this->item_num_in_zone = izh.izh_itemNum;

		/*
		std::cout<<"______________________________"<<std::endl;
		std::cout<<"index_num_in_zone:"<<this->index_num_in_zone<<std::endl;
		std::cout<<"index_zone_num:"<<this->index_zone_num<<std::endl;
		std::cout<<"item_num:"<<this->item_num<<std::endl;
		std::cout<<"______________________________"<<std::endl;
		*/

		if(count <= 0)
		{
			this->index_timestamp = 0;
		}
		else
		{
			offset = this->index_zone_offset;
			offset  += sizeof(struct IndexZoneHead) + sizeof(struct Index) * (count-1);
			lseek(this->log_file_handle, offset, SEEK_SET);
			read(this->log_file_handle, &i, sizeof(struct Index));	
			this->index_timestamp = i.i_timestamp;
			this->end_timestamp = i.i_timestamp;
		}

		//this->data_offset = this->log_indexZoneOffset + sizeof(struct IndexZoneHead) + sizeof(struct Index) * MAX_INDEX_NUM_IN_INDEX_ZONE;
		this->data_offset = izh.izh_dataOffset;
		this->index_offset = this->index_zone_offset + sizeof(struct IndexZoneHead) + sizeof(struct Index) * count;
	}
}


write_stream::~write_stream()
{

	;//std::cout<<"I'm write_stream destructor function."<<std::endl;
}


void write_stream::backup(const char *bakfn)
{	
	struct FileHead fh;
	uint32_t offset = 0;
	int ret = 0;
	std::cout<<"rename from:"<<this->log_file_name<<" to:"<<bakfn<<std::endl;
	close(this->log_file_handle);
	ret = rename(this->log_file_name, bakfn);
	std::cout<<"RET:"<<ret<<std::endl;
	this->log_file_handle = open(this->log_file_name, O_WRONLY | O_CREAT);
	this->index_offset = 0;
	this->index_zone_offset = 0;
	this->data_offset = 0;
	fh.fh_startMagic = MAGIC_F_START;
	fh.fh_endMagic = MAGIC_F_END;
	fh.fh_itemNum = 0;
	fh.fh_firstIndexZone = sizeof(struct FileHead);
	fh.fh_startTime = time(NULL);
	fh.fh_startTime = time(NULL);
	offset = 0;
	lseek(this->log_file_handle, offset ,SEEK_SET);
	write(this->log_file_handle, (char*)&fh, sizeof(struct FileHead));


}

uint32_t write_stream::write_binary(void *ptr, const uint32_t size)
{
	uint32_t ws = 0;
	uint32_t t = 0 ;
	struct FileHead fh;
	struct Index i;
	struct IndexZoneHead izh;
	uint32_t s,e;
	s = MAGIC_D_START;
	e = MAGIC_D_END;

	if(this->index_num_in_zone > this->max_index_num_in_zone_old)
	{
		t= time(NULL);

		lseek(this->log_file_handle, 0, SEEK_SET);
		read(this->log_file_handle, &fh, sizeof(struct FileHead));

		fh.fh_endTime = t;
		this->end_timestamp = t;
		fh.fh_itemNum += this->item_num_in_zone;
		this->item_num += this->item_num_in_zone;
		

		lseek(this->log_file_handle, 0, SEEK_SET);
		write(this->log_file_handle, &fh, sizeof(struct FileHead));

		lseek(this->log_file_handle, this->index_zone_offset, SEEK_SET);
		read(this->log_file_handle, &izh, sizeof(struct IndexZoneHead));

		izh.izh_endTime = t;
		izh.izh_nextIndexZone = this->data_offset;
		izh.izh_indexNum = this->index_num_in_zone;
		izh.izh_itemNum = this->item_num_in_zone;
		lseek(this->log_file_handle, this->index_zone_offset, SEEK_SET);
		write(this->log_file_handle, &izh, sizeof(struct IndexZoneHead));
		
		this->item_num_in_zone = 0;
		this->index_num_in_zone = 0;
		this->index_zone_offset = this->data_offset;
		this->max_index_num_in_zone_old = this->max_index_num_in_zone_new;
		
	}

	if(this->index_num_in_zone <= 0)
	{
		//std::cout<<"addIndexZone"<<std::endl;
		t = time(NULL);
		this->index_timestamp = t;
		this->index_offset = this->index_zone_offset + sizeof(struct IndexZoneHead);
		this->data_offset = this->index_zone_offset + sizeof(struct IndexZoneHead) + sizeof(struct Index) * MAX_INDEX_NUM_IN_INDEX_ZONE;	
		memset(&izh, 0, sizeof(struct IndexZoneHead));
		izh.izh_startTime = t;
		izh.izh_endTime = t;

		izh.izh_nextIndexZone = 0;
		izh.izh_indexNum = 0;
		izh.izh_itemNum = 0;
		izh.izh_maxIndexNum = this->max_index_num_in_zone_new;
		
		izh.izh_startMagic = MAGIC_I_START;
		izh.izh_endMagic = MAGIC_I_END;
		addIndexZone(izh, this->log_file_handle,this->index_zone_offset);

		this->index_zone_num += 1;
		this->max_index_num_in_zone_old = this->max_index_num_in_zone_new;
	}

	t = time(NULL);
	//std::cout<<"Index intervalTime:"<<(t-this->index_timestamp)<<std::endl;
	if((t - this->index_timestamp) >= (this->index_interval_time*60) || this->index_num_in_zone <= 0)
	{
		//std::cout<<"addIndex"<<std::endl;
		i.i_timestamp = t;
		i.i_location = this->data_offset;
		this->index_timestamp = t;
		this->end_timestamp = t;
		addIndex(i,this->log_file_handle,this->index_offset);
		this->index_offset += sizeof(struct Index);
		
		this->end_timestamp = t;
		lseek(this->log_file_handle, this->index_zone_offset, SEEK_SET);
		read(this->log_file_handle, &izh, sizeof(struct IndexZoneHead));
		izh.izh_indexNum += 1;
		this->index_num += 1;
		this->index_num_in_zone += 1;
		lseek(this->log_file_handle, this->index_zone_offset, SEEK_SET);
		write(this->log_file_handle, &izh, sizeof(struct IndexZoneHead));

	}
	//std::cout<<"addData"<<std::endl;
	//std::cout<<"DataOffset:"<<this->data_offset<<std::endl;

	//add data header
	addData(&s,sizeof(s),this->log_file_handle,this->data_offset);	
	this->data_offset += sizeof(s);
	
	//add data size
	//ws = size + size%8?0:(8-size%8);
	addData(&size,sizeof(size),this->log_file_handle,this->data_offset);
	this->data_offset += sizeof(size);

	//std::cout<<"size:"<<size<<std::endl;
	ws = (size%8 == 0)?0:(8-size%8);
	//std::cout<<"ws:"<<ws<<std::endl;
	addData(&ws,sizeof(ws),this->log_file_handle,this->data_offset);
	this->data_offset += sizeof(ws);

	//add data
	addData(ptr,size,this->log_file_handle,this->data_offset);	
	this->data_offset += size;

	//8  Byte alignment
	if(size%8 != 0)
	{
		addData(fill, (8-size%8), this->log_file_handle, this->data_offset);
		this->data_offset += (8-size%8);
	}
	//
	addData(&e, sizeof(e),this->log_file_handle, this->data_offset);
	this->data_offset += sizeof(e);
	this->item_num_in_zone += 1;

	
	lseek(this->log_file_handle, this->index_zone_offset, SEEK_SET);	
	read(this->log_file_handle, &izh, sizeof(struct IndexZoneHead));

	izh.izh_dataOffset = this->data_offset;
	izh.izh_itemNum = this->item_num_in_zone;
	lseek(this->log_file_handle, this->index_zone_offset, SEEK_SET);	
	write(this->log_file_handle, &izh, sizeof(struct IndexZoneHead));

	/*
	std::cout<<"___________________________________________"<<std::endl;
	std::cout<<"index_num_in_zone:"<<this->index_num_in_zone<<std::endl;
	std::cout<<"index_zone_num:"<<this->index_zone_num<<std::endl;
	std::cout<<"item_num:"<<this->item_num<<std::endl;
	std::cout<<"___________________________________________"<<std::endl;
	*/
	return size + ws;
}


uint32_t write_stream::get_log_size(void)
{
	return this->data_offset;
}

uint8_t write_stream::get_index_interval(void)
{
	return this->index_interval_time;
}

void write_stream::set_index_interval(uint8_t minutes)
{
	this->index_interval_time = minutes;
}

void write_stream::set_index_zone_size(uint16_t in)
{
	this->max_index_num_in_zone_new = in;
}

uint16_t write_stream::get_izone_num(void)
{
	return this->index_zone_num;
}

uint32_t write_stream::get_index_num(void)
{
	return this->index_num;
}

uint32_t write_stream::get_item_num(void)
{
	return this->item_num + this->item_num_in_zone;
}

uint32_t write_stream::get_start_timestamp(void)
{
	return this->start_timestamp;
}
uint32_t write_stream::get_end_timestamp(void)
{
	return this->end_timestamp;
}