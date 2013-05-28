/*
 * _ozip.c
 *
 *  Created on: 2013-5-16
 *      Author: ocaml
 */

#include "_ozip.h"
#include "util.h"
#include <stdio.h>
#include "crypt.h"

#ifndef MAXFILENAMEINZIP
#define MAXFILENAMEINZIP (256)
#endif

#ifndef BUFREADCOMMENT
#define BUFREADCOMMENT (0x400)
#endif

uLong search_central_dir(const ozip_filefunc* filefunc,voidpf filestream)
{
    unsigned char* buf;
    uLong uSizeFile;
    uLong uBackRead;
    uLong uMaxBack=0xffff; /* maximum size of global comment */
    uLong uPosFound=0;

    if (ZSEEK(*filefunc,filestream,0,OZIP_SEEK_END) != 0) {
        return 0;
    }

    uSizeFile = ZTELL(*filefunc,filestream);
    OZIP_DEBUG(" ====== uSizeFile = %lu\n", uSizeFile);

    if (uMaxBack>uSizeFile)
    {
    	uMaxBack = uSizeFile;
    }

    /* BUFREADCOMMENT + 4 */
    buf = (unsigned char*)ALLOC(BUFREADCOMMENT+4);
    if (buf==NULL)
        return 0;

    uBackRead = 4;
    while (uBackRead<uMaxBack)
    {
        uLong uReadSize,uReadPos ;
        int i;
        if (uBackRead+BUFREADCOMMENT>uMaxBack)
            uBackRead = uMaxBack;
        else
            uBackRead+=BUFREADCOMMENT;
        uReadPos = uSizeFile-uBackRead ;

        uReadSize = ((BUFREADCOMMENT+4) < (uSizeFile-uReadPos)) ?
                     (BUFREADCOMMENT+4) : (uSizeFile-uReadPos);

        if (ZSEEK(*filefunc, filestream, uReadPos, OZIP_SEEK_SET)!=0)
            break;

        if (ZREAD(*filefunc, filestream, buf, uReadSize)!=uReadSize)
            break;

        //Central Direct 0x06054b50
		for (i = (int) uReadSize - 3; (i--) > 0;)
		{
			if (((*(buf + i)) == 0x50) && ((*(buf + i + 1)) == 0x4b)
					&& ((*(buf + i + 2)) == 0x05)&& ((*(buf + i + 3)) == 0x06))
			{
				uPosFound = uReadPos + i;
				break;
			}
		}
        if (uPosFound!=0)
        {
        	break;
        }
    }

    TRYFREE(buf);

    fprintf(stderr, "central end of file size = %lu \n", uPosFound);
    fprintf(stderr, "\n0x%02x%02x%02x%2x \n", *(buf+uPosFound+3), *(buf+uPosFound+2), *(buf+uPosFound+1), *(buf+uPosFound));
    return uPosFound;
}


