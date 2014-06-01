--[[
	Author : Ma Tao
	Date   : 2014/04/16

	== Introduction ==
	GLR RPC Test-case

]]

__VER__ = "1.0"

module(..., package.seeall)

local rpc = require("rpc")
local client = require("rpc_client").client
local pprint = require("pprint").pprint

-- pprint(rpc, "rpc")
function test_server()
    local test_server = rpc.server:new():init("test_server")
    function test_server:test(params, desc)
        pprint(params, "params")
        pprint(desc, "desc")
        return {msg="hello"}
    end
    function test_server:hello(params, desc)
        self._wait_client = desc
        return self.no_response
    end
    function test_server:on_timeout()
        if self._wait_client then
            glr.send(self._wait_client.addr, self._packer({error=nil,
                                          id = nil,
                                          result = {msg="wait hello"}}),
                      {corrid = self._wait_client.attr.msgid})
        end
    end
    test_server:main()
end

function main(...)
    print("main...")
    local err, pid = glr.spawn("rpc_test", "test_server")
    print("Create rpc server: "..pid)
    pprint(glr.npt.registered())
    local cli = client:new():init({ host = glr.sys.host,
                                    port = glr.sys.port,
                                    gpid = pid
                                  })
    local result = cli:call("test", {hello="world"})
    pprint(result, "result")
    local result = cli:call("test-1", {hello="world"})
    pprint(result, "result")
    local result = cli:call("hello",  {})
    pprint(result, "result")
end

if ... == "__main__" then
    print("Main..." .. __id__)
    main()
end