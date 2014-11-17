module(...,package.seeall)
local socket = require("glr.socket")
local strUtils = require("str_utils")
local request = require(_PACKAGE .. "request")


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
function http:close()
    self._socket:close()
end

function http:get_startLine(timeout)
    return assert(self._socket:recvLine(timeout),"recv start line timeout"):trim()
end

function http:get_headers(timeout)
    local headers = {}
    while true do
        local line = assert(self._socket:recvLine(timeout),"recv headers timeout"):trim()
        if line == "" then
            break
        end
        headers[#headers + 1] = line

        local key,value = string.match(line,"^([^:]+):(.+)$")
        headers[key:trim()] = value:trim()
    end

    return headers
end

function http:get_request(timeout)
    local req=request.new()

    local timeout = timeout or 1000*5 -- ms

    req:parse_startLine(self:get_startLine(timeout))

    req:parse_headers(self:get_headers(timeout))

    local length=req.headers["Content-Length"]
    if length then
        local body = assert(self._socket:recv(tonumber(length), timeout),"recv content timeout")
        req:parse_content(body)
    end

    return req
end

function http:send_response(response)
    self._socket:send(response:to_string())
end

function http:send_chunk(response,content)
    self._socket:send(response:encode_chunk(content))
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