zFile zf_archive_open( voidp path, ozip_filefunc* filefunc, uLong size, uLong limit)
{

	ozip zf;
	ozip* pzf;
    uLong central_pos=0,uL=0;
//    uLong number_disk;
//    uLong number_disk_with_CD;
//    uLong number_entry_CD;

    int err=OZIP_OK;

	if (filefunc == NULL)
	{
		fill_fopen_filefunc(&zf.z_filefunc);
	}
	else
	{
		zf.z_filefunc = *filefunc;
	}

    zf.filestream= (*(zf.z_filefunc.zopen_file))(zf.z_filefunc.opaque,path,
                                                 OZIP_MODE_READ | OZIP_MODE_EXISTING,
                                                 size, limit);
    if (zf.filestream==NULL) {
    	OZIP_DEBUG("failure to open %s \n", path);
        return NULL;
    }

    central_pos = search_central_dir(&zf.z_filefunc,zf.filestream);
    if (central_pos==0)
    {
    	OZIP_DEBUG("failure to search_central_dir in archive %s \n", path);
        err=OZIP_ERRNO;
        return NULL;
    }

    if (ZSEEK(zf.z_filefunc, zf.filestream,central_pos, OZIP_SEEK_SET)!=0) {
        err=OZIP_ERRNO;
    }

    /* the signature, already checked */
    if (get_long(&zf.z_filefunc, zf.filestream, &uL)!=OZIP_OK) {
    	OZIP_DEBUG("error with zipfile in get_long \n");
    	err=OZIP_ERRNO;
    }

    /* number of this disk */
    if (get_short(&zf.z_filefunc, zf.filestream,&zf.eof_cd.num_disk)!=OZIP_OK)
        err=OZIP_ERRNO;

    /* number of the disk with the start of the central directory */
    if (get_short(&zf.z_filefunc, zf.filestream,&zf.eof_cd.num_disk_with_CD)!=OZIP_OK)
        err=OZIP_ERRNO;

    /* total number of entries in the central dir on this disk */
    if (get_short(&zf.z_filefunc, zf.filestream,&zf.eof_cd.sum_entries)!=OZIP_OK)
        err=OZIP_ERRNO;

    /* total number of entries in the central dir */
    if (get_short(&zf.z_filefunc, zf.filestream, &zf.eof_cd.sum_entries_in_CD)!=OZIP_OK)
        err=OZIP_ERRNO;

    /* check some data in central directory */
    if ((zf.eof_cd.sum_entries_in_CD!=zf.eof_cd.sum_entries) ||
        (zf.eof_cd.num_disk_with_CD!=0) ||
        (zf.eof_cd.num_disk!=0))
        err=OZIP_BADZIPFILE;

    /* size of the central directory */
    if (get_long(&zf.z_filefunc, zf.filestream,&zf.eof_cd.size_central_dir)!=OZIP_OK)
        err=OZIP_ERRNO;

    /* offset of start of central directory with respect to the starting disk number */
    if (get_long(&zf.z_filefunc, zf.filestream,&zf.eof_cd.offset_central_dir)!=OZIP_OK)
        err=OZIP_ERRNO;

    /* zipfile comment length */
    if (get_short(&zf.z_filefunc, zf.filestream,&zf.eof_cd.comment_len)!=OZIP_OK)
        err=OZIP_ERRNO;

    if ((central_pos<zf.eof_cd.offset_central_dir+zf.eof_cd.size_central_dir) && (err==OZIP_OK))
        err=OZIP_BADZIPFILE;

    if (err!=OZIP_OK)
    {
        ZCLOSE(zf.z_filefunc, zf.filestream);
        return NULL;
    }

    zf.zip_start_pos = central_pos - (zf.eof_cd.offset_central_dir+zf.eof_cd.size_central_dir);
    zf.central_direct_pos = central_pos;
//    zf.pfile_in_zip_read = NULL;
    zf.encrypted = 0;


    pzf=(ozip*)ALLOC(sizeof(ozip));
    if (pzf!=NULL) {
        *pzf=zf;
        //unzGoToFirstFile((zFile)pzf);
    }
    else
    {
    	//here should close the open file
    }

    return (zFile)pzf;
}

int zf_archive_close(zFile zf) {
	TRYFREE(zf);
	return 0;
}

char* zf_archive_get_comment(zFile file, char *name) {
	return NULL;
}

int zf_archive_set_comment(zFile file, char *name, char* commet) {
	return 0;
}


uLong zf_archive_get_num_files(zFile file) {
	ozip* zf = (ozip*)file;
	if (NULL == file)
	{
		return OZIP_ERRNO;
	}
	return zf->eof_cd.sum_entries_in_CD;
}


