module(..., package.seeall)


-- 模块注释(注册信息) --
__doc__ = {
   ["组件分类"]="GalaTest",
   ["组件等级"]="",
}

-- class AC_GalaTest --
AC_GalaTest = {
-- implement --
   __doc__ =  {
      ["小分类"]="GalaTest",
      ["内部"]=false,
      ["过时"]=false,
   }
   __doc__.AC_pipe = {
      
   }
   function AC_pipe(self, arg1, arg2)
      
      return {1, nil, nil, {arg1}}
   end
   
}
-- class docstring --
AC_GalaTest.__doc__ = {
   ["小分类"]="GalaTest",
   ["内部"]=false,
   ["过时"]=false,
}

-- init function --
function AC_GalaTest:new(o)
   o = o or {}
   setmetatable(o, self)
   self.__index = self
   return o
end



-- component --

-- doc --
AC_GalaTest.__doc__.AC_pipe = {
   ["组件名称"] = "container",
   ["组件风格"] = "判断型",
   ["中文注释"] = "something",
   ["入参"] = {
      {
         ["type"] = "str",
         ["default"] = "",
         ["日志级别"] = "Trace",
         ["注释"] = "表示blabla"
      },
      {
         ["type"] = "str",
         ["default"] = "",
         ["日志级别"] = "Trace",
         ["注释"] = "表示blabla"
      }
   },
   ["出参"] = {
      {
         ["type"] = "str",
         ["default"] = "",
         ["日志级别"] = "Trace",
         ["注释"] = "表示blabla"
      }
   },
   ["返回状态"] = {
      {0, "失败"},
      {1, "成功"}，
   }
}
-- implement --
function AC_GalaTest:AC_pipe(arg1, arg2)

   try(function ()  
          
       end)
   return {1, nil, nil, {arg1}}
end




   



