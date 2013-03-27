# -*- coding: utf-8 -*-
import web
from web import form

import afa_context as context

#session = web.ctx.session

class look:
    def GET(self):  
        self.info =   form.Form(
              form.Textbox("port", value = "9000", readonly="1", description="管理服务端口"),
              #form.Textbox("email", description="E-Mail"),
              # form.Password("password",  description="Password"),
              # form.Password("password2", description="Repeat password"),
              form.Button("submit", type="submit", html=u"编辑此页"),
              # validators = [
              #     form.Validator("Passwords did't match", lambda i: i.password == i.password2)]
              )
        return context.render.config_admin(True, self.info)


class editor:
 
    def GET(self):
        info =   form.Form(
            form.Textbox("port", value = "9000", description="管理服务端口"),
            # form.Textbox("email", description="E-Mail"),
            # form.Password("password",  description="Password"),
            # form.Password("password2", description="Repeat password"),
            form.Button("submit", type="submit", html=u"提交"),
           
            )
        return context.render.config_admin(False, info)

    def POST(self):
        print web.data()
        raise web.seeother('/cfg/admin/')
