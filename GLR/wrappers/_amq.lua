module(..., package.seeall)
local AMQ_INT_NO = 0x03000000
function put(addr,msg)
    if type(addr)=="number" then
        --放到公共队列
        addr={host="AMQ",port=addr,gpid=0}
    end
    addr.host=addr.host or "AMQ"
    return _glr.int(AMQ_INT_NO, 0, addr, msg) -- amq put
end

function get(addr)
    return _glr.int(AMQ_INT_NO, 1, addr) -- amq get
end

--设置私有队列号
function set_queue(queue)
    return _glr.int(AMQ_INT_NO, 2, queue)
end
