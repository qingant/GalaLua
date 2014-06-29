-- module(..., package.seeall)
-- local __main__ = ...
-- local __self__ = package.loaded[__main__]
local M = {}
local require = require
local pairs = pairs
local _glr = _glr
if _VERSION== "Lua 5.2" then
    _ENV=M
else
    setfenv(1, M)
end

__VER__ = "1.0"  -- first api wrapper for glr-1.x

local function mixin(mod)
    for k,v in pairs(mod) do
        M[k] = v
    end
end



-- core sys-calls
mixin(require("wrappers._core"))


-- networking calls
net = require("wrappers._net")

-- utils
mixin(require("wrappers._utils"))

mixin(require("global"))

amq = require("wrappers._amq")

require "gar_loader"
-- NamedProcessTable
npt = _glr.npt

function run_spyer( ... )
    local spyerModuleName = get_option("spyer") or "spyer"
    local spyerModule = require(spyerModuleName)
    return spyerModule.main( ... )
end

return M

