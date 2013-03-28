#!/usr/bin/python
# -*- coding: gbk -*-
import traceback
import datetime
import urwid
import urwid.raw_display
import time
import sys
from galaxy import language as lang
from galaxy import utils as galaxy_utils
from galaxy import supervisor_api as monitor
from galaxy import configure as configure
from galaxy import const as const
from galaxy import customize 
from cmd  import mg as mg
import sigar
import group
import logging
#import pprint
import copy

class MgPage(object):
    _tag=lang.MgPage.tag
    help_info=lang.MgPage.help_info
    def __init__(self, mp):
        self._mp = mp
        self.conf=self._mp._node.GetWorkGroupConfig(0)
    
    def get(self):
        ret=[]
        g_state = {True:lang.already_start, False:lang.already_stop}
        g_act = {True:(lang.stop,self.on_stop_click),False:(lang.start,self.on_start_click)}

        m = monitor.SupervisorApi(0, "user", "123")
        name = urwid.Text("%s"%(lang.ms_group, ))
        status=urwid.Text("[%s]"%(g_state.get(m.Ready())))

        userdata = {}
        userdata["index"] =0 
        userdata["conf"] = self.conf
        userdata["mon"] = m
        userdata['focus_index']=len(ret)
        choice=g_act.get(m.Ready())
        buttons=[(lang.confirm,"confirm",choice[1],userdata)]
        act = customize.PopUpWithCancel(choice[0],buttons)
        sta = urwid.AttrMap(customize.CButton(lang.cat_status,self.on_sta_click,userdata),"button_normal","button_select")

        bar = urwid.Columns([name,status,("fixed", 10, sta),("fixed",8,urwid.Text(" ")), ("fixed", 10, act)], focus_column = 2)
        ret.append(bar)
        return ret

    def on_sta_click(self, b,userdata):
        self._focus=userdata["focus_index"]
        del userdata["focus_index"]
        self._mp.next(MonitorServicePage, None, userdata)
    def on_stop_click(self, userdata):
        self._focus=userdata["focus_index"]
        i = userdata["index"]
        c = userdata["conf"]
        self._mp.next(group.StopPage,group.StopPage.start, iter([(i,c)]))
    def on_start_click(self,userdata):
        self._focus=userdata["focus_index"]
        i =userdata["index"]
        c =userdata["conf"]
        self._mp.next(group.StartPage,  group.StartPage.start, iter([(i,c)]))

class StatusPage(object):
    _tag=lang.StatusPage.tag
    help_info=lang.StatusPage.help_info
    def __init__(self, mp):
        self._mp = mp
    def get(self):
        ret = []
        g_state = {True:lang.already_start, False:lang.already_stop}
        g_act = {True:(lang.stop,self.on_stop_click), False:(lang.start,self.on_start_click)}
        
        for index, conf in self._mp._node.GetWGPList():
            m = monitor.SupervisorApi(index, "user", "123")
            alias=self._mp._node.GetWGName(index)
            if alias!='':
                alias="-%s"%alias
            name=urwid.Text("%s%d%s"%(lang.workgroup,index,alias))
            status=urwid.Text("[%s]"%( g_state.get(m.Ready())))

            b=customize.CButton("%s%d%s        [%s]"%(lang.workgroup,index,alias,g_state.get(m.Ready())))
            userdata={}
            userdata["index"] = index
            userdata["conf"] = conf
            userdata["mon"] = m
            userdata['focus_index']=len(ret)
            
            choice=g_act.get(m.Ready())

            buttons=[(lang.confirm,"confirm",choice[1],userdata)]


            act = customize.PopUpWithCancel(choice[0],buttons)
#            sta = customize.CButton(lang.cat_status,self.on_sta_click,userdata)
            sta = urwid.AttrMap(customize.CButton(lang.cat_status,self.on_sta_click,userdata),{},"button_select")

            bar = urwid.AttrMap(urwid.Columns([name,status,("fixed", 10, sta),("fixed", 8, urwid.Text(" ")), ("fixed", 10, act)], focus_column = 2),"body","line_select")
            ret.append(bar)

        return ret
        
    def on_sta_click(self,b,userdata):
        self._focus=userdata.pop("focus_index",-1)
        self._mp.next(GroupStatusPage, None, userdata)
    def on_stop_click(self,userdata):
        self._focus=userdata["focus_index"]
        i = userdata["index"]
        c = userdata["conf"]
        self._mp.next(group.StopPage,  group.StopPage.start, iter([(i,c)]))
    def on_start_click(self, userdata):
        self._focus=userdata["focus_index"]
        i = userdata["index"]
        c = userdata["conf"]
        self._mp.next(group.StartPage,  group.StartPage.start, iter([(i,c)]))

