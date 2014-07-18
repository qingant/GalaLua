--[[
--Author liuwenxue
--Date 2014-07-17
--
--WorkFlow :
    --simple:
        session:new():init()
        response["Set-Cookie"] = session:tostring()
    --more
        you can set, get, and del session with specification
]]

session = {}

function session:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    self.__newindex = function(t, k, v)
        error("attempt to add session key is forbid!")
    end
    return o
end

function session:init()
    --in case of c10k duplicate
    self.name = math.random(os.time()) + math.ramdom(1000000)
    --step if you want to support transaction
    self.value = 1
    --60*60*24*30  one month
    self.expires = os.time() + 2592000 
    self.path = "/"
    self.domain = ""
    return self
end

function session:getName()
    return self.name
end

function session:getValue()
    return self.value
end

function session:getExpire()
    return self.expires
end

function session:getPath()
    return self.path
end

function session:getDomain()
    return self.domain
end

function session:get()
    return {self.name, self.value, self.expires, self.path, self.domain}
end

--set value as step if you want to support transaction
function session:setValue(step)
    self.value = step
end

function session:setExpire(delay)
    self.expires = os.time() + delay
end

function session:setDomain(host)
    self.domain = host
end

function session:setPath(path)
    self.path = path
end


function session:set(ses)
    self.name = ses.name
    self.value = ses.value
    self.expires = ses.expires
    self.path = ses.path
    self.domain = ses.domain
end

function session:del()
    self.name = nil
    self.value = nil
    self.expires = nil
    self.path = nil
    self.domain = nil
end

--gen Date and If-Modified-Since for request
function get_date(date) 
    --date = os.date("!*t")
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

function session:toString()
    return string.format("SessionID=%d, Step=%s, Expire=%s, Domain=%s, Path=%s",self.name,self.value,get_date(self.expires), self.domain,self.path)
end
