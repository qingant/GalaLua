local string = require("string")
require("class")


class "Base1"
{
	__init__ = function (self, name)
		self.name=name
	end,
	
	pp = function (self)
		print("Hello class")
	end,
	
	getname = function (self)
		return self.name
	end;
	
	age=20
}

local _b = Base1("luozhihui")
print(_b.name)
_b:pp()
print(_b:getname())

-- 如何禁止类似的定义， 如果是这样的定义inher它的class能否找到这个method
function Base1:setname(name)
	self.name = name
end
_b:setname("YOYO")
print(_b:getname())

function _b:sayHi()
	print("Hi")
end
_b:sayHi()

print("==========================")

local _bb = Base1("mimi")
print(_bb:getname())
_bb:setname("HHHHH")
print(_bb:getname())
--_bb:sayHi()
