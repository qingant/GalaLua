module(...,package.seeall)
local pprint = require("pprint").pprint

local socket  = require("glr.socket")
local strUtils = require("str_utils")

request = {}

function request:new(...)
   local o = {}
   o.version = "HTTP/1.1"
   setmetatable(o, self)
   self.__index = self
   return o
end

function _parse_uri(uri)
    --print(uri)
    local err, host, port, path

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

function request:init(type, uri)
    assert(type=="GET" or type=="POST", "Invalid HTTP Method")
    self._type = type
    local host, port, path = _parse_uri(uri)
    self._host = host
    self._path = path or "/"
    self._port = port or 80
    self._params = {} -- {{k,v},{k1,v1}}
    return self
end

function request:set_param(k,v)
    self._params[k] = v
end

function request:to_string()
    local lines = {}
    lines[#lines+1] = string.format("%s %s %s\r\n", self._type, self._path, self.version)
    self:set_param("Host",self._host)
    self:set_param("Connection","keepalive")
    self:set_param("Accept","*/*")
    for k, v in pairs(self._params) do
        lines[#lines+1] = string.format("%s:%s\r\n",k,v)
    end
    lines[#lines+1] = "\r\n"
    --local content = {}
    -- table.foreachi(self._params, function (idx)
    --                                    local item = self._params[idx]
    --                                    content[#content + 1] = string.format("%s=%s",item[1], item[2])
    --                                end)
    -- content = table.concat(content, "&")
    -- print(content)
    -- lines[#lines+1] = string.format("Content-Length:%d", content:len())
    -- lines[#lines+1] = content
    -- pprint.pprint(lines)
    return table.concat(lines)
end

local get_statusCode =  function(line)
    local s = string.match(line,"^%s*HTTP/%d%.%d%s+(%d%d%d)%s+.+$")
    s = s or "not status line"
    return tonumber(s)
end

client = {}

function client:new()
   local o = {}
   setmetatable(o, self)
   self.__index = self
   return o
end

function client:init(host,port)
    self._socket = socket.socket:new()
    local ok,errmsg=self._socket:connect(host, port)
    if not ok then
        return false,errmsg
    end
    return ok,errmsg
end

function client:send_request(req)
    local ret, err_msg = self._socket:send(req:to_string())
    if not ret then
        err_msg = string.format("client send_request error: %s",err_msg)
        return ret,err_msg
    end
    return ret,err_msg
end

function client:get_response()
    local ret, err_msg = self:_get_response()
    if not ret then
        err_msg = string.format("client get_response recv error: %s",err_msg)
    end
    return ret,err_msg
end

function client:close()
    self._socket:close()
end

function client:_get_response(timeout)
    local timeout = timeout or 6000
    local response = {}
    local initLine,err_msg = self._socket:recvLine(timeout)
    if not initLine then
        err_msg= string.format("initLine error: %s",err_msg)
        return initLine,err_msg
    end

    initLine=initLine:trim()
    -- get header
    response["status_code"] = get_statusCode(initLine)

    while true do
        local line,err_msg = self._socket:recvLine(timeout)
        if not line then
            err_msg = string.format("recvLine error: %s",err_msg)
            return line,err_msg
        end
        line=line:trim()
        if line == "" then
            break
        end
        local key, value = unpack(string.split(line, ":"))
        if value then
            response[key] = value:trim()
        else
            response[key] = nil
        end
    end

    -- get body
    if response["Content-Length"] then
        local content,err_msg =
                self._socket:recv(tonumber(response["Content-Length"]), timeout)
        if not content then
            local err_msg = string.format("recv error: %s",err_msg)
            return content, err_msg
        end
        response.content = content
    elseif response["Transfer-Encoding"] == "chunked" then
        response["content"] = ""
        local cnt = 0
        while true do
            local len,errmsg = self._socket:recvLine(timeout)
            if len:trim() ~= "0" then
                len = "0x" .. len
                len = tonumber(len) + 2
                local content,errmsg = self._socket:recv(len,timeout)
                response["content"] =
                string.format("%s%s",response["content"],content:trim() or "")
            else
                len = "0x" .. len
                len = string.format("%d",len) + 2
                local content,errmsg = assert(self._socket:recv(len,timeout))
                break
            end

        end
    end

    local header = {}
    for k,v in pairs(response) do
        if k ~= "content" then
            header[k] = v
        end
    end
    response["header"] = header
    return response
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

function client:recur_request(url,depth)
    ---print("--------url-----------",url)
    local uri = "http://"
    if string.match(url,"http") then
       uri = uri .. string.match(url,"//(.-)/")
    end
    uri = uri .. string.match(url,"(.-)/")
    depth = depth + 1
    local req = request:new():init("GET", url)
    --local cli = client:new()

    self:send_request(req);
    local response,err_msg = self:get_response()
    if not response then
        err_msg = string.format("get_response error:%s", err_msg)
        return response, err_msg
    end
    urls  = find_all_urls(uri,response)
    if urls == "" then
	    return response
    end

    pprint(urls,"-------depth:".. depth .. "-------index:".. 0 .. "---- url:".. url .."--------------")
    for i,_url in pairs(urls) do
        print("-------depth:".. depth .. "-------index:".. i .. "---- url:".. _url .."--------------")
        self:recur_request(_url,depth)
    end
end

function test_recur_request()
    local uri = "127.0.0.1:8080"
    local path = "/static/index.html"
    url= uri .. path
    local depth = 0
    local cli = client:new()

    if recur_request then
        cli:recur_request(url,depth)
    end
end

function test_http_client()
    --local url = "www.baidu.com"
    local url = "127.0.0.1:8080"

    local req = request:new():init("GET",url)
    local cli = client:new()
    print("req:to_string:")
    print(req:to_string())
    print(req._host, req._port)
    cli:init(req._host,req._port)
    local res, err_msg = cli:send_request(req)
    if not res then
        print(res, err_msg)
        return res,err_msg
    end
    res,err_msg = cli:get_response()
    print("status_code: ",res["status_code"])
    --pprint(res["header"],"header")
    pprint(res,"header")
end

if ... == "__main__" then
    test_http_client()
end
