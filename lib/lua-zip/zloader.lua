-- zhihui.luo
-- camel.flying@gmail.com
-- lua load and run from zip file
-- If l.lua in a.zip, It can be test with below cmd.
-- luajit -l zloader -e require(\"l\")

local table,string,os,package = table,string,os,package
local pairs,ipairs,print,loadstring,assert = pairs,ipairs,print,loadstring,assert

package.cpath=package.cpath .. ";" .. "/home/aim/GalaLua/lib/lua-zip/?.so"


local zip = require("minizip")
require "print_r"

module(..., package.seeall)

local htab = {}	 --zip handle
local ftab = {}	 --file list
local ztab = {}     --zip file with pattern

local function mklibs()
   package.zpath = package.zpath or os.getenv('LUA_ZPATH') or ''
   for path,pattern in string.gmatch(package.zpath..";", "([^;]*zip)[\\/](.-);") do	--split at ;
      if not htab[path] then
         local z,err = zip.open(path)
         print("Hello ")
         print_r.print_r(z)
         if z then
            htab[path] = z
            ftab[path] = {}
            for fn in z:files () do
               ftab[path][fn.filename] = path
            end
         end
      end
      
      local t = ztab[path] or {} -- 创建对应的目录结构
      if not t[pattern] then 
         t[pattern] = true 
      end
      ztab[path] = t
   end
   return ztab
end

function zloader(modulename)
   local ztab = mklibs()
   
   --load module
   print("--------zloader seach module name" .. modulename .. "--------")
   print_r.print_r(ztab)
   for path,v in pairs(ztab) do
      for pat,_ in pairs(v) do
         pat = string.gsub(pat, "%?", (string.gsub(modulename, "%.", "/")))         
         if ftab[path] and ftab[path][pat] then
            local z = htab[path]
            local zf = z:open(pat)
            if zf then
               local s = zf:read('*a')
               zf:close()
               return assert(loadstring(s))
            end
         end
      end
   end
end

-- Install the loader so that it's called just before the normal Lua loader
table.insert(package.loaders, 2, zloader)

--for v in pairs(package.loaders) do
--   print(package.loaders)
--end

print_r.print_r(package.loaders)


print("start")
--local zip = require("os")
--require "zip"
--zloader("test")
--require "os"
--assert(require("ssss"))
--file="/TR/2002/REC-xhtml1-20020801/xhtml1.pdf"
--local list = test.ssss.Split(file,"/")
--for key,var in pairs(list) do
--   print(var)
--end
-- 
--print(list[#list])

print("stop")