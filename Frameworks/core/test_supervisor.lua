--[[
	Author : Ma Tao
	Date   : 2014/06/01

	== Introduction ==
    Test case for core.supervisor
]]
module(..., package.seeall)
local rpc = require(_PACKAGE .. "rpc")
local pprint = require("pprint").pprint
function main()
    local supervisor_client = rpc.create_server{mod_name = "core.supervisor", parameters = {"test/supervisor"}}
    local rt = nil
    pprint(supervisor_client, "supervisor_client")
    -- pprint(glr.npt.registered(supervisor_client._server_addr), "addr")
    rt = supervisor_client:call("sys_status")
    pprint(rt, "status")
    rt = supervisor_client:call("start_process", { process_type = "gen",
                                                   process_params = {
                                                       mod_name = "core.test_service",
                                                       parameters = {"test/service"}
    }})
    pprint(rt, "Service")
    pprint(glr.status.processes(), "p")
    
    glr.kill(rt.result.process.gpid)
    print("Kill")
    pprint(glr.status.processes(), "p")
end

if ... == "__main__" then
    main()
end
