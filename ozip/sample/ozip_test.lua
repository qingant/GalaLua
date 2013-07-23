package.cpath="../?.so" .. ";" .. package.cpath

local ozip = require("_ozip")

local file_name = "1.zip"

local Z_DEFLATED  				= 8
local Z_DEFAULT_COMPRESSION 	= -1
local Z_MAX_WBITS				= 15
local Z_DEFAULT_STRATEGY		= 0
local DEF_MEM_LEVEL = 8

local default_add_options = {
	date = nil,                           --table in os.date() format (if missing, dosDate will be used)
	dosDate = 0,                          --date in DOS format
	internal_fa = 0,                      --2 bytes bitfield. format depends on versionMadeBy.
	external_fa = 0,                      --4 bytes bitfield. format depends on versionMadeBy.
	local_extra = nil,                    --cdata or string
	local_extra_size = nil,
	file_extra = nil,                     --cdata or string
	file_extra_size = nil,
	comment = nil,                        --string or char*
	method = Z_DEFLATED,                  --0 = store
	level = Z_DEFAULT_COMPRESSION,        --0..9
	raw = false,                          --write raw data
	windowBits = -Z_MAX_WBITS,            -- -8..-15
	memLevel = 8,                         --1..9 (1 = min. speed, min. memory; 9 = max. speed, max. memory)
	strategy = Z_DEFAULT_STRATEGY,        --see zlib_h.lua
	password = nil,                       --encrypt file with a password
	crc = 0,                              --number; needed if a password is set
	versionMadeBy = 0,                    --version of the zip standard to use. look at section 4.4.2 of the standard.
	flagBase = 0,                         --2 byte "general purpose bit flag" (except the first 3 bits)
	zip64 = false,                        --enable support for files larger than 4G
	
	filename="test",  --需要考虑filename是否放在这里
}


local ggg = ozip.fs_zipOpen(file_name, 0)
--error()
print(" \n -> " .. type(ggg) .. " <-\n ")

-- TODO: more paramater let assert paramater failure
ozip.fs_zipOpenFile(ggg, default_add_options)
--error()
local ctx="Hello ozip, ZhiHui Luo test now"
ozip.fs_zipWriteFile(ggg, ctx)
--error()
ozip.fs_zipCloseFile(ggg)
--error()
ozip.fs_zipClose(ggg, "Test minizip close")
--error()


local zf = ozip.fs_unzOpen(file_name)
print(type(zf) .. "fs_unzOpen Return type\n")
ozip.fs_unzLocateFile(zf, "readme.txt", 1)
info = ozip.fs_unzGetCurrentFileInfo(zf, 256, 256, 256)
for key, val in ipairs(info) do 
   print(key, val)
end
ozip.fs_unzOpenCurrentFile(zf, 0, "lll")
local iret = 1;
while iret > 0 do 
   iret, ctx = ozip.fs_unzReadCurrentFile(zf, 80)
   if iret > 0 then
      print(iret, ctx)
   end

end
ozip.fs_unzCloseCurrentFile(zf)
ozip.fs_unzClose(zf)

print(ozip.OK)