class MethodException(Exception):
    def __init__(self,val):
        self.val=val
    def __str__(self):
        return repr(self.val)

class ProcessTemplate(object):
    '''Template For SVCPage, MonitorServicePage,LSRPage,CNRPage'''
    def __init__(self,mp,group_arg):
        self._mp=mp
        self.group_arg=group_arg
        self._index= group_arg["index"]
        self._conf = group_arg["conf"]
        self._mon  = group_arg["mon"]

    def click(self, b,userdata):
        '''userdata: a dict has keys:"tag,index","title","token","focus_index"'''
        self._focus=userdata.pop("focus_index",-1)
        self._mp.next(ProcessPage, None, self,userdata)

    def show(self,sta,prompt,alias=""):
        if alias!="":
           alias="-%s"%alias
        raw=galaxy_utils.AlignmentStr("%s%s"%(prompt,alias),40)
        try:
            sg=self._mp._main_win.sg
            state=lang.ProcessState.state_map.get(sta["state"],lang.ProcessState.unknown)

#            vsize=sg.proc_mem(sta["pid"]).size()/(1024*1024)
#            cpupercent=sg.proc_cpu(sta["pid"]).percent()
            #raw = "%s%s :  %s[%s]  Pid[%d] cpu[%.1f%%] %s[%dM] "%(prompt,alias,lang.state,state,sta["pid"],cpupercent,lang.mem,vsize)
            raw = "%s[%s]"%(raw,state)
        except:
            state=lang.ProcessState.state_map.get(sta["state"],lang.ProcessState.unknown)
            raw = "%s[%s]"%(raw,state)
        return raw

    def get(self):
        '''pure virtual function,SubClass Must implement this function'''
        raise MethodException("You need to implement this Function")

class SVCPage(ProcessTemplate):
    _tag=lang.SVCPage.tag
    help_info=lang.SVCPage.help_info
    def get(self):
        ret=[urwid.Text("[%s]"%(lang.svc,))]
        for i in range(const.GALA_MAX_SVC):
            if not self._conf.GetSVCUsable(i):
                break
            token = "svc%.4d"%i
            title= "%s%d"%(lang.svc,i)
            sta = self._mon.get_info(token)
            raw=self.show(sta,title)

            userdata={}
            userdata["title"]=title
            userdata["tag"]=title
            userdata["token"]=token
            userdata["focus_index"]=len(ret)
            userdata["index"]=i

            b = urwid.AttrMap(customize.CButton(raw,self.click,userdata),"button_normal","button_select")
            #indent
            b=urwid.Padding(b,left=4)
            ret.append(b)
        return ret

class LSRPage(ProcessTemplate):
    _tag=lang.LSRPage.tag
    help_info=lang.LSRPage.help_info
    def get(self):
        ret=[urwid.Text("[%s]"%(lang.lsr,))]
        for i in range(const.GALA_MAX_LSR):
            if not self._conf.GetListenerUsable(i):
                continue
            token = "lsr%.4d"%i
            tag="%s%d"%(lang.lsr,i)
            sta = self._mon.get_info(token)

            alias=self._conf.GetLSRName(i)

            raw=self.show(sta,tag,alias)

            userdata={}
            userdata["tag"]=tag
            userdata["title"]=alias
            userdata["token"]=token
            userdata["focus_index"]=len(ret)
            userdata["index"]=i
            b = urwid.AttrMap(customize.CButton(raw,self.click,userdata),"button_normal","button_select")
            #for indent
            b=urwid.Padding(b,left=4)
            ret.append(b)

        return ret

