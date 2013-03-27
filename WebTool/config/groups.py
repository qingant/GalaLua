# -*- coding: utf-8 -*-
import web
from web import form as webform
import os
from cnr import Node
import json
import traceback as tb
import afa_context as context
from lxml import etree

monitor_path = os.path.join(os.getenv("HOME"), "cfg/MonitorDef.xml")
define = etree.XML(open(monitor_path).read())

#config = configure.Configure(mode="w")
#node = cnr.Node("0.0.0.0", 1234)

class config(object):
    def __init__(self, host, port):
        self._host = host
        self._port = port
        self._node = Node(host, port)
        self._const = 4

    def Get(self, path):
       
        msg = {'action':'query', 'path':path, 'isset':'false'}
        self._node._send(json.dumps(msg), 3)
      
        result = eval(json.loads(self._node._recv()))
        status = result['status']
        if status == 'true':
            if result['content']:
                content = result['content']
                return content
            else:
                return None
        else: 
            if result['msg']:
                msg = '0'
                return msg

    def Put(self, path, value):
        msg = {'action':'alter', 'path':path, 'value':value}
        self._node._send(json.dumps(msg), 3)
        result = eval(json.loads(self._node._recv()))
        return result

    def get_usable_groups(self):
        available_groups = []
        for i in range(0, self._const):
            path = '/Configure/_STRUCTURE/MonitorList/Monitor[%d]/Usable'%i
            msg = {'action':'query', 'path':path, 'isset':'false'}
            self._node._send(json.dumps(msg), 3)

            result = eval(json.loads(self._node._recv()))
            print result
            status = result['status']
            if status == 'true':
                if result['content'] == '0':
                    available_groups.append(i)

        return available_groups                 




class framed_form:
    def __init__(self, title = None, visible = True, attrib = None):
        self.frames = []
        self.forms = []
        self.title = title
        self.visible = visible
        if attrib is None:
            self.attrib = ""
        else:
            print attrib
            self.attrib = " ".join(['%s="%s"'%(key, str(value if type(value) is not unicode else value.encode("utf-8"))) for key,value in attrib.iteritems()])
 
    def add_form(self,frame):
        self.frames.append(frame)

    def render(self):
        buf = ""
        if self.visible:
            buf += "<div %s class=\"configarea\" id = \"configarea\">\n"%self.attrib
        else:
            buf += "<div %s style=\"display:none\" class=\"configarea\" id=\"configarea\">\n"%self.attrib
        if self.title is not None:
            buf += "<p>%s</p>\n"%self.title
        buf += webform.Form(*self.forms).render()
        # for form in self.forms:

        #     buf += webform.Form(form).render()
        #     buf += "\n"
        for frame in self.frames:
            buf += frame.render()
        buf += "</div>\n"
        return buf
        
       
