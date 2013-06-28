--*-- coding: utf-8 --*--
-- main.lua --

module(..., package.seeall)

function main()
    print("main.lua: main()")
    return "this string is main() in main.lua returned"
end

function helloworld()
    print("main.lua: helloworld()")
    return "this string is helloworld() in main.lua returned"
end

