module(...,package.seeall)

local split = require(_PACKAGE .. "utils").split
local strip = require(_PACKAGE .. "utils").strip
request = {}

function request:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function request:init()
    self.params = {}
    return self
end
function request:parse()
    self:parse_path()
    self:parse_query()
    self:parse_accept_encoding()
    self:parse_accept_language()
    self:parse_cookie()
    self:parse_content_type()
    self:parse_content()
    return self
end

function request:parse_path()
    if not self.uri then
        error("request uri is nil")
    end
    --local query = split(self.uri,"#")[1]
    --query = split(query,"%?")[1]
    local path, query = string.match(self.uri, "(/[^?]*)%??(.*)#?")
    self.path = path
    self.query = query
end

function request:parse_accept_encoding()
    local value = {}
    --print("Accept-Encoding", self["Accept-Encoding"])
    local encode = self["Accept-Encoding"]
    if encode then
       encode = split(encode,",")
       for k,v in pairs(encode) do
           value[k] = strip(v)
       end
       self["Accept-Encoding"] = value
    --pprint.pprint(encode,"--encode--")
    end
    return nil
end

function request:parse_accept_language()
    local value = {}
    local lang = self["Accept-Language"]
    if lang then
        lang = split(lang,";")
        for i,v  in pairs(lang) do
            value[i] = v
        end
        self["Accept-Language"] = value
        --pprint.pprint(self.language, "accept-language")
    end
    return nil
end

function request:parse_query()
    if not self.uri then
        error("request uri is nil")
    end
    local query = self.query
    --print("Q", query)
    self.query = self.params
    if query == nil then
        return
    end
    --print(self.uri,"self.uri")
    --print(query,"query",path,"path")
    if query then
        query = split(query,"&")
        for i,key in pairs(query) do
            local k,v
            local t = split(key,"=")
            self.params[t[1]] = t[2]
        end
    end
    --pprint.pprint(self.query,"--query--")
end

function request:parse_cookie()
    if self["Cookie"] then
        --print("self.Cookie",self.Cookie)
        local ses = split(self["Cookie"],"%s*,%s*")
        local session = {}
        for _,k in pairs(ses) do
            local tmp = split(k,"=")
            session[strip(tmp[1])] = strip(tmp[2])
        end
        self["Cookie"] = session
    end
    return nil
end

function request:parse_content_type()
    --print("---Content-Type----",self["Content-Type"])
    if self["Content-Type"] then
        local content_type,content_encode= string.match(self["Content-Type"],"([%w/-]+);?")
        self["Content-Type"] = {}
        self["Content-Type"]["Type"] = content_type
        if content_type == "application/x-www-form-urlencoded" then
            self["Content-Type"]["encode"] = content_encode
        elseif content_type == "multipart/form-data" then
            --TODO
            error("not support Content-Type " .. self["Content-Type"]["Type"] )
        else
            error("not support Content-Type " .. self["Content-Type"]["Type"] )
        end
    end
end

function request:parse_content()
    if self["body"] then
        local body = self["body"]
        --print("----body----",body)
        --print("--- Content-Type ---",self["Content-Type"]["Type"])
        if self["Content-Type"] then
            if self["Content-Type"]["Type"] == "application/x-www-form-urlencoded"  then
                self["body"] = {}
                local content = split(body,"&")
                for _,k in pairs(content) do
                    key,value = string.match(k,"(.+)=(.*)")
                    self["body"][key] = value
                end
            else
                error("not support Content-Type " .. self["Content-Type"]["Type"] )
            end
        else
            error("Content-Type empty")
        end
    end
end
