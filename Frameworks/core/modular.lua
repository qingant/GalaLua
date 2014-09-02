--[[
    Author : Ma Tao
	Date   : 2014/07/01

	== Introduction ==
    core.app: application building api

    == TODO ==
    1. loadable and disloadable application

    == NOTES ==
]]

module(..., package.seeall)


app_base = {}
app_base.deps = {}
app_base.components = {}

function app_base:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function app_base:init()
end

function app_base:add_component(component)
    self.components[#self.components + 1] = component
end
function app_base:add_components(...)
    for i,v in ipairs(...) do
        self:add_component(v)
    end
end
function app_base:add_dep(dep)
    self.deps[#self.deps + 1] = dep
end

-- bootstrap hooks
function app_base:on_started(app_mgr)
end

function app_base:on_load(app_mgr)
end

function app_base:on_stoped(app_mgr)
end

function inherit(app_name)
    if app_name then
        return require(app_name).app:new()
    else
        return app_base:new()
    end
end