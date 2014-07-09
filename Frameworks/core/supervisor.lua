--[[
	Author : Ma Tao
	Date   : 2014/06/12

	== Introduction ==
    Common Supervisor
]]

module(..., package.seeall)

local rpc = require(_PACKAGE .. "rpc")
-- local rpc_client = require(_PACKAGE .. "rpc_client").client
local pprint = require("pprint").pprint
local pformat = require("pprint").format
local logger = require(_PACKAGE .. "logger").logger              -->added by yangbo for logging test


server = rpc.server:new()
server.raw_process_type = "raw"   -- 裸服务
server.gen_process_type = "gen"   -- 继承自rpc.server的一般服务

local process_table = {}
local proxy = {}
function process_table:new()
    local o = {}
    proxy[o] = {}
    setmetatable(o, self)
    return o
end
local function get_proxy(t)
    return proxy[t]
end
function process_table:__index(k)
    if type(k) == "table" then
        return proxy[self][k.gpid]
    end
end
function process_table:__newindex(k,v)
    if type(k) == "table" then
        proxy[self][k.gpid] = v
    end
end

function server:new(name)
    self:init(name)
    self._processes = process_table:new()
    self._id = 0
    self._processes_indexed_by_id = {}
    self._logger = logger:new():init(self)
    self._logger:info("supervisor(%s) started", name)
    return self
end
function server:on_init()
end
function server:on_message(mtype, desc, msg)
    self._logger:info("on_message", mtype, glr.EXIT, desc, msg)
    if mtype == glr.EXIT then
        -- TODO: logging restart info
        local params = self._processes[desc.addr]
        self._logger:info("restarting...", params)
        self._processes[desc.addr] = nil
        if params then
            self:start_process(params.start_params)
            self._logger:info(pformat(glr.status.processes(), "processes"))
            self._logger:info(pformat(get_proxy(self._processes), "records"))
        else
            -- TODO: logging
        end
    end
end
function server:_start_raw_process(params, desc)
    local spawn_func = nil
    if params.binded then
        spawn_func = glr.spawn_ex
    else
        spawn_func = glr.spawn
    end
    local addr, errmsg = spawn_func(unpack(params.parameters))
    return addr, errmsg
end
function server:start_process(params, desc)
    self._logger:info("params: %s", pformat(params, "params"))
    local addr, errmsg
    local cli
    if params.process_type == self.gen_process_type then
        -- TODO: error handling(when rpc.create_server fails)
        cli = rpc.create_server(params.process_params)
        self._logger:info("process created", pformat(cli))
        addr = cli._server_addr
    elseif params.process_type == self.raw_process_type then
        addr, errmsg = self:_start_raw_process(params, desc)
    end
    self._logger:info("spawn:", addr, errmsg)
    -- 处理启动失败的情况

    local mtype, desc, msg = glr.recv_by_condition(function (msg) return msg[2].addr.gpid == addr.gpid and msg[1] == glr.EXIT end, 1)
    self._logger:debug("mtype:" ..  (mtype or "nil"))
    if mtype == glr.EXIT then
        self._logger:info("Fail to start:%d, %s %s %s", mtype, pformat(desc), pformat(msg),  pformat(params))
        return {status = "failed"}
    end

    if addr then
        self._id = self._id + 1
        self._processes[addr] = {start_params = params,
                                 group = params.group,
                                 id = self._id,
                                 client = cli}
        self._processes_indexed_by_id[self._id] = self._processes[addr]
    else
        self._logger:info("error", errmsg)
        error(errmsg)
    end
    self._logger:debug("process %d started", addr.gpid)
    return {process=addr}
end
function server:stop_process(params, desc)
    local info = self._processes[params.process]
    if info == nil then
        return {status="error", errmsg = "not found"}
    end
    self._processes[params.process] = nil
    if params.process_type == self.gen_process_type then
        local rt = info.client:call("stop", nil, 5)
        if rt == nil then
            glr.kill(params.process)
        end
    elseif params.process_type == self.raw_process_type then
        glr.kill(params.process)
    end
    local mtype, desc, msg = glr.recv_by_addr(params.process, 5)
    if not mtype then
        return nil, "timeout"
    elseif desc.addr == params then
        return self.ok
    end
end
function server:stop_all(params, desc)
    for i = #self._processes_indexed_by_id, 1, -1 do
        self:stop_process(self._processes_indexed_by_id[i])
    end
    return self.ok
end
function server:restart_process(params, desc)
    local info = self._processes[params.process]
    self:stop_process(params, desc)
    return self:start_process(info.start_params, desc)
end
function server:query(params, desc)
    return self._processes
end
