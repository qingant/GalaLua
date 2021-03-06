--[[
	Author : Ma Tao
	Date   : 2014/04/02

	== Introduction ==
	Basic test cases for glr-1.x

	== Usage ==
	glr -m basic -p 2345
]]

module(..., package.seeall)
local __module__ = ...
local pprint = require("pprint")

function test_process()
	print "test_process"
	local pid = assert(glr.spawn(__module__, "worker"), "spawn failed")
	print(string.format(""))
end


function test_self_msg(  )
	local msg = "hello world"
	glr.send(__id__, msg)
	assert(glr.msg_available(), "call msg_available failed")
	local type, addr, recv_msg = glr.recv()
	assert(recv_msg == msg)
end
function test_rpc_msg(  )
	local msg = "hello world"
	glr.send({host="0.0.0.0",port=glr.sys.port,gpid=0}, msg)
	local type, addr, recv_msg = glr.recv()
	assert(recv_msg == msg)
	print("test_rpc_msg")
end

function worker()
	while true do
		local msg = glr.recv()
		pprint.pprint(msg, "Msg")
	end
end

function test_timer( timeout )
	local timeout = timeout or 1
	local now = glr.time.now()
	glr.time.sleep()
	local just_now = glr.time.now()
	assert(now==just_now, "")
end



function main(  )

	test_process()
	test_self_msg()
	test_rpc_msg()
	glr.exit()
end

