-- module(..., package.seeall)
-- local __main__ = ...
-- local __self__ = package.loaded[__main__]
local M = {}
local _glr = _glr
local type = type
local pairs = pairs
local ipairs = ipairs
local string = string

local assert=assert
local require= require
local table=table
local package=package
local loadstring=loadstring
local setmetatable = setmetatable
local pcall = pcall
local error = error
local print = print
local unpack = unpack
setfenv(1, M)
local self_host, self_port = _glr.node_addr()

function spawn(mod_name, entry, ...)
	return _glr.spawn(mod_name, entry, ...)
end
function spawn_ex(bindPid, mod_name, entry, ...)
    return _glr.spawn_ex(bindPid, mod_name, entry, ...)
end

function send(addr, msg)
	if type(addr) == "table" then
		if self_host == addr.host and self_port == addr.port then
			return _glr.send(addr.gpid, msg)
		else
			return _glr.int(2, 1, addr.host, addr.port, msg, addr.gpid) -- node send
		end
	else
		return _glr.send(addr, msg)
	end
end

function connect(host, port, pid, timeout)
	return _glr.int(2, 0, host, port, pid, timeout)
end
function close_node(host, port)
    return _glr.int(2, 4, host, port)
end

local mailBox = {}
function mailBox:new(  )
    local o = {}
    setmetatable(o, self)
    self.__index = self
    self._first = 1
    self._last = 0
    self._mailBox = {}
    return o
end
function mailBox:push( msg )
    local mBox = self._mailBox

    mBox[self._last + 1] = msg
    self._last = self._last + 1

end
function mailBox:pushLeft( msg )
    local mBox = self._mailBox
    assert((self._first - 1) ~= 0, "Cannot Push Over Head")
    mBox[self._first - 1] = msg
    self._first = self._first - 1
end
function mailBox:pop(  )
    if  self._first <= self._last then
        local r = self._mailBox[self._first]
        self._mailBox[self._first] = nil
        self._first = self._first + 1
        return r
    else
        error("No Message")
    end
end
function mailBox:available(  )
    return (self._first <= self._last)
end
local cacheBox = mailBox:new()
function recv(...)
    if cacheBox:available() then
        local result = cacheBox:pop()
        return unpack(result)
    else
        return _glr.recv(...)
   end
end
function recvByCondition( condition , ...)

    local cache = {}
    function restore(  )
        while cacheBox:available() do
            cache[#cache + 1] = cacheBox:pop()
        end
        for i,v in ipairs(cache) do
            cacheBox:push(cache[i])
            cache[i] = nil
        end
        cache = nil
    end
    while true do
        local mType, mAddr, msg = recv(...)
        if mType == nil then
            return nil
        end
        local err, flag = pcall(condition, {mType, mAddr, msg})
        if not err then
            restore()
            error(flag)
        end
        if flag then
            restore()
            return mType, mAddr, msg
        else
            cache[#cache+1] = {mType, mAddr, msg}
        end
    end   
end
function recvByAddr( addr, ... )
    return recvByCondition(function ( msg )
                               return (msg[2].host == addr.host and
                                       msg[2].port == addr.port and
                                       msg[2].gpid == addr.gpid)
                           end, ...)
end
function recvByType( mType, ... )
    return recvByCondition(function ( msg )
                               return (msg[1] == mType)
                           end, ...)
end
function kill(gpid)
	return _glr.kill(gpid)
end


status = {
	processes = function () return _glr.all() end;
	status = function (gpid) return _glr.status(gpid) end;
}

sys = {
	host = self_host,
	port = self_port,
	gpid = __id__,
	--path = _glr.get_path(),
}


global = _glr.global
get_global = _glr.get_global
CLOSED = _glr.CLOSED
KILL = _glr.KILL
APP = _glr.APP
EXIT = _glr.EXIT

AF_INET = _glr.AF_INET
msg_available = _glr.msg_available
set_options=_glr.set_options
glr_stamp = _glr.glr_stamp
exit = _glr.exit
time = {
	now = function () return _glr.int(0,0) end;
	sleep = function (sec)  return _glr.int(0,1,sec) end
}


net = {

	-- const viarible --
	AF_INET = _glr.AF_INET;
	AF_UNIX = _glr.AF_UNIX;



	-- interface -- 
	tcp_server =  function (host, port)
		
		return _glr.int(1,     
						-- Interrupt ID
						0,   
						-- Operation Type: make server
						AF_INET,   
						-- Domain
						host,   
						-- host
						port 
						-- port
		)
	end;

    un_server = function (path)
		return _glr.int(1,       
						-- Interrupt ID
						0, 
						-- Operation Type: make server
						AF_UNIX,   
						-- Domain
						path
						-- Path
		)
	end;

	tcp_conn = function (host, port)
		
		return _glr.int(1,     
						-- Interrupt ID
						1,   
						-- Operation Type: connect to server
						AF_INET,   
						-- Domain
						host,   
						-- host
						port 
						-- port
		)
	end;

	un_conn = function (path)
		return _glr.int(1,       
						-- Interrupt ID
						1, 
						-- Operation Type: connect
						AF_UNIX,   
						-- Domain
						path
						-- Path
		)
	end;
	accept = function (fd)
		return _glr.int(1, 4, fd)
	end;
	recv =  function (fd, len)
		return _glr.int(1,     
						-- Interrupt ID
						2,       
						-- Operation Type: recv 
						fd,           
						-- fd
						len
						-- length
		)           
	end;

	send = function (fd, buf)
		return _glr.int(1, 3, fd, buf)
	end;

	close = function (fd)
		return _glr.int(1, 5, fd)
	end;
}


-- option



local function getopt( arg )
	local tab = {}
	for k, v in ipairs(arg) do
		if string.sub( v, 1, 2) == "--" then
			local x = string.find( v, "=", 1, true )
			if x then tab[ string.sub( v, 3, x-1 ) ] = string.sub( v, x+1 )
			else      tab[ string.sub( v, 3 ) ] = true
			end
		elseif string.sub( v, 1, 1 ) == "-" then
			local y = 2
			local l = string.len(v)
			local jopt
			while ( y <= l ) do
				jopt = string.sub( v, y, y )
				--				if string.find( options, jopt, 1, true ) then
				if true then
					if y < l then
						tab[ jopt ] = string.sub( v, y+1 )
						y = l
					else
						tab[ jopt ] = arg[ k + 1 ]
					end
				else
					tab[ jopt ] = true
				end
				y = y + 1
			end
		end
	end
	return tab
end

local arg_tab

function get_option(key)
	if not arg_tab then
		arg_tab = getopt(_glr.get_option(), key)
	end
	return arg_tab[key]
end

local gar=require "gar_loader"
run_gar=gar.run_gar

function run_spyer( ... )
    local spyerModuleName = get_option("spyer") or "spyer"
    local spyerModule = require(spyerModuleName)
    return spyerModule.main( ... )
end



return M

