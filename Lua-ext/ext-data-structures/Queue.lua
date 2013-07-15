--
--
--


module("Queue", package.seeall)

queue = {}


local io = require("io")
local table_insert = table.insert
local table_remove = table.remove
local table_sort = table.sort
local io_write = io.write

list = {}
list.__index = list

list_metatable = {}
setmetatable(list, list_metatable)
function list_metatable.__call(list_table, args)
   return list:new(args)
end


function list:__index(index)
   local _type = type(index)

   if 'string' == _type then
      local _method = list[index]
      if not _method then
         error("Bad List Method: " .. index)
      end
      return _method
   elseif 'number' == _type then
      if index < 0 then
         local _size = #self
         if i < -_size then
            error("List index out of range:" .. index)
         end
      else
         error("Bad List index:" .. index)
      end
   end
end


function list:__newindex(index, value)
   local _type = type(index)
   if 'number' == _type then
      if index < 0 then
         local _size = #self
         if index < -_size then
            error("List index out of range: " .. index)
         end
         self[_size+1] = value
      else
         error("Bad List Index: " .. index)
      end
   end
end


function queue:new()
   return { first = 0, last = -1}
end

function queue:qsize()
   return #self
end

function queue:full()
end

function queue:put(value)
   local _first = self.first - 1
   self.first = first
   self[first] = value
end

function queue:put_nowait()
end

function queue:get()
   local _first = self.first
   if _first > self.last then
      error("list is empty")
   end
   local _value = self[first]
   self[_first] = nil
   self.first = _first + 1

   return _value
end

function queue:get_nowait()
end

