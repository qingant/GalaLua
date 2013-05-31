module(..., package.seeall)


local function update(tab1, tab2)
	for k,v in pairs(tab2) do
		tab1[k] = v
	end
	return tab1
end
table.update = update

local function map(self, func, ... )
    local result = {}
    for i,v in ipairs(self) do
        result[#result+1] = func(self,...)
    end
    return result
end
table.map = map

local function isArray( this )
    if this[1] then
        return true
    else
        return false
    end
end
table.isArray = isArray
