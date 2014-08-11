module(..., package.seeall)

index = {}

function index:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function index:get(context,...)
    context:get_response():redirect("/html/index.html")
    return "wait while redirect..."
end

function index:post(context,...)
end
