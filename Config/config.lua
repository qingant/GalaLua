module(..., package.seeall)

local xml = require "xml"
local mdb = require("mdb").mdb
local xStr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root></root>"
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
function _XML:isLeaf( ... )
    return (#self:sub_elements() == 0)
end

local _Config = {}
Config = _Config

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
function _Config:prepairPath( xpath )
    local path = string.gsub(xpath, "/[^/]*%[0*([0-9])%]/", "/%1/")
    return path
end
function _Config:get( xpath )
    local path = self:prepairPath(xpath)
    print(path)
    return self._db:withReadOnly(function (db)
                                 local root = db:get_root("Configure"):get_child("Contents")

                                 local vs = root:xpath(path)[1]:get_value()
                                 return vs[1]
                                 end)
end
function _Config:put( xpath, value )
    local path = self:prepairPath(xpath)

    self._db:with(function ( db )
                  local root = db:get_root("Configure"):get_child("Contents")
                  local node = root:xpath(path)[1]

                  for k,v in pairs(node:get_value()) do
                    node:remove_value(v)
                  end
                  node:add_value(value)    
                  db:commit()              
    end)
end
function _Config:import( path )

end
function _Config:export( path )
    self._db:withReadOnly(function ( db )
        local root = db:get_root("Configure")
        local contents = root:get_child("Contents")
        contents:show()
        local f = io.open(path, "w")
        f:write(contents:to_xml())
        f:flush()
    end)
end
function _Config:schemaGen( ... )
    self._db:with(function ( db )
        local root = db:get_root("Configure")
        local contents = root:add_node("Contents")
        local schema = db:get_root("Schema")
        local sStructure = schema:get_child("_STRUCTURE"):get_child()
        for k,v in pairs(sStructure) do
            sStructure = v
        end

        function doSchema( sNode, cNode )

            local meta = sNode:get_attrib()["meta"]
            print("DoSchema", sNode:tag(), cNode:tag(), meta)
            if meta == "entry" then
                local eType = sNode:get_child("type"):get_value()[1]
                cNode:add_attrib("type", eType)
                if eType == "path" or eType == "ip" or eType == "string" then
                    local maxLen = sNode:get_child("size"):get_value()[1]
                    --maxLen = tostring(maxLen)
                    cNode:add_attrib("maxLen", maxLen)
                elseif eType == "int" then
                    local limit = sNode:get_child("limit") 
                    if limit then
                        local max = limit:get_child("max") and limit:get_child("max"):get_value()[1] or "0"
                        local min = limit:get_child("min") and limit:get_child("min"):get_value()[1] or "0"
                        cNode:add_attrib("max", max)
                        cNode:add_attrib("min",min)
                    end
                elseif eType == "enum" then
                    local sRoot = sNode:get_root()

                end




            elseif meta == "struct" then
                for k, v in pairs(sNode:get_child()) do
                    local c
                    if v:get_attrib()["meta"] == "array" then
                        c = cNode:add_vector_node(v:tag(), "Item")
                    else
                       c = cNode:add_node(v:tag())
                   end
                    doSchema(v, c)
                end

            elseif meta == "array" then
                local aStruct
                for k,v in pairs(sNode:get_child()) do   -- get only one child node
                    aStruct = v
                end
                -- assert(aStruct:tag()~="LSR", sNode:to_xml())
                -- local vNode = cNode:add_vector_node(sNode:tag(), "Item")
                                                    --, aStruct:tag())
                local limit = tostring(sNode:get_attrib()["count"])
                for i=0,limit-1 do
                    local item = cNode:add_vector_item()
                    doSchema(aStruct, item)
                end
            elseif meta == "union" then
                local uSelector = sNode:get_child("_SCROLLTYPE") 
                doSchema(uSelector, cNode:add_node("_SCROLLTYPE"))
                for k,v in pairs(sNode:get_child("_UNIONSELECTOR"):get_child()) do
                    doSchema(v, cNode:add_node(v:tag()))
                end
            end

        end

        doSchema(sStructure, contents)
        db:commit()
    end)
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
    local t=io.open(path,"r"):read("*a")
    local reader = xml.cxml_reader(t,#t)
    local xRoot = reader:document()
    local dRoot = self._db:get_root(root)
    self._xRoot = xRoot
    self._dRoot = dRoot
    self._db:with(
                  function (db)
                      self:_import(xRoot, dRoot)
                      db:commit()
                  end
                  )

end

function _Importer:showResult(  )
    self._db:withReadOnly(function ( ... )
                          local s = io.open("out.xml","w")
                          s:write(self._dRoot:to_xml())
                          s:flush()
                          self._dRoot:show()
    end
    )
end
function _Importer:_import( xElement, dElement )
    for k,v in pairs(xElement:properties()) do
        dElement:add_attrib(k, v)
    end

    for i,v in ipairs(xElement:sub_elements()) do
        print("VK",v:key())
        local e = dElement:add_node(v:key())
        self:_import(v, e)
    end
    if #xElement:sub_elements() > 0 then
        return
    end
    if xElement:value() ~= "" then
        local v = xElement:value()
        assert(string.sub(v, 1, 1) ~= "<", v)
        dElement:add_value(xElement:value())
    end
    return dElement
end


if ... == "__main__" then
    local path = "/tmp/test_config"
    os.execute(string.format("rm -rf %s && mkdir -p %s", path, path))
    local db = mdb:new():init(mdb.create_env(path))
    local imp = _Importer:new{_db = db}
    local config_path = "./sample.xml"
    imp:importFromXML(config_path, "Schema")
    imp:showResult()
    local config = _Config:new():init(path)
    config:schemaGen()
    -- config:put("Monitor/Base/AMQToken", "test")
    -- local tk = config:get("Monitor/Base/AMQToken")
    -- print(tk)
    -- config:put("Monitor/Base/AMQToken", "testABC")
    -- print(config:get("Monitor/Base/AMQToken"))
    config:export("exp.xml")
    --config:put("MonitorList/4/SVC/")
end
