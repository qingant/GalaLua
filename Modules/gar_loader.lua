--[[
gar loader
]]

module(...,package.seeall)


local zip=require "minizip"
local xml=require "xml"

local Manifest="Manifest"

local function read_file(zipfd,file)
    local f=zipfd:open(file)
    if f then
        local size=zipfd:stat(file).size
        local s= f:read(size)
        f:close()
        return  s
    end
end

--get Path tag in manifest.
local function get_gpath(str)
    local path="?.lua"
    local exec="Executable"   --default is runnable
    if str then
        local doc=xml.cxml_reader(str,#str)
        local root=doc:document()
        for i,elm in pairs(root:sub_elements()) do
            local k,v=elm:key(),elm:value()
            if k=="Path" then
                path=v
            elseif k=="Catagory" then
                exec=v
            end
        end
    end

    if exec~="Executable" then
        error("not a excutable gar package")
    end

    return path
end

-- gar loader
function loader(modulename)
    if package.gpath==nil then
        return nil
    end
    for gar in string.gmatch(package.gpath, "[^;]+%.gar") do
        local zipfd=zip.open(gar)
        if zipfd then   --if open success
            local gpath=get_gpath(read_file(zipfd,Manifest))

            local real_mod=string.gsub(modulename,"%.","/")

            for path in string.gmatch(gpath, "([^;]+)") do
                local file=string.gsub(path,"%?",real_mod)
                local s=read_file(zipfd, file)
                if s then
                    zipfd:close()
                    return assert(load(s,string.format("@%s:%s",gar,file)))
                end
            end
        end
    end

    return nil
end

table.insert(package.loaders,2,loader)

