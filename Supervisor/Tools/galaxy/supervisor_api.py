#!/usr/bin/env python

import xmlrpclib
import supervisor
import os
import traceback
import sys
import copy
import const
from supervisor import xmlrpc
import logging

SupervisorEntry= """
[unix_http_server]
file=%(home)s/var/supervisor/%(token)s.sock   ; (the path to the socket file)
chmod=0700                       ; sockef file mode (default 0700)

# [inet_http_server]
# port = 0.0.0.0:%(port)s
# username = user
# password = 123

[supervisord]
logfile=%(home)s/var/supervisor/log/supervisord_%(token)s.log ; (main log file;default $CWD/supervisord.log)
pidfile=%(home)s/var/supervisor/supervisord_%(token)s.pid ; (supervisord pidfile;default supervisord.pid)
childlogdir=%(home)s/var/supervisor/log            ; ('AUTO' child log dir, default $TEMP)


[supervisorctl]
serverurl=unix://%(home)s/var/supervisor/%(token)s.sock ; use a unix:// URL  for a unix socket
[rpcinterface:supervisor]
supervisor.rpcinterface_factory = supervisor.rpcinterface:make_main_rpcinterface
"""

ConfigureEntry= """
[program:%(token)s]
command=glr %(host)s %(port)d %(file)s %(entry)s
stdout_logfile=%(home)s/var/supervisor/log/%(token)s.log
autostart=false

"""
MoniterEntry="""
[program:%(token)s]
command=glr %(host)s %(port)d %(file)s %(entry)s
stdout_logfile=%(home)s/var/supervisor/log/%(token)s.log
autostart=false
"""

HOME=os.getenv("HOME")

'''FIXME::if process exit accidently,send message to moniter process'''
SVC_FILE=os.path.join(HOME,"lib/lua/svc.lua")
LSR_FILE=os.path.join(HOME,"lib/lua/lsr.lua")
Moniter_FILE=os.path.join(HOME,"lib/lua/moniter.lua")

class SupervisorApi(object):
    def __init__(self,index,user,passwd):
        self._home=HOME
        self._host="0.0.0.0"
        self._port=2345
        self._ready = False
        self._token="aim"
        self._un_sock= "unix://%s/var/supervisor/aim.sock"%(HOME)
        try:
            self.connect()
            self._ready = self.getState()
        except Exception,e:
            print >>sys.stderr,traceback.format_exc()

    def connect(self):
        '''connect to a xmlrpc server'''
        conn = xmlrpc.SupervisorTransport('', '', self._un_sock)
        self._handle = xmlrpclib.ServerProxy("http://127.0.0.1", conn)
    def Ready(self):
        return self._ready

    def GetHandle(self):
        return self._handle
    def _Methods(self):
        return self._methods

    def getState(self):
        state={0:True,-1:False,-2:False}
        def _getState():
            try:
                state=self._handle.supervisor.getState()
                if state['statecode']==1:
                    return 0
                return -1 
            except:
                return -2
        ret=_getState()
        return state[ret]

    def getPid(self):
        ''''''
        return self._handle.supervisor.getPID()

    def create(self):
        '''start supervisor '''
        path = "%s/var/supervisor/conf/%s.conf"%(self._home,self._token)
        def init_supervisord():
            '''execute supervisor'''
            cmd = "AIMGuard -c %s"%path
            os.system(cmd)

        def init_conf():
            '''initialize supervisor config file'''
            group = {}
            group["token"] = self._token
            group["home"] = self._home
            group["host"] = self._host
            group["port"] = self._port

            f = open(path, "w")
            f.write(SupervisorEntry%group)

            #f.write(MoniterEntry%group)

            lsr = copy.deepcopy(group)
            sec_lsr = []
            lsr["file"]=LSR_FILE
            lsr["entry"]="main"
            for i in range(const.GALA_MAX_LSR):
                lsr["token"] = "lsr%.4d"%i
                sec_lsr.append(ConfigureEntry%lsr)
            f.writelines(sec_lsr)

            sec_svc = []
            svc = copy.deepcopy(group)
            svc["file"]=SVC_FILE
            svc["entry"]="main"
            group["port"] = self._port+1
            for i in range(const.GALA_MAX_SVC):
                svc["token"] = "svc%.4d"%i
                sec_svc.append(ConfigureEntry%svc)
            f.writelines(sec_svc)

            f.close()

        try:
            init_conf()
            init_supervisord()

            self.connect()
            self._ready = True
        except:
            logging.warn(traceback.format_exc())

    def get_info(self, tok):
        '''get information of process tok'''
        return self._handle.supervisor.getProcessInfo(tok)

    def start(self, tok):
        '''start process name tok'''
        try:
            return self._handle.supervisor.startProcess(tok)
        except Exception,e:
            return False

    def stop(self, tok):
        '''stop process name tok'''
        try:
            return self._handle.supervisor.stopProcess(tok)
        except Exception,e:
            return False

    def shutdown(self):
        '''kill supervisor'''
        self._ready = False
        self._handle.supervisor.shutdown()

        def safe_unlink(file):
            try:
                os.unlink(file)
            except:
                print >>sys.stderr,traceback.format_exc()

        # delete the stale socket file 
        # because supervisor shutdown is too slow  
        # xmlrpc may connect to this stale socket file 
        # when the supervisor is not shutdown completly
        safe_unlink(self._un_sock[7:])

        
if __name__ == "__main__":
    s = SupervisorApi()
    if s._ready:
        print("is Ready")
        print("Now We will kill it")
        s.shutdown()
        print("*"*8)
    else:
        s.create()
        print("*"*8)
        print("starting lsr")
        s.start("lsr0000");
        print("*"*8)
