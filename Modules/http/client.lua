--[[
    A clone of python's requests.
    
]]
module(...,package.seeall)
local pprint = require("pprint").pprint

local socket  = require("glr.socket")
local strUtils = require("str_utils")
require("tab_utils")

support_methods={GET=true, PUT=true, POST=true,DELETE=true}

local Request = {}
request=Request
function Request:new(...)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    self.init(o,...)
    return o
end

--[[
TODO:完整的url支持如http://user:pass@host:port/path
]]
function Request:_parse_uri(uri)
    local index,last=uri:find("^"..self._schema)
    if index then
        uri=uri:sub(last+1)
    end

    local http_host, path = uri:match("^([^/]+)(.*)$")
    if path == "" then
        path = "/"
    end
    self._http_host = http_host
    self._path = path

    local index=self._http_host:find(":")
    if index then
        self._host=self._http_host:sub(1,index-1)
        self._port=assert(tonumber(self._http_host:sub(index+1)),"invalid url")
    else
        self._host=self._http_host
        self._port=80
    end
end

function Request:get_host()
    return self._host
end

function Request:get_port()
    return self._port
end

function Request:get_http_host()
    return self._http_host
end

--[[
TODO:
    1.Only support http right now, should support https.
    2.根据@method设置默认header，以及增加更严格的错误检查
]]
function Request:init(url,method,headers,data)

    self._default_method="GET"

    self._schema="http://"
    self._version = "HTTP/1.1"
    self._user_agent="GLR/1.0"
    self._sep="\r\n"

    self:_parse_uri(url)

    self._method=method or self._default_method

    self._headers={
        ["User-Agent"]=self._user_agent,
        Host=self._http_host,
        Connection="Keep-Alive",
        Accept="*/*",
    }

    if self._method=="POST" then
        self:add_headers({["Content-Type"]="application/x-www-form-urlencoded"})
    end

    self:add_headers(headers)

    self:set_data(data)
end

