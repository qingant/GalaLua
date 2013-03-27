# -*- coding: utf-8 -*-
import sys,os
import web
#import afa_context as context
from galaxy import supervisor_api as supervisor
from galaxy import configure
from galaxy import const as const
from galaxy import utils as galaxy_utils
from galaxy import language as lang
from cmd import mg as mg
import logging
import traceback

import html_section as section

class ProcessStates:
    STOPPED = 0
    STARTING = 10
    RUNNING = 20
    BACKOFF = 30
    STOPPING = 40
    EXITED = 100
    FATAL = 200
    UNKNOWN = 1000

STOPPED_STATES = (ProcessStates.STOPPED,
                  ProcessStates.EXITED,
                  ProcessStates.FATAL,
                  ProcessStates.UNKNOWN)

RUNNING_STATES = (ProcessStates.RUNNING,
                  ProcessStates.BACKOFF,
                  ProcessStates.STARTING)

def css_class_for_state(state):
    if state == ProcessStates.RUNNING:
        return 'statusrunning'
    elif state in (ProcessStates.FATAL, ProcessStates.BACKOFF):
        return 'statuserror'
    else:
        return 'statusnominal'

class SupervisorStates:
    FATAL = 2
    RUNNING = 1
    RESTARTING = 0
    SHUTDOWN = -1

def super_css_class_for_state(state):
    if state == SupervisorStates.RUNNING:
        return 'statusrunning'
    elif state == SupervisorStates.FATAL:
        return 'statuserror'
    else:
        return 'statusnominal'

superConfFile=os.path.join(galaxy_utils.getInstanceHome(),"var/supervisor/conf/group0000.conf")
superConf=mg.Supervisor_Config_Parse(superConfFile)
config=configure.Configure(mode="w")

class ControlProcess(object):
    """
    """
    def __init__(self, group,token, name, monitor=None,result_queue=None):
        """
        """
        self._group=group
        self._token=token
        self._name=name
        self._result_queue=result_queue
        if monitor is None:
            self._monitor=supervisor.SupervisorApi(self._group,"user","123")
        else:
            self._monitor=monitor

    def start(self, flag=False):
        """
        """
        result={}
        result["__name__"]=self._name
        result["__type__"]="proc"
        try:
            if self._monitor.get_info(self._token)["state"] == 20:
                result["__status__"]=lang.already_start
                self._result_queue.put(result)
                return True

            result["__status__"]=lang.running
            self._result_queue.put(result)
            self._monitor.start(self._token)
            result["__status__"]=lang.already_start
            self._result_queue.put(result)

            if flag==True:
                result["__finish__"]="Finish"
                result_queue.put(result)
            return True
        except:
            result["__status__"]=lang.fail
            self._result_queue.put(result)
            return False

    def stop(self, flag=False):
        """
        """
        result={}
        result["__name__"]=self._name
        result["__type__"]="proc"
        try:
            if self._monitor.get_info(self._token)["state"] == 200:
                result["__status__"]=lang.already_stop
                self._result_queue.put(result)
                return True

            result["__status__"]=lang.running
            self._result_queue.put(result)
            self._monitor.stop(self._token)
            result["__status__"]=lang.already_stop
            self._result_queue.put(result)

            if flag==True:
                result["__finish__"]="Finish"
                self._result_queue.put(result)
                
            return True
        except:
            print sys.exc_info()[0]
            print traceback.format_exc()
            result["__status__"]=lang.fail
            self._result_queue.put(result)
            return False

    def status(self, flag=False):
        """
        """
        pass

    def restart(self,flag=False):
        """其实不应该有restart逻辑在里面
        """
        self._monitor.stop(self._token)
        self._monitor.start(self._token)
        
        if flag==True:
            #self._result_queue.put("Finish")
            pass

    def render(self,):
        """web展示接口
        """
        proinfo=self._monitor.get_info(self._token)
        proinfo["spawnerr"]=css_class_for_state(proinfo["state"])
        html=section.PRO_CTX%proinfo

        proinfo["group"]=self._group
        proinfo["token"]=self._token
        if proinfo["state"]!=ProcessStates.RUNNING:
            html+=section.PRO_STOP_ACTION%proinfo
        else:
            html+=section.PRO_START_ACTION%proinfo
        
        return html