class SVCOSPage(ProcessTemplate):
    '''SVC operation select page'''
    _tag=lang.SVCOSPage.tag
    help_info=lang.SVCOSPage.help_info
    def __init__(self,mp,group_arg):
        self._mp=mp
        self.group_arg=group_arg
    def get(self):
        ret=[]
        act = customize.PopUpWithCancel(lang.SVCOSPage.start,[(lang.confirm,"confirm",self.on_start_click,len(ret))])
        ret.append(act)
        act = customize.PopUpWithCancel(lang.SVCOSPage.stop,[(lang.confirm,"confirm",self.on_stop_click,len(ret))])
        ret.append(act)
        sta = urwid.AttrMap(customize.CButton(lang.SVCOSPage.add,self.on_add_click,len(ret)),"button_normal","button_select")
        ret.append(sta)
        sta = urwid.AttrMap(customize.CButton(lang.SVCOSPage.status,self.on_sta_click,len(ret)),"button_normal","button_select")
        ret.append(sta)

        return ret

    def on_sta_click(self, b,userdata):
        self._focus=userdata
        self._mp.next(SVCPage, None,self.group_arg)
    def on_stop_click(self, userdata):
        self._focus=userdata
        self._mp.next(SVCOprationPage,SVCOprationPage.do,self.group_arg,"stop")
    def on_start_click(self, userdata):
        self._focus=userdata
        self._mp.next(SVCOprationPage,SVCOprationPage.do,self.group_arg,"start")
    def on_add_click(self,b,userdata):
        self._focus=userdata

class SVCOprationPage(object):
    '''svc start/restart/stop all'''
    tags={"start":lang.start,"stop":lang.stop,"restart":lang.restart}
    def __init__(self,mp,group_arg,action):
        '''action must in [start,stop,restart]'''
        self._mp=mp
        self.group_arg=group_arg
        self._mon=self.group_arg["mon"]
        self.actions={"start":[self.start],"stop":[self.stop],"restart":[self.stop,self.start]}
        self._text=urwid.Text(u"")
        self.action=action
        self._tag="%s%s"%(self.tags[action],lang.svc)
        self._raw="[%s]"%(self._tag,)

    def OutputAndDraw(self, s, flag = False, demi = "\n"):
        self._text.set_text(self._raw + demi + s  )
        if flag:
            self._raw += "%s%s"%(demi, s)
        self._mp.refresh()

    def start(self, tok, name):
        try:
            if self._mon.get_info(tok)["state"] == 20:
                raw = " "*5+"%-16s[%s]"%(name, lang.already_start)
                self.OutputAndDraw(raw, True)
                logging.info(" "+tok+":already started")
                return
            raw = " "*5+"%-16s[%s]"%(name, lang.running)
            self.OutputAndDraw(raw)
            self._mon.start(tok)
            raw = " "*5+"%-16s[%s]"%(name, lang.already_start)
            self.OutputAndDraw(raw, True)
            logging.info(" start   "+tok)
            return True
        except:
            logging.fatal(traceback.format_exc())

            raw =" "*5+ "%-16s[%s]"%(name, lang.fail)
            self.OutputAndDraw(raw, True)
            logging.warning(" start   "+tok+"  Failed")
            return False
    def stop(self, tok, name):
        try:
            if self._mon.get_info(tok)["state"] == 200:
                raw = " "*5+"%-16s[%s]"%(name, lang.already_stop)
                self.OutputAndDraw(raw, True)
                #self._mp.log.LoggerInfo(" "+tok+":already stopped")
                logging.info(" "+tok+":already stopped")
                return
            raw = " "*5+"%-16s[%s]"%(name, lang.running)
            self.OutputAndDraw(raw)
            self._mon.stop(tok)
            raw = " "*5+"%-16s[%s]"%(name, lang.already_stop)
            self.OutputAndDraw(raw, True)
            logging.info(" stop   "+tok)
            return True
        except:
            raw = " "*5+"%-16s[%s]"%(name, lang.fail)
            self.OutputAndDraw(raw, True)
            logging.info("stop   "+tok+"  Failed")
            return False

    def do(self):
        action=self.action
        for i in range(const.GALA_MAX_SVC):
            if not self.group_arg["conf"].GetSVCUsable(i):
                break
            token = "svc%.4d"%i
            for func in self.actions[action]:
                func(token,lang.svc+str(i))
    def get(self):
        return [self._text]

