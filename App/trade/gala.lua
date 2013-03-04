module(..., package.seeall)

-- module doc --
__doc__ = {
   ["name"] = "something",   
}

-- class gala -- 
Gala = {}
Gala.__doc__ = {

}

-- init function -- 
function Gala:new(o)
   o = o or {}
   setmetatable(o, self)
   self.__index = self
   return o
end


-- ... --