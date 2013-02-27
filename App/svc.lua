function main()
   glr.spawn("svc.lua", "dispacher")
end


function dispacher()
   local processes = {}
   local err, gpid = glr.spawn("svc.lua","worker")
   while true do
      msg = amq.recv()
    
      glr.send(gpid, msg)
      
   end
end


function worker()
   while true do
      local err, msg = glr.recv()
   end
end