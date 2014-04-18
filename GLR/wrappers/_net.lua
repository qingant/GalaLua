--[[
	Author : Ma Tao
	Date   : 2014/04/18

	== Introduction ==
	net sys-calls wrapper for glr
	NEVER use this module directly
]]

module(..., package.seeall)

local SOCK_INT_NO = 0x01000000
local function int_arg(no, timeout)
    if timeout == nil or timeout == -1 then
        return no+0x0000ffff
    else
        return no+timeout
    end    
end

-- const viarible --
AF_INET = _glr.AF_INET;
AF_UNIX = _glr.AF_UNIX;



-- interface -- 
tcp_server =  function (host, port)
    
    return _glr.int(int_arg(SOCK_INT_NO),     
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
    return _glr.int(int_arg(SOCK_INT_NO),       
                    -- Interrupt ID
                    0, 
                    -- Operation Type: make server
                    AF_UNIX,   
                    -- Domain
                    path
                    -- Path
                   )
end;

tcp_conn = function (host, port, timeout)

    return _glr.int(int_arg(SOCK_INT_NO, timeout),     
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

un_conn = function (path, timeout)
    return _glr.int(int_arg(SOCK_INT_NO, timeout),       
                    -- Interrupt ID
                    1, 
                    -- Operation Type: connect
                    AF_UNIX,   
                    -- Domain
                    path
                    -- Path
                   )
end;
accept = function (fd, timeout)
    return _glr.int(int_arg(SOCK_INT_NO, timeout), 4, fd)
end;
recv =  function (fd, len, timeout)
    return _glr.int(int_arg(SOCK_INT_NO, timeout),     
                    -- Interrupt ID
                    2,       
                    -- Operation Type: recv 
                    fd,           
                    -- fd
                    len
                    -- length
                   )           
end;
get_line =  function (fd, timeout)
    return _glr.int(int_arg(SOCK_INT_NO, timeout),     
                    -- Interrupt ID
                    7,       
                    -- Operation Type: recv 
                    fd         
                    -- fd
                   )           
end;
send = function (fd, buf)
    return _glr.int(SOCK_INT_NO, 3, fd, buf)
end;

close = function (fd)
    return _glr.int(SOCK_INT_NO, 5, fd)
end;

