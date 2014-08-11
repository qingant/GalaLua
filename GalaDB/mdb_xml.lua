--[[
    Author : Ma Tao
	Date   : 2014/08/11

	== Introduction ==
    mdb-xml utils
]]

module(..., package.seeall)

local dom = require("dom")
local io = require("io")
local pprint = require("pprint")
function xml_to_mdb(root, path)
    local f = io.open(path, "r")
    local text = f:read("*a")
    print(text)
    local doc = dom.from_text(text):document()
    pprint.pprint(doc:properties(), "properties")
    function _import(db_element, xml_element)
        assert(db_element, "db_element cannot be nil")
        assert(xml_element, "xml_element cannot be nil")
        for k,v in pairs(xml_element:properties()) do
            db_element:add_attrib(k,v)
        end
        if xml_element:is_vector() then
            for i,v in ipairs(xml_element:sub_elements()) do
                local e = db_element:add_vector_item()
                _import(e, v)
            end
        else
            for i,v in ipairs(xml_element:sub_elements()) do
                local e
                if v:is_vector() then
                    e = db_element:add_vector_node(v:key())
                else
                    e = db_element:add_node(v:key())
                end
                _import(e, v)
            end
        end
        if xml_element:is_leaf() then
            db_element:add_value(xml_element:value())
        end
    end
    local e = root:add_node(doc:key())
    _import(e, doc)
end

if ... == "__main__" then
    local mdb = require("mdb").mdb
    local path = "/tmp/temp/bar"
    os.execute(string.format("rm -rf %s && mkdir -p %s", path, path))
    local db = mdb:new():init(mdb.create_env(path))
    local root = db:get_root("Test")
    db:with(function (db)
            print("import")
            xml_to_mdb(root, "./cluster.xml")
            print(root:to_xml())
            db:commit()
    end)
end