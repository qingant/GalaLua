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
    pprint.pprint(context,"--context--")
    local ses = context:get_session()
    pprint.pprint(sess, "sess object")
    if not ses then
        local ses = context:create_session()
    else
        --ses:destory()
        ses:set("a","b")
        ses:set("b","c")
    end

    --function index:get(...)
    if ... then
        local str = ""
        for i,k in pairs(...) do
            str = str.."key ="..i .. "\nvalue= " .. k .. "\n" 
        end
        --pprint.pprint(context.response,"context.response")
        return {context.response,str}
    else
        return "Hello world"
    end
end

function index:post(context,...)
    pprint.pprint(context,"context")
--function index:post(...)
    pprint.pprint(...)
    local str = ""
    return {context.response, str}
end