//static walk
static uLong go_to_next_file(zFile file, uLong pre_start_pos, uLong* pos) {
	uLong filename_size=0;
	uLong extra_size=0;
	uLong commit_size=0;
	uLong off_set = pre_start_pos + 28;
	ozip* zf = (ozip*)file;
	int err;

	err = ZSEEK(zf->z_filefunc, zf->filestream, off_set, SEEK_SET);
//	if(err != OZIP_ERRNO) {
//		return err;
//	}
	err = get_short(&zf->z_filefunc, zf->filestream, &filename_size);
	if(err != OZIP_OK) {
			return err;
	}
	err = get_short(&zf->z_filefunc, zf->filestream, &extra_size);
	if(err != OZIP_OK) {
			return err;
	}
	err = get_short(&zf->z_filefunc, zf->filestream, &commit_size);
	if(err != OZIP_OK) {
			return err;
	}

	*pos = pre_start_pos + 46 + filename_size + extra_size + commit_size;

	return err;
}
//
//static uLong go_to_first_file(zFile file) {
//	ozip* zf = (ozip*)file;
//	uLong filename_size=0;
//	uLong extra_size=0;
//	uLong commit_size=0;
//	uLong off_set =  28 + zf->eof_cd.offset_central_dir;
//	uLong next_file_start_pos;
//
//	ZSEEK(zf->z_filefunc, zf->filestream, off_set, SEEK_SET);
//	get_short(zf->z_filefunc, zf->filestream, &filename_size);
//	get_short(zf->z_filefunc, zf->filestream, &extra_size);
//	get_short(zf->z_filefunc, zf->filestream, &commit_size);
//
//	next_file_start_pos = zf->eof_cd.offset_central_dir + 0x2e + filename_size + extra_size + commit_size;
//
//	return next_file_start_pos;
//}



static int zf_archive_get_filename(zFile archive, uLong pos, char* buffer, int len) {
	uLong filename_size=0;
	uLong off_set = pos + 28;
	ozip* zf = (ozip*)archive;
	int err;
	off_t iRet;

	iRet = ZSEEK(zf->z_filefunc, zf->filestream, off_set, SEEK_SET);
	if (iRet == -1) {
		err = OZIP_ERRNO;
	}
	err = get_short(&zf->z_filefunc, zf->filestream, &filename_size);

	off_set =  46 + pos;
	iRet = ZSEEK(zf->z_filefunc, zf->filestream, off_set, SEEK_SET);
	if (iRet == -1) {
			err = OZIP_ERRNO;
	}

    if ((err==OZIP_OK) && (buffer!=NULL))
    {
        uLong uSizeRead ;
        if (filename_size < len)
        {
            *(buffer+filename_size)='\0';
            uSizeRead = filename_size;
        }
        else
        {
        	uSizeRead = len;
        }

        if ((filename_size>0) && (len>0))
        {
        	if (ZREAD(zf->z_filefunc, zf->filestream,buffer,uSizeRead)!=uSizeRead)
        	{
        		err=OZIP_ERRNO;
        	}
        }
    }

	return err;
}

/*
 * return the filename's central directory head pos in the archive
 *
 */
int zf_archive_locate_file(zFile file, const char* pathname, int icase, uLong* pos) {
	int err;
	ozip* zf = (ozip*)file;
	char szFileName[MAXFILENAMEINZIP+1];
	uLong off_set = zf->eof_cd.offset_central_dir+zf->zip_start_pos;
//	uLong off_set = zf->central_direct_pos
	uLong search_start_pos;

    do {
    	search_start_pos = off_set;
        err = zf_archive_get_filename(file, search_start_pos, szFileName, sizeof(szFileName));
        if (err == OZIP_OK)
        {
            if (string_file_name_compare(szFileName, pathname,icase)==0)
            {
            	*pos = off_set;
            	return OZIP_OK;
            }
            err = go_to_next_file(file, search_start_pos, &off_set);
        }
	} while (err == OZIP_OK);

	return err;
}


