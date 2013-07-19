--Logger

local l=require "logger"

for i,v in pairs(l) do
	print(i,v)
end

local ll={}
for i=1,5000000 do
    local a=l.new("./log.txt")
    ll[#ll+1]=a
    a:debug("aaaaaaaaa")
    a:flush()
    a:finalizer()
end
--[[
for i,v in pairs(getmetatable(ll)) do
	print(i,v)
end
]]
print("over...")
