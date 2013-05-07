
module("structX",package.seeall)

local struct = require("struct")

local fmt = ""
local value = {}
local i = 1

function is_entry(type)
   local flg = true
   if type ~= "int8_t" and type ~="uint8_t" and type ~= "int16_t" and type ~= "uint16_t" and type ~= "int32_t" and
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

function get_type_size(value)
    if value == "int8_t" or value == "uint8_t" then
       return 1
    elseif value == "int16_t" or value == "uint16_t" then
       return 2
    elseif value == "int32_t" or value == "uint32_t" then
       return 4
    elseif value == "int64_t" or value == "uint64_t" then
       return 8
    else
       return 0
    end
end

function get_size(tbl)
   if type(tbl) == "table" then
      for k,v in pairs(tbl) do
         if k == "size" then
             return v
         end
      end
   else
      error("the tbl is not table")
   end
end

function table_pading(tbl)
   local name = get_name(tbl)
   local type = get_type(tbl)
   for k,v in pairs(tbl) do
      if k ~= "name" and k ~= "type" and k ~= "items" and k ~= "size" then
         table_pading(v)
      elseif k == "items" then
         if type == "arraystruct" or type == "struct" then
            table_pading(v)
         else
            error("the tbl bad format")
         end
      elseif name ~= nil and type ~= nil and is_entry(type) and k == "name" then
         if type == "string" then
            table.insert(value,"")
         else
            table.insert(value,0)
         end
      end
   end
end

function array_table_func(tblk,size,tbl)
   for j=1,size do
      if tblk[j] then
         table_func(tblk[j],tbl)
      else
         table_pading(tbl)
      end
   end
end

function build_format(type)
   if type == "int8_t" then
      fmt = fmt .. "b"
   elseif type == "uint8_t" then
      fmt = fmt .. "B"
   elseif type == "int16_t" then
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
      fmt = fmt .. "c"
   end
end

function entry_func(tblk,tbl)
   local type = get_type(tbl)
   local name = get_name(tbl)
   if name == nil or type == nil then
      error("then tbl bad format")
   end
   table.insert(value,tblk[name])
end


function table_func(tblk,tbl)
   local type = get_type(tbl)
   local name = get_name(tbl)
   for k,v in pairs(tbl) do
      if k ~= "name" and k ~= "type" and k ~= "items" and k ~= "size" then
         table_func(tblk,v)
      elseif k == "items" then
         if type == "arraystruct" then
            arraysize = 0
            size = get_size(tbl)
            array_table_func(tblk[name],size,v)
         elseif type == "struct" then
            table_func(tblk[name],v)
         else
            error("the tbl bad format")
         end
      elseif name ~= nil and type ~= nil and is_entry(type) and k == "name" then
         entry_func(tblk,tbl)
      end
   end
end

function packX(tblk,endian,tbl)
   fmt = ""
   if type(tblk) == "table" and type(tbl) == "table" then
      if string.lower(endian) == "little" then
         fmt = fmt .. "<"
      elseif string.lower(endian) == "big" then
         fmt = fmt .. ">"
      else
         error("the endian must be little or big")
      end
      build_format_by_des(tbl)
      table_func(tblk,tbl)
      return struct.pack(fmt,unpack(value))
   else
      error("the pack :tblk and tbl is not table")
   end
end

function build_array_table(tbl,des,size,rtbl)
   for j = 1 ,size do
      rtbl[j] = {}
      for k,v in pairs(des) do
         build_table(tbl,v,rtbl[j])
      end
   end
end

function build_entry(tbl,des,rtbl)
   local name = get_name(des)
   local type = get_type(des)
   if name == nil or type == nil then
      error("then des bad format")
   end
   build_format(type);
   if type == "string" then
      fmt = fmt .. get_size(des)
   end
   rtbl[name] = tbl[i]
   i = i + 1
end


function build_table(tbl,des,rtbl)
   local name = get_name(des)
   local type = get_type(des)
   for k,v in pairs(des) do
      if k ~= "name" and k ~= "type" and k ~= "items" and k ~= "size" then
         build_table(tbl,v,rtbl)
      elseif k == "items" then
         if type == "arraystruct" then
            rtbl[name] = {}
            size = get_size(des)
            build_array_table(tbl,v,size,rtbl[name])
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

function unpackX(src,endian,des)
   fmt = ""
   if type(des) == "table" then
      if string.lower(endian) == "little" then
         fmt = fmt .. "<"
      elseif string.lower(endian) == "big" then
         fmt = fmt .. ">"
      else
         error("the endian must be little or big")
      end
      build_format_by_des(des)
      local rt = {}
      local tbl = {struct.unpack(fmt,src)}
      build_table(tbl,des,rt)
      tbl[#tbl] = nil
      return rt
   else
      error("the des must be table")
   end
end

function foreach_array(des,size)
   for k=1,size do
      build_format_by_des(des)
   end
end

function format_entry(des)
   local type = get_type(des)
   build_format(type);
   if type == "string" then
      fmt = fmt .. get_size(des)
   end
end

function build_format_by_des(des)
   local type = get_type(des)
   local name = get_name(des)
   for k,v in pairs(des) do
      if k ~= "name" and k ~= "type" and k ~= "items" and k ~= "size" then
         build_format_by_des(v)
      elseif k == "items" then
         if type == "arraystruct" then
            size = get_size(des)
            foreach_array(v,size)
         elseif type == "struct" then
            build_format_by_des(v)
         else
            error("the tbl bad format")
         end
      elseif name ~= nil and type ~= nil and is_entry(type) and k == "name" then
         format_entry(des)
      end
   end
end