class CNRPage(ProcessTemplate):
    _tag=lang.CNRPage.tag
    help_info=lang.CNRPage.help_info
    def get(self):
        ret=[urwid.Text("[%s]"%(lang.cnr,))]
        for i in range(const.GALA_MAX_CNR):
            if not self._conf.GetCNRUsable(i):
                continue
            token = "cnr%.4d"%i
            tag="%s%d"%(lang.cnr,i)
            sta = self._mon.get_info(token)
            alias=self._conf.GetCNRName(i)

            userdata={}
            userdata["title"]=alias
            userdata["tag"]=tag
            userdata["token"]=token
            userdata["focus_index"]=len(ret)
            userdata["index"]=i
            
            raw=self.show(sta,tag,alias)

            b = urwid.AttrMap(customize.CButton(raw,self.click,userdata),"button_normal","button_select")
            #for indent
            b=urwid.Padding(b,left=4)
            ret.append(b)
        return ret
        
class MonitorServicePage(ProcessTemplate):
    _tag=lang.ms_group
#    help_info=lang.MonitorServicePage.help_info
    def get(self):
        ret=[]
        if self._mon.Ready():
            raw = "  %s : Pid[%d]  State [%s]"%(lang.ms_guard, self._mon.GetHandle().supervisor.getPID(), lang.running)
            b=urwid.Text(raw)
            ret.append(b)
        else:
            raw = "%s : Pid[ ]  State [%s]"%(lang.ms_guard, lang.already_stop)
            b =  urwid.Text(raw)
            ret.append(b)
            return ret
 
        for i in range(const.GALA_MAX_CNR):
            if not self._conf.GetCNRUsable(i):
                continue
            token = "cnr%.4d"%i
            title="%s%d"%(lang.cnr,i)
            sta = self._mon.get_info(token)
            alias=self._conf.GetCNRName(i)

            userdata={}
            userdata["tag"]=title
            userdata["title"]=alias
            userdata["token"]=token
            userdata["focus_index"]=len(ret)
            userdata["index"]=i
            
            raw=self.show(sta,title,alias)

            b = urwid.AttrMap(customize.CButton(raw,self.click,userdata),"button_normal","button_select")
            #for indent
            b=urwid.Padding(b,left=4)
            ret.append(b)

        for i in range(const.GALA_MAX_CTR):
            if not self._conf.GetCTRUsable(i):
                continue
            token = "ctr%.4d"%i
            title="%s%d"%(lang.ctr,i)
            sta = self._mon.get_info(token)
            alias=self._conf.GetCTRName(i)

            userdata={}
            userdata["tag"]=title
            userdata["title"]=alias
            userdata["token"]=token
            userdata["focus_index"]=len(ret)
            userdata["index"]=i
            
            raw=self.show(sta,title,alias)

            b = urwid.AttrMap(customize.CButton(raw,self.click,userdata),"button_normal","button_select")
            #for indent
            b=urwid.Padding(b,left=4)
            ret.append(b)
        for i in range(const.GALA_MAX_TRIGGER):
            if not self._conf.GetTriggerUsable(i):
                continue
            token = "trigger%.4d"%i
            title="%s%d"%(lang.trigger,i)
            sta = self._mon.get_info(token)
            alias=self._conf.GetTriggerName(i)

            userdata={}
            userdata["tag"]=title
            userdata["title"]=alias
            userdata["token"]=token
            userdata["focus_index"]=len(ret)
            userdata["index"]=i
            
            raw=self.show(sta,title,alias)

            b = urwid.AttrMap(customize.CButton(raw,self.click,userdata),"button_normal","button_select")
            #for indent
            b=urwid.Padding(b,left=4)
            ret.append(b)



        for i in range(const.GALA_MAX_SVC):
            if not self._conf.GetSVCUsable(i):
                break
            token = "svc%.4d"%i
            title= "%s%d"%(lang.svc,i)
            sta = self._mon.get_info(token)
            raw=self.show(sta,title)
            userdata={}
            userdata["title"]=title
            userdata["tag"]=title
            userdata["token"]=token
            userdata["focus_index"]=len(ret)
            userdata["index"]=i

            b = urwid.AttrMap(customize.CButton(raw,self.click,userdata),"button_normal","button_select")
            #indent
            b=urwid.Padding(b,left=4)
            ret.append(b)

        for i in range(const.GALA_MAX_LSR):
            if not self._conf.GetListenerUsable(i):
                continue
            token = "lsr%.4d"%i
            title="%s%d"%(lang.lsr,i)
            sta = self._mon.get_info(token)

            alias=self._conf.GetLSRName(i)
            raw=self.show(sta,title,alias)

            userdata={}
            userdata["title"]=alias
            userdata["tag"]=title
            userdata["token"]=token
            userdata["focus_index"]=len(ret)
            userdata["index"]=i
            b = urwid.AttrMap(customize.CButton(raw,self.click,userdata),"button_normal","button_select")
            #for indent
            b=urwid.Padding(b,left=4)
            ret.append(b)

        for i,n in self._mp.MG.getStartAll():
            sta = self._mon.get_info(i)
            raw=self.show(sta,n)

            userdata={}
            userdata["title"]=n
            userdata["tag"]=n
            userdata["token"]=i
            userdata["index"]=0
            userdata["focus_index"]=len(ret)
            b=urwid.AttrMap(customize.CButton(raw,self.click,userdata),"button_normal","button_select")

            b=urwid.Padding(b,left=4)
            ret.append(b)
        return ret

