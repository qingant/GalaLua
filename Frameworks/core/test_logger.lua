module(..., package.seeall)
local pprint = require("pprint")
local debug = require("debug")
local osdatetime = require("osdatetime")
local hexdump = require("hexdump")
local printer = print
assert(require("str_utils"), "Cannot Import str_utils")
assert(require("tab_utils"),"Cannot Import tab_utils")

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
    enum_WARN =4,
    enum_ERROR = 5,
    enum_FATAL = 6,
}
local _loggerFlag ={"DEBUG", "TRACE", "INFO ","WARN ", "ERROR", "FATAL"}
logger = _logger
function _logger:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end
local logPid = 11
function _logger:init(process)
    self._process = process
    self._log_level = self._process._log_level or self.enum_DEBUG
    self._output_func = print
    return self
end
function _logger:turn_on_profile()
   self.flag = 1
end
function _logger:flush( )
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
        info.level = _loggerFlag[level]
        info.msg = str
        --info.gpid = self._handler.gpid  --deleted by yangbo for test;self._handler is nil now
        local timeformat
        local log_str
        if self.flag ~= 0 then
            timeformat = string.format("[%s.%s] ",os.date("%m/%d %H:%M:%S",glr.time.now()),osdatetime.gettimeofday().msec)
            log_str= ("[%(level)s] [%(short_src)s:%(currentline)s] [G:%(gpid)s]:%(msg)s" % info)
        else
            timeformat = string.format("[%s] ",os.date("%m/%d %H:%M:%S",glr.time.now()))
            log_str= ("[%(level)s] [%(short_src)s:%(currentline)s] [G:%(gpid)s]:%(msg)s" % info)
        end
        self._output_func(timeformat.."\n"..log_str)
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
_logger.log = _logger.debug



if ... == "__main__" then
    function test(  )
        local log = logger:new():init("test.log")
        local log = logger:new():init("test1.log")

        log:debug("debug")
        log:trace("trace")
        log:info("info")
        log:error("error")
        log:fatal("fatal")
        log:flush()
    end
    test()

end