--[[
	Author : Ma Tao
	Date   : 2014/04/18

	== Introduction ==
]]

module(..., package.seeall)


local function getopt( arg )
	local tab = {}
	for k, v in ipairs(arg) do
		if string.sub( v, 1, 2) == "--" then
			local x = string.find( v, "=", 1, true )
			if x then tab[ string.sub( v, 3, x-1 ) ] = string.sub( v, x+1 )
			else      tab[ string.sub( v, 3 ) ] = true
			end
		elseif string.sub( v, 1, 1 ) == "-" then
			local y = 2
			local l = string.len(v)
			local jopt
			while ( y <= l ) do
				jopt = string.sub( v, y, y )
				--				if string.find( options, jopt, 1, true ) then
				if true then
					if y < l then
						tab[ jopt ] = string.sub( v, y+1 )
						y = l
					else
						tab[ jopt ] = arg[ k + 1 ]
					end
				else
					tab[ jopt ] = true
				end
				y = y + 1
			end
		end
	end
	return tab
end

local arg_tab

function get_option(key)
    return get_options()[key]
end

--@_API_LEVEL >= 200
function get_options()
	if not arg_tab then
		arg_tab = getopt(_glr.get_option(), key)
	end
    return arg_tab
end

function set_option(key,value)
    get_options()[key]=value
end
