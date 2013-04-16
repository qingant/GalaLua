

--local fullpath = "adslf/gg"
--local i, j = string.find(fullpath, ".lua")
--if j == #fullpath  then
--   print(fullpath)
--end

-- test replace
require("string")
local source_path = "sys/spu/agent"
local  package_name = string.gsub(source_path, "/", "_")
print(package_name)
