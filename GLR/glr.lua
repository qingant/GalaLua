-- module(..., package.seeall)
-- local __main__ = ...
-- local __self__ = package.loaded[__main__]
local M = {}
local _glr = _glr
local type = type
setfenv(1, M)
local self_host, self_port = _glr.node_addr()

function spawn(mod_name, entry, ...)
   return _glr.spawn(mod_name, entry, ...)
end


function send(addr, msg)
   if type(addr) == "table" then
      if self_host == addr.host and self_port == addr.port then
         return _glr.send(addr.gpid, msg)
      else
         return _glr.int(2, 1, addr.host, addr.port, msg, addr.gpid) -- node send
      end
   else
      return _glr.send(addr, msg)
   end
end

function connect(host, port, pid)
   return _glr.int(2, 0, host, port, pid)
end

function recv()
   return _glr.recv()
end

function kill(gpid)
   return _glr.kill(gpid)
end


status = {
   processes = function () return _glr.all() end;
   status = function (gpid) return _glr.status(gpid) end;
}

sys = {
   host = self_host,
   port = self_port,
   gpid = __id__,
   --path = _glr.get_path(),
}


global = _glr.global
get_global = _glr.get_global
CLOSED = _glr.CLOSED
KILL = _glr.KILL
AF_INET = _glr.AF_INET
msg_available = _glr.msg_available
get_option=_glr.get_option
set_options=_glr.set_options
glr_stamp = _glr.glr_stamp
exit = _glr.exit
time = {
   now = function () return _glr.int(0,0) end;
   sleep = function (sec)  return _glr.int(0,1,sec) end
}


net = {

   -- const viarible --
   AF_INET = _glr.AF_INET;
   AF_UNIX = _glr.AF_UNIX;



   -- interface -- 
   tcp_server =  function (host, port)
      
      return _glr.int(1,     
                      -- Interrupt ID
                      0,   
                      -- Operation Type: make server
                      AF_INET,   
                      -- Domain
                      host,   
                      -- host
                      port 
                      -- port
                     )
   end;

    un_server = function (path)
      return _glr.int(1,       
                      -- Interrupt ID
                      0, 
                      -- Operation Type: make server
                      AF_UNIX,   
                      -- Domain
                      path
                      -- Path
                     )
   end;

   tcp_conn = function (host, port)
      
      return _glr.int(1,     
                      -- Interrupt ID
                      1,   
                      -- Operation Type: connect to server
                      AF_INET,   
                      -- Domain
                      host,   
                      -- host
                      port 
                      -- port
                     )
   end;

   un_conn = function (path)
      return _glr.int(1,       
                      -- Interrupt ID
                      1, 
                      -- Operation Type: connect
                      AF_UNIX,   
                      -- Domain
                      path
                      -- Path
                     )
   end;
   accept = function (fd)
      return _glr.int(1, 4, fd)
   end;
   recv =  function (fd, len)
      return _glr.int(1,     
                      -- Interrupt ID
                      2,       
                      -- Operation Type: recv 
                      fd,           
                      -- fd
                      len
                      -- length
                     )           
   end;

   send = function (fd, buf)
      return _glr.int(1, 3, fd, buf)
   end;

   close = function (fd)
      return _glr.int(1, 5, fd)
   end;
}



return M
