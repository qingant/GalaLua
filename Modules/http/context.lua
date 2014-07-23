--[[
--Author liuwenxue
--Date 2014-07-17

session--Call by application
    --if you want session , call context:session()
    --if you want cache-control, call context:cache()
]]

module(...,package.seeall)

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
    self.session = session:new():init()
    return self
end

function context:get_session_id()
    local request = get_request()
    local ses = request["Cookie"]
    pprint.pprint(ses,"Cookie")
    if ses and self.session:exist(ses.id) and
       not self.session:is_expired(ses.expires) then
       return ses.id
    else
       return nil
    end
end
function context:set_session(key,value)
    return self.session:set(key,value)
end

function context:create_session(id)
    if id then 
       self.session:set("id",id)
    end
    key = self.session:get("id")
    value = self.session:to_string()
    print("value",value)
    self.session:save(key,value)
    self.response["Set-Cookie"] = value
end

function context:remove_session(key)
    self.session:remove(key)
end
--function context:destory_session()
--    self.session:remove()
--end

--default
--function context:setSession(value)
--    local ses = session:new():init()
--    ses:set("domain",self.request["Host"])
--    ses:set("value",value)
--    ses:set("expires",10)
--    --pprint.pprint(get_date(ses:getExpire()),"Expire ")
--    local session = sessionGen(ses)
--    pprint.pprint(session,"sessionGen")
--    sessionSave(ses:get("name"), session)
--    self.response["Set-Cookie"] = session
--    --pprint.pprint(self.response,"context.response")
--end
--
---- default support transaction, the value as step
--function context:getSession()
--    local cookie = self.request["Cookie"]
--    return cookie
--end
--
--function context:session()
--    local cookie = self:getSession()
--    print("cookie begin : ",cookie, "\n")
--    if cookie then
--        print(cookie,"--cookie--")
--        local sess = sessionParse(cookie)
--        pprint.pprint(sess,"sess")
--        local d = sessionCheck(sess.expires)
--        if d >= 0 then
--            self:setSession(1)
--        else
--            local s = sessionQuery(sess.name)
--            pprint.pprint(s,"sessionQuery")
--            if s then
--                local tmp = sess.value
--                pprint.pprint(tmp,"tmp")
--                self:setSession(tmp + 1)
--            end
--        end
--    else
--        self:setSession(1)
--    end
--end

function context:get_cache_control(key)
    --if self:revaild_cache_control() then
    --    return self.cache_control:get(key)
    --end
    --return nil

    return self.cache_control:get(key)
end

function context:is_cache_control()
    local request = get_request()
    local cache = request["Cache-Control"]
    if cache.attrib == "no-store" or 
       request["Pargma"] == "no-cache" then
       return false
    else 
       return true
    end

end

function context:vaild_cache_control()
    local request = get_request()
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

--TODO parse pPattern replace %. as %s\ 
--@param : pString : str what to split
--@param : pPattern : character is splited by
--@return : table  like python split
--function split(pString, pPattern)
--    local Table = {}  -- NOTE: use {n = 0} in Lua-5.0
--    local fpat = "(.-)" .. pPattern
--    local last_end = 1
--    --local s, e, cap = pString:find(fpat, 1)
--    print("pString",pString)
--    local s, e, cap = string.find(pString, fpat, 1)
--
--    while s do
--        if s ~= 1 or cap ~= "" then
--            table.insert(Table,cap)
--        end
--        last_end = e+1
--        s, e, cap = string.find(pString, fpat, last_end)
--    end
--    if last_end <= #pString then
--        cap = string.sub(pString, last_end)
--        table.insert(Table, cap)
--    end
--    return Table
--end

--strip space in two side of string
--function strip(pString)
--    local s1,e1 = string.find(tostring(pString), "%s+", 1)
--    print(s1,e1)
--    if not e1 then
--        e1 = 0
--        s2,e2 = string.find(pString,"%s+", e1+1)
--    end
--    print(s2,e2)
--    if s1 ~= 1 and s1 then
--        return string.sub(pString,1,s1-1)
--    end
--    if not s2 then
--        return string.sub(pString,e1+1)
--    end
--    return string.sub(pString,e1+1,s2-1)
--end
--gen Date and If-Modified-Since for request
--there is eight hours left
--function get_date(date)
--    date = os.date("*t",date)
--    local WEEK = {"Sun.","Mon.", "Tues.", "Wed.", "Thur.","Fri.", "Sta."}
--    local MONTH = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"}
--    local week = WEEK[date["wday"]]
--    local day = date["day"]
--    local month = MONTH[date["month"]]
--    local year = date["year"]
--    local time = date["hour"]..":"..date["min"]..":"..date["sec"]
--    date = week.." "..day.." "..month.." "..time.." "..year.." ".."GMT"
--    return date
--end

--------------------------------------
---seperate date to items
--for example, 
--
--Mon. Jul 21 09:59:44 2014 
--
--date_item["wday"] = 1
--date_item["month"] = 7
--date_item["day"] = 21
--date_item["hour"] = 09
--date_item["min"] = 59
--date_item["sec"] = 44
--date_item["year"] 2014
--------------------------------------
--function parse_date(date)
--    local WEEK = {["Sun,"] = 1,["Mon,"] = 2, ["Tues,"] = 3, ["Wed,"] = 4,["Thur,"] = 5,["Fri,"] = 6, ["Sta,"] = 7,
--                 ["Sun"] = 1,["Mon"] = 2, ["Tues"] = 3, ["Wed"] = 4, ["Thur"] = 5,["Fri"] = 6, ["Sta"] = 7}
--    local MONTH = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"}
--    local date_item = {}
--    date = tostring(date)
--    date_table = split(date,"%s+")
--    date_item["wday"] = WEEK[date_table[1]]
--    date_item["month"] = MONTH[date_table[2]]
--    date_item["day"] = date_table[3]
--    time = split(date_table[4],":")
--    date_item["hour"] = time[1]
--    date_item["min"] = time[2]
--    date_item["sec"] = time[3]
--    date_item["year"] = date_table[5]
--    return date_item
--end

-------------------------------
-- date1 and date2 are same as os.data()'s return
-- if date1 old date2 return -1
-- if date1 equal date2 return 0
-- if date1 new date2 return 1
-------------------------------

--function diff_date(date1, date2)
--    if date1 == date2 then
--        return 0
--    end
--    date1 = parse_date(date1)
--    date2 = parse_date(date2)
--    if date1["year"] < date2["year"] then
--        return -1
--    elseif date1["year"] > date2["year"] then
--        return 1
--    else
--        if date1["month"] < date2["month"] then
--            return -1
--        elseif date1["month"] > date2["month"] then
--            return 1
--        else
--            if date1["day"] < date2["day"] then
--                return -1
--            elseif date1["day"] > date2["day"] then
--                return 1
--            else
--                if date1["hour"] < date2["hour"] then
--                    return -1
--                elseif date1["hour"] > date2["hour"] then
--                    return 1
--                else
--                    if date1["min"] < date2["min"] then
--                        return -1
--                    elseif date1["min"] > date2["min"] then
--                        return 1
--                    else
--                        if date1["sec"] < date2["sec"] then
--                            return -1
--                        elseif date1["sec"] > date2["sec"] then
--                            return 1
--                        else
--                            return 0
--                        end
--                    end
--                end
--            end
--        end
--    end
--end
