module(..., package.seeall)
local ffi=require("ffi")

ffi.cdef[[
              typedef struct {
              char *iov_base;
              size_t iov_len;
              }iovec;
                       
              int open(const char* pathname,int flag);
              int close(int fd);
              int writev(int fd,const iovec* iov,int iovcnt);
              long int lseek(int handle, long int offset, int fromwhere);
              int rename(const char* oldname,const char* newname);
              int system(const char* string);
              int write(int handle,const char* buf,int nbyte);
            ]]
            
open = ffi.C.open
--[[
            flags:
            
            O_RDONLY 只读
            O_WRONLY 只写
            O_RDWR   读写
            
            O_CREAT  创建并打开文件
            O_TRUNC  打开一个文件并将文件程度设置为0
            O_EXCL   未使用
            O_APPEND 追加打开文件
            O_TEXT   打开文本文件翻译CR-LF控制字符
            O_BINARY 打开二进制字符，不作CR-LF翻译
            
            mode:
            
            S_IFMT   文件类型掩码
            S_IFDIR  目录
            S_IFIFO  FIFO专用
            S_IFCHR  字符专用
            S_IFBLK  块专用
            S_IFREG  只为0x0000
            S_IREAD  可读
            S_IWRITE 可写
            S_IEXEC  可执行
            ]]
close = ffi.C.close
writev = ffi.C.writev
new = ffi.new
cast = ffi.cast
lseek = ffi.C.lseek
rename = ffi.C.rename
system = ffi.C.system
write = ffi.C.write