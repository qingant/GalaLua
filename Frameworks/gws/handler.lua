--[[
gws handler template
--]]

module(...,package.seeall)

local obj={}
function obj:new(...)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    if type(self.init)=="function" then
        self.init(o,...)
    end
    return o
end

local support_methods={"GET","PUT","POST","DELETE"}
for _,method in ipairs(support_methods) do
    obj[string.lower(method)]=function (self,context,...)
        context:get_response():set_status_code(501)
    end
end

function new(...)
    return obj:new(...)
end

