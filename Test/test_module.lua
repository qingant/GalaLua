print(_VERSION)
local pprint=require "pprint"
local cjson=require "cjson"
local amq=require "amq"
local xml=require "xml"
local plat=require "platform"
local _dir=require "_dir"
local osext=require "osext"
local hex=require "hexdump"

local struct=require "struct"
local codec=require "codec"
local lfs=require "lfs"
local lightningmdb=require "lightningmdb"
local linenoise=require "linenoise"
local md5=require "md5"
local minizip=require "minizip"
local osdatetime=require "osdatetime"
local sigar=require "sigar"

local mods={"cjson","amq","xml","platform","_dir","osext","hexdump","struct","codec","lfs","lightningmdb","linenoise","md5","osdatetime","sigar"}

function pmodule(m)
    local mod=require(m)

    pprint.pprint(mod,m)
end


for i,m in ipairs(mods) do
    pmodule(m)
end
