# -*- coding: utf-8 -*-

import web
import afa_context as context
from galaxy import configure
from galaxy import const
from galaxy import utils
from galaxy import language as lang
from galaxy import supervisor_api as supervisor

config = configure.Configure(mode="w")

def add_hylink(func, value):
    out = ""
    out += "<a href = \"" + func + "\">" + value + "</a>"
    return out

class table_items:
    def __init__(self, value):
        self.value = value
        
    def render(self):
        out = ""
        out += "<tr>"
        for i in self.value:
            out += "<td> " + str(i) + "</td>"
        out += "</tr>"
        return out
        
class table_frame:
    def __init__(self, input, name = None, title = None):
        self.sname = name
        self.inputs = input
        self.form_title = title
        
    def render(self):
        out = ""

        if self.sname is not None:
            out += '<table id = "%s">' % self.sname
        else:
            out += '<table>'

        if self.form_title is not None:
            out += '<thead>'
            out += self.form_title.render()
            out += '</thead><tbody>'
        else:
            out += '<tbody>'

        for i in range(len(self.inputs)):
            out += self.inputs[i].render()
         
        out += '</tbody></table>'
        return out
                
        

class group_dis:
    def GET(self):
        admin_group = [(0, config.GetWorkGroupConfig(0))]  #add the 0th config group
        groups = admin_group+ [x for x in config.GetWGPList()] # 第0个工作组为管理工作组
        
#        print moniter.get_info("cnr0063")
#        print moniter.Ready()
        return context.render.group_display(groups)

class group_info_display(object):
    def GET(self):
 
        ##receive data from page and get platform infomation
        info = []
        table_frames = []
        data = web.input()
        group_num = int(data["group"])
        group_token = str(data["token"])
        
        moniter = supervisor.SupervisorApi(group_num, "user", "123")
            
        if group_num == 0:
            group_config = config.GetWorkGroupConfig(0)
            max_length = const.GALA_MAX_CNR-1
            
        else:
            group_config = config.GetWGPList()[group_num - 1][1]
            max_length = const.GALA_MAX_CNR       
        
        for i in range(max_length):
            ##jugde which link is clicked and get different number of each contents
            if group_token == "cnr":
                usable_num = group_config.GetCNRUsable(i)
            elif group_token == "svc":
                usable_num = group_config.GetSVCUsable(i)
            elif group_token == "lsr":
                usable_num = group_config.GetListenerUsable(i)
            
            if usable_num:
                token = group_token + "%.4d"%i
                #if the gala does not start, return a error information
                try:
                    all_info = moniter.get_info(token)
                except Exception:
                    return "<h2>Gala does not start</h2>"

                if all_info['state'] == 20 or all_info['description'] != "Not Started":
                    start_value = "javascript:start_process(\'%s\', \'%s\')" % (group_num, token)
                    start = add_hylink(start_value, "start")
                
                    stop_value = "javascript:stop_process(\'%s\', \'%s\')" % (group_num, token)
                    stop = add_hylink(stop_value, "stop")
                    info.append({'group':all_info['group'], 'description':all_info['description'], 'pid':all_info['pid'], 'statename':all_info['statename'], 'start':start, 'stop':stop})
        
        ##generate html element 
        for item_info in info:
            group = item_info['group']
            description = item_info['description']
            pid = item_info['pid']
            statename = item_info['statename']
            start_process = item_info['start']
            stop_process = item_info['stop']
            table_frames.append(table_items([group, description, pid, statename, start_process, stop_process]))
            
        table_title = table_items(["group", "description", "pid", "status", "start", "stop"])
        table = table_frame(table_frames, group_token, table_title)
        return table.render()
        
class group_control_start(object):        
    def GET(self):
        data = web.input()
        group_num =  int(data['group'])
        group_token = str(data['token'])
        moniter = supervisor.SupervisorApi(group_num, "user", "123")
        proc_info = moniter.get_info(group_token)
        if proc_info['state'] == 20:
            if proc_info['statename'] == "RUNNING":
                return "It is already running"
        else:
            moniter.start(group_token)
            if moniter.get_info(group_token)['statename'] == "RUNNING":
                return "It started"
            else:
                return "start error"    

class group_control_stop(object):
    def GET(self):
        data = web.input()
        group_num =  int(data['group'])
        group_token = str(data['token'])
        moniter = supervisor.SupervisorApi(group_num, "user", "123")
        proc_info = moniter.get_info(group_token)
        if proc_info['state'] == 20:
            if proc_info['statename'] == "STOPPED":
                return "It is already stopped"
            else:
                moniter.stop(group_token)
                if moniter.get_info(group_token)['statename'] == "STOPPED":
                    return "It stops"
                else:
                    return "stop error"    
        else:
            return "it is error"
    
class group_control_whole(object):
    def GET(self):
        data = web.input()
        group_num = int(data['group'])
        command = data['command']
        moniter = supervisor.SupervisorApi(group_num, "user", "123")
        
        if group_num == 0:
            group_config = config.GetWorkGroupConfig(0)
            max_length = const.GALA_MAX_CNR-1
            
        else:
            group_config = config.GetWGPList()[group_num - 1][1]
            max_length = const.GALA_MAX_CNR     
        try:
            for i in range(max_length):            
                group_token_cnr = "cnr"
                usable_num_cnr = group_config.GetCNRUsable(i)
                if usable_num_cnr:
                    token_cnr = group_token_cnr + "%.4d"%i
                    if command == "stop":
                        moniter.stop(token_cnr)
                    if command == "start":
                        moniter.start(token_cnr)

                group_token_svc = "svc"
                usable_num_svc = group_config.GetSVCUsable(i)
                if usable_num_svc:
                    token_svc = group_token_svc + "%.4d"%i
                    if command == "stop":
                        moniter.stop(token_svc)
                    if command == "start":
                        moniter.start(token_svc)
                    
                group_token_lsr = "lsr"
                usable_num_lsr = group_config.GetListenerUsable(i)
                if usable_num_lsr:
                    token_lsr = group_token_lsr + "%.4d"%i
                    if command == "stop":
                        moniter.stop(token_lsr)
                    if command == "start":
                        moniter.start(token_lsr)    
            return "Done"        
            
        except Exception:
            return "Error Occured"    
