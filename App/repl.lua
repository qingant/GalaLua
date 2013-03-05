local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

function main()
 
   local host, port = glr.node_addr()
   print(string.format("GLR Repl at (%s::%d)", glr.node_addr()))
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
