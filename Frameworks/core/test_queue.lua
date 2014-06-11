--[[
	Author : Ma Tao
	Date   : 2014/06/01

	== Introduction ==
    Test case for utils.queue
    end

]]

module(..., package.seeall)
local queue = require("core.queue")
print("queue")

local pprint = require("pprint")

pprint.pprint(queue, "queue")

function main()
    print "main"
    local q = queue.create_queue("test/1")
    pprint.pprint(q, "q")
    local math = require("math")
    for i = 1, math.huge do
        local rt = q:call("put", "message" .. i)
        pprint.pprint(rt, "rt")
        rt = q:call("get", {})
        pprint.pprint(rt, "get")
    end

end

if ... == "__main__" then
    main()
end
