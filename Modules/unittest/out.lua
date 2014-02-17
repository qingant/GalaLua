module(...,package.seeall)

local io=require "io"
local string=require "string"

function writel(...)
    local format=...
    if format==nil then
        io.write("\n")
    else
        io.write(string.format(...),"\n")
    end
end

function writet(t,sep)
    io.write(table.concat(t,sep or "\n"))
end