int zf_get_file_info(zFile file, oentry* entry, uLong pos) {
    ozip* zf = (ozip*)file;
    int err = OZIP_OK;
    uLong uMagic=0;

    if (file==NULL || NULL==entry) {
    	return OZIP_PARAMERROR;
    }
    if (ZSEEK(zf->z_filefunc, zf->filestream, pos, OZIP_SEEK_SET)!=0) {
    	err=OZIP_ERRNO;
    }

    /* we check the magic */
    if (err==OZIP_OK)
    {
        if (get_long(&zf->z_filefunc,zf->filestream,&uMagic) != OZIP_OK)
        {
        	err=OZIP_ERRNO;
        	return err;
        }
        else if (uMagic!=0x02014b50)
        {
        	err=OZIP_BADZIPFILE;
        	return err;
        }
    }

	/* get the information about the file */
	if (get_short(&zf->z_filefunc, zf->filestream, &entry->head.version)!= OZIP_OK)
		err = OZIP_ERRNO;

	if (get_short(&zf->z_filefunc, zf->filestream,&entry->head.version_needed) != OZIP_OK)
		err = OZIP_ERRNO;

	if (get_short(&zf->z_filefunc, zf->filestream, &entry->head.flag) != OZIP_OK)
		err = OZIP_ERRNO;

	if (get_short(&zf->z_filefunc, zf->filestream, &entry->head.comp_method) != OZIP_OK)
		err = OZIP_ERRNO;

	if (get_long(&zf->z_filefunc, zf->filestream, &entry->head.dos_date) != OZIP_OK)
		err = OZIP_ERRNO;

//	DosDateToTmuDate(file_info.dosDate, &file_info.tmu_date);

	if (get_long(&zf->z_filefunc, zf->filestream, &entry->head.crc32) != OZIP_OK)
		err = OZIP_ERRNO;

	if (get_long(&zf->z_filefunc, zf->filestream,&entry->head.comp_size) != OZIP_OK)
		err = OZIP_ERRNO;

	if (get_long(&zf->z_filefunc, zf->filestream,&entry->head.uncomp_size) != OZIP_OK)
		err = OZIP_ERRNO;

	if (get_short(&zf->z_filefunc, zf->filestream,&entry->head.file_name_len) != OZIP_OK)
		err = OZIP_ERRNO;

	if (get_short(&zf->z_filefunc, zf->filestream,&entry->head.extra_field_len) != OZIP_OK)
		err = OZIP_ERRNO;

	if (get_short(&zf->z_filefunc, zf->filestream,&entry->head.comment_len) != OZIP_OK)
		err = OZIP_ERRNO;

	if (get_short(&zf->z_filefunc, zf->filestream,&entry->head.disk_number_start) != OZIP_OK)
		err = OZIP_ERRNO;

	if (get_short(&zf->z_filefunc, zf->filestream, &entry->head.inter_attr)!= OZIP_OK)
		err = OZIP_ERRNO;

	if (get_long(&zf->z_filefunc, zf->filestream, &entry->head.exter_attr)!= OZIP_OK)
		err = OZIP_ERRNO;

	if (get_long(&zf->z_filefunc, zf->filestream,&entry->head.offset_entry) != OZIP_OK)
		err = OZIP_ERRNO;

	return err;
}


int zf_archive_stat_file(zFile file, const char* pathname) {
	return 0;
}


