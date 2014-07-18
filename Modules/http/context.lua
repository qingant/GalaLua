
module(...,package.seeall)
local session = request("http.session")
local cache = request("http.cache")


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
function context:Setsession()
    local ses = session:new():init()
    ses:setDomain(self.request.host)
    self.response["Set-Cookie"] = ses:toString()
end

function context:getCookie()
    local cookie = self.request["Cookie"]
end

function context:cacheControl()
end

function context:getRequest()
    return self.request
end

function context:sendResponse()
end
