local template = require "template.template"
local t = template.render([[
<h1>{{header:upper()}}</h1>]], 
{ header = "hello, world!" })
print(t)
