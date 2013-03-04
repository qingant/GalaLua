local os = require "os"
local io = require "io"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"


function main(mod, entry)
   local module = requre(mod)
   module[entry]()
end