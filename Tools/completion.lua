
module(...,package.seeall)
local path=require "path"


--[[
--return a table of path start with @s
--@s:
--@prefix:
--@filter: for `ls`
--complete path when press tab
--]]
function path_completion(s,prefix,filter)
    local dirname,basename=path.split(s)
    local ret={}
    local filter=filter or  ".*"
    local l
    if s:match(".*/$") then
        l=path.ls(s,filter)
        basename=""
        dirname=s
    else
        l=path.ls(dirname,filter)
    end

    if dirname=="." then 
        dirname="" 
    end
    for i in pairs(l) do
        local _dirname,_basename=path.split(i)
        if string.match(_basename,"^"..basename) then
            ret[#ret+1]=prefix..path.join(dirname,i)
        end
    end
    return ret
end