class GroupStatusPage(object):
    #help_info=lang.GroupStatusPage.help_info
    def __init__(self, mp, group_arg):
        self._mp = mp
        self.group_arg=group_arg
        self._index= group_arg["index"]
        self._conf = group_arg["conf"]
        self._mon  = group_arg["mon"]
        self._tag="%s%d"%(lang.workgroup,self._index)
        self.all_items={"svc":SVCOSPage,"lsr":LSRPage,"cnr":CNRPage}
        self.title=lang.wg_guard
        self.Render()

    def Render(self):
        self.ret=[]
        if self._mon.Ready():
            raw = "  %s : Pid[%d]  State [%s]"%(self.title, self._mon.GetHandle().supervisor.getPID(), lang.running)
            b=urwid.Text(raw)
            self.ret.insert(0,urwid.Divider())
            self.ret.insert(0,b)
        else:
            raw = "%s : Pid[ ]  State [%s]"%(self.title, lang.already_stop)
            self.ret =[ urwid.Text(raw)]
            return 
        userdata={}
        userdata["type"]="cnr" 
        userdata["focus_index"]=len(self.ret)
        cnr=urwid.AttrMap(customize.CButton(lang.cnr,self.click,copy.deepcopy(userdata)),"button_normal","button_select")
        self.ret.append(cnr)
        userdata["type"]="svc" 
        userdata["focus_index"]=len(self.ret)
        self.ret.append(urwid.AttrMap(customize.CButton(lang.svc,self.click,copy.deepcopy(userdata)),"button_normal","button_select"))
        userdata["type"]="lsr" 
        userdata["focus_index"]=len(self.ret)
        self.ret.append(urwid.AttrMap(customize.CButton(lang.lsr,self.click,copy.deepcopy(userdata)),"button_normal","button_select"))

        return self.ret

    def click(self,button,userdata):
        self._focus=userdata["focus_index"]
        type=userdata["type"]
        self._mp.next(self.all_items[type],None,self.group_arg)

    def get(self):
        return self.ret
      
