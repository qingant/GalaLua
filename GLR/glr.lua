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
setfenv(1, M)
local self_host, self_port = _glr.node_addr()

function spawn(mod_name, entry, ...)
	return _glr.spawn(mod_name, entry, ...)
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

function connect(host, port, pid)
	return _glr.int(2, 0, host, port, pid)
end

function recv()
	return _glr.recv()
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

return M
