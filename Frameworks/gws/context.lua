--[[
--Author liuwenxue
--Date 2014-07-17

session--Call by application
    --if you want session , call context:session()
    --if you want cache-control, call context:cache()
]]

module(...,package.seeall)

local session_manager = require(_PACKAGE .. "session").session_manager
local session = require(_PACKAGE .. "session").session
local pprint = require("pprint")
local response = require(_PACKAGE .. "response").response


context = {}

function context:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function context:init(request, mgr)
    self._request = request
    self._response  = response:new():init()
    self._session_mgr = mgr
    local cookie = request["Cookie"]
    if cookie["sessionId"] and self._session_mgr:exist(cookie["sessionId"]) then
        self._session = self._session_mgr:get_session_ref(id)
    end
    return self
end

function context:get_session()
    return self._session
end

function context:create_session()
    self._session = self._session_mgr:create_session()
    self.response["Set-Cookie"] = string.format("sessionId=%s", self._session:get_id())
    return self.session
end

function context:get_request()
    return self._request
end

