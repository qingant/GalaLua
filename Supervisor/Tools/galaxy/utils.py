#!/usr/bin/python
# -*- coding: gbk -*-

#import PyGalaxy
import os

#FIXME
def getInstanceHome():
    ''' return root directory of Instance 
        MUST be absolute path'''

    return os.getenv("HOME")

#def createQueue(tok):
#    path = os.path.join(getInstanceHome(), "channels/%s.chl"%( tok))
#    #print path
#    try:
#        PyGalaxy.CFileMQCreator(path, 8192,8192, 16, 32)
#        return True
#    except:
#        return False
def checkAndVolidateGroup(index):
     return 
     path = os.path.join(getInstanceHome(), "channels/")
     if not os.path.exists(path):
         os.mkdir(path)
     path = os.path.join(getInstanceHome(), "channels/group%.4d.chl"%(index))
     if not os.path.exists(path):
         createQueue("group%.4d"%index)
    

def checkPathAndCreate():
    home = getInstanceHome()
    host = os.getenv("HOSTNAME")

    var = os.path.join(home, "var")
    cfg = os.path.join(home, "cfg")
    log =os.path.join(home,"log")
    if not os.path.exists(var):
        os.mkdir(var)
    if not os.path.exists(cfg):
        os.mkdir(cfg)

    if not os.path.exists(log):
        os.mkdir(log)

    sup = os.path.join(var, "supervisor")

    if not os.path.exists(sup):
        os.mkdir(sup)

    sup_log = os.path.join(sup, "log")

    if not os.path.exists(sup_log):
        os.mkdir(sup_log)

    sup_log = os.path.join(sup, "conf")
    if not os.path.exists(sup_log):
        os.mkdir(sup_log)

def unicode_str_width(str):
    '''str must be unicode,sometimes it contains Chinese 
       "gbk" char's length is equal with its width
       so this's how we calualate the width'''
    if not isinstance(str,unicode):
        return 0
    return len(str.encode("gbk"))       

def AlignmentStr(unistr,width=40):
    '''return Alignment string with width'''
    len=width/2

    unistr=unistr[0:len]
    word_width=width-unicode_str_width(unistr)
    spaces=" "*word_width

    ret="%s%s"%(unistr,spaces)

    return ret

    
if __name__ == "__main__":
    createQueue("testAMQ")
