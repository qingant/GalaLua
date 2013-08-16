OBJ=lua_minizip.o

all:libzip minizip

AR          = ar
ARFLAGS     = -rcu $(ARFLAG)
RANLIB      = ranlib
PLATFORM=$(shell uname)
LDFLG   =
CFLAGS  =
ifeq (${PLATFORM},Linux)
	MAKE  = make
	LDFLG = -shared
	CFLAGS = -ansi -std=gnu99 -Wall -O2 -fPIC -I../../LuaJIT-2.0.0/src/
	CC=gcc
	AR          = ar
	ARFLAGS     = -rcu
	RANLIB      = ranlib
	RANLIBFLAGS =
else ifeq (${PLATFORM},AIX)
	LDFLG = -qmkshrobj -q64 -bM:SRE -bnoentry -brtl -brtllib -berok
	CFLAGS =  -q64 -O2 -I../../lua-5.1.5/src/ -L $(HOME)/lib -lm
	CC=xlc_r
	AR          = ar
	ARFLAGS     = -X64 -rcu
	RANLIB      = ranlib
	RANLIBFLAGS = -X64
	MAKE        = gmake
endif

libzip:
	cd libzip-0.11.1 &&chmod +x configure && ./configure CC=$(CC) && export OBJECT_MODE=64 && $(MAKE) -C lib && cp -f lib/.libs/libzip.so* $(HOME)/lib/


minizip:
	$(CC) $(LDFLG) $(CFLAGS) lua_minizip.c  -o minizip.so -lzip -Ilibzip-0.11.1/lib -I../../LuaJIT-2.0.0/src/ -L$(HOME)/lib/  -L$(HOME)/lib/lib
	cp -rf minizip.so $(HOME)/lib/lua


%.o: %.c
	$(CC) $(CFLAGS) -c $< -Ilibzip-0.11.1/lib -I../../LuaJIT-2.0.0/src/

static: libzip.a minizip.a

minizip.a:$(OBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $(RANLIBFLAGS) $@
	cp -rf minizip.a $(HOME)/share/static/lua
	rm -f minizip.a

LIBZIP_OBJECT=zip_add.o zip_add_dir.o zip_add_entry.o zip_close.o \
	zip_delete.o zip_dir_add.o zip_dirent.o zip_discard.o zip_entry.o \
	zip_err_str.o zip_error.o zip_error_clear.o zip_error_get.o \
	zip_error_get_sys_type.o zip_error_strerror.o zip_error_to_str.o \
	zip_extra_field.o zip_extra_field_api.o zip_fclose.o zip_fdopen.o \
	zip_file_add.o zip_file_error_clear.o zip_file_error_get.o \
	zip_file_get_comment.o zip_file_get_offset.o zip_file_rename.o \
	zip_file_replace.o zip_file_set_comment.o zip_file_strerror.o \
	zip_filerange_crc.o zip_fopen.o zip_fopen_encrypted.o zip_fopen_index.o \
	zip_fopen_index_encrypted.o zip_fread.o zip_get_archive_comment.o \
	zip_get_archive_flag.o zip_get_compression_implementation.o \
	zip_get_encryption_implementation.o zip_get_file_comment.o \
	zip_get_num_entries.o zip_get_num_files.o zip_get_name.o zip_memdup.o \
	zip_name_locate.o zip_new.o zip_open.o zip_rename.o zip_replace.o \
	zip_set_archive_comment.o zip_set_archive_flag.o \
	zip_set_default_password.o zip_set_file_comment.o zip_set_file_compression.o \
	zip_set_name.o zip_source_buffer.o zip_source_close.o zip_source_crc.o \
	zip_source_deflate.o zip_source_error.o zip_source_file.o zip_source_filep.o \
	zip_source_free.o zip_source_function.o zip_source_layered.o zip_source_open.o \
	zip_source_pkware.o zip_source_pop.o zip_source_read.o zip_source_stat.o \
	zip_source_window.o zip_source_zip.o zip_source_zip_new.o zip_stat.o \
	zip_stat_index.o zip_stat_init.o zip_strerror.o zip_string.o zip_unchange.o \
	zip_unchange_all.o zip_unchange_archive.o zip_unchange_data.o zip_utf-8.o

libzip.a:
	cd libzip-0.11.1 && chmod +x configure && ./configure CC=$(CC) && OBJECT_MODE=64 && $(MAKE) -C lib
	cd libzip-0.11.1/lib/.libs && $(AR) $(ARFLAGS) $@ $(LIBZIP_OBJECT) \
		&& $(RANLIB) $(RANLIBFLAGS) $@ && cp $@ $(HOME)/share/static

clean:
	-rm -f *.o *.a *.so
	-rm -rf libzip-0.11.1/autom4te.cache
	@if [ -f libzip-0.11.1/Makefile ]; then \
		$(MAKE) -C libzip-0.11.1 clean; \
		$(MAKE) -C libzip-0.11.1 distclean; \
	fi

