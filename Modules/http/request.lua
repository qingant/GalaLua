module(...,package.seeall)
require "str_utils"
require "tab_utils"

--local map=table.map
local function map(self, func, ... )
    local result = {}
    for i,v in ipairs(self) do
        result[i] = func(v,...)
    end
    return result
end

local function decodeURI(s)
    s = string.gsub(s, '%%(%x%x)', function(h) return string.char(tonumber(h, 16)) end)
    return s
end


local HTTP = {}

function HTTP:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    self.init(o)
    return o
end

function HTTP:init()
    self.params = {}

    self._body_unpacker={
        ["application/x-www-form-urlencoded"]=function (content)
            --TODO: 1.key是允许重复的
            local body = {}
            for _,k in pairs(string.split(content,"&")) do
                local key,value = string.match(k,"^(.+)=(.*)$")
                body[key] = decodeURI(value)
            end
            return body
        end,
    }

    return self
end

function HTTP:parse_uri()
    local uri=self.uri
    assert(uri,"HTTP uri is nil")

    local path, query = string.match(uri, "^(/[^?]*)%??(.*)#?$")
    self.path = path
    
    if query then
        map(string.split(query,"&"),
            function (key)
                local t = string.split(key,"=")
                self.params[t[1]] = t[2]
            end)
    end
    self.query = self.params
end

function HTTP:parse_startLine(line)
    local t = map(string.split(line, " "),string.trim)
    assert(#t==3,"Invalid start line")

    self.method=t[1]
    self.uri=t[2]
    self.version=t[3]
    
    self:parse_uri()
end

function HTTP:parse_headers(headers)
    self.headers=headers
    self:parse_accept(headers,"Accept-Language")
    self:parse_accept(headers,"Accept-Encoding")

    self:parse_cookie(headers)
    self:parse_content_type(headers)
end

function HTTP:parse_accept(headers,key)
    local value = {}
    local s = headers[key]
    if s then
        self.headers[key]= map(string.split(s,","), string.trim)
    end
end

function HTTP:parse_cookie(headers)
    local cookie=headers["Cookie"]
    if cookie then
        local session = {}
        map(string.split(cookie,","), function(k)
            local t = string.split(k,"=")
            session[string.trim(t[1])] = string.trim(t[2])
        end)
        self.headers.Cookie=session
    end
end

function HTTP:parse_content_type(headers)
    local content_type=headers["Content-Type"]
    local ct={}
    if content_type then
        local t = string.split(content_type,";")
        ct.type=string.trim(assert(t[1],"Invalid Content-Type"))
        ct.encode=t[2] and string.trim(t[2])
    end

    self.headers["Content-Type"]=ct
end

function HTTP:parse_content(content)
    local content = content
    if content then
        local unpacker = self._body_unpacker[self.headers["Content-Type"]["type"]]
        if unpacker then
            self.body=unpacker(content)
            return
        else
            self.body=content
            return
        end
    end
end

function new()
    return HTTP:new()
end

