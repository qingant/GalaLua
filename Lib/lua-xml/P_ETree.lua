#! /usr/bin/env lua
--*-- coding: utf-8 --*--

local lom = require ("lxp.lom")
local etree = require ("etree")
local xpath = require ("xpath")
local exception = require ("P_Exception")

module (..., package.seeall)

__doc__ =
{
    ["文件名称"] = "P_Etree.lua",
    ["组件组名"] = "xml处理类组件",
    ["组件对象"] = "公共",
    ["组件级别"] = "平台级",
    ["作    者"] = "esuomyekcim@AM组",
    ["创建时间"] = "2013/3/11 11:15:13",
    ["创建地点"] = "赞同科技"
}

ElementTree = {}

ElementTree.__doc__ =
    {
        ["小分类"] = "ElementTree",
        ["内部"] = false,
        ["过时"] = false
    }

ElementTree.__doc__.fromstring = {
    ["组件名称"] = "ElementTree.fromstring",
    ["组件风格"] = "处理型",
    ["中文注释"] = "从字符串中导入xml格式的内容",
    ["入参"] = {
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "xml格式的字符串"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回Lua Object Model风格的table对象"
        }
    }
}
function ElementTree.fromstring(xmlString)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local xmlString = xmlString or nil
        if type(xmlString) == "string" then
            local element = etree.fromstring(xmlString)
            return ElementTree.lomobject_to_element(element)
        else
            return nil
        end
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.__doc__.tostring = {
    ["组件名称"] = "ElementTree.tostring",
    ["组件风格"] = "处理型",
    ["中文注释"] = "将Lua Object Model风格的table输出为xml格式的字符串",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "Lua Object Model风格的table"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回xml格式的字符串"
        }
    }
}
function ElementTree.tostring(element)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local element = element or nil
        if type(element) == "table" then
            if type(element.root) == "table" and type(element.root.tag) == "string" then
                return etree.tostring(element.root)
            elseif type(element.tag) == "string" then
                return etree.tostring(element)
            end
        end
        return nil
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.__doc__.parse = {
    ["组件名称"] = "ElementTree.parse",
    ["组件风格"] = "处理型",
    ["中文注释"] = "导入xml文件",
    ["入参"] = {
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "xml文件的路径"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "Lua Object Model风格的table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回Lua Object Model风格的table对象"
        }
    }
}
function ElementTree.parse(handle)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local element = lom.parse(handle)
        return ElementTree.lomobject_to_element(element)
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.__doc__.dump = {
    ["组件名称"] = "ElementTree.dump",
    ["组件风格"] = "处理型",
    ["中文注释"] = "将Lua Object Model风格的table对应的xml格式的内容输出到stdout",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "Lua Object Model风格的table"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "输出xml格式的内容到stdout"
        }
    }
}
function ElementTree.dump(element)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local element = element or nil
        if type(element) == "table" then
            if type(element.root) == "table" and type(element.root.tag) == "string" then
                io.write(etree.tostring(element.root), "\n")
            else
                io.write(etree.tostring(element, "\n"))
            end
        end
        return ""
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.__doc__.fromstringlist = {
    ["组件名称"] = "ElementTree.fromstringlist",
    ["组件风格"] = "处理型",
    ["中文注释"] = "从包含xml数据片段的字符串列表中导入xml格式的内容",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "包含xml数据片段的字符串列表"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回Lua Object Model风格的table"
        }
    }
}
function ElementTree.fromstringlist(sequence)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local sequence = sequence or nil
        if type(sequence) == "table" then
            local ret = etree.fromstring(table.concat(sequence))
            return ElementTree.lomobject_to_element(ret)
        end
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.__doc__.tostringlist = {
    ["组件名称"] = "ElementTree.tostringlist",
    ["组件风格"] = "处理型",
    ["中文注释"] = "从包含xml数据片段的字符串列表中导入xml格式的内容",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "包含xml数据片段的字符串列表"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回Lua Object Model风格的table"
        }
    }
}
function ElementTree.tostringlist(element)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local element = element or nil
        local stringlist = {}
        if type(element) == "table" and 
                ((type(element.tag) == "string") or (type(element.root) == "table" and type(element.root.tag) == "string")) then
            local ret = ElementTree.tostring(element)
            local begin_pos = 1
            local end_pos = 0
            repeat
                end_pos = string.find(ret, "\n", begin_pos)
                if end_pos then
                    stringlist[#stringlist + 1] = string.sub(ret, begin_pos, end_pos)
                    begin_pos = end_pos + 1
                end
            until not end_pos
            stringlist[#stringlist + 1] = string.sub(ret, begin_pos, -1)
        end
        return stringlist
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.__doc__.iselement = {
    ["组件名称"] = "ElementTree.iselement",
    ["组件风格"] = "处理型",
    ["中文注释"] = "判断一个对象是否是xml的element",
    ["入参"] = {
        {
            ["type"] = "variable",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "lua变量"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "如果给定的lua变量是xml的element，返回true；否则，返回false"
        }
    }
}
function ElementTree.iselement(element)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
    return ((type(element) == "table") and (type(element.root) == "table") and (type(element.root.tag) == "string")) and true or false
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.__doc__.SubElement = {
    ["组件名称"] = "ElementTree.SubElement",
    ["组件风格"] = "处理型",
    ["中文注释"] = "将一个xml结点作为子结点插入到另一个结点中",
    ["入参"] = {
        {
            ["type"] = "table",
            ["name"] = "parent",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "Lua Object Model风格的table，作为父结点"
        },
        {
            ["type"] = "string",
            ["name"] = "tag",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "子结点的标签名"
        },
        {
            ["type"] = "table",
            ["name"] = "attrib",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "子结点的属性名列表，如{['name'] = 'value'}"
        },
        {
            ["type"] = "string",
            ["name"] = "text",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "位于子结点的开始标签和结尾标签之间的文本内容"
        },
        {
            ["type"] = "string",
            ["name"] = "tail",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "位于子结点的结束标签之后的文本内容"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回Lua Object Model风格的父结点"
        }
    }
}
function ElementTree.SubElement(parent, tag, text, attrib, tail)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local tag = tag or ""
        local text = text or nil
        local attrib = attrib or {}
        local tail = tail or nil
        local subElement = ElementTree.Element:new(tag, text, attrib, tail)
        local parent = parent or nil
        if type(parent) == "table" then
            if type(parent.root) == "table" and type(parent.root.tag) == "string" then
                parent.root[#parent.root + 1] = subElement.root
            elseif type(parent.tag) == "string" then
                parent[#parent + 1] = subElement
            end
        else
            parent = subElement
        end
        return parent
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

function ElementTree.lomobject_to_element(lomobject)
    local lomobject = lomobject or nil
    if type(lomobject) == "table" then
        if type(lomobject.tag) == "string" then
            local element = ElementTree.Element:new("")
            element.root = lomobject
            return element
        elseif type(lomobject.root) == "table" and type(lomobject.root.tag) == "string" then
            return lomobject
        end
    end
    return nil
end

----------------------------------------

ElementTree.Element = {}

ElementTree.Element.__doc__ =
    {
        ["小分类"] = "ElementTree.Element",
        ["内部"] = false,
        ["过时"] = false
    }

function ElementTree.Element:new(tag, text, attrib, tail)
    local element = {}
    self.__index = self
    setmetatable(element, self)

    local tag = tag or "tag"
    element["root"] = {["tag"] = tag}
    local text = text or nil
    if text then
        element["root"][1] = text
    end
    local tail = tail or nil
    if tail then
        element["root"]["tail"] = tail
    end
    local attrib = attrib or nil
    if attrib then
        element["root"]["attr"] = attrib
        local i = 1
        for k, v in pairs(attrib) do
            if type(k) == "string" then
                element["root"]["attr"][i] = k
                i = i + 1
            end
        end
    else
        element["root"]["attr"] = {}
    end
    return element
end

ElementTree.Element.__doc__.clear = {
    ["组件名称"] = "ElementTree.Element.clear",
    ["组件风格"] = "处理型",
    ["中文注释"] = "重置该element",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回Lua Object Model风格的table"
        }
    }
}
function ElementTree.Element:clear()
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        if self.root.attr then
            self.root.attr = {}
        end
        for i = 1, #self.root do
            self.root[i] = nil
        end
        if self.root.tail then
            self.root.tail = nil
        end
        return self
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.get = {
    ["组件名称"] = "ElementTree.Element.get",
    ["组件风格"] = "处理型",
    ["中文注释"] = "获取指定名字的属性值。如果不存在该属性，返回指定的默认值",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "元素的属性名"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "如果不存在该字段时，所返回的值。"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回属性值"
        }
    }
}
function ElementTree.Element:get(key, default)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local key = key or nil
        local default = default or ""
        if type(key) == "string" and type(default) == "string" and self.root.attr then
            return self.root.attr[key] or default
        else
            return default
        end
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.items = {
    ["组件名称"] = "ElementTree.Element.items",
    ["组件风格"] = "处理型",
    ["中文注释"] = "获取element属性的名字-值对列表",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回element属性的名字-值对列表"
        }
    }
}
function ElementTree.Element:items()
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local ret = {}
        for k, v in ipairs(self.root.attr) do
            ret[v] = self.root.attr[v]
        end
        return ret
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.keys = {
    ["组件名称"] = "ElementTree.Element.keys",
    ["组件风格"] = "处理型",
    ["中文注释"] = "获取element属性的名字列表",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回element属性的名字列表"
        }
    }
}
function ElementTree.Element:keys()
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local ret = {}
        for i, v in ipairs(self.root.attr) do
            ret[i] = v
        end
        return ret
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.set = {
    ["组件名称"] = "ElementTree.Element.set",
    ["组件风格"] = "处理型",
    ["中文注释"] = "设置element的属性值",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "属性名"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "属性值"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回element属性的table"
        }
    }
}
function ElementTree.Element:set(key, value)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local key = key or nil
        local value = value or nil
        if type(key) == "string" and type(value) == "string" and self.root.attr then
            local isExist = false
            for i, v in ipairs(self.root.attr) do
                if v == key then
                    isExist = true
                    break
                end
            end
            self.root.attr[key] = value
            if not isExist then
                self.root.attr[#self.root.attr + 1] = key
            end
        else
            self.root.attr = {[1] = key, [key] = value}
        end
        return self
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.append = {
    ["组件名称"] = "ElementTree.Element.append",
    ["组件风格"] = "判断型",
    ["中文注释"] = "给该element添加子element",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "Lua Object Model风格的subelement"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "boolean",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "如果成功，返回true；否则，返回false"
        }
    }
}
function ElementTree.Element:append(subelement)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local subelement = subelement or nil
        if type(subelement) == "table" then
            if type(subelement.root) == "table" and type(subelement.root.tag) == "string" then
                self.root[#self.root + 1] = subelement.root
                return true
            elseif type(subelement.tag) == "string" then
                self.root[#self.root + 1] = subelement
                return true
            end
        end
        return false
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.extend = {
    ["组件名称"] = "ElementTree.Element.extend",
    ["组件风格"] = "处理型",
    ["中文注释"] = "给该element添加子element",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "Lua Object Model风格的subelements"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "Lua Object Model风格的subelements"
        }
    }
}
function ElementTree.Element:extend(subelements)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local subelements = subelements or nil
        if type(subelements) == "table" then
            for i = 1, #subelements do
                self:append(subelements[i])
            end
        end
        return self
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.remove = {
    ["组件名称"] = "ElementTree.Element.remove",
    ["组件风格"] = "处理型",
    ["中文注释"] = "移除该element的subelement",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "Lua Object Model风格的subelement"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回Lua Object Model风格的element"
        }
    }
}
function ElementTree.Element:remove(subelement)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local subelement = subelement or nil
        if type(subelement) == "table" and type(subelement.root) == "table" and type(subelement.root.tag) == "string" then
            for j, v in ipairs(self.root) do
                if subelement == v then
                    repeat
                        self.root[j] = self.root[j + 1]
                        self.root[j + 1] = nil
                        j = j + 1
                    until not self.root[j + 1]
                    break
                end
            end
        end
        return self
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.getchildren = {
    ["组件名称"] = "ElementTree.Element.getchildren",
    ["组件风格"] = "处理型",
    ["中文注释"] = "获得该element的subelement列表",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回当前element的sub element列表"
        }
    }
}
function ElementTree.Element:getchildren()
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local ret = {}
        for i = 1, #self.root do
            if type(self.root[i]) == "table" and type(self.root[i].tag) == "string" then
                ret[#ret + 1] = self.root[i]
            end
        end
        return ret
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.get_tag = {
    ["组件名称"] = "ElementTree.Element.get_tag",
    ["组件风格"] = "处理型",
    ["中文注释"] = "返回该element的tag名",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回element的tag名"
        }
    }
}
function ElementTree.Element:get_tag()
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        return self.root.tag
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.get_text = {
    ["组件名称"] = "ElementTree.Element.get_text",
    ["组件风格"] = "处理型",
    ["中文注释"] = "返回该element的text内容列表",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回该element的text内容列表"
        }
    }
}
function ElementTree.Element:get_text()
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local ret = {}
        for i = 1, #self.root do
            if type(self.root[i]) == "string" then
                ret[#ret + 1] = self.root[i]
            end
        end
        if #ret == 1 then
            return ret[1]
        else
            return ret
        end
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.get_tail = {
    ["组件名称"] = "ElementTree.Element.get_tail",
    ["组件风格"] = "处理型",
    ["中文注释"] = "移除该element的tail字段",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回element的tail字段"
        }
    }
}
function ElementTree.Element:get_tail()
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        return self.root.tail
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.get_attrib = {
    ["组件名称"] = "ElementTree.Element.get_attrib",
    ["组件风格"] = "处理型",
    ["中文注释"] = "移除该element的attrib字段",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回element的attrib字段"
        }
    }
}
function ElementTree.Element:get_attrib()
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        return self.root.attr
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.insert = {
    ["组件名称"] = "ElementTree.Element.insert",
    ["组件风格"] = "处理型",
    ["中文注释"] = "在指定的索引处插入一个子结点",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "integer",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "索引序号"
        },
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "带插入的子结点"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回element结点自身"
        }
    }
}
function ElementTree.Element:insert(index, subelement)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local index = index or #self.root
        local subelement = subelement or nil
        if type(index) == "number" and type(subelement) == "table" and type(subelement.root) == "table" and type(subelement.root.tag) == "string" then
            local i = 1
            local position = 1
            for j = 1, #self.root do
                if type(self.root[j]) == "table" then
                    position = j
                    if i < index then
                        i = i + 1
                    else
                        break
                    end
                end
            end
            if i < index then
                self.root[#self.root + 1] = subelement.root
            else
                local tmp = self.root[position]
                self.root[position] = subelement.root
                for j = #self, position + 1, -1 do
                    self.root[j + 1] = self.root[j]
                end
                self.root[position + 1] = tmp
            end
        end
        return self
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.find = {
    ["组件名称"] = "ElementTree.Element.find",
    ["组件风格"] = "处理型",
    ["中文注释"] = "返回符合规则的第一个Lua Object Model风格的element",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "xpath的路径规则"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回符合规则的第一个Lua Object Model风格的element"
        }
    }
}
function ElementTree.Element:find(match)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local ret = xpath.selectNodes(self.root, match)
        return ret[1]
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.findall = {
    ["组件名称"] = "ElementTree.Element.findall",
    ["组件风格"] = "处理型",
    ["中文注释"] = "返回符合规则的所有Lua Object Model风格的element",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "xpath的路径规则"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回符合规则的所有Lua Object Model风格的element"
        }
    }
}
function ElementTree.Element:findall(match)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        return xpath.selectNodes(self.root, match)
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.Element.__doc__.findtext = {
    ["组件名称"] = "ElementTree.Element.findtext",
    ["组件风格"] = "处理型",
    ["中文注释"] = "返回符合规则的element的text字段的列表",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "xpath的路径规则"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回符合规则的element的text字段的列表"
        }
    }
}
function ElementTree.Element:findtext(match)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local result = xpath.selectNodes(self.root, match)
        local ret = {}
        for i = 1, #result do
            if type(result[i]) == "string" then
                ret[#ret + 1] = result[i]
            end
        end
        return ret
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

----------------------------------------

ElementTree.ElementTree = {}

ElementTree.ElementTree.__doc__ =
    {
        ["小分类"] = "ElementTree.ElementTree",
        ["内部"] = false,
        ["过时"] = false
    }

function ElementTree.ElementTree:new(element)
    local tree = {}
    self.__index = self
    setmetatable(tree, self)

    local element = element or nil
    if type(element) == "table" then
        if type(element.root) == "table" and type(element.root.tag) == "string" then
            tree.root = element
        elseif type(element.tag) == "string" then
            tree.root = element
        else
            tree.root = {["tag"] = ""}
        end
    else
        tree.root = {["tag"] = ""}
    end
    return tree
end

ElementTree.ElementTree.__doc__._setroot = {
    ["组件名称"] = "ElementTree.Element._setroot",
    ["组件风格"] = "处理型",
    ["中文注释"] = "设置root element",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "Lua Object Model风格的element"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "如果成功，返回true；否则，返回false"
        }
    }
}
function ElementTree.ElementTree:_setroot(element)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local element = element or nil
        if type(element) == "table" then
            if type(element.root) == "table" and type(element.root.tag) == "string" then
                self.root = element.root
                return true
            elseif type(element.tag) == "string" then
                self.root = element
                return true
            end
        end
        return false
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.ElementTree.__doc__.getroot = {
    ["组件名称"] = "ElementTree.Element.getroot",
    ["组件风格"] = "处理型",
    ["中文注释"] = "获取root element",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回Lua Object Model风格的root element"
        }
    }
}
function ElementTree.ElementTree:getroot()
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        return self.root
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.ElementTree.__doc__.parse = {
    ["组件名称"] = "ElementTree.Element.parse",
    ["组件风格"] = "处理型",
    ["中文注释"] = "parse一个xml格式的文件",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "xml格式的文件的路径或文件句柄"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回Lua Object Model风格的element"
        }
    }
}
function ElementTree.ElementTree:parse(handle)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        if type(handle) == "string" then
            self.root = ElementTree.parse(handle)
        else
            local content = handle:read("*all")
            self.root = ElementTree.fromstring(content)
        end
        return self
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.ElementTree.__doc__.find = {
    ["组件名称"] = "ElementTree.ElementTree.find",
    ["组件风格"] = "处理型",
    ["中文注释"] = "返回符合规则的第一个Lua Object Model风格的element",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "xpath的路径规则"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回符合规则的第一个Lua Object Model风格的element"
        }
    }
}
function ElementTree.ElementTree:find(match)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local ret = xpath.selectNodes(self.root, match)
        return ret[1]
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.ElementTree.__doc__.findall = {
    ["组件名称"] = "ElementTree.ElementTree.findall",
    ["组件风格"] = "处理型",
    ["中文注释"] = "返回符合规则的所有Lua Object Model风格的element",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "xpath的路径规则"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回符合规则的所有Lua Object Model风格的element"
        }
    }
}
function ElementTree.ElementTree:findall(match)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        return xpath.selectNodes(self.root, match)
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

