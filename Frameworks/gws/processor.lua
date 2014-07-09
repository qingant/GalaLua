--[[
    Author : Ma Tao
	Date   : 2014/06/26

	== Introduction ==
    GalaLua Web Server processor
]]

module(..., package.seeall)

local base = require("core.processor").server
local http = require("http").http
local response = require("http").response
local pprint = require("pprint")
local processor = base:new()

server = processor

function processor:new(app_name, pool_name, idx, params)
    base.init(self, app_name, pool_name, idx)
    self.urls = params.urls
    return self
end

function processor:on_init(...)
    self:_back_to_pool()
    self._logger:debug("back to pool")
end

function processor:on_task_add(params, desc)
    self._logger:debug("task_add")
    self._fd = params.fd
    self._protocol = http:new():init(self._fd)
    glr.net.poll(self._fd)
end

function processor:on_message(mtype, desc, msg)
    if mtype == glr.IOCP then
        print("IOCP...", self._fd)
        local r, err
        while true do
            r, err = pcall(function ()
                    local request = self._protocol:getRequest()
                    -- self._logger:debug(pprint.format(request, "request"))
                    local rsp = self:_request_dispatch(request)
                    self._protocol:sendResponse(rsp)
                                 end, debug.traceback)
            if not r then
                break
            end
        end
        print("ERROR", err)
        self:_back_to_pool()
        -- glr.net.close(self._fd)
        -- self:_back_to_pool()
    end
end

function get_query(request)
    local query = split(request.uri,"#")
    query = slice(query,1,#query)
    query = split(query,"%?")
    query = slice(query,#query)
    query = split(query,"&")
    pprint.pprint(query,"query")
    for i,key in pairs(query) do
        local k,v 
        t = split(key,"=")
        request["query"][t[1]] = t[2]
    end
    --pprint.pprint(request["query"],"query")
    --request.query = request["query"]
    --pprint.pprint(request.query,"--query--")

end

function processor:_request_dispatch(request)
    -- TODO: url matching
    pprint.pprint(request, "request")
    pprint.pprint(self.urls, "urls")
    for i, k in pairs(self.urls) do
        pprint.pprint(i,"urls")
        pprint.pprint(split(k," "),"split")
    end
    if self.urls then
        request["query"] = {}
        for uri,func in pairs(self.urls) do
            local m = {string.match(request.uri, uri)}
            if m then
                get_query(request)
                local rsp = self:_call_hander(func, request)
                return rsp
            end
        end
    end
    local rsp = response:new()
    rsp.statusCode = 404
    rsp.status = "Service Not Found"
    rsp.version = "http/1.1"
    return rsp
end

function processor:_call_hander(h, request)
    print(h, request)
    -- TODO: load handler and cache
    --header
    local rsp = response:new()
    --TODO parse request.accept
    if request.accept == "text/html" then
        rsp["Content-Type"] = "text/html"
    else 
        rsp["Content-Type"] = "text/html"
    end
   
    tab = split(h,"%.")
    e = #tab
    module = slice(tab,1,e,".")
    cls = slice(tab,e)

    module = require(module)
    cls = module[cls]

    --pprint.pprint(tab, "table.split")
    --pprint.pprint(module, "module")
    --pprint.pprint(cls, "class")

    --body
    if request.query then
        params = request.query
    end
    pprint.pprint(params,"--params---")
    if request.method == "GET" then
        rsp:setContent(cls:get(params))
    elseif request.method == "POST" then
        rsp:setContent(cls:post(params))
    end
    return rsp
end

function slice(table,...)
    s, e, sep = ...
    if not s then 
        s = 1
    end
    if not e then
        e = #table + 1
    end

    if not sep then
        sep = ""
    end

    local str = ""
    for i,k in  ipairs(table) do
        if i == s then
            str = str..k
        elseif i > s and i < e then
            str = str..sep..k
        end
    end
    return str
end


--TODO parse pPattern replace %. as %s\ 
function split(pString, pPattern)
   local Table = {}  -- NOTE: use {n = 0} in Lua-5.0
   local fpat = "(.-)" .. pPattern
   local last_end = 1
   --local s, e, cap = pString:find(fpat, 1)
   local s, e, cap = string.find(pString, fpat, 1)

   while s do
      if s ~= 1 or cap ~= "" then
        table.insert(Table,cap)
      end
      last_end = e+1
      s, e, cap = string.find(pString, fpat, last_end)
   end
   if last_end <= #pString then
      cap = string.sub(pString, last_end)
      table.insert(Table, cap)
   end
   return Table
end

