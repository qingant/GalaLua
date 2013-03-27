# -*- coding: utf-8 -*-
import web
from web import form as webform
from galaxy import configure
import traceback as tb
import afa_context as context

define = context.define
config = configure.Configure(mode="w")


class status_frame(object):
    """
    """
    def __init__(self, ):
        """
        """
        self._header
        self._body
        self._tail
    
class status_table(object):
    """
    """
    
    def __init__(self, ):
        """
        """
        self._header
        self._body
        self._tail
        
        

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
            self.attrib = " ".join(['%s="%s"'%(key, str(value if type(value) is not unicode else value.encode("utf-8"))) \
                                    for key,value in attrib.iteritems()])
            
    def add_form(self,frame):
        self.frames.append(frame)
        
    def render(self):
        buf = ""
        if self.visible:
            buf += "<div %s class=\"configarea\">\n"%self.attrib
        else:
            buf += "<div %s style=\"display:none\" class=\"configarea\">\n"%self.attrib
        if self.title is not None:
            buf += "<p>%s</p>\n"%self.title
        buf += webform.Form(*self.forms).render()
        # for form in self.forms:

        #     buf += webform.Form(form).render()
        #     buf += "\n"
        for frame in self.frames:
            buf += "<br />"
            buf += frame.render()
        buf += "</div>\n"
        return buf
        
        
class group_form:
    def __init__(self, index, cat):
        self.index = int(index)
        self.cat = cat
        self._dispatch = {"svc":self.get_svc,
                          "base":self.get_base,
                          "lsr":self.get_lsr,
                          "cnr":self.get_cnr,
                          "log":self.get_log,
                          "logpattern":self.get_logpattern
                          }
        
    def _gen_union(self, item, item_path, form):
        #root = item.getroottree().getroot()
        union_selector_path = "%s/%s"%(item_path, "_SCROLLTYPE")
        selector = config.Get(union_selector_path)
        type_name = item.xpath("_SCROLLTYPE/type")[0].text
        type_title = item.xpath("_SCROLLTYPE")[0].attrib["title"]
        print type_name
        enums = [e.tag for e in define.xpath("/Configure/_TYPEDEFINE/%s"%type_name)[0].iterchildren()]
        current = enums[int(selector)]
        selector_form = form# framed_form()
        drop = webform.Dropdown(name = union_selector_path, args =  \
                                    [(i, v) for i,v in enumerate(enums)],\
                                    value=int(selector), description=type_title, \
                                    disabled="1", onchange="trigeChange(this)")

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
            usable_path = "%s[%d]/Usable"%(struct_path, i)

            usable = config.Get("%s[%d]/Usable"%(struct_path, i))
            
            if usable == "1":
                is_first = (len(array) == 0)
                tag =  struct_title + " - %d"%i
                array.append((item.tag+str(i), tag))

                
                struct_form = framed_form(visible = is_first, attrib = {"enum":item.tag+str(i)})
                new_form.frames.append(struct_form)
                self._gen_struct(struct, "%s[%d]"%(struct_path, i), struct_form)
                struct_form.frames.append(webform.Button("submit", type="submit", \
                                                             html=u"删除适配器", disabled="1", \
                                                             onclick = "return trigeDelete(\"%s\");"%usable_path))


        
        for i in range(count):
            usable_path = "%s[%d]/Usable"%(struct_path, i)
            usable = config.Get(usable_path)
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
            item_value = config.Get(item_path)
            default = item.attrib.get("default","")
            if item_value == "":
                item_value = default
            else:
                item_value = unicode(item_value, "gbk")

            print item_desc, item_value
            input_component = webform.Textbox(item_path, value = item_value, description=item_desc, disabled="1")
            form.forms.append(input_component)
        elif uitype == "enum":
            item_value = config.Get(item_path)
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
            form.forms.append(webform.Dropdown(item_path, [(i, v) for i,v in enumerate(enums)],\
                                                   description=type_title,  value=int(item_value),\
                                                   disabled="1", onchange="trigeChange(this)"))
        elif uitype == "union":
            new_form = framed_form()
            form.frames.append(new_form)
            self._gen_union(item, item_path, new_form)
        elif uitype == "array":
            self._gen_array(item, item_path, form)
        return form
    
    def get_lsr(self):
        root = define.xpath("/Configure/_STRUCTURE/WorkGroupList/WorkGroup/ListenerList")[0]
        path = "/WorkGroupList/WorkGroup[%d]/ListenerList"%self.index
        return self.gen_form(root, path)
    def get_cnr(self):
        root = define.xpath("/Configure/_STRUCTURE/WorkGroupList/WorkGroup/ConnectorList")[0]
        path = "/WorkGroupList/WorkGroup[%d]/ConnectorList"%self.index
        return self.gen_form(root, path)
    def get_log(self):
        root = define.xpath("/Configure/_STRUCTURE/WorkGroupList/WorkGroup/LowLoger")[0]
        path = "/WorkGroupList/WorkGroup[%d]/LowLoger"%self.index
        
        return self.gen_form(root, path)
    def get_logpattern(self):
        root = define.xpath("/Configure/_STRUCTURE/WorkGroupList/WorkGroup/AppLoger")[0]
        path = "/WorkGroupList/WorkGroup[%d]/AppLoger"%self.index
        return self.gen_form(root, path)
    def get_base(self):
        root = define.xpath("/Configure/_STRUCTURE/WorkGroupList/WorkGroup/GroupBase")[0]
        path = "/WorkGroupList/WorkGroup[%d]/GroupBase"%self.index
        return self.gen_form(root, path)
    def get_svc(self):
        root = define.xpath("/Configure/_STRUCTURE/WorkGroupList/WorkGroup/SvcConf")[0]
        path = "/WorkGroupList/WorkGroup[%d]/SvcConf"%self.index
        return self.gen_form(root, path)
    def get(self):
        form = self._dispatch[self.cat]()
        form.frames.append(webform.Button("submit", type="submit", html=u"更改配置", onclick = "return trigeEdit(this);"))
        return form
        
class look:
    def GET(self, index = None, cat = None, nothing = None):
        groups = [x for x in config.GetWGPList() ] + [config.NewGroup()] # 第0个工作组为管理工作组
        if index is None:

            return context.render.config_group(True, groups, None)
           # elif index == "0":
           #     return context.render.config_group(True, groups, None)
        elif cat == "delete":
            config.Put("/WorkGroupList/WorkGroup[%d]/Usable"%int(index), "0")
            raise web.seeother("/cfg/groups/")
        elif cat == u"commit":
            config.Put("/WorkGroupList/WorkGroup[%d]/Usable"%int(index), "1")
            raise web.seeother("/cfg/groups/")
        elif cat is None:
            raise web.seeother('base/')
        else:
            form = group_form(index, cat).get()
            #print form.render()
            return context.render.config_group(True, groups, form)
            
        
    def POST(self, index = None, cat = None):
        # print web.data()
        data = web.input()
        for k,v in data.iteritems():
          
            if k is not None and not k == "submit" and not k[0] == "@":
                try:
                    print "PUT",k,v
                    config.Put(k,v.encode("gbk"))
                except :
                
                    print tb.format_exc()
                    pass
        raise web.seeother("/cfg/groups/%s/%s/"%(index, cat))
        
