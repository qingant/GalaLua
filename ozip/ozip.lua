--
--
--
package.cpath="/home/ocaml/Downloads/unzip11/?.so" .. ";" .. package.cpath

local ozip = require("_ozip")
local string = require("string")

-- the error central
local errors = {
	
}

local function checknil(handle)
	if handle ~= nil then
		return handle
	end
	error('ozip error')
end

local function check_function(check, ret)
	return function (ret)
		if check(ret) then
			return ret
		end
		error(string.format("ozip error %d: %s\n", ret, errors[ret or 'unknow error']))
	end
end

local checkz   = check_function(function(ret) return ret == 0 end)
local checkpoz = check_function(function(ret) return ret >= 0 end)
local checkeof = check_function(function(ret) return ret == 0 or ret == C.UNZ_END_OF_LIST_OF_FILE end)

-- some const value from c
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
	filename="test",      --需要考虑filename是否放在这里
}

local function deepcopy(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end  -- if
        local new_table = {}
        lookup_table[object] = new_table
        for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)
        end  -- for
        return setmetatable(new_table, getmetatable(object))
    end  -- function _copy
    return _copy(object)
end  -- function deepcopy

-- file system zip api
local fs_zip={}
function fs_zip.new( self, fname, mode )
	local obj = {}
	setmetatable( obj, self )
	self.__index = self
	self._archive = ozip.fs_zipOpen( fname, 0 )
	
	return obj
end

function fs_zip.close(self, global_comment)
	ozip.fs_zipClose( self._archive, global_comment)
end

function fs_zip.openFile(self, file_options)
	ozip.fs_zipOpenFile(self._archive, file_options)
end

function fs_zip.writeFile(self, ctx)
	ozip.fs_zipWriteFile(self._archive, ctx)
end

function fs_zip.closeFile(self)
	ozip.fs_zipCloseFile(self._archive)
end

function fs_zip.closeFileRaw(self, uncomp_size, crc32)
	ozip.fs_zipCloseFileRaw(self._archive, uncomp_size, crc32)
end

function fs_zip.removeExtraInfoBlock(self, data, size_header)
	ozip.fs_zipRemoveExtraInfoBlock(self._archive)
end

-- high level api
function fs_zip.addFile(self, info)
	local _info={}
	if type(info) == 'string' then
		_info=deepcopy(default_add_options)
		_info.filename=info
	else
		_info=deepcopy(info)
	end
	
	assert(_info.filename, "Add  file No name")
	fs_zip.openFile(self, _info)
end

-- file system zip api
local fs_unz={}
function fs_unz.new( self, fname, mode )
	local obj = {}
	setmetatable( obj, self )
	self.__index = self
	self._archive = ozip.fs_unzOpen( fname )
	
	return obj
end
-- maybe a utility function
function fs_unz.stringFileNameCmp(self,fname1, fname2, icase)
	return ozip.fs_unzStringFileNameCompare(fname1, fname2, icase)
end

function fs_unz.close(self)
	return ozip.fs_unzClose(self._archive)
end

function fs_unz.getGlobalInfo(self)
	return ozip.fs_unzGetGlobalInfo(self._archive)
end

function fs_unz.getGlobalComment(self, size)
	return ozip.fs_unzGetGlobalComment(self._archive, size)
end

function fs_unz.goToFirstFile(self)
	return ozip.fs_unzGoToFirstFile(self._archive)
end

function fs_unz.goToNextFile(self)
	return ozip.fs_unzGoToNextFile(self._archive)
end

function fs_unz.locateFile(self, fname, icase)
	return ozip.fs_unzLocateFile(self._archive, fname, icase)
end

function fs_unz.getFilePos(self )
	return ozip.fs_unzGetFilePos(self._archive)
end

function fs_unz.goToFilePos(self, pos_in_dir, num_of_file )
	return ozip.fs_unzGoToFilePos(self._archive)
end

function fs_unz.info(self, fname_size, extra_size, comment_size )
	return ozip.fs_unzGetCurrentFileInfo(self._archive, fname_size, extra_size, comment_size)
end

function fs_unz.getCurrentFileZStreamPos(self )
	return ozip.fs_unzGetCurrentFileZStreamPos(self._archive)
end

function fs_unz.openFile(self, raw, password )
	return ozip.fs_unzOpenCurrentFile(self._archive, raw, password)
end

function fs_unz.closeFile(self)
	return ozip.fs_unzCloseCurrentFile(self._archive)
end

function fs_unz.readFile(self, size )
	return ozip.fs_unzReadCurrentFile(self._archive, size)
end

function fs_unz.tell(self)
	return ozip.fs_unztell(self._archive)
end

function fs_unz.eof(self )
	return ozip.fs_unzeof(self._archive)
end

function fs_unz.getLocalExtrafield(self)
	return ozip.fs_unzGetLocalExtrafield(self._archive)
end

function fs_unz.getOffset(self)
	return ozip.fs_unzGetOffset(self._archive)
end

function fs_unz.setOffset(self, offset )
	return ozip.fs_unzSetOffset64(self._archive, offset)
end

-- unz hight level api
function fs_unz.files(self)
	fs_unz.goToFirstFile(self)
	local _first = true
	return function()
		if _first then
			_first = false
		elseif 0 ==  fs_unz.goToNextFile(self) then
			return
		end
		return fs_unz.info(self, 225, 225, 225)
	end
end

local function open(filename, mode)	
	if not mode or mode == 'r' then
		-- return unz open archive
		return fs_unz:new(filename, mode)
	elseif mode == 'w' then
		-- return zip open archive
		return fs_zip:new(filename, mode)
	elseif mode == 'a' then
		-- return zip open archive with append
		return fs_zip:new(filename, mode)
	else
		error("invalid mode! mode  'r' 'w' 'a' expected\m" )
	end
end

return {
	open = open,
	default_add_options = default_add_options,
	deepcopy = deepcopy,
}