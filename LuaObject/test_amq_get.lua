--[[
    amq
--]]
local amq=require("amq")

--for i,v in pairs(amq) do print(i,v) end
m=amq.new("/home/gala/channels/testAMQ.chl")
for i,v in pairs(getmetatable(m)) do print(i,v) end
print("*****************")
local nqa=m:NQArray()
for i,v in pairs(getmetatable(nqa)) do print(i,v) end
print("*****************")
local nq=nqa:get(0)
for i,v in pairs(getmetatable(nq)) do print(i,v) end

print("*****************")
print(nq:get(nq.MQC_BAT))
