#-*- coding: utf-8 -*-

import sigar
import web
import afa_context as context
import cnr
import json

class process_form(object):
    '''generate a frame for the process to display'''
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
         
class process_item(object):
    '''generate frame item'''
    def __init__(self, value):
        self.values = value
    def render(self):
        out = ""
        out += "<tr>"
        for i in self.values:
            out += "<td>" + str(i) + "</td>"
        out += "</tr>"
        return out

class process_info(object):
    '''get process infomation'''
    def __init__(self, pid):
        self.process_info = []
        self.sg = sigar.open()
        self.pid = pid
    def get_name(self):
        pid_name = self.sg.proc_cred_name(self.pid).user()
        return pid_name
    
    def get_proc_name(self):
        pid_proc_name = self.sg.proc_state(self.pid).name()
        return pid_proc_name
        
    def get_proc_processor(self):
        pid_proc_processor = self.sg.proc_state(self.pid).processor()
        return pid_proc_processor
        
    def get_proc_priority(self):
        pid_proc_priority = self.sg.proc_state(self.pid).priority()
        return pid_proc_priority
        
    def get_proc_nice(self):
        pid_proc_nice = self.sg.proc_state(self.pid).nice()
        return pid_proc_nice     
    
    def get_total_time(self):
        pid_time = self.sg.proc_time(self.pid).total()
        return int('%2d' % pid_time)

    def get_mem(self):
        pid_mem = self.sg.proc_mem(self.pid).size()
        total_mem = self.sg.mem().total()
        percent = float(pid_mem) / float(total_mem)
        return float('%.4f' % percent) * 100
    
    def get_cpu(self):
        pid_cpu = self.sg.proc_cpu(self.pid).percent()
        return pid_cpu * 100 
                
                
                
class process:
    def GET(self, type_value=None):
        sg = sigar.open()
        pids = sg.proc_list()
        pid_infos = []

        if type_value:
            which_type = str(type_value)
        else:
            which_type = 'dev_type'

        node = cnr.Node("0.0.0.0", 2345)
        info = node.get_router()
 
        amq_info = node.amq()
        node_status_info = node.get_nodes_status()

   
#        info = json.loads(info)
       
        type_list = []
        
        for i in info['content']:
            print i
            type_list.append(i[which_type])
            proc = "0.0.0.0:"+i['addr']['host'] + ":"+ str(i['addr']['gpid']) + ":" + str(i['addr']['port'])  
            time = 'TBD'
            start_time = "TBD"
            last_op = "TBD"
            mem = 'TBD'
            stat_info = "TBD"
            pid_info = [proc, time, start_time, last_op, stat_info, mem]
            pid_infos.append(process_item(pid_info))
           
        type_unique = set(type_list)
        type_dict = dict((c,[]) for c in type_unique)

        for i in info['content']:
            for k in type_dict.keys():
                if k == i[which_type]:                
                    type_dict[k].append(i)

        title = process_item([u"进程".encode("utf-8"), u"运行时间".encode("utf-8"), u"起始时间".encode("utf-8"), u"上次操作时间".encode("utf-8"), u"进程信息".encode("utf-8"), u"内存使用率".encode("utf-8")])
        form = process_form(pid_infos, "process_table", title)
        
        print type_value
        return context.render.process(type_dict)
        
class process_per_info:
    def GET(self):
        sub_pid_infos = []
        data = web.input()
        pid = int(data["pid"])
        process_detail = process_info(pid)
        name = process_item(["pid name: ", process_detail.get_proc_name()])
        nice = process_item(["pid nice: ", process_detail.get_proc_nice()])
        priority = process_item(["pid priority: ", process_detail.get_proc_priority()])
        processor = process_item(["pid processor: ", process_detail.get_proc_processor()])
        pid_num = process_item(["pid number: ", pid])
        items = [pid_num, name, nice, priority, processor]
        form = process_form(items)
        
        return form.render()
