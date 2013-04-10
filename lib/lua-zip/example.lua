-- lua-zip的使用例子
-- zhihui.luo
-- camel.flying@gmail.com

local zip = require("minizip")

local test_zip_file = "lzh.zip"

function test_open_close()
   local archive = assert(zip.open("./test.zip", zip.CREATE))
   archive:close()

   local err = select(2, zip.open("test_opencreate_close.zip"))

   --archive = assert(zip.open(test_zip_file, zip.CREATE))
   --archive:close()

   do
      local ar = assert(zip.open("test_open_close.zip",
                                 zip.OR(zip.CREATE,
                                        zip.EXCL,
                                        zip.CHECKCONS)))
      -- 测试异常情况下gc回收ar对象。
      --ar:close()
   end
   print("we need gc here")
   collectgarbage"collect"
end

function test_file_count()
   local zf = assert(zip.open("test.zip"))

   --for file in zf:get_num_files() do
   --   print(file.name)
   --end
   print("test.zip files number = " .. zf:get_num_files())
end

function test_get_name()
    local zf = assert(zip.open("test.zip"))
    local name = nil
    -- 获取zip文件中的文件列表
    for index=zf:get_num_files() , 1, -1 do
       name = zf:get_name(1, zip.FL_UNCHANGED)
       print(name)
    end

    zf:close()
end

-- 测试主函数
function main()
   test_open_close()
   test_file_count()
   test_get_name()
end

-- 测试入口
main()