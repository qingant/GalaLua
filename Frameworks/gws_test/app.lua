--[[
    Author : Ma Tao
	Date   : 2014/06/26

	== Introduction ==
    gws test case

]]

module(..., package.seeall)

local modular = require("core.modular")

app = modular.inherit("gws.app")

app:set_addr(nil, 8080)
app:set_urls{
    ["^/([^/]*)$"] = _PACKAGE .. "main.index",
}
