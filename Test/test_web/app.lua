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
app:set_static_path(norm_path(os.getenv("PWD") .. "/test_web/static/"))
app:set_log_path(os.getenv("HOME") .. "/log")
app:set_urls{
    ["^/index$"] = _PACKAGE .. "main.index",
    --["^/route/list$"] = _PACKAGE .. "route.list",
    --["^/route/structure$"] = _PACKAGE .. "route.structure",
    ["^/sysinfo$"] = _PACKAGE .. "main.sysinfo",
    ["^/cluster/list$"] = _PACKAGE .. "cluster.list",
    ["^/cluster/topo$"] = _PACKAGE .. "cluster.topo",
    ["^/cluster/save$"] = _PACKAGE .. "cluster.save",
    ["^/cluster/status$"] = _PACKAGE .. "cluster.status",
    ["^/cluster/start$"] = _PACKAGE .. "cluster.start",
    ["^/cluster/restart$"] = _PACKAGE .. "cluster.restart",
    ["^/cluster/stop$"] = _PACKAGE .. "cluster.stop",
    ["^/analysis/data$"] = _PACKAGE .. "analysis.data",
    ["^/analysis/list$"] = _PACKAGE .. "analysis.list",
    ["^/analysis/load$"] = _PACKAGE .. "analysis.load",
    ["^/analysis/save$"] = _PACKAGE .. "analysis.save",
}
