--[[
--Author liuwenxue
--Date 2014-07-17

session--Call by application
    --if you want session , call context:session()
    --if you want cache-control, call context:cache()
]]
module(...,package.seeall)
--TODO logging
cache_control = {}
--attrib  no-store no-cache private public
--key  request.uri
--max_age  seconds of expired relative to if_last_modify
cache_control.Fields = {"max_age","key","value","attrib","etag","last_modified"}

function cache_control:new(o)
    o = o or {}
    setmetatable(o,self)
    self.__index = self
    return o
end

function cache_control:init()
    self.max_age = 0
    self.key = nil
    self.attrib = "private"
    self.etag = nil
    --the store must support get set exist save remove, and raise error if
    --set or remove key which isn't exist
    --
    self.store = nil
    return self
end

function cache_control:get(key)
    for i,k in pairs(self.Fields) do
        if k == key then
            return self[key]
        end
    end
    return nil
end

function cache_control:set(key,value)
    for i,k in pairs(self.Fields) do
        if k == key then
            self[key] = value
            break
        end
    end
end

function cache_control:save()
    for _,key in pairs(self.Fields) do
        self.store:save(key,self:get(key))
    end
end

function cache_control:remove(request)
    self.store:remove()
end

function cache_control:valid(request, reponse)
    if request.Pragma == "no-cache" or request.Cache-Control == "no-cache" then
        return false
    return true
end

function cache_control:exist(key)
    if self.store:exist(key) then
       return true
    else
       return false
    end
end

function cache_control:is_expired(client_last_modify, max_age, client_etag)
    --local now = get_date(-max_age) 
    local server_last_modify = get_date(self:get("date") - max_age)
    local t = diff_date(server_last_modify,client_last_modify)
    local server_etag = self:get("etag")
    if t >= 0 or client_etag ~= server_etag then
        return false
    else
        return true
    end
end

function cache_control:to_string()
    return string.format("max-age = %s, %s",self.max_age, self.attrib)
end

--function cache_control:revalidations(request,response)
--    if self:exist_cache(request) then
--        --Delete
--        --[==[
--        if not self.content then
--            response.statusCode = "404"
--            response.status = "Not Found"
--            response.version = "HTTP/1.1"
--        end
--        ]==]
--        --Not Modify
--        if request.If-Modified-Since and request.If-Modified-Since == response.Last-Modified then
--            if request.If-None-Match and request.If-None-Match == response.Etag then
--                response.statusCode = "304"
--                response.status = "Not Modified"
--                response.version = "HTTP/1.1"
--            end
--        --Changed
--        else
--            --TODO
--            local cache = self.get_cache(request.uri)
--            response.set_content(cache)
--            response.statusCode = "200"
--            response.status = "OK"
--            response.version = "HTTP/1.1"
--        end
--    end
--end


--TODO
--reponse.ETag
--reponse.Expire
--reponse.Last-Modified
--function response:setCache(request)
--    request.cache_control = cache_control.get_pairs(request.Cache-Control)
--    if request.cache_control["no-cache"] then
--        --TODO must validation on origin server
--        self.cache_control = "no-cache"
--    elseif  request.cache_control["no-store"] then
--        --TODO
--        --check cache whether the cache is exist,if exist return and delete,
--        --else return and no cache
--        self.cache_control = "no-store"
--    elseif request.cache_control["max-age"] then
--        --if response.Cache-Control.Age > request.Cache-Control.max-age + os.time() then
--            --client will not accept reponse 
--        --response.Cache-Control = 600
--    else
--        local secs = 60
--        local b = os.time()
--        local e = os.time() + secs
--        response.Cache-Control  = "max-age" .. secs
--        response.Last-Modified = get_date(os.date("*t", b))
--        response.Expire =  get_date(os.date("*t"),e)
--    end
--end
