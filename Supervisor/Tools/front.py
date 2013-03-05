#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import urwid
import urwid.raw_display
import time
import sys
from galaxy import language as lang
from galaxy import supervisor_api as monitor
from galaxy import configure as configure
from galaxy import const as const
from galaxy import customize 
from cmd import status as status
from cmd import mg as mg
import thread
import cmd
from galaxy import utils
#import PyGalaxy as PyGala
import logging

TimedSignal = cmd.utils.TimedSignal
StartPage = cmd.group.StartPage
#RestartPage = cmd.group.RestartPage
StopPage = cmd.group.StopPage

class NButton(urwid.Button):
    button_left=urwid.Text("/")
    button_right=urwid.Text("")
    
    def get_prompt_width(self):
        '''label width plus button_left's width and button_right's width'''
        unistr=self.get_label()
        width=utils.unicode_str_width(unistr)
        return width+4

class Navigation:
    '''Navigation bar'''
    # Columns
    def __init__(self,dividechars=0,min_width=15):
        self.min_width=min_width
        self.button=[]
        self.dividechars=dividechars
        self.len=0

    def get(self):
        '''return the list to show'''
        show=[]
        for width,button in self.button:
            if not button.not_show:
                show.append((width,button))
        grid=urwid.Columns(show,dividechars=self.dividechars,min_width=self.min_width)
        focus=len(show)-1
        self.len=focus
        grid.focus_position=focus if focus>=0 else 0
        grid=urwid.AttrWrap(grid,"navigation")
        return grid

    def getShowLen(self):
        return self.len

    def insert(self,pos,prompt,func,arg,not_show=False):
        '''insert to pos '''
        button=NButton(prompt,self.dummy_func)
        width=button.get_prompt_width()
        button.func=func
        button.arg=arg
        button._pos=len(self.button)

        button=urwid.AttrWrap(button,"navigation","button_select")
        button.not_show=not_show
        self.button.insert(0,(width,button))

    def add(self,prompt,func,arg,not_show=False):
        '''append a tag to Navigation,not_show determines whether to show'''
        button=NButton(prompt,self.dummy_func)
        width=button.get_prompt_width()

        button.func=func
        button.arg=arg
        button._pos=len(self.button)

        button=urwid.AttrWrap(button,"navigation","button_select")
        button.not_show=not_show
        self.button.append((width,button))

    def truncate(self,pos):
        '''truncate navigation bar to pos.
           NOTE:You Must call this function manually '''
        del self.button[pos+1:]

    def pop(self):
        self.button.pop()
        
    def dummy_func(self,button):
        '''a wrapper of callback passed by add or insert'''
        button.func(button.arg)


class SwitchingPadding(urwid.Padding):
    def padding_values(self, size, focus):
        maxcol = size[0]
        width, ignore = self.original_widget.pack(size, focus=focus)
        if maxcol > width:
            self.align = "left"
        else:
            self.align = "right"
        return urwid.Padding.padding_values(self, size, focus)

class PlatformManage(object):
    '''Platform manager class'''
    _tag=lang.PlatformManage.tag
    help_info=lang.PlatformManage.help_info
    def __init__(self, mp):
        self._mp = mp
        self.p=[]
        self._focus=-1
        start = customize.PopUpWithCancel(lang.PlatformManage.start, [(lang.confirm,"confirm",self.on_start,len(self.p))])
        self.p.append(start)
      
        stop= customize.PopUpWithCancel(lang.PlatformManage.stop, [(lang.confirm,"confirm",self.on_stop,len(self.p))])
        self.p.append(stop)

    def on_start(self,focus):
        #backup focus
        self._focus=focus
        self._mp.next(StartPage, StartPage.start)

    def on_stop(self,focus):
        self._focus=focus
        self._mp.next(StopPage, StopPage.start)
    def get(self):
        return self.p

class FirstPage(object):
    help_info = lang.FirstPage.help
    _focus=-1
    _tag=lang.FirstPage.tag
    def __init__(self, mp):
        self._mp = mp
        self.p=[]
        platform=urwid.AttrWrap(customize.CButton(lang.FirstPage.platform_1, self.on_platform,len(self.p)),"button_normal","button_select")
        self.p.append(platform)

