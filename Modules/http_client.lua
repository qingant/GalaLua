module(...,package.seeall)
local pprint = require("pprint").pprint

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

function _parseUri(uri)
    --print(uri)
    local err, host, port, path
    err, host, port, path = uri:match("(http://)([^/:]*):?(%d*)(/?.*)")

    if uri:match("http://") then
        err, host, port, path = uri:match("(http://)([^/:]*):?(%d*)(/?.*)")
    else
        host, port, path = uri:match("([^/:]*):?(%d*)(/?.*)")
    end
    --print(err, host, port, path)
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
    --print(self._port)
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


local getStatusCode =  function(line)
    local s=assert(string.match(line,"^%s*HTTP/%d%.%d%s+(%d%d%d)%s+.+$"),"not status line")
    return tonumber(s)
end


httpClient = {}


function httpClient:new()
   local o = {}
   setmetatable(o, self)
   self.__index = self
   return o
end

function httpClient:try_exception(e_type)
    self._socket = socket.socket:new()
    if e_type == "only_connect" then
        assert(self._socket:connect(req._host, req._port))
    elseif e_type == "only_send" then
        assert(self._socket:connect(req._host, req._port))
        self._socket:send(req:toString())
    end
end

function httpClient:get(req)
    self._socket = socket.socket:new()
    assert(self._socket:connect(req._host, req._port))
    --print("Connected!")
    self._socket:send(req:toString())
    local err,msg = pcall(httpClient._getResponse, self, 3000)
    self._socket:close()
    assert(err, msg)
    return err
end

function httpClient:_getResponse(timeout)
    --print("GetResponse")
    local timeout = timeout or 3000
    local response = {}
    local initLine = assert(self._socket:recvLine(timeout)):trim()
    if not initLine then
        return false,errmsg or "timeout"
    end
    --print(initLine)
    -- get header
    local header = {}
    while true do
        local line = assert(self._socket:recvLine(timeout)):trim()
        if line == "" then
            break
        end
        local key, value = unpack(string.split(line, ":"))
        response[key] = value:trim()
    end
    pprint(response,"response")
    -- get body
    if response["Content-Length"] then
        local content =
                       assert(self._socket:recv(tonumber(response["Content-Length"]), timeout))
        response.content = content
        --print("--- response body ---" , response.content)
    elseif response["Transfer-Encoding"] == "chunked" then
        response["content"] = ""
        while true do
            local len,errmsg = assert(self._socket:recvLine(timeout))
            if len ~= 0 then
                len = "0x" .. len
                len = string.format("%d",len) + 2
            else
                local content,errmsg = self._socket:recv(len,timeout)
                break
            end

            local content,errmsg = assert(self._socket:recv(len,timeout))
            response["content"] = string.format("%s%s",response["content"],content:trim())
        end
    end
    --print(response.content, "response")
    return response["content"]

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
    local ok,msg = self:_getResponse2(300)
    self._socket:close()
    return ok,msg
end


function httpClient:_getResponse2(timeout)
    local timeout = timeout or 5000
    local response = {}
    local initLine,errmsg = self._socket:recvLine(timeout)

    if not initLine then
        return false,errmsg or "timeout"
    end

    initLine=initLine:trim()
    -- get header
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
        response[key] = value:trim()
    end

    -- get body
    if response["Content-Length"] then
        local content,errmsg =
        self._socket:recv(tonumber(response["Content-Length"]), timeout)
        if not content then
            return false,errmsg or "timeout"
        end
        response.content = content
        --print("--- response body ---" , response.content)
    elseif response["Transfer-Encoding"] == "chunked" then
        response["content"] = ""
        while true do
            local len,errmsg = self._socket:recvLine(timeout)
            if not len then
                return false,errmsg or "timeout"
            elseif len ~= 0 then
                len = "0x" .. len
                len = string.format("%d",len) + 2
            else
                local content,errmsg = self._socket:recv(len,timeout)
                break
            end

            local content,errmsg = self._socket:recv(len,timeout)
            if not content then
                return false,errmsg or "timeout"
            else
                response["content"] = string.format("%s%s",response["content"],content:trim())
            end

        end
    end

    --return pcall(self.getStatusCode,initLine)
    return pcall(getStatusCode,initLine)
end

function find_all_urls(uri, response)
    local len = #response["content"]
    local content = response["content"]
    local s,e,cap = 1,1,""
    local urls = {}
    local i = 1
    while e <= len do
	s,e, cap = string.find(content,"src=(.-)>",s)
        if cap == nil then
            break
        end
        urls[i] = uri .. string.sub(cap,2,#cap-1)
        i = i + 1
	--print("url",uri .. string.sub(cap,2,#cap-1))
        s = e + 1
    end
    return urls
end

function httpClient:recur_request(uri,url,depth)
    ---print("--------url-----------",url)
    depth = depth + 1
    local req = httpRequest:new():init("GET", url)
    --local cli = httpClient:new()
    local response= self:get(req)
    urls  = find_all_urls(uri,response)
    if urls == "" then
	return ""
    end
    pprint(urls,"-------depth:".. depth .. "-------index:".. 0 .. "---- url:".. url .."--------------")
    for i,_url in pairs(urls) do
        print("-------depth:".. depth .. "-------index:".. i .. "---- url:".. _url .."--------------")
        self:recur_request(uri,_url,depth)
    end
end

function test_get()
    uri = "127.0.0.1:8080"
    local path = "/static/index.html"
    url= uri .. path
    local depth = 0
    local cli = httpClient:new()
    local req = httpRequest:new():init("GET",url)
    local ret, err_msg = cli:get(req)
    return ret,err_msg
end

function test_get2()
    uri = "127.0.0.1:8080"
    local path = "/static/index.html"
    url= uri .. path
    local depth = 0
    local cli = httpClient:new()
    local req = httpRequest:new():init("GET",url)
    local ret, err_msg = cli:get2(req)
    return ret,err_msg
end

function test_recur_request()
    uri = "127.0.0.1:8080"
    local path = "/static/index.html"
    url= uri .. path
    local depth = 0
    local cli = httpClient:new()

    if recur_request then
        cli:recur_request(uri,url,depth)
    end
end


if ... == "__main__" then
    --print(test_get())
    print(test_get2())
end
