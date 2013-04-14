-- zhihui.luo
-- camel.flying@gmail.com
-- gar package tools


--local zip = require ("minizip")
module(..., package.seeall)

local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"
local dir = require "dir"
local pprint = require "pprint"
local zip = require("minizip")
local cjson = require "cjson"
-- pprint.pprint(dir, "dir")
-- pprint.pprint(dir.dir("/home/dev/lib/"))
function walk(path, func)
   for _,p in pairs(dir.dir(path)) do
      func(path, p)
      local dir_path = path .. p 
      if dir.dir_type(dir_path) == "dir" and p ~= "." and p ~= ".." then
         walk(dir_path .. "/", func)
      end
   end
end

function gar_menifest_gen(path)
   local mf = {}
   mf.Define = {}
   mf.Define.Topics = {}
   for _, top in pairs(dir.dir(path)) do
      local dir_path = path .. top
      if dir.dir_type(dir_path) == "dir" and top ~= "." and top ~= ".." then
         mf.Define.Topics[#mf.Define.Topics + 1] = {
            Name = top;
            Events = {};
         }
         local current  =  mf.Define.Topics[#mf.Define.Topics].Events
         
         for _, event in pairs(dir.dir(dir_path .. "/server/")) do
            local event_path = dir_path .. "/server/" .. event
            if dir.dir_type(event_path) == "reg" and string.sub(event, -3) == "lua" and event ~= "." and event ~= ".." then
               current[#current + 1] = {Name = string.sub(event, 0, -5)}
            end
         end
      end
   end
   pprint.pprint(mf)
   return mf
end
function gar_packer(path, name, version)
   local function packer(domain)
      local fname = name .. "-" .. domain .. "-" .. version .. ".gar"
      os.execute(string.format("rm -f \"%s\"", fname))
      local zf = assert(zip.open(fname, zip.OR(zip.CREATE, zip.CHECKCONS)))
      zf:add_dir(name)
      zf:add("Menifest.json", "string", cjson.encode(gar_menifest_gen(path)))
      for _, top in pairs(dir.dir(path)) do
         local dir_path = path .. top
         if dir.dir_type(dir_path) == "dir" and top ~= "." and top ~= ".." then
            zf:add_dir(name .. "/" .. top)
            for _, event in pairs(dir.dir(dir_path .. "/" .. domain .. "/")) do
               local event_path = dir_path .. "/" .. domain .. "/" .. event
               if dir.dir_type(event_path) == "reg" and string.sub(event, -3) == "lua" then
                  zf:add(name .. "/" .. top .. "/" .. event, "file", dir_path .. "/" .. domain .. "/" .. event)
               end
            end
         end
      end
      zf:close()
   end
   packer("server")
   packer("agent")
end

function main()
   --gar_menifest_gen("/home/dev/workspace/trade/sys/")
   gar_packer("/home/dev/workspace/trade/sys/", "sys", "0.1")
   print(glr.time.now())
end



