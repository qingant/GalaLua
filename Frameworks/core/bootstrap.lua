--[[
    Author : Ma Tao
	Date   : 2014/06/22

	== Introduction ==
    Gala Application Framework Bootstrap
]]

module(..., package.seeall)





local fmt = string.format
local pprint = require("pprint")
local rpc = require(_PACKAGE .. "rpc")
local logger = require(_PACKAGE .. "logger").logger
local kernel_app_name = "kernel"
local kernel_sup_name = fmt("%s.supervisor", kernel_app_name)

local function _get_free_bgpid() -- get free binded gpid
    local ps = glr.status.processes()
    for i = 32, 128 do
        if ps[i] == nil then
            return i
        end
    end
end

local app_mgr = {}
function app_mgr:new(app_name, inst_name)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    o.app_name = app_name
    o.inst_name = inst_name
    o:_init()
    return o
end
function app_mgr:_init()
    self.app_module = require(fmt("%s.app", self.app_name))
    self.app = self.app_module.app
    if inst_name then
        self.base_name = fmt("%s.%s.", self.app_name, self.inst_name)
    else
        self.base_name = self.app_module._PACKAGE
    end
    self.app_inst_name = self.base_name:sub(1, #self.base_name - 1)
    self.root_sup = rpc.create_client(kernel_sup_name)
    self.supervisor_id = fmt("%ssupervisor", self.base_name)
    local log_file_path = string.format("%s/app.log", self.app.log_path)
    self._logger = logger:new():init(self, log_file_path)  --TODO:
end
function app_mgr:_start_pool(com)
    -- start pool
    print("start pool")
    self._supervisor:call("start_process", {
                process_type = "gen",
                process_params = {
                    mod_name = "core.pool",
                    parameters = {self.app_inst_name, com.pool_name, com.dispatcher, com.worker, com.params}
                }
    })

end


function app_mgr:_start_component(com)
    if     com.catagory == "server" then
        local rt  = self._supervisor:call("start_process", com)
        -- TODO: error handling and logging
    elseif com.catagory == "pool" then
        self:_start_pool(com)
    end
end
_ROOT_SUPERVISOR_GPID = 3
function app_mgr:_start_app()
    local app = self.app
    if app.on_load then
        app:on_load(self)
    end

    if self.app_module._NAME == "kernel.app" then
        rpc.create_server{mod_name = "core.supervisor", bind_gpid = _ROOT_SUPERVISOR_GPID, parameters = {self.supervisor_id}} -- root supervisor
    else
        local app_sup_start_arguments = {
            process_type = "gen",
            process_params = {
                mod_name = "core.supervisor",
                bind_gpid = _get_free_bgpid(),
                parameters = {self.supervisor_id, self.app.log_path},
            },
        }
        local rt = self.root_sup:call("start_process", app_sup_start_arguments)
        if rt.error then
            error(rt.error)
        end
    end
    glr.time.sleep(30)          -- wait register
    local sup_client = rpc.create_client(self.supervisor_id)
    self._supervisor = sup_client

    -- startup other services
    if app.components then
        for i, v in ipairs(app.components) do
            self:_start_component(v)
        end
    end

    if app.on_started then
        app:on_started(self)
    end
end
function app_mgr:_is_kernel()
    return self.app_module._NAME == "kernel.app"
end
function app_mgr:_stop_app()
    --TODO: deal with dependency
    print("supervisor: ", self.supervisor_id)
    self._supervisor = rpc.create_client(self.supervisor_id)
    self._supervisor:call("stop_all", {})
    if self:_is_kernel() then
        glr.kill(_ROOT_SUPERVISOR_GPID)
    else
        self.root_sup:call("stop", {process=self.supervisor_id})
    end
    --TODO: error handling

    if self.app.on_stoped then
        self.app:on_stoped(self)
    end
end

function start_app(app_name, inst_name)
    local app = require(fmt("%s.app", app_name)).app

    -- start dependent apps first
    if app.dep then
        for i,v in ipairs(app.dep) do
            start_app(v)
        end
    end

    -- real start up
    app_mgr:new(app_name, inst_name):_start_app()
end

function stop_app(app_name, inst_name)
    app_mgr:new(app_name, inst_name):_stop_app()
end

function start_kernel_app()
    start_app("kernel")
end


function bootstrap(init_app, inst_name)
    start_kernel_app()
    if init_app then
        start_app(init_app, inst_name)
    else

    end
end


-- system entry
function main()
    -- TODO: parse comman line arguments
    if glr.get_option("init-app") then
        bootstrap(glr.get_option("init-app"), glr.get_option("inst-name"))
    else
        bootstrap()
    end
    if glr.get_option("interactive") then
        local grepl = require("grepl").GRepl
        local g = grepl:new()
        g:mockEnv({"start_app", start_app},
            {"stop_app", stop_app})
        g:main()
    end
end
if ... == "__main__" then
    main()
end
