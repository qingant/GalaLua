--luajit test_xml.lua
local xml=require "xml"

function pprint(x)
    if not x then
        print("get nil")
        return 
    end
    for i,v in pairs(x) do
        print(i,v)
    end
end

--local f=io.open("MonitorDef.xml","r")
--t=f:read("*all")

t2="<?xml version='1.0' encoding='UTF-8' ?><abc attr='iamroot'><subabc>aaaaa</subabc></abc>"
function read(t)
    local r=xml.cxml_reader(t,#t)
    --pprint(getmetatable(r))
    local root=r:document()
    pprint(root:properties())
    pprint(getmetatable(root))
    print(root:key(),root:value())
  
    local e=root:sub_elements() 
    pprint(e)
    --
    print("properties")
    for i,v in pairs(e) do
        a=(v:sub_elements()[1])
        if a then
            pprint(a:properties())
        end
        pprint(v:properties())
    end
end


--read(t2)
--[[
for i=1,20000 do
    read(t)
end

print("over.....")
while true do
end
   
--]]
--
--print("writer>>>>>>")
--local root=xml.cxml_element("ABC")
--root:add_property("AAAA","asdasd")
--b=xml.cxml_element("C","3")
--root:add_sub_element(b)
--
--print(xml.cxml_writer(root))
--
--]]
--

local cxml={}
function cxml.encode(t)
    function _encode(t,root)
        for k,v in pairs(t) do
            local _type=type(v)
            if _type=="table" then
                local c=xml.cxml_element(k)
                if not root then
                    root=c
                else
                    root=root:add_sub_element(root)
                end
                _encode(v,root)
            elseif _type=="string" or _type=="number" then
                local c=xml.cxml_element(k,v)
                if not root then
                    root=c
                else
                    root:add_sub_element(c) 
                end
            else
                error("not support type ".._type)
            end

        end
        return xml.cxml_writer(root)
    end
    return _encode(t)
end

local t={aaa={22,33}}
print(cxml.encode(t))
