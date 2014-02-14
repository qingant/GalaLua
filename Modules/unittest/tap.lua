module(...,package.seeall)
local io=require "io"
local string=require "string"

function writel(...)
    io.write(string.format(...),"\n")
end

local TAP={}
function TAP:new(result)
    local o={}
    self.init(o,result)
    self.__index=self
    setmetatable(o,self)
    return o
end

function TAP:init(result)
    self.result=result
end

function TAP:output()
    all=#self.result
    writel("1..%d",all)
    local fail=0
    local line=""
    local sep=" - "
    for i,l in ipairs(self.result) do
        local name=l[2]
        if name=="" then
            sep=""
        end
        line=string.format("ok %d%s%s",i,sep,name)
        if not l[1] then
            fail=fail+1
            line="not "..line 
        end
        writel(line)
    end
    if fail>0 then
        writel("# Looks like you failed %d test of %d.",fail,all)
    end
end


function new(...)
    return TAP:new(...)
end
