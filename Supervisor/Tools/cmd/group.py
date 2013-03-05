#!/usr/bin/python
# -*- coding: gbk -*-

import urwid
import urwid.raw_display
import time
import sys
from galaxy import language as lang
from galaxy import supervisor_api as monitor
from galaxy import configure as configure
from galaxy import const as const
from galaxy import utils as galaxy_utils
from galaxy import customize 
from cmd import mg as mg
import thread
import utils
import traceback
import logging
import copy

TimedSignal = utils.TimedSignal

class SummaryPage(object):
    '''Summary of errors when Start/Stop the platform '''
    def __init__(self,mp,type="start",fail_list=[]):
        self._mp=mp
        self._tag=u"%s"%(lang.SummaryPage._tag)
        self.not_show=True
        self.fail_list=fail_list
        self.fail_button=[]
        self.render()

    def render(self):
        fail_button=[]
        fail_button.append(urwid.Text(u"%s:%d %s"%(lang.SummaryPage.err,len(self.fail_list),self.fail_list)))
        for i in range(len(self.fail_list)):
            if self.fail_list[i]==0:
                label=lang.ms_group
                focus=len(fail_button)
                fail_button.append(urwid.AttrMap(customize.CButton(label,self.on_click,(i,focus)),"button_normal","button_select"))
            else:
                label="%s%d"%(lang.workgroup,self.fail_list[i])
                focus=len(fail_button)
                fail_button.append(urwid.AttrMap(customize.CButton(label,self.on_click,(i,focus)),"button_normal","button_select"))
        self.fail_button=fail_button 

    def get(self):
        return self.fail_button

    def on_click(self,button,userdata):
        pos,self._focus=userdata
        self._mp.SafeGoto(pos,button.label)

