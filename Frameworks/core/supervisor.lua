--[[
	Author : Ma Tao
	Date   : 2014/06/12

	== Introduction ==
    Common Supervisor
]]

module(..., package.seeall)

local rpc_server = require(_PACKAGE .. "rpc").server
local rpc_client = require(_PACKAGE .. "rpc_client").client
local pprint = require("pprint").pprint
local pformat = require("pprint").format

function supervisor_server(name)
    local server = rpc_server:new():init(name)
    server._processes = {}
    server._log = print
    function server:on_init()
    end
    function server:on_message(mtype, desc, msg)
        if mtype == glr.EXIT then
            local params = self._processes[desc.addr.gpid]
            self._processes[desc.addr.gpid] = nil
            if params then
                self:start_process(params)
            else
                -- TODO: logging
            end
        end
    end
    function server:start_process(params, desc)
        local spawn_func = nil
        if params.binded then
            spawn_func = glr.spawn_ex
        else
            spawn_func = glr.spawn
        end
        local addr, errmsg = spawn_func(unpack(params.parameters))
        self._log(pformat(params, "params"))
        self._log("spawn:", addr, errmsg)

        -- 处理启动失败的情况

        local mtype, desc, msg = glr.recv_by_addr(addr, 1)
        if mtype then
            self._log("Fail to start", mtype, desc, msg,  pformat(params))
            return {status = "failed"}
        end

        if addr then
            self._processes[addr] = params
        else
            error(errmsg)
        end
        return {process=addr}
    end
    function server:stop_process(params, desc)
        glr.kill(params.process)
        local mtype, desc, msg = glr.recv_by_type(glr.EXIT, 5)
        if not mtype then
            error("timeout")
        elseif desc.addr == params then
            return {status="ok"}
        end
    end
    function server:restart_process(params, desc)
        self:stop_process(params, desc)
        return self:start_process(params, desc)
    end
    function server:query(params, desc)
        return self._processes
    end
    server:main()
end

function create_supervisor(name)
    local supervisor_name = name .. "/supervisor"
    local r = glr.spawn(_NAME, "supervisor_server", supervisor_name)
    local client = rpc_client:new():init(supervisor_name)
    return client
end
