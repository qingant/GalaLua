module(...,package.seeall)
local cjson = require "cjson"
local pprint = require "pprint"

function check(err,method)
   if not err then
      pprint.pprint(string.format("¡¾call function of %s  complete,check failure!¡¿",method))
   else
      pprint.pprint(string.format("¡¾call function of %s  successfully,check ok!¡¿",method))
   end
end

function main()
   
   glr.send(__id__,"check msgbox")
   check(glr.msg_available(__id__),"msg_available")
   if glr.msg_available(__id__) then
      glr.recv()
   end

   local err,status_server0 = glr.spawn("test", "process",__id__)
   local err,status_server1 = glr.spawn("test", "process",__id__)
   check(err,"spawn")
   btime = os.time()
   glr.time.sleep(1)
   atime = os.time()
   local err
   if (atime - btime) >=1 then
      err = true
   else
      err = false
   end
   check(err,"int")   
   i = 0
   while true do     
      local err,process = glr.status.processes()
      check(err,"all")
      local msg_type, addr, msg= glr.recv()    
      local recvmsg = cjson.decode(msg)  
      check(msg_type,"recv")
      local err,states = glr.status.status(recvmsg.id)
      check(err,"status")
      if i % 2 == 0 then
         local err = glr.send(recvmsg.id,"kill")
         check(err,"send")
	 glr.time.sleep(1)
         local err,states = glr.status.status(recvmsg.id)
         check(err,"kill")
      else
         local err = glr.send(recvmsg.id,"exit")
         check(err,"send")
	 glr.time.sleep(1)
         local err,states = glr.status.status(recvmsg.id)
         check(err,"exit")
      end
      i =i + 1      
   end
end

function process(id)
   local err = glr.send(id,cjson.encode({id=__id__,msg="hello word"}))
   check(err,"send")         
   local msg_type, addr, msg = glr.recv()
   check(msg_type,"recv")
   if msg == "kill" then      
      glr.kill(__id__)
   elseif msg == "exit" then
      glr.exit(__id__)         
   end
end
