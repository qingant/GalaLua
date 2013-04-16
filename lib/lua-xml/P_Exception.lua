#! /usr/bin/env lua
--*-- coding: utf-8 --*--

module("P_Exception", package.seeall)

function throw(except)
    error(except, 2)
end

function try(func_throw, func_catch)
    local ret = {pcall(func_throw)}
    if ret[1] then
        return unpack(ret)
    elseif func_catch then
        ret = func_catch(ret[2])
        if ret then
            throw(ret)
        else
            return nil
        end
    elseif ret[2] then
        throw(ret[2])
    end
end

-- 捕获所有异常
function catch(except, func_catch)
    local func_catch = func_catch or nil
    try(function ()
        if func_catch then
            local ret = func_catch(except)
            if ret then
                io.write(string.format("exception: %s\n", tostring(ret)))
            end
        else
            io.write(string.format("exception: %s\n", tostring(except)))
        end
        return nil
    end, function (excpt) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception: %s\n", tostring(excpt)))
        return nil
        -- 异常处理过程（结束）
    end)
    return nil
end

--[[
function ()
    local ret = {try(function ()
    -- 该函数的函数体（开始）
    -- 应用的代码......

    -- 该函数的函数体（结束）
    -- 捕获异常并进行处理（开始）
    end, function (except)
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）

        return nil
        -- 异常处理过程（结束）
    -- 捕获异常并进行处理（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕获嵌套函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        catch(ret[2]) -- 捕获所有异常
        throw(ret[2]) -- 再次抛出异常
        return nil
    else
        return nil
    end
end
--]]

--[=[
function helloworld(a, b, c)
    local ret = {try(function ()
    -- 该函数的函数体（开始）
        local except = {["typename"] = "hello world", ["message"] = {1, 2}}
        if _VERSION == "Lua 5.2" then
            local metatable = {__tostring = function (except) return except["typename"] end}
            setmetatable(except, metatable)
        end
        local sum = 0
        for i = 1, 100 do
            sum = sum + i
        end
        return sum
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        local ret = debug.getinfo(2, "S")
        local info = string.format("information:\n\tfile:%s\n\tfunction:%s\n\ttype:%s\n\tbegin :%d\n\tend:%d", ret.source, ret.short_src, ret.what, ret.linedefined, ret.lastlinedefined)
        local trace = debug.traceback()
        io.write(string.format("%s\n%s\n", info, trace))
        -- 处理所抛出的所有异常
        if type(except) == "table" and except.typename == "hello world" then
            io.write(string.format("%s, %s, %s\n", except["typename"], except["message"][1], except["message"][2]))
        else
            print(except)
        end
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕获嵌套函数产生异常，或该函数的异常处理模块未能处理的异常，也可以再次抛出异常。此处可捕获所有的异常
        -- 异常处理过程（开始）
        catch(ret[2]) -- 捕获所有异常
        throw(ret[2]) -- 再次抛出异常
        return nil
        -- 异常处理过程（结束）
    else
        return nil
    end
end

local ret = helloworld()
io.write(string.format("%s\n", tostring(ret)))

--]=]
