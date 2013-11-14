module(...,package.seeall)

httpClient = {}


function httpClient:new()
   local o = {}
   setmetatable(o, self)
   self.__index = self
   return o
end

function httpClient:init(uri)
   self._socket = sock
end


function httpClient:get(params)
   
end

function httpClient:_getResponse()
end


function test(...)
   if true then
       
   end
 
end