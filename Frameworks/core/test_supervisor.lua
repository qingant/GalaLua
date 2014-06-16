--[[
	Author : Ma Tao
	Date   : 2014/06/01

	== Introduction ==
    Test case for core.supervisor
]]
module(..., package.seeall)
local supervisor = require("core.supervisor")
local pprint = require("pprint").pprint
function main()
    local supervisor_client = supervisor.create_supervisor("test")
    local rt = nil
    pprint(supervisor_client, "supervisor_client")
    pprint(glr.npt.registered(supervisor_client._server_addr), "addr")
    rt = supervisor_client:call("server_status")
    pprint(rt, "status")
    rt = supervisor_client:call("start_process", {parameters = {"core.test_service", "test_server", "test/11"}})
    pprint(rt, "Service")
end

if ... == "__main__" then
    main()
end
