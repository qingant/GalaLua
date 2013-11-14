module(...,package.seeall)
local pprint = require("pprint")

mdb = {}

function mdb:new(id)
    local o = {}
    o.id = id
    setmetatable(o, self)
    self.__index = function (this, k)
        return function (...) pprint.pprint(this);print(k);pprint.pprint({...}) end
    end
    return o
end

if ... == "__main__" then
    local db = mdb:new(1)
    db:test("arg1","argb",{1,2,3})
end
