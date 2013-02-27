module(..., package.seeall)


-- const viarible --
AF_INET = glr.AF_INET
AF_UNIX = glr.AF_UNIX



-- interface -- 
function tcp_server(host, port)
   
   return glr.int(1,     
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
end

function un_server(path)
   return glr.int(1,       
                  -- Interrupt ID
                  0, 
                  -- Operation Type: make server
                  AF_UNIX,   
                  -- Domain
                  path
                  -- Path
                 )
end

function tcp_conn(host, port)
   
   return glr.int(1,     
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
end

function un_conn(path)
   return glr.int(1,       
                  -- Interrupt ID
                  1, 
                  -- Operation Type: connect
                  AF_UNIX,   
                  -- Domain
                  path
                  -- Path
                 )
end
function accept(fd)
   return glr.int(1, 4, fd)
end
function recv(fd, len)
   return glr.int(1,     
                  -- Interrupt ID
                  2,       
                  -- Operation Type: recv 
                  fd,           
                  -- fd
                  len
                  -- length
                 )           
end

function send(fd, buf)
   return glr.int(1, 3, fd, buf)
end

function close(fd)
   return glr.int(1, 5, fd)
end