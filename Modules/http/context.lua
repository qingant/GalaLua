--[[
--Author liuwenxue
--Date 2014-07-17

--Call by application
    --if you want session , call context:session()
    --if you want cache-control, call context:cache()
]]

module(...,package.seeall)

local session = require("http.session").session
local cache = require("http.cache")
local pprint = require("pprint")
local mdb = require("mdb").mdb

function sessionSave(key, value)
    local db = mdb:new():init(mdb.create_env("/tmp/lmdb"))
    local root = "Session"
    function save(db)
        local e = db:get_root(root)
        local id = e:add_node(key)
        id:add_value(value)
        db:commit()
    end
    db:with(save)
end

function sessionQuery(key)
    local db = mdb:new():init(mdb.create_env("/tmp/lmdb"))
    local root = "Session"
    function query(db)
        local e = db:get_root(root)
        return e:_xpath(key)
    end
    return db:with(query)
end

--ses : a session object
function sessionGen(ses)
    return string.format("SessionID=%d, Step=%s, Expire=%s, Domain=%s, Path=%s",ses.name,ses.value,get_date(ses.expires), ses.domain,ses.path)
end

function sessionParse(cookie)
    local ses = split(cookie,",")
    local session = {}
    for _,k in pairs(ses) do
        local tmp = split(k,"=")
        session[strip(tmp[1])] = strip(tmp[2])
    end
    return {name = session["SessionID"],  value = session["Step"], expires = session["Expire"], path = session["Path"], domain = session["Domain"]}
end

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
    return self
end

--default
function context:setSession(value)
    local ses = session:new():init()
    ses:setDomain(self.request["Host"])
    ses:setValue(value)
    local session = sessionGen(ses)
    pprint.pprint(session,"sessionGen")
    sessionSave(ses:getName(), session)
    self.response["Set-Cookie"] = session
    --pprint.pprint(self.response,"context.response")
end

-- default support transaction, the value as step
function context:getSession()
    local cookie = self.request["Cookie"]
    return cookie
end

function context:session()
    local cookie = self:getSession()
    print(cookie,"--cookie--")
    if cookie then
        local sess = sessionParse(cookie)
        --pprint.pprint(sess,"sess")
        local s = sessionQuery(sess.name)
        pprint.pprint(s,"sessionQuery")
        if s then
            local tmp = sess.value
            self:setSession(tmp + 1)
        else
            self:setSession(1)
        end
    else
        self:setSession(1)
    end
end

function context:cacheControl()
end

function context:getRequest()
    return self.request
end

function context:sendResponse()
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

--strip space in two side of string
function strip(pString)
    local s1,e1 = string.find(tostring(pString), "%s+", 1)
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
--gen Date and If-Modified-Since for request
function get_date(date) 
    date = os.date("!*t",date)
    local WEEK = {"Sun.","Mon.", "Tues.", "Wed.", "Thur.","Fri.", "Sta."}
    local MONTH = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"}
    local week = WEEK[date["wday"]]
    local day = date["day"]
    local month = MONTH[date["month"]]
    local year = date["year"]
    local time = date["hour"]..":"..date["min"]..":"..date["sec"].." ".."GMT"
    date = week.." "..day.." "..month.." "..year.." "..time
    return date
end
