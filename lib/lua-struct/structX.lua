
module("structX",package.seeall)

local struct = require("struct")

local fmt = ""
local value = {}
local i = 1

function is_entry(type)
   local flg = true
   if type ~= "int16_t" and type ~= "uint16_t" and type ~= "int32_t" and
      type ~= "uint32_t" and type ~= "int64_t" and type ~= "uint64_t" and type ~= "string" then
      flg = false
   end
   return flg
end

function get_type(tbl)
   if type(tbl) == "table" then
      for k,v in pairs(tbl) do
         if k == "type" then
             return v
         end
      end
   else
      error("the tbl is not table")
   end
end

function get_name(tbl)
   if type(tbl) == "table" then
      for k,v in pairs(tbl) do
         if k == "name" then
             return v
         end
      end
   else
      error("the tbl is not table")
   end
end

function array_table_func(tblk,tbl)
   for k,v in pairs(tbl) do
      table_func(tblk[k],v)
   end
end

function build_format(type)
   if type == "int16_t" then
      fmt = fmt .. "h"
   elseif type == "uint16_t" then
      fmt = fmt .. "H"
   elseif type == "int32_t" then
      fmt = fmt .. "i4"
   elseif type == "uint32_t" then
      fmt = fmt .. "I4"
   elseif type == "int64_t" then
      fmt = fmt .. "i8"
   elseif type == "uint64_t" then
      fmt = fmt .. "I8"
   elseif type == "string" then
      fmt = fmt .. "s"
   end
end

function entry_func(tblk,tbl)
   local type = get_type(tbl)
   local name = get_name(tbl)
   if name == nil or type == nil then
      error("then tbl bad format")
   end
   build_format(type);
   table.insert(value,tblk[name])
   return tblk[name]
end


function table_func(tblk,tbl)
   local type = get_type(tbl)
   local name = get_name(tbl)
   local value
   for k,v in pairs(tbl) do
      if k ~= "name" and k ~= "type" and k ~= "items" then
         table_func(tblk,v)
      elseif k == "items" then
         if type == "arraystruct" then
            array_table_func(tblk[name],v)
         elseif type == "struct" then
            table_func(tblk[name],v)
         else
            error("the tbl bad format")
         end
      elseif name ~= nil and type ~= nil and is_entry(type) and k == "name" then
         print(entry_func(tblk,tbl))
      end
   end
end

function packX(tblk,endian,tbl)
   if type(tblk) == "table" and type(tbl) == "table" then
      if string.lower(endian) == "little" then
         fmt = fmt .. "<"
      elseif string.lower(endian) == "big" then
         fmt = fmt .. ">"
      else
         error("the endian must be little or big")
      end
      table_func(tblk,tbl)
      return fmt,struct.pack(fmt,unpack(value))
   else
      error("the pack :tblk and tbl is not table")
   end
end

function build_array_table(tbl,des,rtbl)
   for k,v in pairs(des) do
      rtbl[k] = {}
      build_table(tbl,v,rtbl[k])
   end
end

function build_entry(tbl,des,rtbl)
   local name = get_name(des)
   local type = get_type(des)
   if name == nil or type == nil then
      error("then des bad format")
   end
   rtbl[name] = tbl[i]
   i = i + 1
end

function build_table(tbl,des,rtbl)
   local name = get_name(des)
   local type = get_type(des)
   for k,v in pairs(des) do
      if k ~= "name" and k ~= "type" and k ~= "items" then
         build_table(tbl,v,rtbl)
      elseif k == "items" then
         if type == "arraystruct" then
            rtbl[name] = {}
            build_array_table(tbl,v,rtbl[name])
         elseif type == "struct" then
            rtbl[name] = {}
            build_table(tbl,v,rtbl[name])
         else
            error("the tbl bad format")
         end
      elseif name ~= nil and type ~= nil and is_entry(type) and k == "name" then
         build_entry(tbl,des,rtbl)
      end
   end
end

function unpackX(format,src,des)
   if type(des) == "table" then
      local tbl = {struct.unpack(format,src)}
      local rt = {}
      build_table(tbl,des,rt)
      return rt
   else
      error("the des must be table")
   end
end

