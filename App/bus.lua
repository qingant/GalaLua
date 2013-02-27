module(..., package.seeall)


function register(host, port, pid)
   return glr.int(2, 0, host, port, pid)
end

function send(host, port, msg)
   return glr.int(2, 1, host, port, msg)
end