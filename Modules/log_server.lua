module(..., package.seeall)

local rpc=require(_PACKAGE .. "rpc")
local cjson = require("cjson")
local pprint = require("pprint")
local memcached = require("memcached").GlrBuffer
local slog=require("slog")
local ffi=require("ffi")
local bit=require("bit")
local c_flag=require("c_flag")

server = rpc.server:new()

function server:new(name)
    self:init(name)
    self._buf={}
    self._buf_len=0
    self._buf_max_len=500
    self._timeout=3
    return self
end

local function fSize( file )
    local current = file:seek()
    local size = file:seek("end")
    file:seek("set", current)
    return size
end
--鑾峰彇璺緞
local function getFilePath(filename)
    return string.match(filename, "(.+)/[^/]*%.%w+$")
end
--鑾峰彇鏂囦欢鍚�
local function getFileName(filename)
    return string.match(filename, ".+/([^/]*%.%w+)$")
end
--鍘婚櫎鎵╁睍鍚�
local function getFileNameWithoutExtension(filename)
    local idx = filename:match(".+()%.%w+$")
    if(idx) then
        return filename:sub(1, idx-1)
    else
        return filename
    end
end

--鑾峰彇鎵╁睍鍚�
function getFileExtension(filename)
    return filename:match(".+%.(%w+)$")
end

function server:log(params)
    local file_path = params.log_path
    local level = params.level
    local msg = params.msg
    if nil == msg then
        print("msg is null")
    end
    if nil == file_path then
        print("path unlegal")
    end
    self:warn(params.level,params.msg)
    self:_write_buf(file_path,msg) --这里只负责往buf里写，buf写满了会自动将内容写入log中
    print("herehrehrereherherherhrhehrehreh3")
end

function server:_write_buf(file_path,msg)
    if (nil == self._buf[file_path]) then
        self._buf[file_path]={}
        self._buf[file_path][1]=msg
        self._buf_len=string.len(msg)
    else
        self._buf[file_path][#(self._buf[file_path])+1]=msg
        self._buf_len=self._buf_len+string.len(msg)
        
    end
    if (self._buf[file_path] ~= nil) and (self._buf_len >= self._buf_max_len) then
        self:_wirte_v(file_path)
    end
end
--[[
function server:_write_log(file_path) --used for slog
        local writer = slog.create_writer(file_path)          --打开0.log日志文件,added for test
        if nil == writer then
            print("creat writer failed")
        end
        writer:write_array(self._buf[file_path])
end
]]
function server:_wirte_v(file_path) -- used for text
    buf = self._buf[file_path]
    if nil ~= buf then -- 定时写入时检测是否需要
        n=#buf
        if 0 ~= n then -- 异常保护
            print("herehrehrereherherherhrhehrehreh2")
            ffi.cdef[[
              typedef struct {
              char *iov_base;
              size_t iov_len;
              }iovec;
              int open(const char* pathname,int flag);
              int writev(int fd,const iovec* iov,int iovcnt);
            ]]
            local iov = ffi.new("iovec[?]", n)
             for i=0,n-1 do
                iov[i].iov_len=string.len(buf[i+1])
                local ptr=ffi.cast("char *",buf[i+1])
                iov[i].iov_base=ptr
            end
            --local path=ffi.cast("char *",file_path)
            local fd=ffi.C.open(file_path,bit.bor(c_flag.O_CREAT,bit.bor(c_flag.O_APPEND,c_flag.O_RDWR)))
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
            res=ffi.C.writev(fd,iov,n)
            print("herehrehrereherherherhrhehrehreh4")
            if -1 == res then
                print("write failed")
            end
            self._buf[file_path] = nil
        end
    end
end

function server:on_timeout()
    self:_flush()
end

function server:_flush()
    if nil ~= self._buf then 
        for file_path,_ in pairs(self._buf) do
            if nil ~= self._buf[file_path] then
                self:_wirte_v(file_path)
            end
        end
    end
end

function server:warn(level,msg)

end