class ControlGroup(object):
    """
    """
    def __init__(self,index,conf,result_queue=None):
        """
        index: 平台中该工作组编号
        conf: 工作组配置
        self._monitor: 一个工作组一个监控，AFA3.0对应的是每个进程一个守护
        self._superConf: 该工作组对应的启动配置,AFA4.0对于supervisor的配置
        """
        self._index=index
        self._conf=conf
        self._result_queue=result_queue
        self._monitor=supervisor.SupervisorApi(self._index,"user","123")
        #self._superConf=superConf

    def stopMonitor(self,name=lang.wg_guard,flag=False):
        """
        """
        result={}
        result["__name__"]=name
        result["__type__"]="mon"
        try:
            self._monitor.shutdown()
            result["__status__"]=lang.already_stop
            self._result_queue.put(result)

            if flag==True:
                #self._result_queue.put("Finish")
                pass
                
            return True
        except:
            result["__status__"]=lang.fail
            return False
        
    def startMonitor(self,name=lang.wg_guard,flag=False):
        """
        """
        result={}
        result["__name__"]=name
        result["__type__"]="mon"
        try:
            self._monitor.create()
            result["__status__"]=lang.already_start
            self._result_queue.put(result)

            if flag==True:
                #self._result_queue.put("Finish")
                pass
                
            return True
        except:
            result["__status__"]=lang.fail
            self._result_queue.put(result)
            return False
    
    def startGroup(self, flag=False):
        """
        """
        print "start Group"
        result={}
        result["__type__"]="grp"

        result["__name__"]=lang.workgroup
        self._result_queue.put(result)
        
        galaxy_utils.checkAndVolidateGroup(self._index)
        #result["__name__"]=lang.start_page_guard
        self._result_queue.put(result)
        
        if self._monitor.Ready():
            #result["__name__"]=lang.start_page_guard
            pass
                
            result["__status__"]=lang.already_start
            self._result_queue.put(result)
        else:
            if not self.startMonitor():
                return
                
        for var in range(const.GALA_MAX_CNR):
            if not self._conf.GetCNRUsable(var):
                break
            token = "cnr%.4d"%var
            name=lang.cnr+str(var)
            proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
            proc.start()

        for var in range(const.GALA_MAX_SVC):
            if not self._conf.GetSVCUsable(var):
                break
            token = "svc%.4d"%var
            name=lang.svc+str(var)
            proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
            proc.start()

        for var in range(const.GALA_MAX_LSR):
            if not self._conf.GetListenerUsable(var):
                break
            token = "lsr%.4d"%var
            name=lang.lsr+"4%d",str(var)
            proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
            proc.start()

        if flag==True:
            
            #result_queue.put("Finish")
            return True
                    
    def stopGroup(self, flag=False):
        """
        """
        if not self._monitor.Ready():
            print "Group monitor Not Ready"
        else:
            print "Group Monitor Ready"
            for var in range(const.GALA_MAX_LSR):
                if not self._conf.GetListenerUsable(var):
                    break
                token = "lsr%.4d"%var
                name=lang.lsr+str(var)
                proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
                proc.stop()
            
            for var in range(const.GALA_MAX_SVC):
                if not self._conf.GetSVCUsable(var):
                    break
                token = "svc%.4d"%var
                name=lang.svc
                proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
                proc.stop()

            for var in range(const.GALA_MAX_CNR):
                if not self._conf.GetCNRUsable(var):
                    break
                token = "cnr%.4d"%var
                name=lang.cnr+str(var)
                proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
                proc.stop()
                
            #stop the group monitor 
            self.stopMonitor()
            
    def statusGroup(self, ):
        """
        """
        pass
    
    def getMonitorRender(self,):
        """web展示内容接口
        """
        dic={}
        dic["id"]="%dmon"%self._index
        dic["group"]=self._index
        dic["class"]=""
        dic["name"]=lang.wg_guard
        state=self._monitor.native_getState()
        dic["description"]="pid %d"%self._monitor.getPid()
        dic["spawnerr"]=super_css_class_for_state(state["statename"])
        dic["statename"]=state["statename"]
        
        html=u""
        html+=section.GROUP_MON_HEAD%dic
        html+=section.TABLE_HEAD
        html+=section.PRO_HEAD%dic
        html+=section.PRO_CTX%dic
        if self._monitor.getState()==True:
            html+=section.MON_START_ACTION
        else:
            html+=section.MON_STOP_ACTION
        html+=section.PRO_TAIL
        html+=section.TABLE_TAIL
        html+=section.GROUP_MON_TAIL
        return html
    
    def getServiceRender(self,):
        html=""
        dic={}
        dic["group"]=self._index
        dic["name"]=lang.lsr
        dic["body"]="%dlsrbody"%self._index
        html+=section.TABLE_TITLE%dic
        
        html+=section.TABLE_HEAD%dic
        for var in range(const.GALA_MAX_LSR):
            if not self._conf.GetListenerUsable(var):
                break
            token = "lsr%.4d"%var
            name=lang.lsr+str(var)
            dic["id"]="%d%s"%(self._index,token)
            if var%2==0:
                dic["class"]="shade"
            else:
                dic["class"]=""
            proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
            html+=section.PRO_HEAD%dic
            html+=proc.render()
            html+=section.PRO_TAIL
        html+=section.TABLE_TAIL

        dic["name"]=lang.svc
        dic["body"]="%dsvcbody"%self._index
        html+=section.TABLE_TITLE%dic
        
        html+=section.TABLE_HEAD%dic
        for var in range(const.GALA_MAX_SVC):
            if not self._conf.GetSVCUsable(var):
                break
            token = "svc%.4d"%var
            name=lang.svc
            dic["id"]="%d%s"%(self._index,token)
            if var%2==0:
                dic["class"]="shade"
            else:
                dic["class"]=""
            proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
            html+=section.PRO_HEAD%dic
            html+=proc.render()
            html+=section.PRO_TAIL
        html+=section.TABLE_TAIL

        dic["name"]=lang.cnr
        dic["body"]="%dcnrbody"%self._index
        html+=section.TABLE_TITLE%dic
        
        html+=section.TABLE_HEAD%dic
        for var in range(const.GALA_MAX_CNR):
            if not self._conf.GetCNRUsable(var):
                break
            token = "cnr%.4d"%var
            name=lang.cnr+str(var)
            dic["id"]="%d%s"%(self._index,token)
            if var%2==0:
                dic["class"]="shade"
            else:
                dic["class"]=""
            proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
            html+=section.PRO_HEAD%dic
            html+=proc.render()
            html+=section.PRO_TAIL
            
        html+=section.TABLE_TAIL

        return html

    def render(self,):
        """web展示内容接口
        """
        html=""
        html+=self.getMonitorRender()
        
        dic={}
        dic["name"]=lang.workgroup
        dic["group"]=self._index
        html+=section.GROUP_SER_HEAD%dic
        html+=self.getServiceRender()
        html+=section.GROUP_SER_TAIL

        return html