function Request:set_data(data)
    self._data=data or ""
    self:add_headers({["Content-Length"]=#self._data})
end

--[[
TODO: 1.支持多种参数模式：add_headers(key,value,key,value)和 add_headers({key=value,key=value})
]]
function Request:add_headers(headers)
    if headers then
        assert(type(headers)=="table","invalid headers")
        table.update(self._headers,headers)
    end
end

function Request:to_string()
    local lines = {string.format("%s %s %s", self._method, self._path, self._version)}

    for k, v in pairs(self._headers) do
        lines[#lines+1] = string.format("%s: %s",k,v)
    end

    --\r\n after header
    lines[#lines+1]=""

    lines[#lines+1]=self._data or ""

    return table.concat(lines,self._sep)
end



local HTTPConn={}
HTTPConnection = HTTPConn
function HTTPConn:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    self.init(o)
    return o
end

function HTTPConn:init()

end

function HTTPConn:get_request_num()
    return self.request_num
end

function HTTPConn:connect(host,port)
    --self.host和self.port赋值后不可变
    if self.host then
        assert(host==self.host and port==self.port,string.format("Can only send request to http://%s:%d.",self.host,self.port))
        if self._socket then
            self.request_num=self.request_num + 1
--            print("reuse connection",host,port)
            return
        end
    else
        self.host=assert(host,"host is nil")
        self.port=port or 80
    end

--    print("new connection to",host,port)
    self._socket = socket.socket:new()
    assert(self._socket:connect(self.host, self.port))
    self.request_num=1
end

function HTTPConn:get_statusCode(line)
    return tonumber(string.match(line,"^%s*HTTP/%d%.%d%s+(%d%d%d)%s+.+$"))
end

function HTTPConn:send_request(req)
    self:connect(req:get_host(),req:get_port())

    self:safe_assert(self._socket:send(req:to_string()))
end

function HTTPConn:safe_assert(flag,...)
    if not flag then
        self:close()
        error(string.format(...))
    end
    return flag
end

function HTTPConn:close()
    if self._socket then
        self._socket:close()
        self._socket=nil
    end
end

function HTTPConn:recv_headers(timeout)
    local headers={}
    while true do
        local line,err_msg = self._socket:recvLine(timeout)
        self:safe_assert(line,"recvLine error: %s",err_msg)
        line=line:trim()
        if line == "" then
            break
        end
        local key, value = string.match(line, "([^:]+):(.+)")
        self:safe_assert(key,string.format("invalid headers: %s",line))
        headers[key] = value:trim()
    end
    return headers
end

function HTTPConn:recv_initline(timeout)
    local initLine,err_msg = self._socket:recvLine(timeout)
    self:safe_assert(initLine, string.format("initLine error: %s",err_msg))
    initLine=initLine:trim()
    return self:get_statusCode(initLine)
end

function HTTPConn:recv_body(headers,timeout)
    local content_length= tonumber(headers["Content-Length"])

    if content_length then
        local content,err_msg = self._socket:recv(content_length, timeout)
        return self:safe_assert(content, string.format("recv content error: %s",err_msg))

    elseif headers["Transfer-Encoding"] == "chunked" then
        local chunk={}
        while true do
            local len,errmsg = self._socket:recvLine(timeout)
            if len:trim() == "0" then
                local content,errmsg = self._socket:recv(2,timeout)
                self:safe_assert(content, string.format("invalid chunked message:%s",errmsg))
                break
            else
                len = tonumber(len,16) + 2
                local content,errmsg = self._socket:recv(len,timeout)
                self:safe_assert(content,string.format("invalid chunked message:%s",errmsg))
                chunk[#chunk+1] = content:trim()
            end
        end
        return table.concat(chunk)
    end
end

function HTTPConn:get_response(timeout)
    local timeout = timeout or 6000

    local response = {}

    response.status_code = self:recv_initline(timeout)

    local headers = self:recv_headers(timeout)
    response.headers = headers

    response.content=self:recv_body(headers,timeout)
    
    return response
end



local Client={}
client=Client
function Client:new()
    local o={}
    setmetatable(o,self)
    self.__index=self
    self.init(o)
    return o
end

function Client:init()
    self._http_connections={}
    self.retry=2
end

--[[
TODO: table参数转化为url查询参数
如：{key1= 'value1', key2= 'value2'}===>"?key2=value2&key1=value1"
]]
function Client:params_encode(parameters)
    return ""
end

--[[
@req: string==> url字符串
      table ==>  request对象
]]
function Client:get_conn(req)
    if type(req)=="string" then
        req=Request:new(req)
    end

    local http_host=req:get_http_host()

    local http_conn=self._http_connections[http_host]
    if http_conn==nil then
        http_conn=HTTPConn:new()
        self._http_connections[http_host]=http_conn
    end
    return http_conn
end

function Client:close_conn(url)
    self:get_conn(url):close()
end

--[[
@kwargs: {headers={},data=, params=}
    @headers: 
    @data: 
    @params: url parameters
    @timeout: request timeout
]]
function Client:request(method,url,kwargs)
    local kwargs=kwargs or {}
    local timeout=kwargs.timeout

    local url=url..self:params_encode(kwargs.params)

    local req=Request:new(url,method,kwargs.headers,kwargs.data)

    local http_conn=self:get_conn(req)

    --[[
        即使客户端请求r1中使用HTTP1.1或者headers里包含"Connection: Keep-Alive"，服务器仍然可能在该请求完成后关闭连接，
        导致下一个请求（即r2）重用该连接可能会失败，因此应该确保请求r2有一次重新建立连接的机会。
    ]]
    local ok,res
    for i=1,self.retry do
        ok,res=pcall(function()
                http_conn:send_request(req)
                return http_conn:get_response(timeout)
            end)
        if ok then
            return res
        elseif not ok then
            if http_conn:get_request_num()>1 then
                --print("reuse connection error:",res)
            else
                break
            end
        end
    end
    error(res)
end

for m in pairs(support_methods) do
    Client[string.lower(m)]= function (self,...) return self:request(m,...) end
end


if ... == "__main__" then
    local cjson=require "cjson"

    local function test_HTTPConnection(url,url2)
        local req=Request:new(url,"GET",{["Content-Type"]="application/json"},cjson.encode{abc=122})

        local http_conn=HTTPConnection:new()
        http_conn:send_request(req)
        pprint(http_conn:get_response())
    
        if url2 and url1~=url2 then
            --应该要出错
            local req_err=Request:new(url2,"GET",{["Content-Type"]="application/json"},cjson.encode{abc=122})
            http_conn:send_request(req_err)
        end
    end

    local function test_client(url,url2)
        local cli=Client:new()
        pprint(cli:get(url))
        print(":::::::::::::::")
        print("wait 65s for connection down")
        glr.time.sleep(65)
        pprint(cli:get(url))
        print(":::::::::::::::")
        if url2 then
            pprint(cli:get(url2))
        end
    end

    local function test_client_close(url)
        local cli=Client:new()
        pprint(cli:get(url))
        print(":::::::::::::::")
        pprint(cli:get(url))
        print(":::::::::::::::")
        cli:close_conn(url)
        pprint(cli:get(url))
        print(":::::::::::::::")
    end


    local url=glr.get_option("u") or "http://httpbin.org"

    local url2=glr.get_option("U")
--[[
    test_HTTPConnection(url,url2)

    test_client(url)

    ]]
    test_client_close(url)

end
