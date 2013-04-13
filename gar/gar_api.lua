-- zhihui.luo
-- camel.flying@gmail.com
-- gar API


--local zip = require ("minizip")
require ("walk")
--参数处理
--遍历目录同时生成对应的gar包

local gar_name=""
local gar_root=""

function traverse_dir(dirpath)  
    local bok, dir = search_dir_first(dirpath)
    if bok then  
        while true do  
            local b, filename, filetype = search_dir_next(dir)  
            if b then  
                local fullpath = dirpath .. "/" .. filename  
                if filetype == PQFT_DIR then  
                    if filename == "." then  
                    elseif filename == ".." then  
                    else 
                       if filename != "server" then 
                          gar_root = gar_root + filename
                       end
                       gar_name = gar_name + filename
                       traverse_dir(fullpath)  
                    end  
                else  
                   local i, j = string.find(fullpath, ".lua")
                   if j == #fullpath  then
                      --local ii, jj = string.find(fullpath, "")
                      print(dirpath)
                      print(filename)
                      -- 处理成对应的zip中的路径
                   end
                end   
            else  
                break         
            end  
        end  
        search_dir_finish(dir)  
    end  
end  

traverse_dir("/home/aim/GalaLua/App/trade") 

function create_gala_gar_file(source_path)
   
end

function create_file_gar(gar_name, source_path)
end

function create_str_gar()
end


