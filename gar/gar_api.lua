-- zhihui.luo
-- camel.flying@gmail.com
-- gar API

package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so;"

local zip = require ("minizip")
require ("string")
require ("walk")
--参数处理
--遍历目录同时生成对应的gar包
function traverse_dir(dirpath, package_name)  
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
                       traverse_dir(fullpath, package_name)  
                    end  
                else  
                   local i, j = string.find(fullpath, ".lua")
                   if j == #fullpath  then
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

--
--
-- create_gala_gar_file
function create_gala_gar_file(source_path) 
end

-- work_path : 如果你要处理的目录是：/home/ocaml/github/Gala/trunk/GalaLua/App/trade/sys/cpu/server/...
-- 与gar包有关的目录是：sys/cpu/server/... 调用参数如下：
-- create_file_gar("/home/ocaml/github/Gala/trunk/GalaLua/App/trade/", "sys/cpu/server")
-- 处理逻辑：server一下的目录自动忽略，文件放到server目录下。
-- 并且输出一个名为“sys_cpu_server.gar”的gar包。 
function create_file_gar(work_path, src_path)
   local package_name =  string.gsub(source_path, "/", "_")
   traverse_dir("/home/aim/GalaLua/App/trade/", package_name) 
end

function create_str_gar()
end


