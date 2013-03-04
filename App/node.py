# -*- coding: utf-8 -*-
"""
    @auther ma.tao
"""
import struct
import socket
import os
import json

class Node(object):
    def __init__(self, host, port):
        self._host = host
        self._port = port
        self._self_port = id(self)%1024
        self._self_host = "python@%d"%self._self_port
        self._bind = 0
        self._register()
    def _register(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack("ii", 1, 0))
        self._sock = s
        s.connect((self._host, self._port))
        buf = "\x00"
        buf += self._self_host.ljust(20, "\x00")
        buf += struct.pack("<I", self._self_port)
        buf += struct.pack(">I", 0)

        buf += self._host.ljust(20, "\x00")
        buf += struct.pack(">I", self._port)
        buf += struct.pack(">I", 0)
        self._protocol_head_len = len(buf)+4
        buf += "0x00"
        buf = struct.pack(">I", len(buf)) + buf
        
        s.send(buf)

    

        length = self._sock.recv(4, socket.MSG_WAITALL);
        ilength = struct.unpack(">I",length)[0]
        print ilength

        text = self._sock.recv(ilength, socket.MSG_WAITALL)
        print repr(text)
    def _recv(self):
        length = self._sock.recv(4, socket.MSG_WAITALL);
        ilength = struct.unpack(">I",length)[0]
        print ilength

        text = self._sock.recv(ilength, socket.MSG_WAITALL)
        return length+text
    def send(self, data):
        
        
        buf = "\x03"
        buf += self._self_host.ljust(20, "\x00")
        buf += struct.pack(">I", self._self_port)
        buf += struct.pack(">I", 0)

        buf += self._host.ljust(20, "\x00")
        buf += struct.pack(">I", self._port)
        buf += struct.pack(">I", self._bind)
        
        buf += json.dumps(data)
        buf = struct.pack(">I", len(buf)) + buf
        print repr(buf)
        print self._sock.send(buf)
        rt = self._recv()

        return json.loads(rt[self._protocol_head_len:])
    def recv(self):
        rt = self._recv()
        
        return json.loads(rt[self._protocol_head_len:])


if __name__ == "__main__":
    n = Node("0.0.0.0", 2345)
    rt = n.send({
            "host":n._self_host,
            "port":n._self_port,
            "src_gpid":0,
            "dev_type":"monitor",
            })

    bind_gpid = rt["bind_gpid"]

    print rt