#        status= urwid.AttrWrap(customize.CButton(lang.FirstPage.workgroup_2,self.on_status,len(self.p)),"button_normal","button_select")
#        self.p.append(status)

        mg=urwid.AttrWrap(customize.CButton(lang.FirstPage.monitor_3,self.on_mg,len(self.p)),"button_normal","button_select")
        self.p.append(mg)
        
    def on_mg(self,b,userdata):
        self._focus=userdata
        self._mp.next(status.MgPage,None)

    def on_platform(self, b,userdata):
        '''press platform manage'''
        #backup focus
        self._focus=userdata
        self._mp.next(PlatformManage,None)

    def on_status(self, b,userdata):
        self._focus=userdata
        self._mp.next(status.StatusPage, None)
    def get(self):
        return self.p

class StartUp(object):
    _focus=-1
    def __init__(self,mp):
        self._mp = mp
        self._info = lang.StartupPage.build_info
        self._text = urwid.Text(self._info)
        self._tip = urwid.Text(lang.StartupPage.startup_tip,'center')
        self._tag=""

    def get(self):
        return [self._text, self._tip]

class ErrorPage(object):
    _focus=-1
    _tag=lang.ErrorPage.tag
    def __init__(self,mp,str=""):
        self._mp = mp
        self._info = "[Error]"
        self._text = urwid.Text(self._info)
        self._tip = urwid.Text(str)
        self._tip = urwid.Columns([('fixed',5,urwid.Text("")),self._tip])

    def get(self):
        return ([self._text, self._tip],None)

class MainPage(object):
    def __init__(self, win):
        self._main_win = win

        self._ui_obj= StartUp(self)
        self._rtti_ = []
        self._tags=[]    #navigation text list

        self.navigation=Navigation()
        self.has_failed=[]

        node = configure.Configure()
        self._node = node       # Configure Object
        self.MG=mg.Supervisor_Config_Parse(os.path.join(os.getenv("HOME"),"var/supervisor/conf/aim.conf"))

    def set_filter(self,flag=True):
        self._main_win.keyfilter=flag
        self._main_win.refresh()
        self.refresh()

    def getGroups(self):
        GPList={}
        for index,conf in self._node.GetWGPList():
            GPList[index]=conf
        return GPList
    
    def SafeGoto(self,to=-1,tag=u''):
        ''' For start/stop platform. Will not delete any page in self._rtti_
            these Pages's tag always not show at Navigation bar,
            And You can specify tag by argument 'tag'.'''
        focus=self._ui_obj._focus
        offset=self.navigation.getShowLen() 
        cls,args,obj,focus_of_last_onepage=self._rtti_[offset+to]
        self._ui_obj=obj

        if tag==u'':
            tag=self._ui_obj._tag

        self.navigation.add(tag,self.pre,len(self._rtti_))
        self._rtti_.append([cls,args, self._ui_obj,focus])
        self._main_win.refresh()
        self.refresh()

    def next(self,cls, func=None, *args):
        '''Go to the next page,Save the focus of current page (for pre())
           and get the tag of next page(use for Navigation)
           cls:the next page we will go. Such kind of class Must have 
               attribute _tag and _focus and function get() which return a list to show
           func:run this function of cls'''
        #save focus of last one page
        focus=self._ui_obj._focus

        self._ui_obj = cls(self, *args)
        tag=self._ui_obj._tag
        
         # The not_show attribute can Specify Whether to show self._ui_obj.tag in Navigation
        if hasattr(self._ui_obj,"not_show"):
            notshow=self._ui_obj.not_show
        else:
            notshow=False

        self.navigation.add(tag,self.pre,len(self._rtti_),notshow)
        
        self._main_win.refresh()
        self.refresh()

        #TODO:: help to show spicified by page
        help_info = getattr(cls, "help_info","")
        help_common = lang.help_common
        if help_common!=u'':
            help_common="%s\n"%help_common
        self._main_win.chars_avail.set_text("%s%s"%(help_common, help_info))

        if func is not None:
            self._rtti_.append([cls,args, self._ui_obj,focus])
            func(self._ui_obj)
        else:
            self._rtti_.append([cls,args, None,focus])
        
    def refresh(self):
        #if obj is self._ui_obj:
        self._main_win.loop.draw_screen()

    def BackToMainPage(self):
        '''only back to MainPage if not in'''
        self.pre(0)

    def pre(self,to=-1):
        ''' Back to the last page or specified page'''
        if to==-1:
            focus=self._rtti_[-1][3]
            self._rtti_.pop()
            self.navigation.pop()
        else:
            if len(self._rtti_)==to+1:
                focus=-1
            else:
                focus=self._rtti_[to+1][3]
            del self._rtti_[to+1:]
            self.navigation.truncate(to)

        cls,args,obj,focus_of_last_onepage=self._rtti_[-1]

        if obj is None:
            self._ui_obj = cls(self, *args)
        else:
            self._ui_obj = obj
        self._main_win.refresh()

        #if it's a available focus
        if focus>=0:
            self._main_win.page.set_focus(focus+1)    #plus the Navigation line

    def get(self):
        items= self._ui_obj.get()   #must return list
        focus=0 # Navigation 
        for i in range(len(items)):
            if items[i].selectable():
                focus=i+1   #insert navigation to 0 below
                break
            
        if type(self._ui_obj) is not StartUp:
            s=[self.navigation.get()]
        else:
            s=[urwid.Text("")] 

        #*NOTE*:NEVER modify items(which is a list) directly.Because it will 
        #change the element return by get() in self._ui_obj if you do 
        #not create new element every time using get()
        return urwid.Pile(s+items,focus_item=focus)

