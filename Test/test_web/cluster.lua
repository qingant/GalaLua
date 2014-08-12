module(..., package.seeall)
local json = require "cjson"

list = {}
topo = {}
save = {}
status = {}
start = {}
restart = {}

function list:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function list:get(context,query,...)
    local node = {}
    node.err_code = 0
    node.clusters={
                     {id="cluster1",name="核心集群A"},
                     {id="cluster2",name="核心集群B"}
                  }
    local res=json.encode(node)
    return res
end

function list:post(context,query,...)
end

function topo:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function topo:get(context,query,...)
    --context:get_response():redirect("/html/index.html")
    pprint.pprint(query,"topo input")
    local node={}
    node={
            err_code = 0,
            topo = {
                      id = 0,
                      name = "Root",
                      status = "normal",
                      description = "This is the root N",
                      children = {
                                    {
                                      id = 1,
                                      name = "N1",
                                      status = "normal",
                                      description = "This is the N 1",
                                      children = {}
                                    },
                                    {
                                      id = 2,
                                      name = "N2",
                                      status = "normal",
                                      description = "This is the N 2",
                                      children = {}
                                    }
                                 }
                    }  
           }

    local res=json.encode(node)
    return res
end

function topo:post(context,query,...)
    pprint.pprint(query,"topo input")
end

function save:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function save:get(context,query,...)
    --context:get_response():redirect("/html/index.html")
    pprint.pprint(query,"save input")
    local node = {}
    node.err_code = 0
    local res=json.encode(node)
    return res
end

function save:post(context,query,...)
    pprint.pprint(query,"save input")
end

function status:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function status:get(context,...)
    --context:get_response():redirect("/html/index.html")
    local node = {}
    node.err_code = 0
    node.status={
                    {id=0,status="Normal"},
                    {id=1,status="Warning"},
                    {id=2,status="Error"}
                }
    local res=json.encode(node)
    return res
end

function status:post(context,...)
end

function start:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function start:get(context,query,...)
    --context:get_response():redirect("/html/index.html")
    pprint.pprint(query,"start input")
    local node = {}
    node.err_code = 0
    local res=json.encode(node)
    return res
end

function start:post(context,query,...)
    pprint.pprint(query,"start input")
end

function restart:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function restart:get(context,query,...)
    --context:get_response():redirect("/html/index.html")
    pprint.pprint(query,"restart input")
    local node = {}
    node.err_code = 0
    local res=json.encode(node)
    return res
end

function restart:post(context,query,...)
    pprint.pprint(query,"restart input")
end

function stop:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function stop:get(context,query,...)
    pprint.pprint(query,"stop input")
    local node = {}
    node.err_code = 0
    local res=json.encode(node)
    return res
end

function stop:post(context,query,...)
    pprint.pprint(query,"stop input")
end