ElementTree.ElementTree.__doc__.findtext = {
    ["组件名称"] = "ElementTree.ElementTree.findtext",
    ["组件风格"] = "处理型",
    ["中文注释"] = "返回符合规则的element的text字段的列表",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "xpath的路径规则"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回符合规则的element的text字段的列表"
        }
    }
}
function ElementTree.ElementTree:findtext(match)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local result = xpath.selectNodes(self.root, match)
        local ret = {}
        for i = 1, #result do
            if type(result[i]) == "string" then
                ret[#ret + 1] = result[i]
            end
        end
        return ret
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end

--[[
ElementTree.ElementTree.__doc__.write = {
    ["组件名称"] = "ElementTree.Element.write",
    ["组件风格"] = "处理型",
    ["中文注释"] = "parse一个xml格式的文件",
    ["入参"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "element自身"
        },
        {
            ["type"] = "userdata",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "打开的文件句柄"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "xml文件的编码"
        },
        {
            ["type"] = "string",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "xml declaration"
        }
    },
    ["出参"] = {},
    ["返回状态"] = {
        {
            ["type"] = "table",
            ["default"] = "",
            ["日志级别"] = "Trace",
            ["注释"] = "返回Lua Object Model风格的element"
        }
    }
}
function ElementTree.ElementTree:write(file, encoding, xml_declaration)
    local ret = {exception.try(function ()
    -- 该函数的函数体（开始）
        local options = {
            decl       = false,
            empty_tags = false,
            attr_sort  = etree.lexicographic,
            encoding   = etree.encoding.most
            }
        etree.ElementTree(self, options):write(file)
    -- 该函数的函数体（结束）
    end, function (except) -- 捕获异常并进行处理
        -- 如果需要再次抛出异常，那么返回需要抛出的异常消息体；否则，在异常处理过程结束时返回nil
        -- 异常处理过程（开始）
        io.write(string.format("exception caught: %s\n", tostring(except)))
        return nil
        -- 异常处理过程（结束）
    end)}
    if ret[1] then
        return unpack(ret, 2)
    elseif ret[2] then
        -- 捕捉其它函数产生异常，或该函数的异常处理模块未能处理的异常，也可以继续抛出异常。此处可捕获所有的异常
        exception.catch(ret[2])
        return nil
    else
        return nil
    end
end
--]]
