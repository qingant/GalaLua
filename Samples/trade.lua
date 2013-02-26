module(..., package.seeall)


-- module doc --
__doc__ = {
   ["name"] = "something",   
}


local gala = require("gala").gala



Trade = gala:new()
Trade.__doc__.flow1_step2_node3 = {

}
function Trade:flow1_step2_node3()
   return 1
end