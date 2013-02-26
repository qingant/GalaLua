module(..., package.seeall)

-- module doc --
__doc__ = {
   ["name"] = "something",   
}

-- class gala -- 
gala = {}
gala.__doc__ = {

}

-- init function -- 
function gala:new(o)
   o = o or {}
   setmetatable(o, self)
   self.__index = self
   return o
end


-- ... --