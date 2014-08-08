--[[
    Author : Ma Tao
	Date   : 2014/06/26

	== Introduction ==
    gws test case

]]

module(..., package.seeall)

local modular = require("core.modular")
local os = require("os")
app = modular.inherit("gws.app")

app:set_addr(nil, 8080)
app:set_session_storage_path(os.getenv("HOME") .. "/session/")
app:set_static_path(os.getenv("HOME") .. "/Dev/GalaLua/Test/test_webframework/statics/")
app:set_log_path(os.getenv("HOME") .. "/log")
app:set_urls{
    ["^/([^/]*)$"] = _PACKAGE .. "main.index",
}
