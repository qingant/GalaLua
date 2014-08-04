local template = require "template.template"
local view = template.new("view.html")
view.message = "Hello World"
view:render()
template.render("view.html", {message = "Hello World"})
template.render([[
<!DOCTYPE html>
<html>
<body>
  <h1>{{message}}</h1>
</body>
</html>]],{ message = "Hello World"})
