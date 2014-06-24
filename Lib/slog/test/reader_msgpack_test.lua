#! /bin/env lua

--[[

author: Zhou Xiaopeng
date: 2014/6/20
description: test the read_stream module, read indexes of a log file.

]]
local slog = require("slog")
local msg_pack = require("msg_pack/message_pack")


function print_indexes(idxs)
	for idx, it in pairs(idxs) do
		print(it[1],"-->",it[2])
	end
end

function main()
	local reader = slog.create_reader("log_data/2.log")

	
	
	local it= reader:read_indexes();
	local location = it["indexes"][1][2]
--[[
	for k, v in pairs(it["indexes"]) do
		print(k, v)
	end
]]
	local start = os.time()
	reader:seek(location)
	local rows = 0
	local dat = nil
	local t = reader:read_next()
	while t ~= nil do
		rows = rows + 1
		dat = msg_pack.unpack(t)
		for k, v in pairs(dat) do
			print(k, v)
		end		
		t = reader:read_next()
	end 



	print("Last time1:",os.time() - start)
	print("Row num1:", rows)
--[[
	location = it["indexes"][2][2]
	reader:seek(location)
	rows = 0
	start = os.time()
	t = reader:read_next()
	while t ~= nil do
		rows = rows + 1
		for k, v in pairs(t) do
			print(k, v)
		end
		t = reader:read_next()
	end 
	print("Last time2:",os.time() - start)
	print("Row num2:", rows)
]]
end


main()

