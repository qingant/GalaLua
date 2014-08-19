--[[
    Author : Ma Tao
	Date   : 2014/07/07 
	== Introduction ==
    gws test case

]]

module(..., package.seeall)
local pprint = require("pprint")
index = {}

function index:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function index:get(context,...)
    local sess =  context:get_session() 
    --pprint.pprint(sess,"get_session")
    if not sess then
        sess = context:create_session()
    end
    local expires = sess:get("Expires")
    local path = sess:get("Path")
    sess:set("Domain","2222")
    local domain = sess:get("Domain")
    context:get_response():set_content_type("text/plain")
    return expires .. path .. domain .. "\nHello WORLD\n"
end

function index:post(context,...)
    local body = context:get_request()
    ---pprint.pprint(body["body"])
    local str = ""
    for i,k in pairs(body["body"]) do
        str = string.format("%s%s=%s;",str,i,k)
    end
    return str.."\n"
end

