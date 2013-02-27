
local node = require "node"
local struct = require "struct"
local pprint = require "pprint"
local cjson = require "cjson"

function repr(obj)
  local tobj = type(obj)
  local s = ""
  if tobj == "string" then
    s = string.format("%q", obj)
  elseif tobj == "number" or tobj == "boolean" then
    s = tostring(obj)
  elseif tobj == "nil" then
    s = "nil"
  elseif tobj == "table" then
    s = s  .. "{"
    for k,v in pairs(obj) do
      s = s .. "[" .. repr(k) .. "] = " .. repr(v) .. ", "
    end
    s = s  .. "}"
  else
    error("can't repr this object.")
  end
  return s  
end

function main()
   glr.spawn("lsr.lua","worker")
   local err, all = glr.all()
   print(pprint.pprint(all))
   local err, sta = glr.status(glr.id)
   print(pprint.pprint(sta))
   local err, sta = glr.status(all[1])
   print(pprint.pprint(sta))
   while true do
      local status, msg = glr.recv()
      print("MSG")
      print(msg)
      if msg then
         print(pprint.pprint(msg))
         error("normal")
      end
      print(msg)
   end
end




function worker()
   while true do
      local status,msg = glr.recv()
      print(msg)
   end
end