module(...,package.seeall)

response = {}
response.Fields = {"Set-Cookie","Date","Content-Type" , "Last-Modified","Content-Length", "Expire","Cache-Control","Transfer-Encoding", "Location"}
function response:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function response:init()
    self._sep = "\r\n"
    self.status={
        [200] = "Okay",
        [404] = "Not Found",
        [401] = "Unauthorized",
        [403] = "Forbidden",
        [422] = "Unprocessable Entity",
        [429] = "Too Many Requests",
        [202] = "Okay",
    }

    self.version = "HTTP/1.1"

    self.statusCode = "200"
    self.reasonPhrase = "Okay"

    self.headers={
        ["Server"] = "GLR/GHS 1.0",
        ["Content-Type"] = "text/html;charset=utf-8",
    }

    self.content = ""
    return self
end

function response:set_status_code(code,msg)
    self.statusCode = tonumber(code)
    self.reasonPhrase = msg or self.status[self.statusCode]
    return self
end

function response:set_content(content)
    self.content = content
end

function response:set_content_type(content_type)
    self.headers["Content-Type"] = content_type
end

function response:set_cookie(v)
    self.headers["Set-Cookie"] = v
end

function response:redirect(location)
    self:set_status_code(302)
    self.headers["Location"] = location
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
                self.headers["Content-Encoding"] = "gzip"
                local len = gzipEncode(self.content)
                self.headers["Content-Length"] = len
            else
                local len = #self.content
                self.headers["Content-Length"] = len
            end
        end
    end
end

function response:startline()
    return string.format("%s %s %s", self.version, self.statusCode, self.reasonPhrase or "")
end

function response:enable_chunk()
    self.headers["Transfer-Encoding"]="chunked"
end

function response:disable_chunk()
    self.headers["Transfer-Encoding"]="chunked"
end

function response:is_chunked()
    return self.headers["Transfer-Encoding"]=="chunked"
end

function response:to_string()
    if not self:is_chunked() then
        self.headers["Content-Length"] = #self.content
    end

    local lines = {self:startline()}
    for k,v in pairs(self.headers) do
        lines[#lines + 1] = string.format("%s: %s", k, v)
    end

    lines[#lines + 1] = ""
    lines[#lines + 1] = self.content or ""

    return table.concat(lines,self._sep)
end

function response:encode_chunk(content)
    assert(self.is_chunked(),"not chunked response")

    local data = content or ""
    if type(data)=="table" then
        data=table.concat(data)
    end

    local data_len= #data

    local chunk={}
    chunk[#chunk + 1] = string.format("%x",data_len)
    chunk[#chunk + 1] = data
    if data_len==0 then
        --TODO：1.增加可选的trailer-part

        --chunked transfer coding要求空行结尾
        chunk[#chunk + 1] = ""
    end

    return table.concat(chunk,self._sep)
end

