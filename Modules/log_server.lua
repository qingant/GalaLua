module(..., package.seeall)

local rpc = require(_PACKAGE .. "rpc")
local cjson = require("cjson")
local pprint = require("pprint")
local memcached = require("memcached").GlrBuffer
local slog = require("slog")
local bit = require("bit")
local c_flag = require("c_flag")
local io = require(_PACKAGE .. "io")

server = rpc.server:new()

function server:new(name)
    self:init(name)
    self._buf = {}
    self._buf_len = 0
    self._buf_max_len = 500
    self._timeout = 3
    self._file_size = 2000
    self._log_path = "../../log/65535.log"
    self._max_file_num = 10
    self._times_to_get_filesize = 20
    self._times = 0
    self._fd=io.open(self._log_path,bit.bor(c_flag.O_CREAT,bit.bor(c_flag.O_APPEND,c_flag.O_RDWR)))
    return self
end

function server:__gc()
    local rt = io.close(self._fd)
    if 0~= rt then
        print("destructed!")
    end
end

function server:_fSize( fd )
    local fsize = io.lseek(fd, 0, c_flag.SEEK_END)
    print("******************size",fsize,"***********************")
    return fsize
end

function server:_log_full(file_path)
    local fd = self._fd
    local filesize = self:_fSize(fd)
    if filesize >= self._file_size then
        local rt=io.close(fd)
        for i = self._max_file_num-1,1,-1 do
            io.rename(file_path..'.'..i-1,file_path..'.'..i)
        end
        local rt=io.rename(file_path,file_path..'.'.. 0)
        print("******************pathchange:",rt,"**********************")
        self._fd=io.open(file_path,bit.bor(c_flag.O_CREAT,bit.bor(c_flag.O_APPEND,c_flag.O_RDWR)))
    end
end

function server:_wirte_v(file_path) -- used for text
    buf = self._buf[file_path];
    local fd = self._fd
    if buf ~= nil then -- 定时写入时检测是否需要
        n=#buf
        if n ~= 0 then -- 异常保护
            local iov = io.new("iovec[?]", n)
             for i=0,n-1 do
                iov[i].iov_len=string.len(buf[i+1])
                local ptr = io.cast("char *",buf[i+1])
                iov[i].iov_base = ptr
            end
            --local path=io.cast("char *",file_path)
            --local fd=io.open(file_path,bit.bor(c_flag.O_CREAT,bit.bor(c_flag.O_APPEND,c_flag.O_RDWR)));
            if -1 ~= fd then
                res=io.writev(fd,iov,n)
                if -1 == res then
                    print("write failed")
                end
                self._buf[file_path] = nil
            end
        end
    end
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

function server:_flush()
    if nil ~= self._buf then 
        for file_path,_ in pairs(self._buf) do
            if nil ~= self._buf[file_path] then
                self:_wirte_v(file_path)
            end
        end
    end
end

function server:on_timeout()
    self:_flush()
end

function server:log(params)
    local level = params.level
    local msg = params.msg
    if nil == msg then
        print("msg is null")
    end
    if nil == self._log_path then
        print("path unlegal")
    end 
    if self._times + 1 < self._times_to_get_filesize then
        self._times=self._times+1
    else
        self._times=0
        self:_log_full(self._log_path)
    end
    self:_warn(params.level,params.msg)
    self:_write_buf(self._log_path,msg) --这里只负责往buf里写，buf写满了会自动将内容写入log中
end

function server:_warn(level,msg)
end