-- lua-zip的使用例子
-- zhihui.luo
-- camel.flying@gmail.com

local zip = require("minizip")
require("os")

local zf_name="lzh.zip"

-- 主函数
function main()
   os.execute("rm -f " .. zf_name)

   -- 创建一个zip文件
   local zf = assert(zip.open(zf_name, zip.CREATE))

   print("example go ...")

   -- 增加文件到zip文件中
   --zf.add("test_minizip.lua", "file", _0, 0, -1)
   zf:add("dir/add.txt", "string", "Contents")
   zf:close()

   zf = assert(zip.open(zf_name, zip.CHECKCONS))
   -- 操作文件对象
  -- local file = zf.open("add.TXT", zip.OR(zip.FL_NOCASE, zip.FL_NODIR))
   local ctx = assert(file:read(256))
   print(ctx)
   file:close()

   -- 关闭一个代开的zip文件
   zf:close()
end

-- 测试入口
main()