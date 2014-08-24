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

function request:to_string()
    self.header[#self.header + 1] = ""
    return table.concat(self.header, "\r\n")
end
function request:parse()
    self:parse_path()
    self:parse_query()
    self:parse_accept_encoding()
    self:parse_accept_language()
    self:parse_cookie()
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
    print("Q", query)
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
    if self["Content-Type"] then
        local content_type = split(self["Content-Type"])
        self["Content-Type"]["Type"] = concat_type[1]
        if concat_type[1] == "application/x-www-form-urlencoded" then
            self["Content-Type"]["encode"] = concat_type[2]
        elseif concat_type[1] == "multipart/form-data" then
            --TODO
        else
            error("not support Content-Type " .. self["Content-Type"]["Type"] )
        end
    end
end

function request:parse_content()
    if self["body"] then
        local body = self["body"]
        if self["Content-Type"]["Type"] == "application/x-www-form-urlencoded"  then
            self["params"] = {}
            local content = split(body,"&")
            for _,k in pairs(content) do
                key,value = split(k,"=")
                self["params"][key] = value
            end
        else
            error("not support Content-Type " .. self["Content-Type"]["Type"] )
        end
    end
end
