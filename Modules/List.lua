-- luozhihui
-- date
-- lua5.1

module("List",package.seeall)

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

local function simple_table(param_table)
   local _type = type(param_table)
   local _meta_table = getmetatable(param_table)
   return 'table' == _type and _meta_table ~= list
end

function list:new(list_table)
   if not list_table then 
      list_table = {}
   elseif not simple_table(list_table) then
      local _table = list_table
      list_table = {}
      for index, value in iter(_table) do
         table_insert(list_table, value)
      end
   end
   setmetatable(list_table, list)
   return list_table
end

function list:append(index)
   table_insert(self, index)
end

function list:extend(lis)
   for index, value in ipairs(lis) do
      table_insert(self, value)
   end
end

function list:insert(index, value)
   table_insert(self, index, value)
end

list.delete = table_remove

function list:remove(value)
   for index = 1, #self do
      if self[index] == value then
         table_remove(self, index)
         return
      end
   end
end

function list:pop(index)
   if not index then 
      index = #self
   end
   return table_remove(self, index)
end

function list:index(value)
   for index =1, #self do
      if self[index] == value then
         return index
      end
   end
   error("List:Index failure, value" .. value .. "not found")
end

function list:count(value)
   local _count=0
   for index=1,#self do
      if self[index] == value then 
         _count = _count + 1
      end
   end

   return _count
end


function list:sort()
   table_sort(self)
end


function list:reverse()
   local _table = {}
   local _size = #self

   for index=1, _size do
      _table[index]=self[_size-index+1]
   end
   for index=1, _size do
      self[index] = _table[index]
   end
end

local function _slice(self, first, last)
   local _size = #self
   if not first then 
      first = 1
   end
   if first < 0 then 
      first = _size + first + 1 
   end

   if not last then
      last = _size
   end

   if last < 0 then
      last = last + _size
   end

   return first, last
end

function list:slice(first, last)
   first, last = _slice(self, first, last)
   local _table = self:new()
   for index=first, last do
      table_insert(_table, self[index])
   end
   return _table
end

function list:clear()
   for index=1, #self do
      table_remove(self, index)
   end
end

function list:len()
  return #self
end
