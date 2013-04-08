#!/usr/bin/python
# -*- coding: gbk -*-
import PyGalaxy
import os
import const

class GroupConfig(object):
    def __init__(self, node, index):
        self._node = node
        self._index = index
        self._prefix = "/MonitorList[%d]/Monitor"%index
        
    def Get(self, path):
        return self._node.Get(self._prefix+path)
    def Put(self, path, value):
        return self._node.Put(self._prefix + path, value)

    def GetLSRName(self, index):
        rt =  self.Get("/LSRList[%d]/LSR/Alias"%index)
        return rt
    def GetCNRName(self, index):
        rt =  self.Get("/ConnectorList[%d]/Connector/Alias"%index)
        return rt

    def GetTriggerName(self,index):
        return "trigger0"
    def GetTriggerUsable(self,index):
        return True
    def GetTriggerList(self):
        l = []
        for i in range(const.GALA_MAX_TRIGGER):
            if self.GetTriggerUsable(i):
                l.append(i)
        return l

    def GetCTRName(self, index):
        return "ctr0"

    def GetCTRList(self):
        l = []
        for i in range(const.GALA_MAX_CTR):
            if self.GetCTRUsable(i):
                l.append(i)
        return l

    def GetCTRUsable(self,index):
        return True

    def GetListenerUsable(self, index):
        _d = {0:False, 1:True}
        rt =  self.Get("/LSRList[%d]/LSR/Usable"%index)
        rt = int(rt)
        return _d.get(rt)
    def GetCNRUsable(self, idx):
        _d = {0:False, 1:True}
        rt =  self.Get("/ConnectorList[%d]/Connector/Usable"%idx)
        rt = int(rt)
        return _d.get(rt)
    def GetSVCUsable(self, index):
        _d = {0:False, 1:True}
        rt =  self.Get("/Base/SVCLimits")
        rt = int(rt)
        return index < rt
    def GetListenerList(self):
        l = []
        for i in range(const.GALA_MAX_LSR):
            if self.GetListenerUsable(i):
                l.append(i)
        return l
class Configure(object):
    def __init__(self, tok = "node.conf",mode='r'):
        if os.path.isabs(tok):
            path=tok
        else :
            base_dir = os.path.join(os.getenv("HOME"), "cfg/shmfile")
            path = os.path.join(base_dir, tok)
        self._conf = PyGalaxy.Configure(path,mode)
        self._prefix = "/Configure/_STRUCTURE"

    def GetWGName(self,index):
        rt =  self.Get("/MonitorList[%d]/Monitor/Base/Alias"%index)
        return rt
    def GetWorkGroupConfig(self, index):
        return GroupConfig(self, index)
        
    def Get(self,path):
        #print self._prefix+path
        
        return self._conf.Get(self._prefix + path)
    def Put(self, path, value):
        return self._conf.Put(self._prefix + path, value)
    def GetWorkGroupUsable(self, index):
        _d = {0:False, 1:True}
        try:
            rt =  self.GetWorkGroupConfig(index).Get("/Usable")
        except:
            return False;
        rt = int(rt)
        return _d.get(rt)
    def GetWGPList(self):
        l = []
        # 0 for manager group
        for i in range(1,const.GALA_MAX_WGC):
            if self.GetWorkGroupUsable(i):
                l.append((i, self.GetWorkGroupConfig(i)))
        return l
    def NewGroup(self):
        # 0 for manager group
        for i in range(const.GALA_MAX_WGC):
            if not self.GetWorkGroupUsable(i):
                return (i, self.GetWorkGroupConfig(i))


if __name__ == "__main__":
    path="/home/gala/cfg/shmfile/node.conf"
#    print "path:",path
#    p = Configure(path,mode='w')
    p = Configure(mode='w')
    g = p.GetWorkGroupConfig(0)
#    g.Put("/Base/Alias",'kgroupname11j')
#    g.Put("/LSRList[0]/LSR/Protocol/_UNIONSELECTOR/LSRSTREAM/Address/_UNIONSELECTOR/TCP/Port",'10234')

    print g.Get("/Base/Alias")
    print g.Get("/LSRList[0]/LSR/Protocol/_UNIONSELECTOR/LSRSTREAM/Address/_UNIONSELECTOR/TCP/Port")

    print p.GetWorkGroupUsable(0)
    rt = g.Get("/LSRList[0]/LSR/Protocol/_SCROLLTYPE")
    print repr(rt)
    rt = g.Get("/LSRList[0]/LSR/Protocol/_UNIONSELECTOR/LSRSTREAM/Address/_UNIONSELECTOR/TCP/Port")
    print rt
    rt = g.Get("/Usable")
    print rt
    print "lsr0", g.GetListenerUsable(0)
    print "lsr1", g.GetListenerUsable(1)
    print "lsr2", g.GetListenerUsable(2)

    print g.GetSVCUsable(0)
    print g.GetSVCUsable(5)
    print repr(rt)
    grp=p.GetWGPList()
    print type(grp)
    print grp
    #print type(grp[0][1])
    #print grp[0][1].GetListenerList()
    print g.GetListenerList()