static int check_local_head (zf,entry, piSizeVar,
                                                 poffset_local_extrafield,
                                                 psize_local_extrafield)
    ozip* zf;
	oentry* entry;
    uInt* piSizeVar;
    uLong* poffset_local_extrafield;
    uInt* psize_local_extrafield;
{
    uLong uMagic,uData,uFlags;
    uLong size_filename;
    uLong size_extra_field;
    int err=OZIP_OK;

    *piSizeVar = 0;
    *poffset_local_extrafield = 0;
    *psize_local_extrafield = 0;

    if (ZSEEK(zf->z_filefunc, zf->filestream,(zf->zip_start_pos+entry->head.offset_entry),OZIP_SEEK_SET)!=0)
    {
    	return OZIP_ERRNO;
    }


    if (err==OZIP_OK)
    {
        if (get_long(&zf->z_filefunc, zf->filestream,&uMagic) != OZIP_OK)
        {
        	err=OZIP_ERRNO;
        }
        /* the header of a entry */
        else if (uMagic!=0x04034b50)
        {
        	err=OZIP_BADZIPFILE;
        }
    }

    OZIP_DEBUG("uMagic check = %xlu", uMagic);
    /*version needed to extract*/
    if (get_short(&zf->z_filefunc, zf->filestream,&uData) != OZIP_OK)
        err=OZIP_ERRNO;
/*
    else if ((err==OZIP_OK) && (uData!=s->cur_file_info.wVersion))
        err=OZIP_BADZIPFILE;
*/
    //general purpose bit flag
    if (get_short(&zf->z_filefunc, zf->filestream,&uFlags) != OZIP_OK)
        err=OZIP_ERRNO;

    //compression method
    if (get_short(&zf->z_filefunc, zf->filestream,&uData) != OZIP_OK)
    {
    	err=OZIP_ERRNO;
    }
    else if ((err==OZIP_OK) && (uData!=entry->head.comp_method))
    {
    	err=OZIP_BADZIPFILE;
    }

    if ((err==OZIP_OK) && (entry->head.comp_method!=0) &&
/* #ifdef HAVE_BZIP2 */
                         (entry->head.comp_method!=Z_BZIP2ED) &&
/* #endif */
                         (entry->head.comp_method!=Z_DEFLATED))
        err=OZIP_BADZIPFILE;

    //last mod file time && last mod file date
    if (get_long(&zf->z_filefunc, zf->filestream,&uData) != OZIP_OK) /* date/time */
    {
    	err=OZIP_ERRNO;
    }

    //crc-32
    if (get_long(&zf->z_filefunc, zf->filestream,&uData) != OZIP_OK) /* crc */
    {
    	err=OZIP_ERRNO;
    }
    else if ((err==OZIP_OK) && (uData!=entry->head.crc32) && ((uFlags & 8)==0))
    {
    	err=OZIP_BADZIPFILE;
    }

    //compressed size
    if (get_long(&zf->z_filefunc, zf->filestream,&uData) != OZIP_OK) /* size compr */
    {
    	err=OZIP_ERRNO;
    }
    else if ((err==OZIP_OK) && (uData!= entry->head.comp_size) && ((uFlags & 8)==0))
    {
        err=OZIP_BADZIPFILE;
    }
    //uncompressed size
    if (get_long(&zf->z_filefunc, zf->filestream,&uData) != OZIP_OK) /* size uncompr */
    {
        err=OZIP_ERRNO;
    }
    else if ((err==OZIP_OK) && (uData!=entry->head.uncomp_size) && ((uFlags & 8)==0))
    {
        err=OZIP_BADZIPFILE;
    }

    //file name length
    if (get_short(&zf->z_filefunc, zf->filestream,&size_filename) != OZIP_OK)
    {
    	err=OZIP_ERRNO;
    }
    else if ((err==OZIP_OK) && (size_filename!=entry->head.file_name_len))
    {
    	err=OZIP_BADZIPFILE;
    }

    *piSizeVar += (uInt)size_filename;

    // extra field length
    if (get_short(&zf->z_filefunc, zf->filestream,&size_extra_field) != OZIP_OK)
    {
    	err=OZIP_ERRNO;
    }

    *poffset_local_extrafield= entry->head.offset_entry + SIZEZIPLOCALHEADER + size_filename;
    *psize_local_extrafield = (uInt)size_extra_field;

    *piSizeVar += (uInt)size_extra_field;

    return err;
}


