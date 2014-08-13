module(..., package.seeall)
local json = require "cjson"

list = {}
topo = {}
save = {}
status = {}
start = {}
restart = {}

local clusters = {
                     {id="cluster1",name="核心集群A"},
                     {id="cluster2",name="核心集群B"}
                 }


local cluster_topo = {}

cluster_topo[clusters[1].id] = {
                      id = 1,
                      name = "核心集群A",
                      status = "normal",
                      description = "This is the cluster 1",
                      children = {
                                    ["1"]={
                                      id = 1,
                                      name = "N1",
                                      status = "normal",
                                      description = "This is the N 1",
                                      children = {}
                                    },
                                    ["2"]={
                                      id = 2,
                                      name = "N2",
                                      status = "normal",
                                      description = "This is the N 2",
                                      children = {}
                                    }
                                 }
                  }

cluster_topo[clusters[2].id] = {
                      id = 2,
                      name = "核心集群A",
                      status = "normal",
                      description = "This is the cluster 2",
                      children = {
                                    ["1"]={
                                      id = 1,
                                      name = "N1",
                                      status = "normal",
                                      description = "This is the N 1",
                                      children = {}
                                    },
                                    ["2"]={
                                      id = 2,
                                      name = "N2",
                                      status = "normal",
                                      description = "This is the N 2",
                                      children = {}
                                    },
                                    ["3"]={
                                      id = 3,
                                      name = "N3",
                                      status = "normal",
                                      description = "This is the N 3",
                                      children = {}
                                    }
                                 }
           }

local cluster_status = {}
cluster_status[clusters[1]] = {
                                  {id=1,status="Normal"},
                                  {id=2,status="Warning"}
                              }
cluster_status[clusters[2]] = {
                                  {id=1,status="Normal"},
                                  {id=2,status="Warning"},
                                  {id=3,status="Normal"}
                              }

function list:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function list:get(context,query,...)
    local node = {}
    node.err_code = 0
    node.clusters = clusters
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
    pprint.pprint(query,"topo input")
    local cid = query.cid
    
    local node = {}
    if cluster_topo[cid] ~= nil then
        node = {
                  err_code = 0,
                  topo = cluster_topo[query.cid]
               }
    else
        node = {
                  err_code = 0x11,
                  topo = nil
               }

    end
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
    pprint.pprint(query,"status input")
    local cid = query.cid
    
    local node = {}
    if cluster_status[cid] ~= nil then
        node = {
                  err_code = 0,
                  status = cluster_status[query.cid]
               }
    else
        node = {
                  err_code = 0x11,
                  status = nil
               }

    end
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
    pprint.pprint(query,"start input")
    local cid = query.cid
    local nid = query.nid
    
    local node = {}
    if nil == cluster_topo[cid] then
        node.err_code = 0x11
    elseif nil == cluster_topo[cid].children[nid] then
        node.err_code = 0x11
    else
        node.err_code = 0
    end
    
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
    pprint.pprint(query,"restart input")
    local cid = query.cid
    local nid = query.nid
    
    local node = {}
    
    if nil == cluster_topo[cid] then
        node.err_code = 0x11
    elseif nil == cluster_topo[cid].children[nid] then
        node.err_code = 0x11
    else
        node.err_code = 0
    end
    
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