class StopPage(object):
    '''stop platform or stop the specify group'''
    _focus=-1
    has_failed=[]  # group index failed starting process
    All=False    # True if start all group
    def __init__(self, mp, grp = None,restart=False,specify_group=False,not_show=False):
        '''not_show:tell the Navigation not to show'''
        self._mp = mp
        self._raw = lang.StopPage.title
        self._text = urwid.Text(self._raw)
        self._type=type
        self.not_show=not_show
        self.group_funcs={0:self.stop_manager_group}

        if grp is None:           #stop platform
            self._tag=lang.StopPage.tag
            self._grp=None
            StopPage.All=True
        else:
            l=list(grp)
            index=l[0][0]
            if index!=0:
                prompt="%s%d"%(lang.workgroup,l[0][0])
            else:
                prompt=lang.ms_group
            self._tag="%s%s"%(lang.stop,prompt)
            self._grp=iter(l)

        self.restart=restart
        self.specify_group=specify_group

    def get(self):
        return [self._text]

    def FailSummary(self,index):
        if index not in self.has_failed:
            self.has_failed.append(index)

    def OutputAndDraw(self, s, flag = False, demi = "\n"):
        self._text.set_text(self._raw + demi + s  )
        if flag:
            self._raw += "%s%s"%(demi, s)
        self._mp.refresh()

    def on_next(self):
        self._mp.next(StopPage,StopPage.start, self._grp,self.restart,self.specify_group,True)

    def stopMonitor(self,m,prompt=lang.wg_guard):
        '''stop supervisord'''
        try:
            prompt=galaxy_utils.AlignmentStr(prompt,40)
            raw =" "*5+"%s[%s]"%(prompt,lang.running)
            self.OutputAndDraw(raw)
            m.shutdown()
            raw = " "*5+"%s[%s]"%(prompt,lang.already_stop)
            self.OutputAndDraw(raw, True)
            return True
        except:
            raw =" "*5+"%s[%s]"%(prompt,lang.fail)
            self.OutputAndDraw(raw)
            return False

        
    def stopProcess(self, tok, name):
        '''True if no error occur
            maximum width 40'''
        try:
            name=galaxy_utils.AlignmentStr(name,40)

            if self._monitor.get_info(tok)["state"] == 200:
                raw = " "*5+"%s[%s]"%(name,lang.already_stop)
                self.OutputAndDraw(raw, True)
                logging.info(" "+tok+":already stopped")
                return True
            raw = " "*5+"%s[%s]"%(name,lang.running)
            self.OutputAndDraw(raw)
            self._monitor.stop(tok)
            raw = " "*5+"%s[%s]"%(name,lang.already_stop)
            self.OutputAndDraw(raw, True)
            logging.info(" stop   "+tok)
            return True
        except:
            logging.info("stop   "+tok+"  Failed")
            raw = " "*5+"%s[%s]"%(name,lang.fail)
            self.OutputAndDraw(raw, True)
            return False

    def stop_group(self,index,conf):
        logging.info("******BEGIN stoping WorkGroup<%d>******"%index)

        gmonitor = monitor.SupervisorApi(index, "user", "123")
        self._monitor = gmonitor

        self.OutputAndDraw("%s%d"%(lang.workgroup, index), True, "")
        time.sleep(0.5)

        if not gmonitor.Ready():
            name=galaxy_utils.AlignmentStr(lang.wg_guard,40)
            raw = " "*3+"%s[%s]"%(name, lang.already_stop)
            self.OutputAndDraw(raw, True)
        else:
            self.OutputAndDraw("\n  [%s]"%(lang.lsr,), True)        
            for i in range(const.GALA_MAX_LSR):
                if not conf.GetListenerUsable(i):
                    break
                token = "lsr%.4d"%i

                name="%s%d"%(lang.lsr,i)
                alias=conf.GetLSRName(i)
                if alias!='':
                    alias="-%s"%alias
                if not self.stopProcess(token,"%s%s"%(name,alias)):
                    self.FailSummary(index)
            
            self.OutputAndDraw("\n  [%s]"%(lang.svc,),True)

            for i in range(const.GALA_MAX_SVC):
                if not conf.GetSVCUsable(i):
                    break
                token = "svc%.4d"%i
                if not self.stopProcess(token,lang.svc+str(i)):
                    self.FailSummary(index)

            self.OutputAndDraw("\n  [%s]"%(lang.cnr,), True)        

            for i in range(const.GALA_MAX_CNR):
                if not conf.GetCNRUsable(i):
                    break
                token = "cnr%.4d"%i
                name="%s%d"%(lang.cnr,i)
                alias=conf.GetCNRName(i)
                if alias!='':
                    alias="-%s"%alias
                if not self.stopProcess(token,"%s%s"%(name,alias)):
                    self.FailSummary(index)

            self.OutputAndDraw("\n  [%s]"%(lang.wg_guard,), True)
            if not self.stopMonitor(gmonitor):
                self.FailSummary(index)
            
        logging.info("******FINISH stoping WorkGroup<%d>******"%index)

    def stop_manager_group(self,index,conf):
        '''stop manager group'''
        logging.info("******BEGIN stoping Monitor Services******")
        logging.info("%d"%index)

        gmonitor = monitor.SupervisorApi(index, "user", "123")
        self._monitor = gmonitor

        self.OutputAndDraw("%s"%(lang.ms_group), True, "")
        time.sleep(0.5)

        if not gmonitor.Ready():
            name=galaxy_utils.AlignmentStr(lang.ms_guard,40)
            raw = " "*3+"%s[%s]"%(name,lang.already_stop)
            self.OutputAndDraw(raw, True)
        else:
            self.OutputAndDraw("\n  [%s]"%(lang.ms_group), True)        
            for i in range(const.GALA_MAX_LSR):
                if not conf.GetListenerUsable(i):
                    break
                token = "lsr%.4d"%i
                name="%s%d"%(lang.lsr,i)
                alias=conf.GetLSRName(i)
                if alias!='':
                    alias="-%s"%(alias,)
                if not self.stopProcess(token,"%s%s"%(name,alias)):
                    self.FailSummary(index)
            
            for i in range(const.GALA_MAX_SVC):
                if not conf.GetSVCUsable(i):
                    break
                token = "svc%.4d"%i
                if not self.stopProcess(token,lang.svc+str(i)):
                    self.FailSummary(index)

            for i in range(const.GALA_MAX_CNR):
                if not conf.GetCNRUsable(i):
                    break
                token = "cnr%.4d"%i
                name="%s%d"%(lang.cnr,i)
                alias=conf.GetCNRName(i)
                if alias!='':
                    alias="-%s"%(alias)
                if not self.stopProcess(token,"%s%s"%(name,alias)):
                    self.FailSummary(index)
            # services from configure file
            for i,n in self._mp.MG.getStopAll():
                if not self.stopProcess(i,n):
                    self.FailSummary(index)

            self.OutputAndDraw("\n  [%s]"%(lang.ms_guard,), True)
            if not self.stopMonitor(gmonitor,lang.ms_guard):
                self.FailSummary(index)
            
        logging.info("******FINISH stoping Monitor Services******")

    def start(self):
        try:
            self._mp.set_filter()
            self._start()
        except:         # you can't use finally  here
            logging.fatal(traceback.format_exc())
            self._mp.set_filter(False)
            
    def _start(self):
        self._mp.set_filter()
        if self._grp is None:
            glist=self._mp._node.GetWGPList()
            # and finally stop monitor services 
            conf=self._mp._node.GetWorkGroupConfig(0)
            glist.append((0,conf))
            self._grp = iter(glist)

        index, conf =  self._grp.next()

        func=self.group_funcs.get(index,self.stop_group)
        func(index,conf)

        if self._grp.__length_hint__() != 0:
            TimedSignal(self._text, "next", 1, self.on_next)
        else:
            if StopPage.All:
                StopPage.All=False
                l=copy.deepcopy(self.has_failed)
                del self.has_failed[0:]
                self._mp.next(SummaryPage,None,"stop",l)
            self._mp.set_filter(False)

