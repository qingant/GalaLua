module(..., package.seeall)
local json = require "cjson"

index = {}
sysinfo = {}

function index:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function index:get(context,query,...)
    context:get_response():redirect("/html/index.html")
    return "wait while redirect..."
end

function index:post(context,query,...)
end

function sysinfo:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function sysinfo:get(context,query,...)
    --context:get_response():redirect("/html/index.html")
    local node={}
    node.err_code=0
    node.sysinfo={
                    {
                      name="cluster1",
                      nodes={
                              {name="node1",description="This Node1",status="Normal"},
                              {name="node2",description="This Node2",status="Warning"},
                              {name="node3",description="This Node3",status="Error"}
                            }
                    },
                    {
                      name="cluster2",
                      nodes={
                              {name="node1",description="This Node1",status="Normal"},
                              {name="node2",description="This Node2",status="Warning"},
                              {name="node3",description="This Node3",status="Error"}
                            }
                    }
                  }
    local struct=json.encode(node)
    return struct
end

function sysinfo:post(context,query,...)
end
