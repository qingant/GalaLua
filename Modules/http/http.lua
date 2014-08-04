module(...,package.seeall)
local socket = require("glr.socket")
local strUtils = require("str_utils")
local request = require(_PACKAGE .. "request").request
local split = require(_PACKAGE .. "utils").split
local strip = require(_PACKAGE .. "utils").strip
local get_date = require(_PACKAGE .. "utils").get_date


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

function http:get_request(timeout)
    local timeout = timeout or 3000 -- ms
    local request = request:new():init()
    local initLine = assert(self._socket:recvLine(timeout)):trim()
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
        --local key, value = unpack(string.split(line, ":"))
        --print("line",line)
        local s,e = string.find(line,"%s*:%s*") 
        --print(s,e)
        key = string.sub(line,1,s-1)
        value = string.sub(line,e+1,#line)
        --print("key",key)
        --print("value",value)
        request[strip(key)] = value:trim()
    end
    request:parse_query()
    request:parse_path(request)
    request:parse_accept_encoding()
    request:parse_accept_language()
    request:parse_cookie()
    return request
end


function http:send_response(response)
    local header = response:to_string()
    local body = response.content
    self._socket:send(header)
    if body then
        if response.chunked then
            response:encode_chunk(body)
            for i,v in pairs(response.chunk) do
                self._socket:send(v)
            end
        else
            self._socket:send(body)
        end
    end
end

if ... == "__main__" then
    table = {1,2,4,5,5}
    print(table.concat(table,1,3, "."))
    local pprint = require("pprint")
    local rsp = response:new()
    rsp:set_content("<html></html>")
    print(rsp:to_string())
    print(rsp.content)
    local sFd = glr.net.tcp_server("0.0.0.0", 8080)
    while true do
        local fd = glr.net.accept(sFd)
        local protocol = http:new():init(fd)
        local request = protocol:getRequest()
        pprint.pprint(request, "Request")
        local rsp = response:new()
        rsp["Content-Type"] = "text/plain"
        rsp:set_content("Hello World")
        protocol:send_response(rsp)
        glr.net.close(fd)
    end
end
