module(...,package.seeall)

local socket  = require("glr.socket")
local strUtils = require("str_utils")

httpRequest = {}

function httpRequest:new(...)
   local o = {}
   o.version = "HTTP/1.1"
   setmetatable(o, self)
   self.__index = self
   return o
end
local pprint = require("pprint")
function _parseUri(uri)
    print(uri)
    local err, host, port, path
    err, host, port, path = uri:match("(http://)([^/:]*):?(%d*)(/?.*)")

    if uri:match("http://") then
        err, host, port, path = uri:match("(http://)([^/:]*):?(%d*)(/?.*)")
    else
        host, port, path = uri:match("([^/:]*):?(%d*)(/?.*)")
    end
    print(err, host, port, path)
    if port == "" then
        port = nil
    end
    if path == "" then
        path = nil
    end
    return host,port,path
end
function httpRequest:init(type, uri, params)
    assert(type=="GET" or type=="POST", "Invalid HTTP Method")
    self._type = type
    local host, port, path = _parseUri(uri)
    self._host = host
    self._path = path or "/"
    self._port = port or 80
    print(self._port)
    self._params = params or {} -- {{k,v},{k1,v1}}
    return self
end
function httpRequest:setParam(k,v)
    self._params[#self._params + 1] = {k,v}
end
function httpRequest:toString(...)
    local lines = {}
    lines[#lines+1] = string.format("%s %s %s\r\n", self._type, self._path, self.version)
    lines[#lines+1] = string.format("HOST:%s\r\n", self._host)
    lines[#lines+1] = "Connection:Closed\r\n"
    local content = {}
    -- table.foreachi(self._params, function (idx)
    --                                    local item = self._params[idx]
    --                                    content[#content + 1] = string.format("%s=%s",item[1], item[2])
    --                                end)
    -- content = table.concat(content, "&")
    -- print(content)
    -- lines[#lines+1] = string.format("Content-Length:%d", content:len())
    lines[#lines+1] = "\r\n"
    -- lines[#lines+1] = content
--    pprint.pprint(lines)
    return table.concat(lines)
end




httpClient = {}


function httpClient:new()
   local o = {}
   setmetatable(o, self)
   self.__index = self
   return o
end


function httpClient:get(req)
    self._socket = socket.socket:new()
    assert(self._socket:connect(req._host, req._port))
    print("Connected!")
    self._socket:send(req:toString())
    local err,msg = pcall(httpClient._getResponse, self, 30)
    self._socket:close()
    assert(err, msg)
    return err
end

function httpClient:_getResponse(timeout)
    print("GetResponse")
    local timeout = timeout or 30
    local response = {}
    local initLine = assert(self._socket:recvLine(timeout)):trim()
    
    print(initLine)
    local header = {}
    while true do
        local line = assert(self._socket:recvLine(timeout)):trim()
        if line == "" then
            break
        end
        -- header[#header+1] = line
        print("LINE",line)
        pprint.pprint(line)
        local key, value = unpack(string.split(line, ":"))
        response[string.upper(key)] = value:trim()
        print(key, value)
    end
    if response["CONTENT-LENGTH"] then
        local content = assert(self._socket:recv(tonumber(response["CONTENT-LENGTH"]), timeout))
        response.content = content
    elseif response["TRANSFER-ENCODING"] == "chuncked" then
        
    end
    pprint.pprint(response, "response")
    return response

end

function httpClient:get2(req)
    self._socket = socket.socket:new()
    local ok,errmsg=self._socket:connect(req._host, req._port)
    if not ok then
        return false,errmsg
    end
    local ok,errmsg=self._socket:send(req:toString())
    if not ok then
        return false,errmsg
    end
    local ok,msg = self:_getResponse2(30)
    self._socket:close()
    return ok,msg
end


function getStatusCode(line)
    return tonumber(string.match(line," (%d%d%d) "))
end

function httpClient:_getResponse2(timeout)
    local timeout = timeout or 30
    local response = {}
    local initLine,errmsg = self._socket:recvLine(timeout)

    if not initLine then
        return false,errmsg or "timeout"
    end

    initLine=initLine:trim()
    
    local header = {}
    while true do
        local line,errmsg = self._socket:recvLine(timeout)
        if not line then
            return false,errmsg or "timeout"
        end
        line=line:trim()
        if line == "" then
            break
        end
    end

    return pcall(getStatusCode,initLine)
end

function httpClient:get2(req,timeout)
    self._socket = socket.socket:new()
    local ok,errmsg=self._socket:connect(req._host, req._port)
    if not ok then
        return false,errmsg
    end
    local ok,errmsg=self._socket:send(req:toString())
    if not ok then
        return false,errmsg
    end
    local ok,msg = self:_getResponse2(timeout)
    self._socket:close()
    return ok,msg
end


function getStatusCode(line)
    local s=assert(string.match(line,"^%s*HTTP/%d%.%d%s+(%d%d%d)%s+.+$"),"not status line")
    return tonumber(s)
end

function httpClient:_getResponse2(timeout)
    local timeout = timeout or 5
    local response = {}
    local initLine,errmsg = self._socket:recvLine(timeout)

    if not initLine then
        return false,errmsg or "timeout"
    end

    initLine=initLine:trim()
    
    while true do
        local line,errmsg = self._socket:recvLine(timeout)
        if not line then
            return false,errmsg or "timeout"
        end
        line=line:trim()
        
        if line == "" then
            break
        end
        local key, value = unpack(string.split(line, ":"))
        response[string.upper(key)] = value:trim()
    end
    if response["CONTENT-LENGTH"] then
        local content,errmsg = self._socket:recv(tonumber(response["CONTENT-LENGTH"]), timeout)
        if not content then
            return false,errmsg or "timeout"
        end
        response.content = content
    elseif response["TRANSFER-ENCODING"] == "chuncked" then
        
    end
    
    return pcall(getStatusCode,initLine)
end

if ... == "__main__" then
    local uri = "http://joncraton.org/blog/46/netcat-for-windows"
    -- uri="192.188.150.119:8888"
    uri = "www.google.com"
    uri = "www.baidu.com"
    uri = "www.youtube.com"
    uri = "127.0.0.1:880"
    -- uri = "http://qt-project.org/doc/qt-5.0/qtmultimedia/audiooverview.html"
    -- uri = "http://dlang.org/phobos/std_container.html"
    -- uri = "http://www.iteye.com"
    while true do
        local req = httpRequest:new():init("GET", uri)
        local cli = httpClient:new()
        print("\nxxxx:",cli:get2(req,20))
    end
    --end
    
end