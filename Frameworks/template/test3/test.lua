local template = require "template.template"

local layout = template.new("layout.html")
layout.title = "Test lua-resty-template"
layout.view = template.compile("../test1/view.html"){ message = "Hello, World"}
layout:render()