File zf_archive_open_file(zFile zfile, const char* pathname,
		char* passwd, int* level, int* method, int raw, int icase) {

    int err=OZIP_OK;
    uInt iSizeVar=0;
    ozip* zf = (ozip*)zfile;

    oentry* file_obj=NULL;
    uLong offset_local_extrafield=0;  /* offset of the local extra field */
    uInt  size_local_extrafield=0;    /* size of the local extra field */
    uLong _pos=0;

    /* Return the File Object */

#    ifndef NOUNCRYPT
    	char source[12];
#    else
    if (password != NULL)
        return OZIP_PARAMERROR;
#    endif

    if (zfile==NULL)
    {
    	err = OZIP_PARAMERROR;
    }
    zf=(ozip*)zfile;
//    if (!zf->current_file_ok)
//        return OZIP_PARAMERROR;

//    if (zf->pfile_in_zip_read != NULL)
//        unzCloseCurrentFile(file);

    file_obj = (oentry*)ALLOC(sizeof(oentry));
    if (file_obj==NULL)
    {
    	err = OZIP_INTERNALERROR;
    }


    err = zf_archive_locate_file(zfile, pathname, icase, &_pos);
    if(OZIP_OK != err) {
    	return NULL;
    }


    /* Get the file information from central directory */
    if (zf_get_file_info(zfile,file_obj, _pos) != OZIP_OK) {
    	return NULL;
    }

    /* Check file information between central directory and local head */
    if (check_local_head(zf,file_obj, &iSizeVar,&offset_local_extrafield,&size_local_extrafield)!=OZIP_OK)
    {
    	err = OZIP_BADZIPFILE;
    }

    file_obj->read_buffer=(char*)ALLOC(OZIP_BUFSIZE);
//    file_obj->head.offset_entry;
    file_obj->offset_local_extrafield = offset_local_extrafield;
    file_obj->size_local_extrafield = size_local_extrafield;
    file_obj->pos_local_extrafield=0;
    file_obj->raw=raw;

    if (file_obj->read_buffer==NULL)
    {
//        TRYFREE(file_obj);
        err =  OZIP_INTERNALERROR;
    }

    file_obj->stream_initialised=0;

    if (method!=NULL)
        *method = (int)file_obj->head.comp_method;

    if (level!=NULL)
    {
        *level = 6;
        switch (file_obj->head.flag & 0x06)
        {
          case 6 : *level = 1; break;
          case 4 : *level = 2; break;
          case 2 : *level = 9; break;
        }
    }

    if ((file_obj->head.comp_method!=0) &&
/* #ifdef HAVE_BZIP2 */
        (file_obj->head.comp_method!=Z_BZIP2ED) &&
/* #endif */
        (file_obj->head.comp_method!=Z_DEFLATED))
        err=OZIP_BADZIPFILE;

//    file_object->crc32_wait=zf->cur_file_info.crc;
    file_obj->sum_crc32=0;
//    file_object->compression_method =  zf->cur_file_info.compression_method;
//    file_object->filestream=zf->filestream;
//    file_object->z_filefunc=zf->z_filefunc;
//    file_object->byte_before_the_zipfile=zf->byte_before_the_zipfile;

    file_obj->stream.total_out = 0;

    if ((file_obj->head.comp_method==Z_BZIP2ED) && (!raw))
    {
#ifdef HAVE_BZIP2
      file_obj->bstream.bzalloc = (void *(*) (void *, int, int))0;
      file_obj->bstream.bzfree = (free_func)0;
      file_obj->bstream.opaque = (voidpf)0;
      file_obj->bstream.state = (voidpf)0;

      file_obj->stream.zalloc = (alloc_func)0;
      file_obj->stream.zfree = (free_func)0;
      file_obj->stream.opaque = (voidpf)0;
      file_obj->stream.next_in = (voidpf)0;
      file_obj->stream.avail_in = 0;

      err=BZ2_bzDecompressInit(&file_obj->bstream, 0, 0);
      if (err == Z_OK)
        file_obj->stream_initialised=Z_BZIP2ED;
      else
      {
        TRYFREE(file_obj);
        return err;
      }
#else
      file_obj->raw=1;
#endif
    }
    else
    if ((file_obj->head.comp_method==Z_DEFLATED) &&
        (!raw))
    {
      file_obj->stream.zalloc = (alloc_func)0;
      file_obj->stream.zfree = (free_func)0;
      file_obj->stream.opaque = (voidpf)0;
      file_obj->stream.next_in = (voidpf)0;
      file_obj->stream.avail_in = 0;

      err=inflateInit2(&file_obj->stream, -MAX_WBITS);
      if (err == Z_OK)
        file_obj->stream_initialised=Z_DEFLATED;
      else
      {
        TRYFREE(file_obj);
        return NULL;
      }
    }
//    file_object->rest_comp = file_ob ;
//    file_object->rest_uncomp = zf->cur_file_info.uncompressed_size ;


    file_obj->pos_in_zipfile = file_obj->head.offset_entry + SIZEZIPLOCALHEADER + iSizeVar;

    file_obj->stream.avail_in = (uInt)0;

//    zf->pfile_in_zip_read = file_object;

    zf->encrypted = 0;

#		ifndef NOUNCRYPT
    if (passwd != NULL)
    {
        int i;
        zf->pcrc_32_tab = get_crc_table();
        init_keys(passwd,zf->keys,zf->pcrc_32_tab);
        if (ZSEEK(zf->z_filefunc, zf->filestream,file_obj->pos_in_zipfile + zf->zip_start_pos,SEEK_SET)!=0)
            return NULL;
        if(ZREAD(zf->z_filefunc, zf->filestream,source, 12)<12)
            return NULL;

        for (i = 0; i<12; i++)
            zdecode(zf->keys,zf->pcrc_32_tab,source[i]);

        file_obj->pos_in_zipfile+=12;
        zf->encrypted=1;
    }
#    endif

    if (err!=OZIP_OK)
    {
    	TRYFREE(file_obj);
        return NULL;
    }

    file_obj->zip = zf;
    return file_obj;
}

