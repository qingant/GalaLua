# -*- coding: utf-8 -*-
import web
import afa_context as context

class pageContent:
    def GET(self):
        name = u"欢迎使用Web配置监控工具"
        return context.render.index("self", name)

class index:
    def GET(self):
        name = u"欢迎使用Web配置监控工具"
        return context.render_index.index("GalaWebTools", name)
