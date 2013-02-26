--print("test.lua");
local net  = require "net"
function main()

   print("I am Node " .. glr.id);
   for i = 0, 1 do
      id = glr.spawn("test.lua", "node_main");
      print("Node "..id.."  Created");
   end
   print(id);
   -- coroutine.yield();
   --yield();
   print("resumed");
   glr.send(1, "hello node 8");
   --glr.int(0,1,2)
   print "ok"
   local fd = net.tcp_server( "0.0.0.0", 8099)
   print("listen")
   print(fd)
   glr.spawn("test.lua", "node_client")


   while true do
      local recvfd = net.accept(fd)
      print("accepted")
      print(recvfd)
      print(i)
      local rt = net.recv(recvfd, 5)
      print(rt)
      status, msg = net.recv(recvfd, 5);
      print(msg)

   end
   -- node_send(7, "hello node 7");
   -- for i = 0, 1 do
   --    node_send(8, "hello node 8 agin " .. i);
   -- end

end


function node_client()
 
   for i=0, 100 do
      local fd = net.tcp_conn("127.0.0.1", 8099)
      print("connected!")
      print(fd)
      print("I am Node " .. glr.id);
      print(i)
      net.send(fd, "hello")
      net.close(fd)
   end
end
function retry(fun)
   while true do
      r = nil;
      r = fun();
      print("Node "..nid.." Called");
      if r ~= nil then
         return r;
      else
         print("suspend");
      end
   end
end
-- function dispatch_node ()
--    {id:node}
--    mq_recv
-- end
function node_main()
   print("I am Node "..glr.id);
   -- open dbc
   -- node_recv()
   while true do
      local msg = glr.recv();
      print("msg '" .. msg .. "' recved");
      if nid == 8 then
         node_send(7, "hello node 7 from node(8) " .. msg);
      elseif  nid == 7 then
         node_send(6, "hello node 6 from node(7) " .. msg);
      end
   end
end


-- if (nid==0) then

--    print("main");
--    main()
-- end

