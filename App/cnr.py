import struct
import socket
import os
import json
import monitor_protocol as protocol

class Node(object):
    def __init__(self, host, port):
        self._host = host
        self._port = port
        self._self_port = 10086
        self._self_host = "0.0.0.0"
        self._register()

    def _send(self,msg,msg_type,dest_gpid=0):
        '''send msg of msg_type to dest_gpid'''
        def header(type,dest_gpid=0,src_gpid=0):
            '''GLR_BUS_HEAD'''
            buf = ""
            buf += struct.pack("B", type)
            buf += struct.pack(">I", dest_gpid)

            buf += self._self_host.ljust(20, "\x00")
            buf += struct.pack(">I", self._self_port)
            buf += struct.pack(">I", src_gpid)
            return buf

        buf=header(msg_type,dest_gpid)

        #FIXME::ugly,This should be doing one time,not every time
        self.GLR_BUS_HEAD_len=len(buf) 

        buf+=msg
        buf=struct.pack(">I", len(buf)) + buf
        print "sending(%d):"%(len(buf)-4),repr(buf)        
        self._sock.send(buf)

    def _recv(self,RAW_RECEIVE=False):
        length = self._sock.recv(4, socket.MSG_WAITALL);
        ilength = struct.unpack(">I",length)[0]
        text = self._sock.recv(ilength, socket.MSG_WAITALL)
        def get_rid_of_GLR_BUS_HEAD():
            return text[self.GLR_BUS_HEAD_len:]
        if not RAW_RECEIVE: 
            return get_rid_of_GLR_BUS_HEAD()
        return text

    def _register(self):
        self._sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack("ii", 1, 0))
        self._sock.connect((self._host, self._port))

#        self._self_host,self._self_port=s.getsockname()
#        print self._self_port,self._self_host

        self._send("",protocol.MSG_TYPE["REGISTER"])
            
        #FIXME::must recv REGISTER_OK
        text=self._recv(True)
        print "recv:",repr(text)

        def get_services_list():
            #asking for the services list 
            mon_head=self.moniter_header(protocol.MONITOR_ACTION["REG"],protocol.MONITOR_TYPE["ALL"])
            self.MONITER_HEADER_len=len(mon_head)
            self._send(mon_head,protocol.MSG_TYPE["APP"])

            text=self._recv()
            self._services_list=json.loads(text[self.MONITER_HEADER_len:])
        get_services_list()
        print "register over"

    def moniter_header(self,action,type):
        msg_id=12312    #useless?
        buf = struct.pack(">I", msg_id)
        buf += struct.pack("B",action) #action
        buf += struct.pack("B",type) #
        return buf

    def router_method(self,action,name='',field='',app_type=""):
        print "get_router"
        #monitor header
        buf=self.moniter_header(action,protocol.MONITOR_TYPE["ROUTER"])
        #arguments
        buf += name.ljust(20, "\x00")
        buf += field.ljust(20, "\x00")
        buf += app_type.ljust(20, "\x00")
        print len(buf)
        self._send(buf,protocol.MSG_TYPE["APP"],self._services_list["router"])
        
        text=self._recv()
        print repr(text)
        print("get router over")

    def del_router(self,name=''):
        return self.router_method(protocol.MONITOR_ACTION["DEL"],name)

    def get_router(self,name='',field='',app_type=""):
        return self.router_method(protocol.MONITOR_ACTION["GET"],name,field,app_type)
    
    def amq(self):
        buf=self.moniter_header(protocol.MONITOR_ACTION["GET"],protocol.MONITOR_TYPE["AMQ"])
        self._send(buf,protocol.MSG_TYPE["APP"],self._services_list["amq"])
        text=self._recv()
        print repr(text)
        print("get amq over")
        
        
        
if __name__ == "__main__":
    n = Node("0.0.0.0", 2345)
    n.amq()
#    n.get_router()
#    n.del_router("apple")
#    n.del_router()
    while True:
        pass

