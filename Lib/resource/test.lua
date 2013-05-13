local pack = require("hello_lua")
print("test_lua, blah blah")

hello_lua.helloworld()
pack.helloworld()

pack.main()

print("---------pairs(package.loaded)--------")
for k, v in pairs(package.loaded) do
    print(k, v)
end

print("---------ipairs(package.loaded)--------")
for k, v in ipairs(package.loaded) do
    print(k, v)
end

if package.loaders then
    print("---------pairs(package.loaders)--------")
    for k, v in ipairs(package.loaders) do
        print(k, v)
    end
end
