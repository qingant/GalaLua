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
    get_query(request)
    return request
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

function http:sendResponse(o)
    self._socket:send(o:toString())
    if o.content then
        self._socket:send(o.content)
    end
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
