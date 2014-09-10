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
    return "Hello WORLD\n"
    
end


function index:post(context,...)
end

summary = {}
function summary:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function summary:get(context,...)
    local request = context:get_request()
    local params = request["params"]
    local path = string.format("/statistics_200/error_statistics_%d_%d_%d_%d.csv",
            params['c'],
            params['n']/params['c'],
            params['i'],
            params['j'])
    --print("path",path)
    local pwd = os.getenv("PWD")
    local fd = assert(io.open(pwd .. path))
    local content = fd:read("*a")
    context:get_response():set_content_type("text/plain")
    return content
end

processing = {}
function processing:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function processing:get(context,...)
    local request = context:get_request()
    local params = request["params"]
    local path = string.format("/statistics_200/complete_statistics_%d_%d_%d_%d.csv",
            params['c'],
            params['n']/params['c'],
            params['i'],
            params['j'])
    local pwd = os.getenv("PWD")
    --print("path",path)
    local fd = assert(io.open(pwd .. path))
    local content = fd:read("*a")
    context:get_response():set_content_type("text/plain")
    return content
end

tps = {}
function tps:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end
function tps:get(context,...)
    local request = context:get_request()
    local params = request["params"]
    local path = string.format("/statistics_200/tps_statistics_%d_%d_%d_%d.csv",
            params['c'],
            params['n']/params['c'],
            params['i'],
            params['j'])

    --print("path",path)
    local pwd = os.getenv("PWD")
    local fd = assert(io.open(pwd .. path))
    local content = fd:read("*a")
    context:get_response():set_content_type("text/plain")
    return content
end
