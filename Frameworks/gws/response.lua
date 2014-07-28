module(...,package.seeall)

response = {}
response.Fields = {"Set-Cookie","Date","Content-Type" , "Server", "Last-Modified","Content-Length", "Expire","Cache-Control","Transfer-Encoding"}
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
function response:set_status_code(code)
    self.statusCode = tostring(code)
    return self
end
function response:set_content(content)
    self.content = content
    local len = #content
    self["Content-Length"] = tostring(len)
    if len > 500 then
        self.chunked = true
    end
end
function response:set_content_type(content_type)
    self["Content-Type"] = content_type
end
function response:set_cookie(v)
    self["Set-Cookie"] = v
    --self["Set-Cookie"] = self["Set-Cookie"] or {}
    --self["Set-Cookie"][k] = tostring(v)
end

function response:set_chunk()
    if self.chunked then
        self["Transfer-Encoding"] = "chunked"
    end
end


--TODO
--compress
--deflate
--identity　　
--gziEncode
function response:set_content_encode(request)
    local encode = parse_accept_encoding(request)
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

function response:to_string()
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
function response:encode_chunk(content)
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
