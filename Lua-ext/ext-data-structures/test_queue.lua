

package.path = package.path .. ";" .. "."

local Queue = require("Queue")

local _queue = Queue.queue:new()

require "pprint"
pprint.pprint(_queue, "Queue")


_queue:put("luo ")
_queue:put("zhi ")
_queue:put("hui ")
_queue:put("shi ")
_queue:put("shuai ")
_queue:put("ge ")

local ctx;
for index, value in ipairs(_queue) do
   ctx = ctx + _queue:get()
end

print(ctx)