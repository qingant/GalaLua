--[[
--Author liuwenxue
--Date 2014-07-17

session--Call by application
    --if you want session , call context:session()
    --if you want cache-control, call context:cache()
]]

module(...,package.seeall)

local session_manager = require("http.session").session_manager
local session = require("http.session").session
local cache = require("http.cache")
local pprint = require("pprint")
local mdb = require("mdb").mdb
local get_date = require("http.utils").get_date
local split = require("http.utils").split
local parse_date = require("http.utils").parse_date
local strip = require("http.utils").strip
local diff_date = require("http.utils").diff_date

context = {}

function context:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function context:init(request,response)
    self.request = request
    self.response  = response

    self.session_manager = session_manager:new():init()

    local request = self:get_request()
    local ses = request["Cookie"]

    if ses and self.session_manager:exist(ses.id) and
       not session_manager:is_expired(ses.expires) then
       self.session = session:new():init(self.session_manager, ses.id)
       self.session.expired = ses.expires
       pprint.pprint(ses,"cookie")
       self.response["Set-Cookie"] = self.session:to_string()
    else
       self.session = nil
    end
    return self
end

function context:get_session()
    return self.session
end

function context:create_session()
    self.session = self.session_manager:create()
    pprint.pprint(self.session,"create session")
    self.response["Set-Cookie"] = self.session:to_string()
    return self.session
end


function context:get_cache_control(key)
    --if self:revaild_cache_control() then
    --    return self.cache_control:get(key)
    --end
    --return nil

    return self.cache_control:get(key)
end

function context:is_cache_control()
    local request = self:get_request()
    local cache = request["Cache-Control"]
    if cache.attrib == "no-store" or 
       request["Pargma"] == "no-cache" then
       return false
    else 
       return true
    end

end

function context:vaild_cache_control()
    local request = self:get_request()
    if self:is_cache_control() then
        local cache = request["Cache-Control"]
        local date = request["If-Modified-Since"]
        local uri = request["uri"]
        local etag = request["If-None-Match"]
        if self.cache_control:is_expired(date,cache.max_age,etag) or 
           self.cache_control:exist(uri)  or 
           cache.attrib == "no-cache" then
            return false
        else
            return true
        end
    else
        return false
    end
end

function context:set_cache_control(key,value)
    if self:revaild_cache_control() then
        return self.cache_control:set(key,value)
    end

    --框架是否需要控制 Etag 的生成方式
    --if key == "etag" then
    --    local content = gen_etag(value)
    --    self.cache_control:set(key,content)
    --else
    --    self.cache_control:set(key,value)
    --end

    --self.cache_control:set(key,value)

end

function context:remove_cache_control()
    if self:revaild_cache_control() then
        return self.cache_control:remove()
    end
    
    --self.cache_control:remove()
end

function context:create_cache_control()
    --if self:revaild_cache_control() then
    --    return self.cache_control:set(key,value)
    --end
    self.response["Cache-Control"] = self.cache_control:to_string()
    self.response["Etag"] = gen_etag(self.cache_control:get("value"))
    self.response["Last-Modified"] = self.cache_control:get("last-modified")
    self.response["Content"] = self.cache_control:get("value")
    self.cache_control:save()
end

local function gen_etag(value)
    return value
end

function context:get_request()
    return self.request
end