class MainLoop(object):

    palette = [ 
        ('body','black','light gray', 'standout'),
        ('header','white','dark red',   'bold'),
        ('button_normal','black', 'light gray', 'standout'),
        ('button_select','white','dark green','bold'),
        ('line_select','black','dark cyan'),
        ('edit',         'light gray', 'dark blue'),
        ('navigation',   'black', 'dark magenta'),
        ('bigtext',      'white',      'black'),
        ('chars',        'light gray', 'black'),
        ('exit',         'white',      'dark cyan'),
        ('popbg', 'white', 'dark blue'),
        ('popup_button_select','white','dark green','bold'),
        ]
        
    def __init__(self,action=None,function=None,name=None,bigtext=None):
        self.action=action
        self.function=function
        self.name=name
        self._bigtext=bigtext

    def setup_view_1(self):
        self.bigtext = urwid.Text(lang.welcom_info)
        bt = urwid.Padding(self.bigtext, 'left')
        bt = urwid.AttrWrap(bt, 'bigtext')
        bt = urwid.Filler(bt, 'middle', None, 7)
        bt = urwid.BoxAdapter(bt, 7)
        
        # Create Help 
        cah = urwid.Text(lang.StartupPage.help)
        self.chars_avail = urwid.Text(lang.StartupPage.info, wrap='any')
        ca = urwid.AttrWrap(self.chars_avail, 'chars')
        
        # Main Page
        chars = urwid.Pile([cah, ca])
        self.chars = chars
        self._mp = MainPage(self)
        #self.page = page
        self.keyfilter=False
     
        bt = urwid.Pile([bt], focus_item=0)
        self._bt = bt
        return self.create()


    def setup_view_2(self):
        '''use bigtext'''
        self.bigtext = urwid.BigText("Gala 2.0 Welcome You", None)
        bt = SwitchingPadding(self.bigtext, 'left', None)
        bt = urwid.AttrWrap(bt, 'bigtext')
        bt = urwid.Filler(bt, 'bottom', None, 7)
        bt = urwid.BoxAdapter(bt, 7)
        