class StartPage(object):
    _focus=-1
    has_failed=[]  # group index failed starting process
    All=False    # True if start all group
    def __init__(self, mp, grp = None,not_show=False):
        self._mp = mp
        self._raw = lang.StartPage.title
        self._text = urwid.Text(self._raw)
        self.not_show=not_show
        self.group_funcs={0:self.start_manager_group}
        if grp is None:
            self._grp = None
            self._tag=lang.StartPage.tag
            StartPage.All=True
        else:
            l=list(grp)

            if l[0][0]!=0:
                self._tag="%s%s%d"%(lang.start,lang.workgroup,l[0][0])
            else:
                self._tag="%s%s"%(lang.start,lang.ms_group)
            self._grp=iter(l)
        self._mp.set_filter()
  
    def get(self):
        return [self._text]

    def on_next(self):
        self._mp.next(type(self),getattr(type(self),"start"),self._grp,True)
    def OutputAndDraw(self, s, flag = False, demi = "\n"):
        self._text.set_text(self._raw + demi + s  )
        if flag:
            self._raw += "%s%s"%(demi, s)
        self._mp.refresh()

    def FailSummary(self,index):
        '''Summary failure'''
        if index not in self.has_failed:
            self.has_failed.append(index)

    def startMonitor(self,m,prompt=lang.wg_guard):
        ''''''
        try:
            prompt=galaxy_utils.AlignmentStr(prompt,40)

            raw =" "*5+"%s[%s]"%(prompt,lang.running)
            self.OutputAndDraw(raw)
            m.create()
            raw = " "*5+"%s[%s]"%(prompt,lang.already_start)
            self.OutputAndDraw(raw, True)
            return True
        except:
            raw = " "*5+"%s[%s]"%(prompt,lang.fail)
            self.OutputAndDraw(raw)
            return False
            
    def startProcess(self, tok, name):
        '''started :True 
           Maximum width:40'''
        try:
            name=galaxy_utils.AlignmentStr(name,40)

            if self._monitor.get_info(tok)["state"] == 20:
                raw = " "*5+"%s[%s]"%(name,lang.already_start)
                self.OutputAndDraw(raw, True)
                logging.info(" "+tok+":already started")
                return True
            raw = " "*5+"%s[%s]"%(name, lang.running)
            self.OutputAndDraw(raw)
            self._monitor.start(tok)
            raw = " "*5+"%s[%s]"%(name,lang.already_start)
            self.OutputAndDraw(raw, True)
            logging.info(" start   "+tok)
            return True
        except:
            logging.fatal(traceback.format_exc())

            raw =" "*5+ "%s[%s]"%(name,lang.fail)
            self.OutputAndDraw(raw, True)
            logging.warning(" start   "+tok+"  Failed")
            return False

    def start_manager_group(self,index,conf):
        ''' start monitor group (group 0)'''
        logging.info("******BEGIN starting Monitor Services******")
        gmonitor = monitor.SupervisorApi(index, "user", "123")
        self._monitor = gmonitor
        self.OutputAndDraw("%s"%(lang.ms_group), True, "")
        time.sleep(0.5)

        galaxy_utils.checkAndVolidateGroup(index)
        self.OutputAndDraw("\n  [%s]"%(lang.ms_guard), True)

        if gmonitor.Ready():
            name=galaxy_utils.AlignmentStr(lang.ms_guard)
            raw = " "*5+"%s[%s]"%(name, lang.already_start)
            self.OutputAndDraw(raw, True)
        else:
            if not self.startMonitor(gmonitor,lang.ms_guard):
                self.FailSummary(index)
                return

        self.OutputAndDraw("\n  [%s]"%(lang.ms_group), True)        
        for i,n in self._mp.MG.getStartAll():
            if not self.startProcess(i,n):
                self.FailSummary(index)

        for i in range(const.GALA_MAX_CNR):
            if not conf.GetCNRUsable(i):
                break
            token = "cnr%.4d"%i
            #print >> sys.stderr, token
            name="%s%d"%(lang.cnr,i)
            alias=conf.GetCNRName(i)
            if alias!='':
                alias="-%s"%alias
            if not self.startProcess(token,"%s%s"%(name,alias)):
                self.FailSummary(index)

        for i in range(const.GALA_MAX_SVC):
            if not conf.GetSVCUsable(i):
                break
            token = "svc%.4d"%i
            if not self.startProcess(token,lang.svc+str(i)):
                self.FailSummary(index)

        for i in range(const.GALA_MAX_LSR):
            if not conf.GetListenerUsable(i):
                break
            token = "lsr%.4d"%i
            name="%s%d"%(lang.lsr,i)
            alias=conf.GetLSRName(i)
            if alias!='':
                alias="-%s"%alias
            if not self.startProcess(token,"%s%s"%(name,alias)):
                self.FailSummary(index)
                    
        logging.info("******FINISH starting Monitor Services******")

    def start_group(self,index,conf):
        logging.info("******BEGIN starting WorkGroup<%d>******"%index)
        gmonitor = monitor.SupervisorApi(index, "user", "123")
        self._monitor = gmonitor
        self.OutputAndDraw("%s%d"%(lang.workgroup, index), True, "")
        time.sleep(0.5)

        galaxy_utils.checkAndVolidateGroup(index)
        self.OutputAndDraw("\n  [%s]"%(lang.wg_guard,), True)

        if gmonitor.Ready():
            name=galaxy_utils.AlignmentStr(lang.wg_guard)
            raw = " "*5+"%s[%s]"%(name, lang.already_start)
            self.OutputAndDraw(raw, True)
        else:
            if not self.startMonitor(gmonitor):
                self.FailSummary(index)
                return

        self.OutputAndDraw("\n  [%s]"%(lang.cnr,), True)

        for i in range(const.GALA_MAX_CNR):
            if not conf.GetCNRUsable(i):
                break
            token = "cnr%.4d"%i

            name="%s%d"%(lang.cnr,i)
            alias=conf.GetCNRName(i)
            if alias!='':
                alias="-%s"%alias
            if not self.startProcess(token,"%s%s"%(name,alias)):
                self.FailSummary(index)

        self.OutputAndDraw("\n  [%s]"%(lang.svc,), True)

        for i in range(const.GALA_MAX_SVC):
            if not conf.GetSVCUsable(i):
                break
            token = "svc%.4d"%i

            if not self.startProcess(token,lang.svc+str(i)):
                self.FailSummary(index)

        self.OutputAndDraw("\n  [%s]"%(lang.lsr,), True)

        for i in range(const.GALA_MAX_LSR):
            if not conf.GetListenerUsable(i):
                break
            token = "lsr%.4d"%i

            name="%s%d"%(lang.lsr,i)
            alias=conf.GetLSRName(i)
            if alias!='':
                alias="-%s"%alias
            if not self.startProcess(token,"%s%s"%(name,alias)):
                self.FailSummary(index)
            
        logging.info("******FINISH starting WorkGroup<%d>******"%index)

    def start(self):
        try:
            self._mp.set_filter()
            self._start()
        except:
            self._mp.set_filter(False)
    def _start(self):
        self._mp.set_filter()
        if self._grp is None:
            node = self._mp._node
            glist = node.GetWGPList()
            # add group 0
            conf=self._mp._node.GetWorkGroupConfig(0)
            glist.insert(0,(0,conf))
            self._grp = iter(glist)
        index, conf =  self._grp.next()

        func=self.group_funcs.get(index,self.start_group)
        func(index,conf)

        if self._grp.__length_hint__() != 0:
            TimedSignal(self._text, "next", 1, self.on_next)
        else:
            if StartPage.All:
                StartPage.All=False
                l=copy.deepcopy(self.has_failed)
                del self.has_failed[0:]
                self._mp.next(SummaryPage,None,"stop",l)
            self._mp.set_filter(False)

#class RestartPage(StopPage):
#    _focus=-1
#    def __init__(self,mp,grp=None):
#        specify_group=False
#        if grp is not  None:
#            specify_group=True
#        super(RestartPage,self).__init__(mp,grp,True,specify_group)
#
