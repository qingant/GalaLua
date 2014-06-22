--[[
    Author : Ma Tao
	Date   : 2014/06/22

	== Introduction ==
    Gala Application Framework Bootstrap
]]

module(..., package.seeall)





local fmt = string.format



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
local function _start_pool(sc, com)
end


local function _start_component(sc, com)
    if     com.catagory == "server" then
        local rt  = sc:call("start_process", com)
        -- TODO: error handling and logging
    elseif com.catagory == "pool" then
        _start_pool(sc, com)
    end
end

local function _start_app(app, inst_name)
    local rpc = require("core.rpc")
    local base_name
    if inst_name then
        base_name = fmt("%s.", app._PACKAGE, inst_name)
    else
        base_name = app._PACKAGE
    end

    local sup_id = fmt("%s.supervisor", base_name)
    if app._NAME == "kernel.app" then
        rpc.create_server{mod_name = "core.supervisor", bind_gpid = 3, parameters = {sup_id}} -- root supervisor
    else
        local root_sup = rpc.create_client(kernel_sup_name)
        local app_sup_start_arguments = {
            process_type = "gen",
            process_params = {
                mod_name = "core.supervisor",
                bind_gpid = _get_free_bgpid(),
                parameters = {sup_id},
            },
        }
        local rt = root_sup:call("start_process", app_sup_start_arguments)
        if rt.error then
            error(rt.error)
        end
    end

    local sup_client = rpc.create_client(sup_id)


    -- startup other services
    if app.components then
        for i, v in ipairs(app.components) do
            _start_component(sc, v)
        end
    end
    if app.on_started then
        app.on_started()
    end
end



function start_app(app_name, inst_name)
    local app = require(fmt("%s.app", app_name))
    print(fmt("Start app: %s", app._NAME))

    -- start dependent apps first
    if app.dep then
        for i,v in ipairs(app.dep) do
            start_app(v)
        end
    end
    _start_app(app)
    -- real start up
end

function start_kernel_app()
    start_app("kernel")
end


function bootstrap(init_app, inst_name)
    start_kernel_app()
    if init_app then
        start_app(init_app, inst_name)
    else
        local grepl = require("grepl").GRepl
        grepl:new():main()
    end
end


-- system entry
function main()
    -- TODO: parse comman line arguments
    bootstrap()
end
if ... == "__main__" then
    main()
end