class group_form:

    def __init__(self, cat):
        self.cat = cat
        self._dispatch = {"svc":self.get_svc,
                          "base":self.get_base,
                          "lsr":self.get_lsr,
                          "cnr":self.get_cnr,
                          "log":self.get_log,
                          "logpattern":self.get_logpattern
                          }
        self._config = config("0.0.0.0", 1234)


    def _gen_union(self, item, item_path, form):

        #root = item.getroottree().getroot()
        union_selector_path = "%s/%s"%(item_path, "_SCROLLTYPE")
        selector = self._config.Get(union_selector_path)
        type_name = item.xpath("_SCROLLTYPE/type")[0].text
        type_title = item.xpath("_SCROLLTYPE")[0].attrib["title"]
        print type_name
        enums = [e.tag for e in define.xpath("/Configure/_TYPEDEFINE/%s"%type_name)[0].iterchildren()]
        current = enums[int(selector)]
        selector_form = form# framed_form()
        drop = webform.Dropdown(name = union_selector_path, args =  [(i, v) for i,v in enumerate(enums)], value=int(selector), description=type_title, disabled="1", onchange="trigeChange(this)")

        selector_form.forms.append(drop)
        print enums
        #form.frames.append(selector_form)
        selector_form.frames.append(webform.Textbox("%s/Usable"%item_path,value="1", style="display:none"))

        for enum in enums:
            print item,item.tag,enum
            _path = "%s/_UNIONSELECTOR/%s"%(item_path, enum)

            if not (enum == "NONETYPE"):
                enum_structs = item.xpath("_UNIONSELECTOR/%s"%enum)
                if enum_structs.__len__() == 1:
                    enum_struct = enum_structs[0]
                    visib = (enum == current)
                    print current
                    newform = framed_form(visible = visib, attrib = {"enum":enum})
                    selector_form.frames.append(self.gen_form(enum_struct, _path, newform))

    def _gen_array(self, item, item_path, form):
        new_form = framed_form()
        form.frames.append(new_form)
        count = int(item.attrib["count"])
        struct = item.getchildren()[0]
        struct_path = "%s/%s"%(item_path, struct.tag)
        struct_title = struct.attrib["title"]
        title = item.attrib["title"]
        array = []
        for i in range(count):
            print "im the i ", i, count
            
            usable_path = "%s[%d]/Usable"%(struct_path, i)

            usable = self._config.Get("%s[%d]/Usable"%(struct_path, i))
            if usable == "1":
                is_first = (len(array) == 0)
                tag =  struct_title + " - %d"%i
                array.append((item.tag+str(i), tag))

                
                struct_form = framed_form(visible = is_first, attrib = {"enum":item.tag+str(i)})
                new_form.frames.append(struct_form)
                self._gen_struct(struct, "%s[%d]"%(struct_path, i), struct_form)
                struct_form.frames.append(webform.Button("submit", type="submit", html=u"删除适配器", disabled="1", onclick = "return trigeDelete(\"%s\");"%usable_path))

        for i in range(count):
            usable_path = "%s[%d]/Usable"%(struct_path, i)
            usable = self._config.Get(usable_path)
            if usable == "0":
                is_first = (len(array) == 0)
                tag = u"新建" + struct_title  + " - %d"%i
                array.append((item.tag+str(i), tag))

                struct_form = framed_form(visible = is_first, attrib = {"enum":item.tag+str(i)})
                new_form.frames.append(struct_form)

                self._gen_struct(struct, "%s[%d]"%(struct_path, i), struct_form)
                #struct_form.frames.append(webform.Button("submit", type="submit", html=u"删除适配器", onclick = "return trigeDelete(\"%s\");"%usable_path))
                break

        new_form.forms.append(webform.Dropdown("@"+struct_path, array, bytag=0, description=title, onchange="trigeChange(this)"))
        
    def _gen_struct(self, root, path, form):
        for item in root.iterchildren():
            item_path = "%s/%s"%(path, item.tag)
            self.gen_form(item, item_path, form)
        if form.visible:
            form.frames.append(webform.Textbox("%s/Usable"%path,value="1", style="display:none"))
        else:
            form.frames.append(webform.Textbox("%s/Usable"%path,value="0", style="display:none"))

    #add a function to get textbox help infomation
    def get_help_info(self, item):
        if item is None:
            return "there is no infomation about " + item;
        info = {
                     "Alias" : "This is Alias help_info",
                     "Token" : "This is Token help_info",
                     "SVC" : "This is SVC help_info",
                     "Threads" : "This is Threads help_info",
                     "IsFlush" : "This is IsFlush help_info",
                     "IP" : "This is IP help_info",
                     "SID" : "This is SID help_info",
                     "User" : "This is User help_info",
                     "Passwd" : "This is Passwd help_info",
                     "DBFile" : "This is DBFile help_info",
                     "ImplDllName" : "This is ImpIDllName help_info",
                     "ThreadStackSize" : "This is ThreadStackSize help_info",
                     "Port" : "This is Port help_info",
                     "NamePattern" : "This is NamePattern help_info",
                     "RecvTimeOut" : "This is RecvTimeOut help_infp",
                     "PORT" : "This is PORT help_info",
                     "ExchangeTMO" : "This is exchangetmo help_info",
                     "PTCDllName" : "This is ptcdllname help_info"
                     }
        if item not in info.keys():
            return "error, this is no help info for " + item
        else:             
            return info[item]                         


    #add the help infomation into the textbox by calling class method

    def gen_form(self, root, path, form = None):
        if form is None:
            form = framed_form()
        #for item in root.iterchildren():
        item = root
        uitype = root.attrib["UIType"]
        
        item_path = path # "%s/%s"%(path, root.tag)
        item_desc = root.attrib["title"]
        if uitype == "struct":
            new_form = framed_form()

            form.frames.append(new_form)
            self._gen_struct(item, item_path, new_form)

        elif uitype == "entry" or uitype == "readonly":

            item_value = self._config.Get(item_path)
            
            default = item.attrib.get("default","")
            if not item_value:
                item_value = default
            else:
                item_value = unicode(item_value, "gbk")

            print item_desc, item_value
            
            #add the help info here            
            infomation = self.get_help_info(item.tag)
            input_component = webform.Textbox(item_path, value = item_value, description=item_desc, disabled="1", help_info=infomation, onMouseOver="show_form_help(this)")
            form.forms.append(input_component)
        elif uitype == "enum":
            item_value = self._config.Get(item_path)
            print "ENUM:",item_value
            default = item.attrib.get("default","")
            if item_value == "":
                item_value = default
            else:
                item_value = unicode(item_value, "gbk")
            type_name = item.xpath("type")[0].text
            type_title = item.attrib["title"]
            enums = [e.tag for e in define.xpath("/Configure/_TYPEDEFINE/%s"%type_name)[0].iterchildren()]
            current = enums[int(item_value)]
            print current
            form.forms.append(webform.Dropdown(item_path, [(i, v) for i,v in enumerate(enums)], description=type_title,  value=int(item_value), disabled="1", onchange="trigeChange(this)"))
            
        elif uitype == "union":
            new_form = framed_form()
            form.frames.append(new_form)
            self._gen_union(item, item_path, new_form)
        elif uitype == "array":
            self._gen_array(item, item_path, form)
        return form
        
    def get_lsr(self):
        root = define.xpath("/Configure/_STRUCTURE/MonitorList/Monitor/LSRList")[0]
        path = "/Configure/_STRUCTURE/MonitorList/Monitor[00]/LSRList"
        return self.gen_form(root, path)
    def get_cnr(self):
        root = define.xpath("/Configure/_STRUCTURE/MonitorList/Monitor/CNRList")[0]
        path = "/Configure/_STRUCTURE/MonitorList/Monitor[00]/CNRList"
        return self.gen_form(root, path)
    def get_log(self):
        root = define.xpath("/Configure/_STRUCTURE/MonitorList/Monitor/LowLoger")[0]
        path = "/Configure/_STRUCTURE/MonitorList/Monitor[00]/LowLoger"
        
        return self.gen_form(root, path)
    def get_logpattern(self):
        root = define.xpath("/Configure/_STRUCTURE/MonitorList/Monitor/AppLoger")[0]
        path = "/Configure/_STRUCTURE/MonitorList/Monitor[00]/AppLoger"
        return self.gen_form(root, path)



    def get_base(self):
        root = define.xpath("/Configure/_STRUCTURE/MonitorList/Monitor/Base")[0]
        path = "/Configure/_STRUCTURE/MonitorList/Monitor[00]/Base"
        return self.gen_form(root, path)

