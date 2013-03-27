# -*- coding: utf-8 -*-

import web
import lxml.etree
import os, sys
import afa_context as context
os.chdir(os.path.dirname(__file__))
def_path = os.path.join("static/test.xml")


class login:
    def GET(self):
        name = "welcom"
        return context.render.login(name);

class logout:
    def GET(self):
        context.session.loginned = 0
        context.session.kill()
        name = "欢迎使用AFA web配置监控工具"
        return context.render_index.index("GalaWebTools",name)

class checkout:
    def POST(self):
      # name, passwd = web.input().usrname, web.input().passwd
      
      # file = lxml.etree.XML(def_path)
      # root = file.getroot()
      # user = root.findall("user")
      # psw = root.findall("passwd")
      
      # for i in range(len(user)):
      #    if name == user[i].text and passwd == psw[i].text:
      # web.ctx.session.username = name
      # web.ctx.session.login = 1
      raise web.seeother("/")




