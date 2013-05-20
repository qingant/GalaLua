module(..., package.seeall)
local pprint = require("pprint")
local debug = require("debug")
local printer = print

local strMt = getmetatable("")
local function split(pString, pPattern)
   local Table = {}  -- NOTE: use {n = 0} in Lua-5.0
   local fpat = "(.-)" .. pPattern
   local last_end = 1
   local s, e, cap = pString:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
     table.insert(Table,cap)
      end
      last_end = e+1
      s, e, cap = pString:find(fpat, last_end)
   end
   if last_end <= #pString then
      cap = pString:sub(last_end)
      table.insert(Table, cap)
   end
   return Table
end
string.split = split


local function contains(s, p)
	if string.find(s, p) == nil then
		return false
	else
		return true
	end
end
string.contains = contains
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

strMt.__mod = interp

-- enum log level
DEBUG = 0
TRACE = 2
INFO = 3
ERROR = 4
FATAL = 5

_logger = {
    enum_DEBUG = 0,
    enum_TRACE = 2,
    enum_INFO = 3,
    enum_ERROR = 4,
    enum_FATAL = 5,
}
logger = _logger
function _logger:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return self
end
function _logger:init( printer, level_func )
    self._printer = printer or print
    self._get_level = level_func or function () return self.enum_DEBUG end
    return self
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
        local log = logger:new():init()
        log:debug("debug")
        log:trace("trace")
        log:info("info")
        log:error("error")
        log:fatal("fatal")
    end
    test()

end
