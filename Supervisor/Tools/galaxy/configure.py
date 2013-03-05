#!/usr/bin/python
# -*- coding: gbk -*-

import os
import const
# base_dir = os.path.join(os.getenv("HOME"), "cfg/shmfile")
# path = os.path.join(base_dir, "node.conf")
# p = PyGalaxy.Configure(path)
# print p.Get("/Configure/_STRUCTURE/WorkGroupList[0]/WorkGroup/ListenerList[0]/Listener/Protocol/_SCROLLTYPE")

class GroupConfig(object):
    def __init__(self, node, index):
        self._node = node
        self._index = index
        
    def GetLSRName(self, index):
        return "lsr%.4d"%index
    def GetCNRName(self, index):
        return "cnr%.4d"%index
    def GetListenerUsable(self, index):
        return True
    def GetCNRUsable(self, idx):
        return True
    def GetSVCUsable(self, index):
        return True
    def GetListenerList(self):
        l = []
        for i in range(const.GALA_MAX_LSR):
            if self.GetListenerUsable(i):
                l.append(i)
        return l

class Configure(object):
    def __init__(self, tok = "node.conf",mode='r'):
        pass

    def GetWGName(self,index):
        return "group%.4d"%index
    def GetWorkGroupConfig(self, index):
        return GroupConfig(self, index)
        
    def GetWorkGroupUsable(self, index):
        return True
    def GetWGPList(self):
        return []
        
