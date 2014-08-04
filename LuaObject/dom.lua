--[[
    Author : Ma Tao
	Date   : 2014/07/17

	== Introduction ==
]]

module(..., package.seeall)

local xml = require "xml"
local mdb = require("mdb").mdb
local xStr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root></root>"
local reader = xml.cxml_reader(xStr,#xStr)
local root = reader:document()
local _XML = getmetatable(root)
form_text = xml.cxml_reader
function _XML:get_element_by_tag( tag )
    local result = {}
    for i,v in ipairs(self:sub_elements()) do
        if v:key() == tag then
            result[#result+1] = v
        end
    end
    return result
end
function _XML:is_leaf( ... )
    return (#self:sub_elements() == 0)
end
function _XML:is_vector( ... )
    local subs = self:sub_elements()
    if #subs > 1 then
        local key = subs[1]:key()
        for i,v in ipairs(subs) do
            if key ~= v:key() then
                return false
            end
        end
        return true
    end
    return false
end
