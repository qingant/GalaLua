module(..., package.seeall)

local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

function main()
 
   local host, port = glr.sys.host,glr.sys.port
   print(string.format("GLR Repl at (%s::%d)", host, port))
   while true do
      -- err,msg = glr.recv()
      -- print(msg)
      local io = require "io"
      io.stdout:write("\n=> ")
      io.stdout:flush()
      -- io.stdin:seek("set",0)
      local cmd = io.stdin:read("*l")
      print("-> return " .. cmd)
      local status, fun = pcall(function () return loadstring("return "..cmd) end)
      if status then
         local status, rt = pcall(fun)
         io.stdout:write(tostring(rt).."     ::(" .. type(rt) .. ")")
      else
         io.stdout:write(tostring(fun))
      end
      io.flush()
   end

end
