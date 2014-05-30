--[[
	Author : Ma Tao
	Date   : 2014/04/18

	== Introduction ==
	core sys-calls wrapper for glr
	NEVER use this module directly
]]

module(..., package.seeall)




local self_host, self_port = _glr.node_addr()
local SOCK_INT_NO = 0x01000000
local CLOK_INT_NO = 0x00000000
local GBUS_INT_NO = 0x02000000

-- msg types
CLOSED = _glr.CLOSED
KILL = _glr.KILL
APP = _glr.APP
EXIT = _glr.EXIT

AF_INET = _glr.AF_INET
msg_available = _glr.msg_available
set_options=_glr.set_options
glr_stamp = _glr.glr_stamp
exit = _glr.exit

function spawn(mod_name, entry, ...)
	return _glr.spawn(mod_name, entry, ...)
end
function spawn_ex(bindPid, mod_name, entry, ...)
    return _glr.spawn_ex(bindPid, mod_name, entry, ...)
end

function send(addr, msg, attr)
	if type(addr) == "table" then
		if self_host == addr.host and self_port == addr.port then
			return _glr.send(addr.gpid, msg, attr)
		else
			return _glr.int(GBUS_INT_NO, 1, addr, msg, attr) -- node send
		end
	elseif type(addr) == "number" then
        return _glr.send(addr, msg, attr)
    elseif type(addr) == "string" then
        return _glr.send(_glr.npt.get(addr), msg, attr)
	end
end


-- TODO: use general timeout
function connect(host, port, pid, timeout)
	return _glr.int(GBUS_INT_NO, 0, host, port, pid, timeout)
end
function close_node(host, port)
    return _glr.int(GBUS_INT_NO, 4, host, port)
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
function recv_by_condition( condition , ...)

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
function recv_by_addr( addr, ... )
    return recv_by_condition(function ( msg )
                               return (msg[2].addr.host == addr.host and
                                       msg[2].addr.port == addr.port and
                                       msg[2].addr.gpid == addr.gpid)
                           end, ...)
end
function recv_by_type( mType, ... )
    return recv_by_condition(function ( msg )
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




time = {
	now = function () return _glr.int(CLOK_INT_NO,0) end;
	sleep = function (sec)  return _glr.int(CLOK_INT_NO,1,sec) end
}
