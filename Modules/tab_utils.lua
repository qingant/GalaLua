module(..., package.seeall)

local function dup(t)
    local o={}
    for k,v in pairs(t) do
        if type(v)=="table" then
            o[k]=dup(v)
        else
            o[k]=v
        end
    end
    return o
end
table.dup=dup

local function update(tab1, tab2)
	for k,v in pairs(tab2) do
		tab1[k] = v
	end
	return tab1
end
table.update = update

local function map(self, func, ... )
    local result = {}
    for i,v in pairs(self) do
        result[#result+1] = func(v,...)
    end
    return result
end
local function reduce(self, func, init)
    assert(init ~= nil)
    for _,v in ipairs(self) do
        init = func(init, v)
    end
    return init
end
table.map = map
table.reduce = reduce

local function show(t)
    if type(t) ~= "table" then
        return tostring(t)
    else
        local tmp = table.map(t, show)
        return "[" .. table.concat(tmp,", ") .. "]"
    end
end

table.show = show
table.sum = function (self)
    return table.reduce(self, function (x, y)
            return x + y
    end, 0)
end


local function isArray( this )
    if this[1] then
        return true
    else
        return false
    end
end
table.isArray = isArray
table.is_array = isArray
if ... == "__main__" then
    local pprint=require "pprint"
    local t2={"ss",m=11,v={dd=11,"xxx","ddd",xx={}}}
    pprint.pprint(table.dup(t2))
    --[[
    local t = {1,2,3}
    local t1 = table.map(t, function (x) return x + 1 end)
    print(table.show(t1))
    print(table.sum(t1))
    print(table.sum(t))
    ]]
end
