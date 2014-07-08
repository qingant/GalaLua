module(...,package.seeall)
local socket = require("glr.socket")
local strUtils = require("str_utils")
response = {}
response.Fields = {"Content-Type" , "Server", "Last-Modified","Content-Length"}
function response:new(o)
    local o = o or {}
    o.statusCode = "200"
    o.status = "Okay"
    o.version = "HTTP/1.1"
    o.Server = "GLR/GHS 1.0"
    o["Content-Type"] = "text/html"
    setmetatable(o, self)
    self.__index = self
    return o
end
function response:setContent(content)
    self.content = content
    self["Content-Length"] = tostring(#content)
end
function response:setCookie(k,v)
    self["Set-Cookie"] = self["Set-Cookie"] or {}
    self["Set-Cookie"][k] = tostring(v)
end
function response:toString()
    local lines = {}
    lines[#lines+1] = string.format("%s %s %s\r\n", self.version, self.statusCode, self.status)
    for i,k in ipairs(self.Fields) do
        local v = self[k]
        if v  then
            lines[#lines + 1] = string.format("%s:%s\r\n", k, v)
        end
    end
    lines[#lines + 1] = "\r\n"
    return table.concat(lines)
end

http = {}

function http:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function http:init(fd)
    self._socket = socket.socket:new():init(fd)
    return self
end

function http:getRequest(timeout)
    local timeout = timeout or 30
    local request = {}
    local initLine = assert(self._socket:recvLine(30)):trim()
    local method, path, version = unpack(string.split(initLine, " "))
    request.method = method:trim()
    request.uri = path:trim()
    request.version = version:trim()
    local header = {}
    while true do
        local line = assert(self._socket:recvLine(30)):trim()
        if line == "" then
            break
        end
        -- header[#header+1] = line
        local key, value = unpack(string.split(line, ":"))
        request[key] = value:trim()
    end
    return request
end


function http:sendResponse(o)
    self._socket:send(o:toString())
    if o.content then
        self._socket:send(o.content)
    end
end

if ... == "__main__" then
    local pprint = require("pprint")
    local rsp = response:new()
    rsp:setContent("<html></html>")
    print(rsp:toString())
    print(rsp.content)
    local sFd = glr.net.tcp_server("0.0.0.0", 8080)
    while true do
        local fd = glr.net.accept(sFd)
        local protocol = http:new():init(fd)
        local request = protocol:getRequest()
        pprint.pprint(request, "Request")
        local rsp = response:new()
        rsp["Content-Type"] = "text/plain"
        rsp:setContent("Hello World")
        protocol:sendResponse(rsp)
        glr.net.close(fd)
    end


end
