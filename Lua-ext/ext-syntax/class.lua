
module(..., package.seeall)

local string = require("string")

local function parseName(name)
	local _begin, _end, _class_name = assert(name:find('%s*([a-zA-Z][a-zA-Z0-9_]*)%s*%(?'))
	if not name:find('%(', _end) then
		return _class_name, {}
	end
	local _bases = {}
	while true do
		local _base
		_begin, _end, _base = name:find('%s*([a-zA-Z][a-zA-Z0-9._]*)%s*%,?', _end+1)
		if _base then
			table.insert(_bases,_base)
		else
			break	
		end
	end
	return _class_name, _bases
end

--local _class_name, _bases = parseName("A(B1, xxx.C,  Base2, Base3)")
--print(_class_name)
--for key, var in pairs(_bases) do
--	print(key .. " " .. var .. " ")
--end

local function new(_table, ...)
    local _obj = {}
    if _table.__init__ then
        _table.__init__(_obj, ...)
    end
    return setmetatable(_obj, {__index=_table, __class__=_table})
end

function class(name)
	local _env = getfenv(2)
	local _class_name, _bases = parseName(name)
	for index, value in ipairs(_bases) do
		_bases[index] = assert(_env[value])
	end
	
	-- class create closure	
	return function (_table)
		local _meta = {__call = new, __bases__ = _bases}
		_meta.__index = function (nouse, key)
			for _, _class in ipairs(_meta.__bases__) do
				local _value = _class[key]
				if _value ~= nil then
					return _value
				end
			end
		end
		local _class_obj = setmetatable(_table, _meta)
		_env[_class_name] = _class_obj
		return  _class_obj
	end
end

_G.class = class
