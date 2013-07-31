module(...,package.seeall)

GlrBuffer = {}
function GlrBuffer:new()
    local o = {}
    setmetatable(o,self)
    self.__index = self
    return o
end
function GlrBuffer:init(size)
    self.cache = {}
    self.length = 0
    self.total = size
    return self
end
function GlrBuffer:put(str)
    function calculate(obj1,obj2)
        if type(obj2) == "string" then
            return #obj2
        elseif getmetatable(obj2) == getmetatable(obj1) then
            local size = 0
            for k, v in pairs(obj2.cache) do
               size  = #v + size
            end
            return size
        else
            error("the type unkown")
        end
    end
    if self:isfull() == true then
        return
    end
    local length = calculate(self,str)
    if type(str) == "string" and self.total >= (length+self.length) then
        self.cache[#self.cache+1] = str
        self.length = self.length + #str
    elseif getmetatable(str) == getmetatable(self) and self.total >= (length+self.length) then
        for _, v in pairs(str.cache) do
            self.cache[#self.cache + 1] = v
            self.length = self.length + #v
        end
    elseif (length+self.length) >= self.total then
        error("not enough memory for buff")
    else
        error("bad #2 operand to buff copy operation", 2)
    end
end
function GlrBuffer:size()
    return self.length
end
function GlrBuffer:cap()
    return self.total
end
function GlrBuffer:isfull()
    return self.total == self.length
end
function GlrBuffer:clean()
    self.cache = {}
    self.length = 0
end
function GlrBuffer:get()
    return self.cache
end

