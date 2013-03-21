local os = require "os"
local string = require "string"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local cjson = require "cjson"
local node = require "node"
local conf = require "Configure"
local prepath = "/Configure/_STRUCTURE/MonitorList[00]/Monitor/"
local confpath = os.getenv("HOME") .. "/cfg/shmfile/node.conf"
local mode = "w"

function response_format(status,code,_type,msg)
    return cjson.encode({
    ['status'] = status,
    ['errcode'] = code,
    [_type] = msg,
    })
end

function main()
   local mtrobj = conf.new(confpath,mode)
   if not mtrobj then
       error("new configure object failure")
   end
   while true do
      local err,addr,msg = glr.recv()
      if err then
         local request = cjson.decode(msg)
         local response
         local host = addr.host
         local port = addr.port
	     local gpid = addr.gpid
         local oper = request.action
	     if not oper then
	         response = response_format("false","","msg","miss operater cmd:alter or query")
	         local err = node.send(host,port,gpid,cjson.encode(response))
             if not err then
                error("send error")
             end
	     elseif oper == "alter" then
	         local fpath = request.path
	         if string.sub(fpath,1,1) ~= '/' then
	            fpath = prepath .. fpath
	         end
	         local value = request.value
             local err,msg = pcall(mtrobj.put,mtrobj,fpath,value)
	         if not err then
	             response = response_format("false","","msg",msg)
	         else
	             response = response_format("true","","","")
             end
             local err = node.send(host,port,gpid,cjson.encode(response))
             if not err then
                error("send error")
             end
	     elseif oper == "query" then
	         local fpath = request.path
	         if string.sub(fpath,1,1) ~= '/' then
	            fpath = prepath .. fpath
	         end
	         local flag = request.isset
	         if not flag then
	             response = response_format("false","","msg","the operater cmd is query but it has not flag: true or false ")
                 local err = node.send(host,port,gpid,cjson.encode(response))
                 if not err then
                    error("send error")
                 end
	         elseif flag == "false" then
	             local result,msg = pcall(mtrobj.get,mtrobj,fpath)
		         if not result then
		             response = response_format("false","","msg",msg)
		         else
		             response = response_format("true","","content",msg)
		         end
                 local err = node.send(host,port,gpid,cjson.encode(response))
                 if not err then
                    error("send error")
                 end
	         else
	         end
	     else
	         response = response_format("false","","msg","operater cmd must be :alter or query")
             local err = node.send(host,port,gpid,cjson.encode(response))
             if not err then
                error("send error")
             end
	     end
      end
   end
end