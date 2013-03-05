#!/usr/bin/env python
#Customize urwid.Button

import sys
import urwid
import language as lang

class CButton(urwid.Button):
    button_left=urwid.Text("")
    button_right=urwid.Text("")

class CListBox(urwid.ListBox):
    def __init__(self,body,keyfilter):
        '''set keyfilter to pass keys'''
        self._keyfilter=keyfilter
        self.__super.__init__(body)
    def keypress(self,size,key):
        if self._keyfilter:
            return
        else:
            return self.__super.keypress(size,key)

class PopUpDialog(urwid.WidgetWrap):
    signals = ["cancel","confirm","close"]
    def __init__(self,prompt,button_list):
        '''button_list:(prompt,signal,callback,callback_arg)
           callback_arg will be ignored if None,others must not be None'''
        buttons=[]
        for label,signal,callback,arg in button_list:
            if signal not in self.signals: 
                continue
            buttons.append(urwid.AttrMap(CButton(label,self.on_click,signal),"popbg","popup_button_select"))
            #buttons.append(CButton(label,self.on_click,signal))
            
        pile = urwid.Pile([urwid.Text(prompt),urwid.Columns(buttons)])
        fill = urwid.Filler(pile)
        self.__super.__init__(urwid.AttrWrap(fill, 'popbg'))

    def on_click(self,button,userdata):
        self._emit(userdata)

    def keypress(self,size,key):
        if key not in ["left","right","enter"]:
            return
        else:
            return self.__super.keypress(size,key)

class PopUp(urwid.PopUpLauncher):
    def __init__(self,label,pop_up_prompt,button_list=[(None,None,None,None)]):
        '''button_list is a list of (label,signal,callback,callback_arg)
           callback_arg will be ignored if None,others must not be None
           will be ignored if signal not in PopUpDialog.signals '''
        b=CButton(label)
        self.__super.__init__(urwid.AttrMap(b,{},"button_select"))
        urwid.connect_signal(b, 'click',lambda button: self.open_pop_up())
        self.button_list=button_list
        self.pop_up_prompt=pop_up_prompt

    def click(self,pop_up,callback): 
        func,arg=callback 
    
        if func is not None:
            if arg is not None:
                func(arg)
            else:
                func()
        self.close_pop_up()
    def create_pop_up(self):
        pop_up = PopUpDialog(self.pop_up_prompt,self.button_list)
        for label,signal,callback,arg in self.button_list:
            if signal not in pop_up.signals:
                continue
            urwid.connect_signal(pop_up,signal,self.click,(callback,arg))
        return pop_up

    def get_pop_up_parameters(self):
        return {'left':0, 'top':1, 'overlay_width':20, 'overlay_height':3}


class PopUpWithCancel(PopUp):
    def __init__(self,label,button_list=[(None,None,None,None)]):
        buttons=[(lang.cancel,"cancel",None,None)]
        buttons+=button_list
        self.__super.__init__(label,lang.ask,buttons)


def dummy(show):
    with open("abc","w") as d:
        d.write("%s"%show)

if __name__=='__main__':
    buttons=[("cancel","cancel",dummy,"Fuicccc:"),("confirm","confirm",dummy,"confirm:"),("confirm","cnfirm",dummy,"confirm:")]
    fill = urwid.Filler(urwid.Padding(PopUp("Donotclickme",buttons), 'center', 15))
    loop = urwid.MainLoop(
        fill,
        [('popbg', 'white', 'dark blue')],
        pop_ups=True)
    loop.run()

