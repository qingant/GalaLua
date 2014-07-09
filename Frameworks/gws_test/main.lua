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

function index:get(...)
    if ... then
        local str = ""
        for i,k in pairs(...) do
            str = str.."key ="..i .. "value= " .. k
        end
        return str
    else
        return "Hello world"
    end
end

function index:post(...)
    pprint.pprint(...)
end