class ControlMonitorServiceGroup(ControlGroup):
    """工作组0000被当作是 监控服务工作组
    """
    def __init__(self,index,conf,result_queue=None):
        """
        index: 平台中该工作组编号
        conf: 工作组配置
        result_queue: 结果队列
        self._monitor: 一个工作组一个监控，AFA3.0对应的是每个进程一个守护
        self._superConf: 该工作组对应的启动配置,AFA4.0对于supervisor的配置
        """
        self._index=index
        self._conf=conf
        self._result_queue=result_queue
        self._monitor=supervisor.SupervisorApi(self._index,"user","123")
        self._superConf=superConf
        ControlGroup.__init__(self, self._index, self._conf, self._result_queue)

    def startGroup(self, flag=False):
        """工作组0为：监控服务
        """
        result={}
        result["__type__"]="grp"

        result["__name__"]=lang.ms_group
        self._result_queue.put(result)
        
        galaxy_utils.checkAndVolidateGroup(self._index)
        #result["__name__"]=lang.monitor_service_guard
        self._result_queue.put(result)
        
        if self._monitor.Ready():
            #result["__name__"]=lang.monitor_service_guard
            result["__status__"]=lang.already_start
            self._result_queue.put(result)
        else:
            if not self.startMonitor():
                return

        #0工作组为管理工作组，启动webTools服务和应用部署服务 具体参加group0000.conf
        for token,name in superConf.getStartAll():
            proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
            proc.start()
                
        for var in range(const.GALA_MAX_CNR):
            if not self._conf.GetCNRUsable(var):
                break
            token = "cnr%.4d"%var
            name=lang.cnr+str(var)
            proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
            proc.start()

        for var in range(const.GALA_MAX_SVC):
            if not self._conf.GetSVCUsable(var):
                break
            token = "svc%.4d"%var
            name=lang.svc+str(var)
            proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
            proc.start()

        for var in range(const.GALA_MAX_LSR):
            if not self._conf.GetListenerUsable(var):
                break
            token = "lsr%.4d"%var
            name=lang.lsr+str(var)
            proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
            proc.start()

        if flag==True:
            self._result_queue.put("Finish")
            return True
                    
    def stopGroup(self, flag=False):
        """
        """
        if not self._monitor.Ready():
            print "Group monitor Not Ready"
        else:
            print "Group Monitor Ready"
            for var in range(const.GALA_MAX_LSR):
                if not self._conf.GetListenerUsable(var):
                    break
                token = "lsr%.4d"%var
                name=lang.lsr+str(var)
                proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
                proc.stop()
            
            for var in range(const.GALA_MAX_SVC):
                if not self._conf.GetSVCUsable(var):
                    break
                token = "svc%.4d"%var
                name=lang.svc
                proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
                proc.stop()

            for var in range(const.GALA_MAX_CNR):
                if not self._conf.GetCNRUsable(var):
                    break
                token = "cnr%.4d"%var
                name=lang.cnr+str(var)
                proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
                proc.stop()
                
            # services from configure file
            for token,name in superConf.getStartAll():
                proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
                proc.stop()
                
            #stop the group monitor 
            self.stopMonitor()
            
    def statusGroup(self, ):
        """
        """
        pass
    
    def getServiceRender(self,):
        html=""
        dic={}
        dic["group"]=self._index

        # services from configure file
        for token,name in superConf.getStartAll():
            dic["name"]=name
            dic["body"]="%d%sbody"%(self._index,token)
            html+=section.TABLE_TITLE%dic
            html+=section.TABLE_HEAD%dic
            dic["id"]="%d%s"%(self._index,token)
            dic["class"]=""
            proc=ControlProcess(self._index,token,name,self._monitor,self._result_queue)
            html+=section.PRO_HEAD%dic
            html+=proc.render()
            html+=section.PRO_TAIL
            html+=section.TABLE_TAIL
        html+=ControlGroup.getServiceRender(self)
        return html

    def render(self,):
        """web展示内容接口
        """
        html=u""
        html+=ControlGroup.getMonitorRender(self)
        
        dic={}
        dic["name"]=lang.ms_group
        dic["group"]=self._index
        html+=section.GROUP_SER_HEAD%dic
        html+=self.getServiceRender()
        html+=section.GROUP_SER_TAIL

        return html
    
