--[[
	Author : Ma Tao
	Date   : 2014/06/01

	== Introduction ==
    Test case for utils.queue
]]

module("test_queue", package.seeall)
print("queue")

local rpc = require("core.rpc")

local pprint = require("pprint")
local server_name = "test/1"
function recv(n)
    local q = rpc.create_client(server_name)
    for i=1,n do
        -- print(i)
        q:call("get")
    end
    glr.exit(0)
end
function main()

    local q = rpc.create_server{ mod_name = "core.queue", parameters = {server_name}}
    pprint.pprint(q, "q")
    local math = require("math")
    math.huge = 1000000
    glr.spawn(_NAME, "recv", math.huge)
    local msg = "massage"
    for i = 1, math.huge do
        -- print(i)
        local rt = q:request("put", msg)
        -- pprint.pprint(rt, "rt")
        -- rt = q:call("get", {})
        -- pprint.pprint(rt, "get")
    end
    -- glr.exit()
end

if ... == "__main__" then
    main()
end
