module(..., package.seeall)

index = {}

function index:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function index:get(context,...)
    context:redirect("static/index.html")
end

function index:post(context,...)
end
