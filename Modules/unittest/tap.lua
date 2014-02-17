module(...,package.seeall)
local out=require "out"
local writel=out.writel

local TAP={}
function TAP:new(result)
    local o={}
    self.init(o,result)
    self.__index=self
    setmetatable(o,self)
    return o
end

function TAP:init(results)
    self.info=results.info
    self.results=results.result
    self.total_cases=#self.results
end

function TAP:output()
    writel("1..%d",self.total_cases)
    local fail=0
    local line=""
    for i,l in ipairs(self.results) do
        local sep=" - "
        local name=l.name or ""
        if name=="" then
            sep=""
        end
        if not l.status then
            fail=fail+1
            writel("not ok %d%s%s",i,sep,name)
            local msg=l.msg or ""
            if msg ~="" then
                writel("#%s", msg)
            end
            --writel("#test failed at line:%d",l.line)
        else
            writel("ok %d%s%s",i,sep,name)
        end
    end
    --if fail>0 then
    --    writel("# Looks like you failed %d test of %d.",fail,all)
    --end
end


function new(...)
    return TAP:new(...)
end
