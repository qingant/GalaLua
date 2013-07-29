module(..., package.seeall)
local pprint = require("pprint")
local debug = require("debug")
local printer = print
local configure = require "config".Config
assert(require("str_utils"), "Cannot Import str_utils")
assert(require("tab_utils"),"Cannot Import tab_utils")

local conf_path = os.getenv("HOME") .. "/var/conf/"

local function interp(s, tab)
	
	return (s:gsub('%%%((%a[%w_,]*)%)([-0-9%.]*[cdeEfgGiouxXsq])',
		function(k, fmt) 
			

			local ss = "(nil)"
			if string.contains(k, ",") then
				for i,v in ipairs(string.split(k, ",")) do
					
					if tab[v] then

						ss = ("%"..fmt):format(tab[v])
                        break
					end
				end
			else
				ss = tab[k] and ("%"..fmt):format(tab[k]) or ss 
			end
			
			return ss
		end))
end

--strMt.__mod = interp
-- glr  printer
function glrLoggerServer( targetPath , confPath)
    --获取文件大小
    function fsize( file )
	local current = file:seek()	-- 获取当前位置
	local size = file:seek("end")   -- 获取文件大小
	file:seek("set", current)
	return size
    end
    --获取路径
    function stripfilename(filename)
	return string.match(filename, "(.+)/[^/]*%.%w+$")
    end
    --获取文件名
    function strippath(filename)
	return string.match(filename, ".+/([^/]*%.%w+)$")
    end
    --去除扩展名
    function stripextension(filename)
	local idx = filename:match(".+()%.%w+$")
        if(idx) then
            return filename:sub(1, idx-1)
	else
            return filename
	end
    end

    --获取扩展名
    function getextension(filename)
        return filename:match(".+%.(%w+)$")
    end

    local rollingIndex = 0    
    local fileHandle = io.open(targetPath, "a")
    fileHandle:setvbuf("full")
    local conf = configure:new():init(confPath)
    local singleLogSize = conf:get("AppLoger/Size")
    if singleLogSize == nil or singleLogSize == "" then
        error("please put Apploger/Size")
    end
    local splitLogMaxNum = conf:get("AppLoger/Files")
    if splitLogMaxNum == nil or splitLogMaxNum == "" then
        error("please put Apploger/Size")
    end
    local svcNum = conf:get("Base/SVCLimits")
    if svcNum == nil or svcNum == "" then
        error("please put Base/SVCLimits")
    end
    while true do
        local msgType, addr, msg = glr.recv()
        if msg:sub(1,2) == "!!" then
            print("Flush")
            fileHandle:flush()
        else
            print("Write", msg)
	    fileHandle:write(msg)
	    fileHandle:write("\n")
        end
	local logSize = fsize(fileHandle)/(1024*1024)
	if logSize >= (tonumber(singleLogSize)*tonumber(svcNum)*1024*1024) then
	    local extension = getextension(targetPath)
	    local filename = stripextension(targetPath)
	    local rollingpath = string.format("%s_%d.%s",filename,rollingIndex,extension)
            fileHandle = io.open(rollingpath, "w+")
            fileHandle:close()
            fileHandle = io.open(rollingpath, "a")
            fileHandle:setvbuf("full")
            rollingIndex = (rollingIndex + 1) % (tonumber(splitLogMaxNum)*tonumber(svcNum))
	end
    end
end
local GlrLoggerAppender = {}
function GlrLoggerAppender:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function GlrLoggerAppender:init( path )
    local rt, pid = glr.spawn("logging", "glrLoggerServer", path,conf_path)

    self._appender = pid
    return self
end
function GlrLoggerAppender:print( msg )
    glr.send(self._appender, msg)
end
function GlrLoggerAppender:flush(  )
    glr.send(self._appender, "!!")
end

-- enum log level
local DEBUG = 0
local TRACE = 2
local INFO = 3
local ERROR = 4
local FATAL = 5

local _logger = {
    enum_DEBUG = 0,
    enum_TRACE = 2,
    enum_INFO = 3,
    enum_ERROR = 4,
    enum_FATAL = 5,
}
local logger = _logger
function _logger:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end
function _logger:init( printer, level_func )
    self._printer = printer and function (...) printer.print(printer, ...) end or print
    self._printerHandle = printer
    self._get_level = level_func or function () return self.enum_DEBUG end
    return self
end
function _logger:flush( )
    self._printerHandle:flush()
end
function _logger:_log(level, format, ...)
    if self._get_level() <= level then
        local str = string.format(format, ...)
        local info = debug.getinfo(3)

        info.msg = str
        local log_str = ("File=%(short_src)s, Func=%(name,func)s, Line=%(currentline)s : %(msg)s" % info)
        self._printer(log_str)
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
function _logger:fatal(format, ...)
    self:_log(self.enum_FATAL, format, ...)
end
_logger.log = _logger.debug

-- function log(msg, ...)
-- 	--local msg = pprint.format({...}, "msg")
-- 	local level = ...
-- 	level = level or DEBUG
-- 	local info = debug.getinfo(2)
-- 	info.msg = pprint.format(msg)
	
-- 	local log_str = ("File=%(short_src)s, Func=%(name,func)s, Line=%(currentline)s : %(msg)s" % info)
-- 	printer(log_str)
-- end

if ... == "__main__" then
    function test(  )
        local log = logger:new():init(GlrLoggerAppender:new():init("test.log"))
        log:debug("debug")
        log:trace("trace")
        log:info("info")
        log:error("error")
        log:fatal("fatal")
        log:flush()
    end
    test()

end
