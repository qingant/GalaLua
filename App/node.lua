module(..., package.seeall)


function register(host, port, pid)
   return glr.int(2, 0, host, port, pid)
end

function send(host, port, pid, msg)
   return glr.int(2, 1, host, port, msg, pid)
end
function get_all_links()
   return glr.int(2, 3)
end
