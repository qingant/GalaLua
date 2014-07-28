module(..., package.seeall)
local pprint = require("pprint")
local debug = require("debug")
local osdatetime = require("osdatetime")
local hexdump = require("hexdump")
local printer = print
local slog = require("slog")                                   --added for test
local rpc = require(_PACKAGE .. "rpc")
local c_flag = require("c_flag")                               --added for c flags
local bit = require("bit")
local glr = require("glr")
local io = require(_PACKAGE .. "io")
assert(require("str_utils"), "Cannot Import str_utils")
assert(require("tab_utils"),"Cannot Import tab_utils")

-- enum log level

local _logger = {
    enum_DEBUG = 1,
    enum_TRACE = 2,
    enum_INFO = 3,
    enum_WARN =4,
    enum_ERROR = 5,
    enum_FATAL = 6,
}
local _logger_Flag ={"DEBUG", "TRACE", "INFO ","WARN ", "ERROR", "FATAL"}
logger = _logger

function _logger:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function _logger:init(process,log_path)
    self._process = process
    self._log_level = self._process._log_level or self.enum_DEBUG
    --self._log_client = rpc.create_server{mod_name = "log_server", parameters = {"test/log_server"}}
    self._buf = {}
    self._buf_len = 0
    self._buf_max_len = 500
    self._log_path = log_path
    --self._file_size = 4*1024*1024
    self._file_size = 5000
    self._max_file_num = 10
    self._times_to_get_filesize = 1 --每_times_to_get_filesize次flush buf，检查一次文件大小；为了测试方便，这里暂时设为1
    self._times = 0
    self._fd=io.open(self._log_path,bit.bor(c_flag.O_CREAT,bit.bor(c_flag.O_APPEND,c_flag.O_RDWR)))
    --self._timeout = 3
    return self
end

function _logger:__gc()
    local rt = io.close(self._fd)
    if 0~= rt then
        print("destructed!")
    end
end

function _logger:flush()
    if nil ~= self._buf then 
        for file_path,_ in pairs(self._buf) do
            if nil ~= self._buf[file_path] then
                self:_wirte_v(file_path)
            end
        end
    end
end

function _logger:_fSize( fd )
    local fsize = io.lseek(fd, 0, c_flag.SEEK_END)
    print("******************size:",fsize,"***********************")
    return fsize
end

function _logger:_log_full(file_path)
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

function _logger:_wirte_v(file_path) -- used for text
    buf = self._buf[file_path];
    local fd = self._fd
    if buf ~= nil then -- 定时写入时检测是否需要
        n=#buf
        if n ~= 0 then -- 异常保护
            local iov = io.new("iovec[?]", n)
             for i=0,n-1 do
                iov[i].iov_len=string.len(buf[i+1])
                local ptr=io.cast("char *",buf[i+1])
                iov[i].iov_base=ptr
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

function _logger:_write_buf(file_path,msg)
    if (nil == self._buf[file_path]) then
        self._buf[file_path]={}
        self._buf[file_path][1]=msg
        self._buf_len=string.len(msg)
    else
        self._buf[file_path][#(self._buf[file_path])+1]=msg
        self._buf_len=self._buf_len+string.len(msg)
    end
    if ( self._buf[file_path] ~= nil ) and ( self._buf_len >= self._buf_max_len ) then
        if self._times + 1 < self._times_to_get_filesize then
            self._times=self._times+1
        else
            self._times=0
            self:_log_full(file_path)
        end
        self:_wirte_v(file_path)
    end
end
--[[
function _logger:_write_log(file_path) --used for slog
        local writer = slog.create_writer(file_path)          --打开0.log日志文件,added for test
        if nil == writer then
            print("creat writer failed")
        end
        writer:write_array(self._buf[file_path])
end
]]

function _logger:_log_local(file_path,msg)
    if nil == msg then
        print("msg is null")
    end
    if nil == file_path then
        print("path unlegal")
    end
    self:_write_buf(file_path,msg) --这里只负责往buf里写，buf写满了会自动将内容写入log中
end

function _logger:_log(level, format, ...)
    if self._log_level <= level then
        local str
        if #{...} == 0 then
            str = format
        else
            str = string.format(format, ...)
        end
        local info = debug.getinfo(3)
        info.level = _logger_Flag[level]
        info.msg = str
        --info.gpid = self._handler.gpid  --deleted by yangbo for test;self._handler is nil now
        local time_format
        local log_str
        if self.flag ~= 0 then
            time_format = string.format("[%s.%s] ",os.date("%m/%d %H:%M:%S",glr.time.now()),osdatetime.gettimeofday().msec)
            log_str= ("[%(level)s] [%(short_src)s:%(currentline)s] [G:%(gpid)s]:%(msg)s" % info)
        else
            time_format = string.format("[%s] ",os.date("%m/%d %H:%M:%S",glr.time.now()))
            log_str= ("[%(level)s] [%(short_src)s:%(currentline)s] [G:%(gpid)s]:%(msg)s" % info)
        end
        self:_log_local(self._log_path,time_format.."\n"..log_str)
        if level >= self.enum_DEBUG then   --used for warn
            --self._log_client:call("log", {level=info.level,msg=time_format.."\n"..log_str})
        end
    end
end

function _logger:debug(format, ...)
    self:_log(self.enum_DEBUG, format, ...)
end

function _logger:error(format, ...)
    self:_log(self.enum_ERROR, format, ...)
end
function _logger:info(format, ...)
    self:_log(self.enum_INFO, format, ...)
end
function _logger:trace(format, ...)
    self:_log(self.enum_TRACE, format, ...)
end

function _logger:warn(format, ...)
    self:_log(self.enum_WARN, format, ...)
end
function _logger:fatal(format, ...)
    self:_log(self.enum_FATAL, format, ...)
end
function _logger:close()
    self:flush()
    glr.send(self._handler,"!#")
end
function _logger:reset()
   glr.send(logPid,"!*")
end
function _logger:dump(str)
   return hexdump.dump(str)
end
--_logger.log = _logger.debug



if ... == "__main__" then
    function test(  )
        local log = logger:new():init("test.log")
        local log = logger:new():init("test1.log")

        log:debug("debug")
        log:trace("trace")
        log:info("info")
        log:error("error")
        log:fatal("fatal")
    end
    test()

end