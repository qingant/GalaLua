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
local response = require("http.response").response
local get_date = require(_PACKAGE .. "utils").get_date


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
    local cookie = request["Cookie"] or ""
    if cookie and cookie["sessionId"] and self._session_mgr:exist(cookie["sessionId"]) then
        self._session = self._session_mgr:get_session_ref(cookie["sessionId"])
        if not self._session:is_expired(cookie["Expires"]) then
           self._session:set("Expires",cookie["Expires"])
           self._response:set_cookie(string.format("sessionId=%s, Expires=%s,Path=%s, Domain=%s",
                                                    self._session:get_id(),
                                                    self._session:get("Expires"),
                                                    self._session:get("Path"),
                                                    self._session:get("Domain")))
        else
            self._session = nil
        end
    end
    return self
end

function context:get_session()
    return self._session
end

function context:create_session()
    self._session = self._session_mgr:create_session()
    self._response["Set-Cookie"] = string.format("sessionId=%s, Expires=%s,Path=%s, Domain=%s", 
            self._session:get_id(),
            self._session:get("Expires"),
            self._session:get("Path"),
            self._session:get("Domain"))
    return self._session
end

function context:get_request()
    return self._request
end

function context:get_response()
    return self._response
end