#different orcal
    def get_svc(self):
        root = define.xpath("/Configure/_STRUCTURE/MonitorList/Monitor/SVC")[0]
        path = "/Configure/_STRUCTURE/MonitorList/Monitor[00]/SVC"
        return self.gen_form(root, path)

    def get(self):
        form = self._dispatch[self.cat]()
        form.frames.append(webform.Button("submit", type="submit", html=u"更改配置", onclick = "return trigeEdit(this);"))
        return form
        
class look:
    def __init__(self):
        self._node = config('0.0.0.0', 1234)

    def GET(self, cat = None, nothing = None):
#        admin_group = [(0, config.GetWorkGroupConfig(0))]  #add the 0th config group
#        groups = admin_group+ [x for x in config.GetWGPList() ] # 第0个工作组为管理工作组
        
        if cat is None:
            return context.render.config_group(True)
#            raise web.seeother('/')
        else:
            form = group_form(cat).get()
            return context.render.config_group_form(True, form)
            
        
    def POST(self, cat = None, index = None):
        # print web.data()
        
        data = web.input()
        print "*"*20
        print cat
        for k,v in data.iteritems():
            print "im the k ", k  
            if k is not None and not k == "submit" and not k[0] == "@":
                try:
                    print "PUT",k,v
                    self._node.Put(k,v.encode("gbk"))
                except :
                    print tb.format_exc()
                    pass

        raise web.seeother("/cfg/groups/%s/"%(cat))
        
