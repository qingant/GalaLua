
--TODO logging
cache_control = {}

function cache_control:new(o)
    o = o or {}
    setmetatable(o,self)
    self.__index = self
    return o
end

function cache_control:init()
    self.cache = {}
    self.cache_control = {}
end

function cache_control:get_pairs(cache)
    objs = string.split(cache,",")
    for i,k in pairs(objs) do
        obj = split(k,"=")
        self.cache_control[strip(obj[1])] = strip(obj[2])
    end
end

function cache_control:set_cache(request)
    self.cache[request.uri] = request.content
end

function cache_control:del_cache(request)
end

function cache_control:valid(request, reponse)
end
function cache_control:exist_cache(request)
    for key, value in pairs(self.cache) do
        if request.uri == key then
            return false
        end
    end
    return true
end

function cache_control:revalidations(request,response)
    if self:exist_cache(request) then
        --Delete
        --[==[
        if not self.content then
            response.statusCode = "404"
            response.status = "Not Found"
            response.version = "HTTP/1.1"
        end
        ]==]
        --Not Modify
        if request.If-Modified-Since == self.Last-Modified then
            if request.If-None-Match and request.If-None-Match == self.Etag then
                response.statusCode = "304"
                response.status = "Not Modified"
                response.version = "HTTP/1.1"
            end
        --Changed
        else
            --TODO
            local cache = self.get_cache(request.uri)
            response.setContent(cache)
            response.statusCode = "200"
            response.status = "OK"
            response.version = "HTTP/1.1"
        end
    end
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
    local s1,e1 = string.find(pString, "%s+", 1)
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

--TODO
function response:setCache(request)
    request.cache_control = cache_control.get_pairs(request.Cache-Control)
    if request.cache_control["no-cache"] then
        --TODO must validation on origin server
        self.cache_control = "no-cache"
    elseif  request.cache_control["no-store"] then
        --TODO
        --check cache whether the cache is exist,if exist return and delete,
        --else return and no cache
        self.cache_control = "no-store"
    elseif request.cache_control["max-age"] then
        --if response.Cache-Control.Age > request.Cache-Control.max-age + os.time() then
            --client will not accept reponse 
        --response.Cache-Control = 600
    else
        local secs = 60
        local b = os.time()
        local e = os.time() + secs
        response.Cache-Control  = "max-age" .. secs
        response.Last-Modified = get_date(os.date("*t", b))
        response.Expire =  get_date(os.date("*t"),e)
    end
end
