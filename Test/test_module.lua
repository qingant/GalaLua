print(_VERSION)
local pprint=require "pprint"
local mods={"cjson","amq","xml","platform","_dir","osext","hexdump",
            "struct","codec","lfs","lightningmdb","linenoise","md5",
            "osdatetime","sigar","snappy"}

function pmodule(m)
    local mod=require(m)

    pprint.pprint(mod,m)
end


for i,m in ipairs(mods) do
    pmodule(m)
end
