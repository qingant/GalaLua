--Logger

local l=require "logger"

for i,v in pairs(l) do
	print(i,v)
end

ll=l.new("./log.txt")
for i,v in pairs(getmetatable(ll)) do
	print(i,v)
end
ll:finalizer()
