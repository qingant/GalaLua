# -*- coding: utf-8 -*-

import sys,os
import web
import afa_context as context
from galaxy import supervisor_api as supervisor
from galaxy import configure
from galaxy import const as const
from galaxy import utils as galaxy_utils
from cmd import mg as mg
import logging
import threading
import traceback
import Queue
import monitorLogic

config=configure.Configure()

def startPro(config,result_queue,flag=False):
    """
    """
    if flag==True:
        result_queue.put("Finish")

#def command_dispatcher(result_queue,group,token,command):
#    print group+token+command
#    
#    if token=="":
#        pass
#    elif group=="":
#        pass
#    else:
#        
#    
#    platform=monitorLogic.ControlPlatform(config, result_queue)
#    platform.startPlatform()
#    
#    result={}
#    result["__finish__"]="Finish"
#    result_queue.put(result)

def stopPlt(result_queue):
    #platform=monitorLogic.ControlPlatform(config, result_queue)
    #platform.stopPlatform()
    result_queue.put("Finish")

class look(object):
    """
    """
    platform_status=[]
    
    def GET(self, ):
        """
        """
        import Queue
        result=Queue.Queue()
        galaxy=monitorLogic.ControlPlatform(config,result)
        return context.render.manage_status(galaxy)
        
        

class start(object):
    """
    """
    result_dic={}
    def GET(self,):
        """
        token==""的时候，command对group起作用
        token！=""的时候，command对process起作用
        """
        param=web.input()
        print param
        command=param["command"]
        group=param["group"]
        token=param["token"]
        
        try:
            import time
            result_id=time.strftime("%d%H%M%S",time.localtime(time.time()))
            result_queue=Queue.Queue()
            if start.result_dic.has_key(result_id):
                result_id=result_id[1:len(result_id)]
            start.result_dic[result_id]=result_queue
           
            #thread start platform
            command_dispatcher_thread=threading.Thread(target=monitorLogic.command_dispatcher,args=[result_queue,group,token,command],name="command")
            command_dispatcher_thread.start()
            return result_id
        except:
            print sys.exc_info()[0]
            print traceback.format_exc()
    
    def POST(self,):
        """
        """
        param=web.input()
        print param
        result_id=param["result_id"]
        result_queue=start.result_dic[result_id]
        try:
            #result=result_queue.get(timeout=30)
            result=result_queue.get()
        except:
            print sys.exc_info()[0]
            print traceback.format_exc()
        if result.has_key("__finish__") == True:
            start.result_dic.pop(result_id)
            return "Finish"
        elif result.has_key("__name__") & result.has_key("__status__"):
            html="<p>"+result["__name__"]+"  "+result["__status__"]+"</p>"
            return html
        else:
            return str(result)

class PltStartResult(object):
    """
    """
    def GET(self, ):
        """
        """
        result=start.result_queue.get(timeout=30)

class stop(object):
    """
    """
    result_queue=Queue.Queue()
    def __init__(self, ):
        """
        """        
        pass
    def GET(self, ):
        """
        """
        plt_stop_thread=threading.Thread(target=stopPlt,args=[self.result_queue],name="pltstart thread")
        plt_stop_thread.start()
        return u"停止AFA4.0平台"
    
    def POST(self,):
        """
        """
        result=result_queue.get()
        return result

class status(object):
    """
    """
    def _init_(self, ):
        """
        """
        pass

    def GET(self, ):
        """
        """
        ret_html="<table id=\"\" class=\"vars\" style=\"display: block;\">\
        <thead>\
        <tr><th>Key</th><th>Value</th></tr>\
        </thead>\
        <tbody>"
        monitor=supervisor.SupervisorApi(0, "user", "123")
        dic=monitor.get_info("svc0000")
        for key,val in dic.items():
            ret_html+="<tr><td>"+str(key)+"</td><td class=\"code\"><div>"+str(val)+";</div></td></tr>"
        ret_html+="</tbody></table>"
        return ret_html
            
    
    def POST(self, ):
        """
        """
        param=web.input()
        print param
        group=param["group"]
        token=param["token"]
        if token != "" and group!="":
            #更新一个proc的 html
            index=int(group)
            name=token
            proc=monitorLogic.ControlProcess(index,token,name)
            return proc.render()
        elif group!="" and token=="":
            _group=int(group)
            if _group == 0:
                cfg=config.GetWorkGroupConfig(0)
                ctr=monitorLogic.ControlMonitorServiceGroup(0,cfg)
                return ctr.render()
            else:
                cfg=config.GetWorkGroupConfig(_group)
                ctr=monitorLogic.ControlGroup(_group,cfg,result_queue)
                return ctr.render()
        else:
            return
