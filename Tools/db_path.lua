--[[
--
-- path about db and xml
--
--]]

module(...,package.seeall)
local os=require "os"

--{{
local home=os.getenv("HOME")
config=home.."/var/conf"
router=home.."/var/platdb"
supervisor=home.."/var/supervisor"
--}}



GDK=os.getenv("GDK_HOME")
config_schema=GDK.."/var/xmlconf/schema.xml"
config_default=GDK.."/var/xmlconf/default.xml"

gar_path=GDK.."/bin"
