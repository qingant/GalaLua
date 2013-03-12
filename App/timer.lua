module(...,package.seeall)


function sleep(sec)
   return glr.int(0,1,sec)
end

function time()
   return glr.int(0,0)
end