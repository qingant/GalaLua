--*-- coding: utf-8 --*--
-- test2.lua --

print("test2.lua, blah blah")

print("-----------------package.loaded begin-----------------")
for k, v in pairs(package.loaded) do
    print(k, v)
end
print("------------------package.loaded end------------------")

if package.loaders then
    print("-----------------package.loaders begin-----------------")
    for k,v in ipairs(package.loaders) do
        print(k, v)
    end
    print("------------------package.loaders end------------------")
end

local alias = require("auxiliary")
print("alias.main()")
alias.main()
print("auxiliary.main()")
auxiliary.main()

local main = require("main")
print("main.main()")
main.main()
print("main.helloworld()")
main.helloworld()
status, retval = pcall(main.helloworld)
print("status = ", status, ", retval = '", retval, "'")

print("-----------------package.loaded begin-----------------")
for k, v in pairs(package.loaded) do
    print(k, v)
end
print("------------------package.loaded end------------------")

