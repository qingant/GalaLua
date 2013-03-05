#!/usr/bin/env python
import urwid
import os
import ConfigParser
import sys

def listcomp(x,y):
    a=x[0]
    b=y[0]
    if a>b:
        return 1
    if a<b:
        return -1
    if a==b:
        return 0
class Supervisor_Config_Parse(object):
    '''only parse sections with startindex variable'''
    def __init__(self,file):
        self.f=ConfigParser.ConfigParser()
        self.f.read(file)

    def sections(self):
        return self.f.sections()

    def getvar(self,section,var):
        return self.f.get(section,var)

    def getAllProgram(self):
        '''Get All program sections'''
        all=[]
        for i in self.sections():
            if i.startswith("program:"):
                all.append(i)
        return all

    def getCustom(self):
        '''Get All custom sections'''
        all=self.getAllProgram()
        s=[]
        for i in all:
            if self.f.has_option(i,'startindex'):
                s.append(i)
        return s

    def getStartAll(self):
        return self.getUsable()
    def getStopAll(self):
        return self.getUsable(True)

    def getUsable(self,stop=False):
        '''get All available sections name in order'''
        s=[]
        return s
        for i in self.getCustom():
            try :
                #if error occurs just ignore it
                index=int(self.getvar(i,"startindex"))
            except:
                continue
            p=i.split("program:")[1]
            n=p
            if  self.f.has_option(i,"title"):
                n=self.getvar(i,"title").decode("utf8")

            s.append((index,p,n))
        #sorted by startindex
        s.sort(listcomp,reverse=stop)

        t=[]

        for i in s:
            t.append(i[1:])

        return t
            

class ConfigParse(object):
    '''
    # id for this item
    [example]
    # must specify 
    command="example arg"
    # optional default is %logpath/example.log
    log_file="path/to/log" 
    # optional default is false
    start=false  #start or not when start the monitor services
    '''
    pat='''
[program:%s]
command=%s
stdout_logfile=%s
autostart=false
'''
    def __init__(self,file):
        self.f=ConfigParser.ConfigParser()
        self.f.read(file)

    def sections(self):
        return self.f.sections()

    def getvar(self,section,var):
        return self.f.get(section,var)

    def getAll(self):
        '''Get All available sections'''
        all=[]
        for i in self.sections():
            if self.f.has_option(i,"command"):
                all.append(i)
        return all

    def getUsable(self):
        '''Get All start=true sections'''
        all=self.getAll()
        s=[]
        for i in all:
            if not self.f.has_option(i,'start') or self.getvar(i,'start')=='true':
                s.append(i)
        return s

    def get_item(self,section):
        cmd=self.getvar(section,"command")
        log="var/supervisor/log/%s.log"%section
        log=os.path.join(os.getenv("HOME"),log)
        if self.f.has_option(section,"stdout_logfile"):
            log=self.getvar(section,"stdout_logfile")
        return self.pat%(section,cmd,log)

    def get(self):
        item=''
        all=self.getAll()
        for i in all:
            cmd=self.getvar(i,"command")
            log="var/supervisor/log/%s.log"%i
            log=os.path.join(os.getenv("HOME"),log)
            if self.f.has_option(i,"stdout_logfile"):
                log=self.getvar(i,"stdout_logfile")
            item+=self.pat%(i,cmd,log)
        return item

class ProcessPage(object):
    def __init__(self,mp,str):
        self._mp=mp
        self._str=str
    def get(self):
        return urwid.Text(self._str)
        
def main():
    a=ConfigParse("/home/gala/.mg")
    with open("qq",'w') as f:
        print a.get()
        f.write(a.get())

if __name__=='__main__':
    main()
