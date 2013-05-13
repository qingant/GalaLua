module(..., package.seeall)
local pprint = require("pprint")
local debug = require("debug")
local _ = require("str_utils")
local printer = print


-- enum log level
DEBUG = 0
TRACE = 2
INFO = 3
ERROR = 4
FATAL = 5


-- -- outputs "concentration is   56.28%"

function log(msg, ...)
	--local msg = pprint.format({...}, "msg")
	local level = ...
	local info = debug.getinfo(2)
	info.msg = pprint.format(msg)
	
	local log_str = ("File=%(short_src)s, Func=%(name,func)s, Line=%(currentline)s : %(msg)s" % info)
	printer(log_str)
end

if ... == "__main__" then
	log("test")
end
