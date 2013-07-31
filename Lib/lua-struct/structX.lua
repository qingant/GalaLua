
module(...,package.seeall)

local struct = require("struct")
local pprint = require("pprint")
local strop  = require("stringop")

format_cache = {}

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

function table_pading(tbl,value_set)
   local tmp_set = value_set
   local name = get_name(tbl)
   local type = get_type(tbl)
   for k,v in pairs(tbl) do
      if k ~= "name" and k ~= "type" and k ~= "items" and k ~= "size" then
         tmp_set = table_pading(v,tmp_set)
      elseif k == "items" then
         if type == "arraystruct" or type == "struct" then
            tmp_set = table_pading(v,tmp_set)
         else
            error("the tbl bad format")
         end
      elseif name ~= nil and type ~= nil and is_entry(type) and k == "name" then
         if type == "string" then
            table.insert(tmp_set,"")
         else
            table.insert(tmp_set,0)
         end
      end
   end
   return tmp_set
end

function array_table_func(tblk,size,tbl,value_set)
   local tmp_set = value_set
   for j=1,size do
      if tblk[j] then
         tmp_set = table_func(tblk[j],tbl,tmp_set)
      else
         tmp_set = table_pading(tbl,tmp_set)
      end
   end
   return tmp_set
end

function build_format(type,format_str)
   local format_tmp = format_str
   if type == "int8_t" then
      format_tmp = format_tmp .. "b"
   elseif type == "uint8_t" then
      format_tmp = format_tmp .. "B"
   elseif type == "int16_t" then
      format_tmp = format_tmp .. "h"
   elseif type == "uint16_t" then
      format_tmp = format_tmp .. "H"
   elseif type == "int32_t" then
      format_tmp = format_tmp .. "i4"
   elseif type == "uint32_t" then
      format_tmp = format_tmp .. "I4"
   elseif type == "int64_t" then
      format_tmp = format_tmp .. "i8"
   elseif type == "uint64_t" then
      format_tmp = format_tmp .. "I8"
   elseif type == "string" then
      format_tmp = format_tmp .. "c"
   end
   return format_tmp
end

function entry_func(tblk,tbl,value_set)
   local tmp_set = value_set
   local type = get_type(tbl)
   local name = get_name(tbl)
   if name == nil or type == nil then
      error("then tbl bad format")
   end
   --table.insert(value,tblk[name])
   tmp_set[#tmp_set+1]=tblk[name]
   return tmp_set
end


function table_func(tblk,tbl,value_set)
   local tmp_set = value_set
   local type = get_type(tbl)
   local name = get_name(tbl)
   for k,v in pairs(tbl) do
      if k ~= "name" and k ~= "type" and k ~= "items" and k ~= "size" then
         tmp_set = table_func(tblk,v,tmp_set)
      elseif k == "items" then
         if type == "arraystruct" then
            arraysize = 0
            size = get_size(tbl)
            tmp_set = array_table_func(tblk[name],size,v,tmp_set)
         elseif type == "struct" then
            tmp_set = table_func(tblk[name],v,tmp_set)
         else
            error("the tbl bad format")
         end
      elseif name ~= nil and type ~= nil and is_entry(type) and k == "name" then
         tmp_set = entry_func(tblk,tbl,tmp_set)
      end
   end
   return tmp_set
end

function packX(tblk,endian,tbl)
   local fmt = ""
   if type(tblk) == "table" and type(tbl) == "table" then
      if not format_cache[tbl] or format_cache[tbl]== "" then
         if string.lower(endian) == "little" then
            fmt = fmt .. "<"
         elseif string.lower(endian) == "big" then
            fmt = fmt .. ">"
         else
            error("the endian must be little or big")
         end
         local format_str = build_format_by_des(tbl,fmt)
         format_cache[tbl] = format_str
      end
      fmt = format_cache[tbl]
      local value_set = {}
      local value_res = table_func(tblk,tbl,value_set)
      return strop.rtrim(struct.pack(fmt,unpack(value_res)))
   else
      error("the pack :tblk and tbl is not table")
   end
end

function build_array_table(tbl,des,size,rtbl,idx)
   local index = idx
   for j = 1 ,size do
      rtbl[j] = {}
      for k,v in pairs(des) do
         index = build_table(tbl,v,rtbl[j],index)
      end
   end
   return index
end

function build_entry(tbl,des,rtbl,idx)
   local index = idx
   local name = get_name(des)
   local type = get_type(des)
   if name == nil or type == nil then
      error("then des bad format")
   end
   rtbl[name] = tbl[index]
   index = index + 1
   return index
end


function build_table(tbl,des,rtbl,idx)
   local index = idx
   local name = get_name(des)
   local type = get_type(des)
   for k,v in pairs(des) do
      if k ~= "name" and k ~= "type" and k ~= "items" and k ~= "size" then
         index = build_table(tbl,v,rtbl,index)
      elseif k == "items" then
         if type == "arraystruct" then
            rtbl[name] = {}
            size = get_size(des)
            index = build_array_table(tbl,v,size,rtbl[name],index)
         elseif type == "struct" then
            rtbl[name] = {}
            index = build_table(tbl,v,rtbl[name],index)
         else
            error("the tbl bad format")
         end
      elseif name ~= nil and type ~= nil and is_entry(type) and k == "name" then
         index = build_entry(tbl,des,rtbl,index)
      end
   end
   return index
end

function unpackX(src,endian,des)
   local fmt = ""
   if type(des) == "table" then
      if not format_cache[des] or format_cache[des]== "" then
         if string.lower(endian) == "little" then
            fmt = fmt .. "<"
         elseif string.lower(endian) == "big" then
            fmt = fmt .. ">"
         else
            error("the endian must be little or big")
         end
         local format_str = build_format_by_des(des,fmt)
         format_cache[des] = format_str
      end
      fmt = format_cache[des]
      local rt = {}
      local tbl = {struct.unpack(fmt,src)}
      local idx=1
      build_table(tbl,des,rt,idx)
      tbl[#tbl] = nil
      return rt
   else
      error("the des must be table")
   end
end

function foreach_array(des,size,format_str)
   local format_tmp = format_str
   for k=1,size do
      format_tmp = build_format_by_des(des,format_tmp)
   end
   return format_tmp
end

function format_entry(des,format_str)
   local format_tmp = format_str
   local type = get_type(des)
   format_tmp = build_format(type,format_str);
   if type == "string" then
      format_tmp = format_tmp .. get_size(des)
   end
   return format_tmp
end

function build_format_by_des(des,format_str)
   local format_tmp = format_str
   local type = get_type(des)
   local name = get_name(des)
   for k,v in pairs(des) do
      if k ~= "name" and k ~= "type" and k ~= "items" and k ~= "size" then
         format_tmp = build_format_by_des(v,format_tmp)
      elseif k == "items" then
         if type == "arraystruct" then
            size = get_size(des)
            format_tmp = foreach_array(v,size,format_tmp)
         elseif type == "struct" then
            format_tmp = build_format_by_des(v,format_tmp)
         else
            error("the tbl bad format")
         end
      elseif name ~= nil and type ~= nil and is_entry(type) and k == "name" then
         format_tmp = format_entry(des,format_tmp)
      end
   end
   return format_tmp
end


