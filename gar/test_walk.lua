-- zhihui.luo
-- camel.flying@gmail.com
-- test walk

-- local walk = require("walk")
local walk = require "walk"

print("the type of walk " .. type(walk))
print("the type of search_dir_first " .. type(search_dir_first))
  
function traverse_dir(dirpath)  
    print("search: " .. dirpath)  
    local bok, dir = search_dir_first(dirpath)
    print("search dir first   " .. type(bok) .. "     ".. type(dir))
    if bok then  
        while true do  
            local b, filename, filetype = search_dir_next(dir)  
            print(filename)
            if b then  
                local fullpath = dirpath .. "/" .. filename  
                if filetype == PQFT_DIR then  
                    if filename == "." then  
                    elseif filename == ".." then  
                    else traverse_dir(fullpath)  
                    end  
                else  
                    print(fullpath)  
                end   
            else  
                break         
            end  
        end  
        search_dir_finish(dir)  
    end  
end  
  
traverse_dir("/home/aim/lib") 