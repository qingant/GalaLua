--[[
 gar loader
]]

module(...,package.seeall)

function run_gar(gar)
    local zip=require "minizip"
    local xml=require "xml"

    local Manifest="Manifest"

    function read_file(zipfd,file)
        local f=zipfd:open(file)
        if f then
            local size=zipfd:stat(file).size
            return f:read(size)
        end
    end
    
    --get Path tag in manifest.
    function get_gpath(str)
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
        local gar=gar
        local zipfd=zip.open(gar)
        if zipfd then   --if open success
            local gpath=get_gpath(read_file(zipfd,Manifest))

            local real_mod=string.gsub(modulename,"%.","/")

            for path in string.gmatch(gpath, "([^;]+)") do
                local file=string.gsub(path,"%?",real_mod)
                local f=zipfd:open(file)
                if f then
                    return assert(loadstring(f:read(zipfd:stat(file).size)))
                end
            end
        end

        return nil
    end

    table.insert(package.loaders,2,loader)

end

