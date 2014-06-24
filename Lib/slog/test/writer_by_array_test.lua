#!/usr/bin/lua

--[[

author: Zhou Xiaopeng
date: 2014/6/18
description: test the read_stream module, read indexes of a log file.

]]
local slog = require("slog")
local msg_pack = require("msg_pack/message_pack")


function write_log_by_num(writer, num)
	local log = nil
	for i=1,num do
		log = "start xxx-xxx-xxx:"..(i+1).." time:"..(os.time()).." end."
		writer:write(log)
	end		
end

function write_log_by_table(writer, num)
	local log = {app_id = 0, app_name="sys", msg_content="system will be halted."}
	local dat = nil
	local array = {}

	for j=1, num do
		array = {}
		for i=0,4 do
			log["app_id"] = i+j
			dat = msg_pack.pack(log)
			array[i] = dat
		end
		writer:write_array(array)
		j = j+5
	end
	for i=1, num do
		
		writer:write(dat)
	end
end

function print_indexes(idxs)
	for idx, it in pairs(idxs) do
		print(it[1],"-->",it[2])
	end
end

function main()
	local writer = slog.create_writer("log_data/2.log")
	start = os.time();
	-- local reader = slog.create_reader("log_data/2.log")
	write_log_by_table(writer, 10000)
	print("Last time:",os.time() - start)
	
--[[
]]


	--[[
	local it, s = reader.read_all_indexes()
	for k, v in pairs(it) do
		print(k, v)
	end
    ]]
end


main()

