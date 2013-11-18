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
    pprint.pprint(lines)
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


if ... == "__main__" then
    _parseUri("http://www.google.com:80/login/test")
    _parseUri("www.google.com:80/login")
    _parseUri("www.google.com/login")
    _parseUri("http://www.google.com/login")
    local uri = "http://joncraton.org/blog/46/netcat-for-windows"
    local req = httpRequest:new():init("GET", uri)
    print(req:toString())
    local cli = httpClient:new()
    --while true do
    cli:get(req)
    --end
    
end