
Http_Cache = {}

function Http_Cache:new(o)
    o = o or {}
    setmetatable(o,self)
    self.__index = self
    return o
end

function Http_Cache:init()
    self.prop = prop
    self.default = default
    self.none = none
    self.Type = Tyep
end

function Http_Cache:__set__(obj, value)
end
function Http_Cache:__get__(obj, Type)
    if no obj then
        return self
end
function Http_Cache:__del__(obj)
end
