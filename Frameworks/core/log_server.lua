module(..., package.seeall)

local rpc = require(_PACKAGE .. "rpc")
local cjson = require("cjson")
local pprint = require("pprint")
local memcached = require("memcached").GlrBuffer
local slog = require("slog")
local bit = require("bit")
local c_flag = require("c_flag")
local cio = require("native.io")
print("_PACKAGE", _PACKAGE)
server = rpc.server:new()

function server:new(name)
    self:init(name)
    self._buf = {}
    self._buf_len = 0
    self._buf_max_len = 500
    self._timeout = 3
    self._file_size = 5000
    self._file_path = "../../log/65535.log"
    self._max_file_num = 10
    self._times_to_get_filesize = 1
    self._times = 0
    self._log_output = print
    self:_set_path(self._file_path)
    return self
end

function server:_set_path(path)
    local flag = bit.bor(c_flag.O_CREAT,c_flag.O_APPEND,c_flag.O_RDWR)
    print("flag", flag)
    self._fd = cio.open(self._file_path, flag, c_flag.S_IFMT)
    print("LOG", path, self._fd)
    if self._fd ~= -1 then
        self._log_output = function (msg) return self:_write(msg) end
    end
end

function server:__gc()
    self:finalize()
end

function server:finalize()
    self:flush()
    if -1 == self._fd then
        return
    end
    local rt = cio.close(self._fd)
    if rt ~= 0 then
        print("destruct logger io failed!")
        return
    end
    self._fd = -1
end

function server:_fSize( fd )
    local fsize = cio.lseek(fd, 0, c_flag.SEEK_END)
    print("******************size",fsize,"***********************")
    return fsize
end

function server:_log_full()
    local fd = self._fd
    local filesize = self:_fSize(fd)
    if filesize >= self._file_size then
        local rt=cio.close(fd)
        for i = self._max_file_num-1,1,-1 do
            cio.rename(self._file_path..'.'..i-1,self._file_path..'.'..i)
        end
        local rt=cio.rename(self._file_path,self._file_path..'.'.. 0)
        print("******************pathchange:",rt,"**********************")
        self._fd=cio.open(self._file_path,bit.bor(c_flag.O_CREAT,bit.bor(c_flag.O_APPEND,c_flag.O_RDWR)),c_flag.S_IFMT)
    end
end

function server:_wirte_v() -- used for text
    local buf = self._buf
    local fd = self._fd
    local n = #buf
    if 0 == n then  -- there is no data to flush
        return
    end
    local iov = cio.new("iovec[?]", n)
    for i=0,n-1 do
        iov[i].iov_len=string.len(buf[i+1])
        local ptr=cio.cast("char *",buf[i+1])
        iov[i].iov_base=ptr
    end    
    if fd ~= -1 then
        res=cio.writev(fd,iov,n)
        if -1 == res then
            print("write failed")
        end
        self._buf = {}
    end
end

function server:_write(msg)
    self._buf[#self._buf + 1]=msg
    self._buf_len = self._buf_len + string.len(msg)
    if self._times >= self._times_to_get_filesize then
        self._times = 0
        self:_log_full()
    end
    self._times=self._times + 1
    if self._buf_len >= self._buf_max_len  then
        self:flush()
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

function server:_flush()
    self:_wirte_v()
end

function server:on_timeout()
    self:_flush()
end

function server:log(params)
    local level = params.level
    local msg = params.msg
    print(params.msg)
    if nil == msg then
        print("msg is null")
    end
    if nil == self._file_path then
        print("path is null")
    end 
    if self._times + 1 < self._times_to_get_filesize then
        self._times=self._times+1
    else
        self._times=0
        self:_log_full(self._file_path)
    end
    self:_warn(params.level,params.msg)
    self:_write(params.msg) --这里只负责往buf里写，buf写满了会自动将内容写入log中
end

function server:_warn(level,msg)
end