class ControlPlatform(object):
    """
    """
    def __init__(self, config, result_queue=None):
        """
        config: 平台配置文件
        result_queue: 启动返回结果队列
        """
        self._config=config
        self._result_queue=result_queue
    
    def startPlatform(self, flag=False):
        """
        """
        try:
            monitor_service_group=self._config.GetWorkGroupConfig(0)
            control=ControlMonitorServiceGroup(0,monitor_service_group,self._result_queue)
            control.startGroup()
            
            groups=self._config.GetWGPList()
            for group in groups:
                control=ControlGroup(group[0],group[1],self._result_queue)
                control.startGroup()

            if flag==True:
                #self._result_queue.put("Finish")
                pass
        except:
            print sys.exc_info()[0]
            print traceback.format_exc()
    
    def stopPlatform(self, flag=False):
        """
        """
        try:
            monitor_service_group=self._config.GetWorkGroupConfig(0)
            control=ControlMonitorServiceGroup(0,monitor_service_group,self._result_queue)
            control.stopGroup()

            groups=self._config.GetWGPList()
            for group in groups:
                control=ControlGroup(group[0],group[1],self._result_queue)
                control.stopGroup()
                
            if flag==True:
                #self._result_queue.put("Finish")
                return
        except:
            print sys.exc_info()[0]
            print traceback.format_exc()
        
    def restartPlatform(self, ):
        """ no this logic
        """
        pass
    
    def statusPlatform(self,):
        """
        """
        pass
    

    def render(self,):
        """web展示内容接口
        """
        html=""
        dic={}
        dic["index"]=0
        dic["name"]=lang.ms_group+lang.workgroup
        html+=section.GROUP_HEAD%dic
        html+=section.GROUP_BODY_HEAD
        monitor_service_group=self._config.GetWorkGroupConfig(0)
        control=ControlMonitorServiceGroup(0,monitor_service_group,self._result_queue)
        html+=control.render()
        html+=section.GROUP_BODY_TAIL
        html+=section.GROUP_TAIL

        groups=self._config.GetWGPList()
        for group in groups:
            dic["index"]=group[0]
            dic["name"]=lang.workgroup+"%4d",group[0]
            html+=section.GROUP_HEAD%dic
            html+=section.GROUP_BODY_HEAD
            control=ControlGroup(group[0],group[1],self._result_queue)
            html+=control.render()
            html+=section.GROUP_BODY_TAIL
            html+=section.GROUP_TAIL

        return html

