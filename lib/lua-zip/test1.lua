-- zhihui.luo
-- camel.flying@gmail.com
-- test lua load and run from zip file

local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so;" .. "/usr/local/lib/lua/5.1/?.so;" .. "/usr/local/lib"

-- 设置zloader模块的加载路径
package.path =  "/home/aim/dev/lua-zip/?.lua" .. ";" .. package.path 
-- 安装zip的loader
assert(require "zloader")

-- 设置zip文件的加载路径
package.zpath="/home/ocaml/lib/?.zip;/home/ocaml/workspace/HelloLua/src/test.zip/?.lua;/home/aim/dev/lua-zip/test.zip/?.lua"

-- 测试从zip loader进来的lua脚本
assert(require "ssss")


file="/TR/2002/REC-xhtml1-20020801/xhtml1.pdf"
local list = ssss.Split(file,"/")
for key,var in pairs(list) do
   print(var)
end
