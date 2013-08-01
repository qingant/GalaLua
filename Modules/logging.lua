module(..., package.seeall)
local pprint = require("pprint")
local debug = require("debug")
local printer = print
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
GlrLoggerAppender = require("log_server").GlrLoggerAppender

-- enum log level
local DEBUG = 0
local TRACE = 2
local INFO = 3
local ERROR = 4
local FATAL = 5

local _logger = {
    enum_DEBUG = 1,
    enum_TRACE = 2,
    enum_INFO = 3,
    enum_ERROR = 4,
    enum_FATAL = 5,
}
local _loggerFlag ={"DEBUG", "TRACE", "INFO", "ERROR", "FATAL"}
logger = _logger
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
        info.level = _loggerFlag[level]
        info.msg = str
        local log_str = ("[%(level)s] [File=%(short_src)s Function=%(name)s Line=%(currentline)s] : %(msg)s" % info)
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
function _logger:close()
    self._printerHandle:close()
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