def command_dispatcher(result_queue,group,token,command):
    print group+token+command
    try:
        if group=="":
            plat=ControlPlatform(config,result_queue)
            plat.startPlatform()
            getattr(plat,command+"Platform")(flag=True)
        elif token=="":
            _group=int(group)
            if _group == 0:
                cfg=config.GetWorkGroupConfig(0)
                ctr=ControlMonitorServiceGroup(0,cfg,result_queue)
            else:
                cfg=config.GetWorkGroupConfig(_group)
                ctr=ControlGroup(_group,cfg,result_queue)
            getattr(ctr,command+"Group")(flag=True)
        else:
            _group=int(group)
            name=token
            proc=ControlProcess(_group,token,name,result_queue=result_queue)
            getattr(proc,command)(flag=True)
    except:
        print sys.exc_info()[0]
        print traceback.format_exc()
        return False
        
    result={}
    result["__finish__"]="Finish"
    result_queue.put(result)

if __name__ == '__main__':
    try:
        import Queue
        result=Queue.Queue()
        platform=ControlPlatform(config,result)
        #command_dispatcher(result,"0","svc0000","start")
        #command_dispatcher(result,"0","svc0001","start")
        #command_dispatcher(result,"0","svc0003","start")
        #command_dispatcher(result,"0","svc0002","start")
        command_dispatcher(result,"0","","start")
        #command_dispatcher(result,"0","","stop")
        #platform.startPlatform()
        # 
        ##print result.get()
        # 
        #import time
        #time.sleep(4)
        # 
        #platform.stopPlatform()
        f=open("test.html","w")
        import sys
        sys_encode = sys.getfilesystemencoding() 
        #f.write(platform.render().encode(sys_encode))
        #print platform.render()
    except:
        print sys.exc_info()[0]
        print traceback.format_exc()
