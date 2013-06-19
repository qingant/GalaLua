package.path = package.path .. ";" .. "."

print(package.path)

local list = require("list")
require "pprint"

pprint.pprint(list, "list")
print(type(list))

lst = list.list:new()
lst:append("Hello")
lst:insert(2, "luo")
lst:insert(2, "luo")
lst:insert(2, "luo")
lst:insert(2, "luo")

print(lst:index("luo"))
print(lst:count("luo"))
print(lst:pop())
print(lst:count("luo"))
lst:remove("luo")
print(lst:count("luo"))

print(lst:len())
lst:extend({"zhi",10, "hui", "shuai", "ge"})
print(lst:len())

for index, value in ipairs(lst) do
   print( index .. " " .. value )
end

-- lst:sort(lst)
lst:reverse(lst)

for index, value in ipairs(lst) do
   print( index .. " " .. value )
end

lst:delete(lst:len())

print("------------------------------")
for index, value in ipairs(lst) do
   print( index .. " " .. value )
end

local ll = lst:slice(2,4)
print("------------------------------")
for index, value in ipairs(ll) do
   print( index .. " " .. value )
end


