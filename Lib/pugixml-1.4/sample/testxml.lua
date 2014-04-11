module(...,package.seeall)

local xml = require("minixml")
local doc = xml.new()
doc:load_file("xgconsole.xml")
local xpath_node_set = doc:xpath("/Profile/Tools/Tool[@AllowRemote='true' and @DeriveCaptionFrom='lastparam']")
local xpath_node = xpath_node_set:xpath_node_by_index(0)
local node = xpath_node:node()
local child = node:append_child("element")
child:set_name("xpath")
child:append_child("pcdata"):set_value("qqqqqqqq")
local attr = child:append_attribute("attr")
attr:set_value("attr1111")

doc:save("test_xgconsole.xml")