#set font manually 
        self.exit_font=urwid.Thin6x6Font()
        self.bigtext.set_font(urwid.Thin6x6Font())
        # Create Help 
        cah = urwid.Text(lang.StartupPage.help)
        self.chars_avail = urwid.Text(lang.StartupPage.info, wrap='any')
        ca = urwid.AttrWrap(self.chars_avail, 'chars')
        
        # Main Page
        self.chars = urwid.Pile([cah, ca])
        self._mp = MainPage(self)
        #self.page = page
        self.keyfilter=False
     
        bt = urwid.Pile([bt], focus_item=0)
        self._bt = bt
        return self.create()

    def setup_view(self):
        if self._bigtext:
            return self.setup_view_2()
        return self.setup_view_1()

    def refresh(self):
        self.page = self._mp.get()
        col = urwid.Columns([('fixed',16,self.chars), self.page],  
                            focus_column=1)
        #l = [self._bt, urwid.Divider(), col]
        l = [self._bt, col]

        w =customize.CListBox(urwid.SimpleListWalker(l),self.keyfilter)
        w = urwid.AttrWrap(w, 'body')
       
        self.view.body = w

    def create(self):
        self.page = self._mp.get()
        col = urwid.Columns([('fixed',16,self.chars), self.page], 3, 
                            focus_column=1)
        l = [self._bt,col]
        w = urwid.ListBox(urwid.SimpleListWalker(l))
        w = urwid.AttrWrap(w, 'body')
        banner = "Gala2.0 Supervisor Tool " # Agree Inc. (2012)    All Rights Reserved"
        hdr = urwid.Text(banner)
        hdr = urwid.AttrWrap(hdr, 'header')
        help=urwid.Text(lang.help)
        w = urwid.Frame(header=hdr, body=w,footer=help)
        
        if self._bigtext:
            exit = urwid.BigText(('exit'," Quit?"), self.exit_font)
            exit = urwid.Overlay(exit, w, 'center',None, 'middle',None)
            return w,exit
        
        raw=u"Gala2.0 Supervisor\n"+u"-"*30+"\n\n    %s\n\n"%lang.exit_info
        exit= urwid.Text(raw)
        exit= urwid.Padding(exit, 'left')
        exit= urwid.AttrWrap(exit, 'exit')
        exit= urwid.Filler(exit, 'middle', None, None)
        exit = urwid.Overlay(exit, w, 'center',30 , 'middle',5,20,1)
        return w,exit

    def main(self):
        self.view, self.exit_view = self.setup_view()

        self.loop = urwid.MainLoop(self.view, self.palette, 
                                   unhandled_input=self.unhandled_input,pop_ups=True)

        if self.action is not None:
            self._mp._ui_obj=StartUp(self._mp)
            self.loop.set_alarm_in(0.5,self.callback)

    #    self._mp.log.LoggerInfo(" Gala2.0 Supervisor Tool    Agree Inc. (2012)    All Rights Reserved ")
        self.loop.run()
    
    def unhandled_input(self, key):
        #ESC to quit anywhere
        if key =='esc':
            self.loop.widget = self.exit_view
            return True
            #raise urwid.ExitMainLoop()

        if self.loop.widget == self.exit_view:
            if key in ('y', 'Y', 'enter'):
                raise urwid.ExitMainLoop()
            if key in ('n', 'N'):
                self.loop.widget = self.view
                return True
            return True

        if type(self._mp._ui_obj) is StartUp:
            self._mp.next(FirstPage, None)
            return  True
        if key in ["r",'R']:
            self.refresh()
            return True
        if key in ['m','M']:
            self._mp.BackToMainPage()
            return True
        if key in ['q','b','Q','B']:
            #raise Exception(str(self._mp._rtti_))
            if len(self._mp._rtti_) >1:
                self._mp.pre()
                return True
            self.loop.widget = self.exit_view
            return True

    def callback(self,loop,userdata):
        #back to FirstPage
        self._mp._rtti_.insert(0,(FirstPage,None,FirstPage(self._mp),FirstPage._focus))
        self._mp.navigation.insert(0,FirstPage._tag,self._mp.BackToMainPage,None)

        if self.name is None:
            self._mp.next(self.action,self.function)
        else:
            groupdict=self._mp.getGroups()
            try:
                conf=groupdict[self.name]
            except:
                err=" %d is Not a valid workgroup\n Check the argument passed by -g"%self.name
                self._mp.next(ErrorPage,None,err)
                w=err.splitlines()
                logging.warning(w[0]+','+w[1])
                #self._mp.log.LoggerError(err)
                return
            self._mp.next(self.action,self.function,iter([(self.name,conf)]))
#        self._mp.set_filter(False)

def main():
    MainLoop().main()
    
if '__main__'==__name__:
    main()
