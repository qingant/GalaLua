--[[
	Author : Ma Tao
	Date   : 2014/04/16

	== Introduction ==
	GLR RPC Test-case

]]

__VER__ = "1.0"

module(..., package.seeall)

local rpc = require("rpc")
local client = require("rpc_client").client
local pprint = require("pprint").pprint

-- pprint(rpc, "rpc")
test_server = rpc.server:new():init("test_server")

function test_server:test(params, addr)
    pprint(params, "params")
    pprint(addr, "addr")
    return {msg="hello"}
end

function main(...)
    print("main...")
    local err, pid = glr.spawn("rpc_test", "test_server:main")
    print("Create rpc server: "..pid)
    local cli = client:new():init({ host = glr.sys.host,
                                    port = glr.sys.port,
                                    gpid = pid
                                  })
    local result = cli:call("test", {hello="world"})
    pprint(result, "result")
    local result = cli:call("test-1", {hello="world"})
    pprint(result, "result")

end

if ... == "__main__" then
    print("Main..." .. __id__)
    main()
end