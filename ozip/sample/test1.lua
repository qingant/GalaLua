package.path="?.lua" .. ";" .. package.path

local ozip = require("ozip")
local print_r = require("print_r")

local _ar = ozip.open("/home/ocaml/Downloads/unzip11/1.zip", "w")
local tt = ozip.deepcopy(ozip.default_add_options)
tt.filename="luozhihui"
--_ar:openFile(tt)
_ar:addFile("ggg.txt")
local ctx="Hello ozip, ZhiHui Luo test now LLLLLLLLLLLLLLLLLLLLLSSSDDDDDDDDDDDDDDDDDDDDDDDD\0 a;ddddddddddddddddddddddddddddd"
_ar:writeFile(ctx)
_ar:closeFile()
_ar:close("Test  ozip close")

local zf = ozip.open("/home/ocaml/Downloads/unzip11/1.zip")
zf:locateFile("luozhihui", 1)
info = zf:info(256, 256, 256)
--
--print("--------------------------->")
--for key, val in pairs(info) do 
--   print(key, val)
--end
--print("--------------------------->")

zf:openFile(0, "lll")
local iret = 1;

while iret > 0 do 
   iret, ctx = zf:readFile(80)
   if iret > 0 then
      print(iret, ctx)
   end
end

zf:closeFile()


--- print all info
for info in zf:files() do
	print("--------------------------->")
	for key, val in pairs(info) do
		print(key, val)
	end
	print("--------------------------->")
end

zf:close()


