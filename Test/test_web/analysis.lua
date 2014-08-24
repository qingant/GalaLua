module(..., package.seeall)
local json = require "cjson"
local pprint = require "pprint"

data = {}
list = {}
load = {}
save = {}

local datas = {}
datas = {
["meter"] = {
                { key = "12:01:52",value = 22 },
                { key = "12:01:53",value = 60 }    
            },
["bar"] = {
                { key = "06:30:11",value = 22 },
                { key = "07:01:53",value = 60 }    
            },
["line"] = {
                { key = "09:45:10",value = 22 },
                { key = "10:01:53",value = 60 }    
            }          
}

function data:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function data:get(context,query,...)
    pprint.pprint(query,"data input")
    local node = {}
    
    if nil ~= query.id then
        node.err_code = 0
        node.datas = datas[query.id]
    else
        node.err_code = 0x11
    end
    local res=json.encode(node)
    return res
end

function data:post(context,query,...)
    pprint.pprint(query,"data input")
end

function list:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function list:get(context,query,...)
    local node = {}
    node.err_code = 0
    node.layouts = {
                      { id = 1,name = "集群A数据分析" },
                      { id = 2,name = "集群B数据分析" }    
                   }
    local res=json.encode(node)
    return res
end

function list:post(context,query,...)
end

function load:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function load:get(context,query,...)
    pprint.pprint(query,"load input")
    local node = {}
    node.err_code = 0
    node.Layout = {
                     {
                        { type = "amMeter" , bind = "http://server/analysis/data?id=meter,size:[400,300]"},
                        { type = "amLine" , bind = "http://server/analysis/data?id=line"}
                     },
                     {
                        { type = "amBar’" , bind = "http://server/analysis/data?id=bar"},
                        { type = "amIndicator’" , bind = "http://server/analysis/data?id=indicator"}
                     }
                  }
    local res=json.encode(node)
    return res
end

function load:post(context,query,...)
    pprint.pprint(query,"load input")
end

function load:save()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function load:save(context,query,...)
    pprint.pprint(query,"save input")
    local node = {}
    node.err_code = 0
    local res=json.encode(node)
    return res
end

function load:save(context,query,...)
    pprint.pprint(query,"save input")
end