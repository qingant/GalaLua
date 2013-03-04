# -*- coding: utf-8 -*-
"""
    @auther ma.tao
"""

import node


n = node.Node("0.0.0.0", 2345)


rt = n.send({
            "host":n._self_host,
            "port":n._self_port,
            "src_gpid":0,
            "dev_type":"display",
            })
print rt

gpid = rt["bind_gpid"]
n._bind = gpid
rt = n.send({
        "host":n._self_host,
        "port":n._self_port,
        "src_gpid":0,
        "command":"register",
        "name":"ui@python@matao",
        "app_type":"",
        })

print rt

while True:
    rt = n.recv()
    print rt
