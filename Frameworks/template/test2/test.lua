local template = require "template.template"
template.render("header.html", {
        title = "Testing lua-resty-template",
        message = "Hello, World!",
        names = { "James", "Jack", "Anne" },
        jquery = '<script src="//ajax.googleapis.com/ajax/libs/jquery/2.1.0/jquery.min.js"></script>'
})
