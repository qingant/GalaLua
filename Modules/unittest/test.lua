local pprint=require "pprint"

local test={}
Test=test
function test:init()

end

function test:new()
    local o={}
    self.init(o)
    self.__index=self
    setmetatable(o,self)

    return o
end

function test:test(x,y)
    return x+y
end

--if ...=="__UNITTEST__" then
glr.unittest[[
    o=Test:new()
    ok(o:test(1,2)==4,"dddd")
    ok(o:test(1,3)==4,"ggg")
    is(o:test(1,31),32,"is 4")
    is_(o:test(1,2),"Number")
    is_number(o:test(1,2))
    is_string(o:test(1,2))
]]
