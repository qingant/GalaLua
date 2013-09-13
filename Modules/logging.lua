module(..., package.seeall)
local pprint = require("pprint")
local debug = require("debug")
local cjson = require("cjson")
local datetime = require("datetime")
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

--strMt.__mod = interp
-- glr  printer
--GlrLoggerAppender = require("log_server").GlrLoggerAppender

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
local _loggerFlag ={"DEBUG", "TRACE", "INFO","WARN", "ERROR", "FATAL"}
logger = _logger
function _logger:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end
local logPid = 11
function _logger:init(path,size,copys,level_func )
    --glr.spawn_ex(logPid, "log_server", "glrLogServerDispatch", "{}")
    pcall(glr.spawn_ex,logPid, "log_server", "glrLogServerDispatch", "{}")

    self._printer = print
    self._get_level = level_func or function () return self.enum_DEBUG end
    local info = {}
    info.path = path
    info.size = size or (64*1024*1024)
    info.copys = copys or 2000
    glr.send(logPid,"!@"..cjson.encode(info))
    local _,_,msg = glr.recvByAddr({host=glr.sys.host,port=glr.sys.port,gpid=logPid})
    self._handler = cjson.decode(msg)
    pprint.pprint(self._handler)
    return self
end

function _logger:flush( )
    glr.send(self._handler,"!!")
end
function _logger:_log(level, format, ...)
    if self._get_level() <= level then
        local str
        if #{...} == 0 then
            str = format
        else
            str = string.format(format, ...)
        end
        local info = debug.getinfo(3)
        info.level = _loggerFlag[level]
        info.msg = str
        local timeformat = string.format("[%s] ",datetime:new():init():today():strtime())
        local log_str = ("[%(level)s] [File=%(short_src)s  Line=%(currentline)s] : %(msg)s" % info)
        glr.send(self._handler,timeformat..log_str)
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
    glr.send(self._handler,"!#")
end
function _logger:reset()
   glr.send(logPid,"!*")
end
function _logger:dump(str)
   return _dump(str)
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