/*
 * File in zip api
 */
int zf_file_close(File file) {
	TRYFREE(file);
	return 0;
}

int zf_file_read(oentry* file, char* buffer, uLong len) {
    int err=OZIP_OK;
    uInt iRead = 0;
    uLong rest_comp;
    uLong rest_uncomp;


    if (file==NULL)
    {
    	return OZIP_PARAMERROR;
    }
    if ((file->read_buffer == NULL))
    {
    	return OZIP_END_OF_LIST_OF_FILE;
    }
    if (len==0)
    {
    	return 0;
    }

    file->stream.next_out = (Bytef*)buffer;
    file->stream.avail_out = (uInt)len;
    rest_comp = file->head.comp_size;
    rest_uncomp = file->head.uncomp_size;


    if ((len>rest_uncomp) && (!(file->raw)))
    {
    	file->stream.avail_out = (uInt)rest_uncomp;
    }

    if ((len>rest_comp+file->stream.avail_in) && (file->raw))
    {
    	file->stream.avail_out = (uInt)rest_comp+ file->stream.avail_in;
    }

    OZIP_DEBUG(" === rest_uncomp = %ld  and rest_comp = %ld\n", rest_uncomp, rest_comp);

    while (file->stream.avail_out>0)
    {
        if ((file->stream.avail_in==0) && (rest_comp>0))
        {
            uInt uReadThis = OZIP_BUFSIZE;
            if (rest_comp<uReadThis)
            {
            	uReadThis = (uInt)rest_comp;
            }
            if (uReadThis == 0)
            {
            	return OZIP_EOF;
            }
            if (ZSEEK(file->zip->z_filefunc,file->zip->filestream, file->pos_in_zipfile + file->zip->zip_start_pos, OZIP_SEEK_SET)!=0)
            {
            	return OZIP_ERRNO;
            }
            if (ZREAD(file->zip->z_filefunc,file->zip->filestream,file->read_buffer,uReadThis)!=uReadThis)
            {
            	return OZIP_ERRNO;
            }


#         ifndef NOUNCRYPT
            if(file->zip->encrypted)
            {
                uInt i;
                for(i=0;i<uReadThis;i++)
                  file->read_buffer[i] =zdecode(file->zip->keys,file->zip->pcrc_32_tab,file->read_buffer[i]);
            }
#         endif


            file->pos_in_zipfile += uReadThis;

            rest_comp-=uReadThis;

            file->stream.next_in =(Bytef*)file->read_buffer;
            file->stream.avail_in = (uInt)uReadThis;
        }

        if ((file->head.comp_method==0) || (file->raw))
        {
            uInt uDoCopy,i ;

            if ((file->stream.avail_in == 0) && (rest_comp == 0))
            {
            	return (iRead==0) ? OZIP_EOF : iRead;
            }

            if (file->stream.avail_out < file->stream.avail_in)
            {
            	uDoCopy = file->stream.avail_out ;
            }
            else
            {
            	uDoCopy = file->stream.avail_in ;
            }

            for (i=0;i<uDoCopy;i++)
            {
            	*(file->stream.next_out+i) = *(file->stream.next_in+i);
            }

            file->sum_crc32 = crc32(file->head.crc32,file->stream.next_out,uDoCopy);
            rest_comp-=uDoCopy;
            file->stream.avail_in -= uDoCopy;
            file->stream.avail_out -= uDoCopy;
            file->stream.next_out += uDoCopy;
            file->stream.next_in += uDoCopy;
            file->stream.total_out += uDoCopy;
            iRead += uDoCopy;
        }
        else
        if (file->head.comp_method==Z_BZIP2ED)
        {
#ifdef HAVE_BZIP2
            uLong uTotalOutBefore,uTotalOutAfter;
            const Bytef *bufBefore;
            uLong uOutThis;

            file->bstream.next_in        = file->stream.next_in;
            file->bstream.avail_in       = file->stream.avail_in;
            file->bstream.total_in_lo32  = file->stream.total_in;
            file->bstream.total_in_hi32  = 0;
            file->bstream.next_out       = file->stream.next_out;
            file->bstream.avail_out      = file->stream.avail_out;
            file->bstream.total_out_lo32 = file->stream.total_out;
            file->bstream.total_out_hi32 = 0;

            uTotalOutBefore = file->bstream.total_out_lo32;
            bufBefore = file->bstream.next_out;

            err=BZ2_bzDecompress(&file->bstream);

            uTotalOutAfter = file->bstream.total_out_lo32;
            uOutThis = uTotalOutAfter-uTotalOutBefore;

            file->sum_crc32 =crc32(file->head.crc32,bufBefore,(uInt)(uOutThis));

            rest_uncomp -= uOutThis;

            iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);

            file->stream.next_in   = file->bstream.next_in;
            file->stream.avail_in  = file->bstream.avail_in;
            file->stream.total_in  = file->bstream.total_in_lo32;
            file->stream.next_out  = file->bstream.next_out;
            file->stream.avail_out = file->bstream.avail_out;
            file->stream.total_out = file->bstream.total_out_lo32;

            if (err==BZ_STREAM_END)
                return (iRead==0) ? OZIP_EOF : iRead;
            if (err!=BZ_OK)
                break;
#endif
        }
        else
        {
            uLong uTotalOutBefore,uTotalOutAfter;
            const Bytef *bufBefore;
            uLong uOutThis;
            int flush=Z_SYNC_FLUSH;

            uTotalOutBefore = file->stream.total_out;
            bufBefore = file->stream.next_out;

            /*
            if ((pfile_in_zip_read_info->rest_read_uncompressed ==
                     pfile_in_zip_read_info->stream.avail_out) &&
                (pfile_in_zip_read_info->rest_read_compressed == 0))
                flush = Z_FINISH;
            */
            err=inflate(&file->stream,flush);

            if ((err>=0) && (file->stream.msg!=NULL))
            {
            	err = Z_DATA_ERROR;
            }

            uTotalOutAfter = file->stream.total_out;
            uOutThis = uTotalOutAfter-uTotalOutBefore;

            file->sum_crc32 =crc32(file->head.crc32,bufBefore, (uInt)(uOutThis));

            rest_uncomp -= uOutThis;

            iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);

            if (err==Z_STREAM_END)
            {
            	return (iRead==0) ? OZIP_EOF : iRead;
            }
            if (err!=Z_OK)
            {
            	break;
            }
        }
    }

    if (err==Z_OK)
    {
    	return iRead;
    }

    return -1;
}

int zf_file_get_comment(File file, char* name, char* buffer, int len) {

	return 0;
}


int zf_file_stat(oentry* file, ozip_stat* file_stat) {
    if(NULL == file || NULL == file_stat){
    	return OZIP_PARAMERROR;
    }
    file_stat->comp_method = file->head.comp_method;
    file_stat->comp_size = file->head.comp_size;
    file_stat->crc = file->head.crc32;
	//file_stat->encryption_method = file->cur_file_info
    file_stat->flags = file->head.flag;
    file_stat->index = 0;
//	file_stat->mtime = file->cur_file_info.tmu_date;
	//file_stat->name = file->cur_file_info.
    file_stat->size = file->head.uncomp_size;
    file_stat->valid = file->head.inter_attr;

	return OZIP_OK;
}

