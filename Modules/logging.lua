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