class ProcessPage(object):
    #has_info=''
    help_info=lang.ProcessPage.help_info
    def __init__(self, mp, group,process_arg):
        self._mp = mp
        self._group = group
        self._alias= process_arg["title"]
        self._token = process_arg["token"]
        self._groupindex=process_arg["index"]

        self._tag=process_arg["tag"]
        self._title="[%s]"%self._tag

        self._stop = customize.PopUpWithCancel(lang.stop,[(lang.confirm,"confirm",self.on_stop_click,None)])
        self._restart= customize.PopUpWithCancel(lang.restart,[(lang.confirm,"confirm",self.on_restart_click,None)])
        self._start= customize.PopUpWithCancel(lang.start,[(lang.confirm,"confirm",self.on_start_click,None)])
        self._refresh=urwid.AttrMap(customize.CButton(lang.refresh,self.on_refresh_click),"button_normal","button_select")
    
    def show_line(self,key,value):
        '''key     value'''
        k=urwid.Text("%s"%key)
        v=urwid.Text("%s"%value)
        self.lines.append(urwid.Columns([k,v,urwid.Divider()]))

    def get(self):
        self.lines=[urwid.Text(self._title)]
        button=[self._refresh]

        self._info= self._group._mon.get_info(self._token)

        stop=datetime.datetime.fromtimestamp(self._info['stop']).strftime('%Y-%m-%d %H:%M:%S')
        now=datetime.datetime.fromtimestamp(self._info['now']).strftime('%Y-%m-%d %H:%M:%S')

        uptime=datetime.timedelta(seconds=self._info['now']-self._info['start'])

        start=datetime.datetime.fromtimestamp(self._info['start']).strftime('%Y-%m-%d %H:%M:%S')

        if self.is_stopped():
                self.show_line(lang.ProcessPage.start_time,start)
                self.show_line(lang.ProcessPage.stop_time,stop)
                button.append(self._start)
        else:
                sg=self._mp._main_win.sg
                procmem=sg.proc_mem(self._info["pid"])
                vsize=procmem.size()/(1024*1024)
                rss=procmem.resident()/(1024*1024)

                percent=sg.proc_cpu(self._info["pid"])
                percent=percent.percent()

                self.show_line(lang.ProcessPage.process_name,self._alias)
                self.show_line("PID","%(pid)d "%self._info)
                self.show_line(lang.ProcessPage.cpu,"%.1f%%"%(percent))

                self.show_line(lang.ProcessPage.mem,"%dM"%vsize)
                self.show_line(lang.ProcessPage.rss,"%dM"%(rss))
                self.show_line(lang.ProcessPage.start_time,start)
                self.show_line(lang.ProcessPage.uptime,uptime)

                button.append(self._stop)
                button.append(self._restart)
                
        button.append(urwid.Divider())
        self.lines.append(urwid.GridFlow(button,8,8,3,'center'))
        return self.lines
        
    def is_stopped(self):
        if self._info['pid'] ==0:
            return True

    def draw(self,line,text,status):
        self._mp._ui_obj.modify(line,text,status)
        self._mp.refresh()

    def click(self,action):
        #For self._mp.pre()
        self._focus=-1
        line=range(1)
        prompt=[lang.stopping]
        func=[self._group._mon.stop]

        if action=="stop":
            line=range(1)
            tag=lang.stop
        elif action=="start":
            prompt=[lang.starting]
            func=[self._group._mon.start]
            tag=lang.start
        elif action=="restart":
            line=range(2)
            prompt.append(lang.starting)
            func.append(self._group._mon.start)
            tag=lang.restart

        self._mp.next(ProcessControlPage,None,self._title,tag);
        workgroup=" WorkGroup<%d>:"%self._groupindex

        for i in range(len(func)):
            try:
                self.draw(line[i],prompt[i],"[....]")
                arg="%(group)s:%(name)s"%self._info
                func[i](arg)
                self.draw(line[i],prompt[i],"[ OK ]")
            except:
                #print >>sys.stderr,action,'err'
                self.draw(line[i],prompt[i],"[Fail]")
                #self._mp.log.LoggerWarn(workgroup+action+'  '+arg+"   Failed")
                logging.warning(workgroup+action+'  '+arg+"   Failed")

                #Donot go back to previous page if error occurs
                return 
        #self._mp.log.LoggerInfo(workgroup+action+'  '+self._token)
        logging.info(workgroup+action+'  '+self._token)
        self._mp.pre()

    def on_stop_click(self):
        self.click("stop");
    def on_start_click(self):
        self.click("start");

    def on_restart_click(self):
        self.click("restart");
    def on_refresh_click(self,button):
        self._mp._main_win.refresh()
class ProcessControlPage:
    def __init__(self,mp,title,tag,lines=2):
        self._mp=mp
        self._title=title
        self._lines=lines
        self._tag="%s"%(tag)
        
    def get(self):
        title=urwid.Text(self._title)
        self.screen=[title]
        self.text=[]
        self.status=[]
        for i in range(self._lines):
            self.text.append(urwid.Text(u""))
            self.status.append(urwid.Text(u" "))
            self.screen.append(urwid.Columns([self.text[i],self.status[i]]))

        return [urwid.Pile(self.screen)]

    def modify(self,line,text,status):
        self.text[line].set_text(text)
        self.status[line].set_text(status)

