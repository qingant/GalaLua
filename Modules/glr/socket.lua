module(...,package.seeall)

socket = {}


function socket:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function socket:init(fd)
    self._fd = fd
    return self
end
function socket:connect(host, port)

    local fd,errmsg=glr.net.tcp_conn(host, port, 10)
    if not fd then
        return false,errmsg
    end
    self._fd =fd
    return self
end
function socket:accept(timeout)
    return glr.net.accept(self._fd, timeout)
end

function socket:recv(len, timeout)
    return glr.net.recv(self._fd, len, timeout)
end

function socket:recvLine(timeout)
    return glr.net.get_line(self._fd, timeout)
end

socket.recv_line = socket.recvLine

function socket:close()
    local ok = nil
    local emsg = "Invalid fd: " .. self._fd
    if type(self._fd) == "number" and self._fd > 0 then
        ok, emsg = glr.net.close(self._fd)
        self._fd = -1
    end
    return ok, emsg
end

function socket:send(buf, timeout)
    return glr.net.send(self._fd, buf, timeout)
end

function socket:__gc()
    self:close()
end
