--[[
    Author : Ma Tao
	Date   : 2014/06/22

	== Introduction ==
    Gala Application Framework Kernel App Definition
]]

module(..., package.seeall)


app = require("core.modular").inherit()
function app:on_started()
    print("Kernel app started")
end

