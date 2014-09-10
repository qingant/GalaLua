--[[
    Author : Ma Tao
	Date   : 2014/06/26

	== Introduction ==
    gws test case

]]

module(..., package.seeall)

local modular = require("core.modular")
local os = require("os")
local norm_path = require("os.path").norm_path
app = modular.inherit("gws.app")

app:set_addr(nil, 8080)
app:set_session_storage_path(os.getenv("HOME") .. "/session/")
app:set_static_path(norm_path(os.getenv("PWD") .. "../../Test/web_test/"))
app:set_log_path(os.getenv("HOME") .. "/log")
app:set_urls{
    ["^/$"] = _PACKAGE .. "main.index",
    ["^/index$"] = _PACKAGE .. "main.index",
    ["^/summary"] = _PACKAGE .. "main.summary",
    ["^/processing"] = _PACKAGE .. "main.processing",
    ["^/tps"] = _PACKAGE .. "main.tps",

}
