module(..., package.seeall)

local xml = require "xml"
local mdb = require "mdb"
local xStr = "<root></root>"
local reader = xml.cxml_reader(xStr,#xStr)
local root = reader:document()
local _XML = getmetatable(root)
local schemaTag = "Schema"
function _XML:getElementByTag( tag )
    local result = {}
    for i,v in ipairs(self:sub_elements()) do
        if v:key() == tag then
            result[#result+1] = v
        end
    end
    return result
end

local _Config = {}


function _Config:new( o )
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function _Config:init( path )
    local db = mdb:new():init(mdb.create_env(path))
    self._db = db
    return self
end

function _Config:get( xpath )
    return self._db:withReadOnly(function (db)
                                 local vs = db:xpath(xpath).get_value()
                                 return vs
                                 end)
end
function _Config:put( xpath, value )
    self._db:with(function ( db )
        local num = db:xpath_set(xpath, value)
        if num == 0 then
            error("No record altered")
        end
        return num
    end)
end

function _Config:schemaGen( ... )

end

local _Importer = {

}
function _Importer:new( o )
    local o = o or {}
    setmetatable(o, self)    
    self.__index = self
    return o
end
function _Importer:importFromXML( path , root)
    local f=io.open(path,"r")
    local reader = xml.cxml_reader(t,#t)
    local xRoot = reader:document()
    local dRoot = self._db:get_root(root)


end

function _Importer:_import( xElement, dElement )
    for k,v in pairs(xElement:properties()) do
        dElement:add_attrib(k, v)
    end

    for i,v in ipairs(xElement:sub_elements()) do
        local e = dElement:add_node(v:key())
        self:_import(v, e)
    end

    if xElement:value() ~= "" then
        dElement:add_value(xElement:value())
    end
end


if ... == "__main__" then
    local db = mdb:new():init(mdb.create_env(path))
    local imp = _Importer:new{_db = db}
    local config_path = "/vagrant/code/GalaLua/"

end
