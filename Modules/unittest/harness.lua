module(...,package.seeall)
local out=require "out"
local writel=out.writel
local ansi=require "ansi"

--[[
{1,3,4,5,8,10}  ==> "1, 3-5, 8, 10"
]]
function list2string(t,start)
    local l={}
    local last=start or 1
    for _,i in ipairs(t) do
        if #l>0 and i==last+1 then
            l[#l]=string.format("%d-%d",last,i)
        else
            l[#l+1]=tostring(i)
        end
        last=i
    end
    return table.concat(l,", ")
end

local Harness={}

function Harness:new(results)
    local o={}
    self.init(o,results)

    setmetatable(o,{__index=self})
    return o
end

function Harness:init(results)
    self.info=results.info
    self.results=results.result
    self.total=#self.results
    self.failed_cases={}
    self.state_name="PASS"

    self.color=ansi.green
    for i,l in ipairs(self.results) do
        if not l.status then
            self.failed_cases[#self.failed_cases+1]=i
            self.state_name="FAIL"
            self.color=ansi.red
        end
    end

    self.failed=#self.failed_cases

end

function Harness:output()
    if self.state_name=="PASS" then
        self:pass_output()
    else
        self:failed_output()
    end
    writel()
    self:summary()
    writel()
end

function Harness:failed_output()
    writel(self.color"%s .. 1/%s",self.info.file,self.total)
    for _,i in ipairs(self.failed_cases) do
        local result=self.results[i]
        local token=""
        if result.name and result.name~="" then
            token=string.format("'%s'",result.name)
        else
            token=tostring(i)
        end
        writel("# Failed test(%s) at line %d",token,self.results[i].line)
    end
    writel("# Looks like you failed %d test of %d.",self.failed,self.total)
end

function Harness:pass_output()
    writel("%s .. ok",self.info.file)
end


function Harness:summary()
    writel("Test Summary Report")
    writel(("-"):rep(30))

    writel(self.color"%s (Test: %d, Failed: %d)",self.info.file,self.total,self.failed)
    local sw=(" "):rep(2)
    if self.state_name=="PASS" then
        writel(self.color"%sAll Tests Successful",sw)
    else
        writel(self.color"%sFailed Test: %s",sw,list2string(self.failed_cases))
    end
    writel("Result: %s",self.state_name)
end

function new(results)
    return Harness:new(results)
end
