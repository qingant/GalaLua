module(...,package.seeall)
local socket = require("glr.socket")
local strUtils = require("str_utils")
--local split = require("http.utils").split

response = {}
response.Fields = {"Date","Content-Type" , "Server", "Last-Modified","Content-Length", "Expire","Cache-Control","Transfer-Encoding"}
function response:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function response:init()
    self.statusCode = "200"
    self.status = "Okay"
    self.version = "HTTP/1.1"
    self.Server = "GLR/GHS 1.0"
    self["Content-Type"] = "text/html;charset=utf-8"
    self.chunked = false
    self.chunk = {}
    self.header = "" --toString
    return self
end
function response:setContent(content)
    self.content = content
    local len = #content
    self["Content-Length"] = tostring(len)
    if len > 500 then
        self.chunked = true
    end
end
function response:setCookie(k,v)
    self["Set-Cookie"] = self["Set-Cookie"] or {}
    self["Set-Cookie"][k] = tostring(v)
end

function response:setChunk()
    if self.chunked then
        self["Transfer-Encoding"] = "chunked"
    end
end


--TODO
--compress
--deflate
--identity　　
--gziEncode
function response:setEncode(request)
    local encode = get_encode(request)
    if encode then
	for _, v in pairs(encode) do
	    if v == "gzip" then
		self["Content-Encoding"] = "gzip"
		local len = gzipEncode(self.content)
		self["Content-Length"] = len
	    else
		local len = #self.content
		self["Content-Length"] = len
	    end
	end
    end
end
function response:toString()
    if self.chunked then
        self["Transfer-Encoding"] = "chunked"
    	self["Content-Length"] = nil
    end
    local lines = {}
    lines[#lines+1] = string.format("%s %s %s\r\n", self.version, self.statusCode, self.status)
    for i,k in ipairs(self.Fields) do
        local v = self[k]
        if v then
            lines[#lines + 1] = string.format("%s:%s\r\n", k, v)
        end
    end
    lines[#lines + 1] = "\r\n"
    self.header = table.concat(lines)
    return self.header
end

--TODO
--A server MUST NOT send transfer-codings to an HTTP/1.0 client.
--A server which receives an entity-body with a transfer-coding it does not understand SHOULD return 501 (Unimplemented)
function response:encodeChunk(content)
    local len = 500
    local s = 1
    local chunk = ""
    repeat
        local data = string.sub(content,s,s + len)
        local header = string.format("%x",#data)
        --print("data", data)
        --print("header", header)
        if not data then
            chunk = "0"
            self.chunk[#self.chunk + 1] = string.format("%s\r\n",chunk)
            self.chunk[#self.chunk + 1] = "\r\n" 
        else
            chunk = string.format("%s\r\n%s\r\n",header,data)
            self.chunk[#self.chunk + 1] = chunk
        end
        s = s + len + 1
    until header == "0"
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
    get_query(request)
    get_path(request)
    get_encode(request)
    get_language(request)
    return request
end

function get_path(request)
    request.path = ""
    local query = split(request.uri,"#")
    query = slice(query,1,#query)
    query = split(query,"%?")
    local path = slice(query,1,#query)
    --pprint.pprint(path,"--path--")
    request.path = path
end
function get_query(request)
    request.query = {}
    local query = split(request.uri,"#")
    query = slice(query,1,#query)
    query = split(query,"%?")
    query = slice(query,#query)
    query = split(query,"&")
    pprint.pprint(query,"query")
    for i,key in pairs(query) do
        local k,v 
        t = split(key,"=")
        request.query[t[1]] = t[2]
    end
    --pprint.pprint(request.query,"--query--")
end

function get_encode(request)
    request.encode = {}
    --print("Accept-Encoding", request["Accept-Encoding"])
    local encode = request["Accept-Encoding"] 
    if encode then
       encode = split(encode,",")
       for k,v in pairs(encode) do
           request.encode[k] = strip(v)
       end
       return request.encode
    --pprint.pprint(encode,"--encode--")
    end
    return nil
end

function get_language(request)
    request.language = {}
    local lang = request["Accept-Language"]
    if lang then
        lang = split(lang,";")
        for i,v  in pairs(lang) do
            request.language[i] = v
        end
        --pprint.pprint(request.language, "accept-language")
    end
end

function http:sendResponse(response)
    local header = response:toString()
    local body = response.content
    --print("header:\n ",header)
    --print("content :\n ",content)
    --print("chunked :\t ",o.chunked)
    self._socket:send(header)
    if response.chunked then
        response:encodeChunk(body)
        for i,v in pairs(response.chunk) do
            print("chunk",v)
            self._socket:send(v)
        end
    else
        self._socket:send(body)
    end
end

--gen Date and If-Modified-Since for request
function get_date(date) 
    --date = os.date("!*t")
    local WEEK = {"Sun.","Mon.", "Tues.", "Wed.", "Thur.","Fri.", "Sta."}
    local MONTH = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"}
    local week = WEEK[date["wday"]]
    local day = date["day"]
    local month = MONTH[date["month"]]
    local year = date["year"]
    local time = date["hour"]..":"..date["min"]..":"..date["sec"].." ".."GMT"
    date = week.." "..day.." "..month.." "..year.." "..time
    return date
end

--table get between e,b seperate by sep 
--@param table
--@param s : start
--@param e : end
--@param sep : seprate character
--@return : string 
function slice(table,...)
    s, e, sep = ...
    if not s then 
        s = 1
    end
    if not e then
        e = #table + 1
    end

    if not sep then
        sep = ""
    end

    local str = ""
    for i,k in  ipairs(table) do
        if i == s then
            str = str..k
        elseif i > s and i < e then
            str = str..sep..k
        end
    end
    return str
end


--TODO parse pPattern replace %. as %s\ 
--@param : pString : str what to split
--@param : pPattern : character is splited by
--@return : table  like python split
function split(pString, pPattern)
    local Table = {}  -- NOTE: use {n = 0} in Lua-5.0
    local fpat = "(.-)" .. pPattern
    local last_end = 1
    --local s, e, cap = pString:find(fpat, 1)
    local s, e, cap = string.find(pString, fpat, 1)

    while s do
        if s ~= 1 or cap ~= "" then
            table.insert(Table,cap)
        end
        last_end = e+1
        s, e, cap = string.find(pString, fpat, last_end)
    end
    if last_end <= #pString then
        cap = string.sub(pString, last_end)
        table.insert(Table, cap)
    end
    return Table
end

function strip(pString)
    local s1,e1 = string.find(tostring(pString), "%s+", 1)
    print(s1,e1)
    if not e1 then
        e1 = 0
        s2,e2 = string.find(pString,"%s+", e1+1)
    end
    print(s2,e2)
    if s1 ~= 1 and s1 then
        return string.sub(pString,1,s1-1)
    end
    if not s2 then
        return string.sub(pString,e1+1)
    end
    return string.sub(pString,e1+1,s2-1)
end
if ... == "__main__" then
    table = {1,2,4,5,5}
    print(utils.slice(table,1,3, "."))
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
