# -*- coding: utf-8 -*-
import web
import sqlite3
import afa_context as context
import os

user_dir = os.path.join(os.getenv("HOME"), "var/user_admin/")
    
if not os.path.exists(user_dir):
    os.mkdir(user_dir)
os.chdir(user_dir)


class add_page:
    def GET(self):
        return context.render.register()

class add_user:
    def POST(self):
        data = web.input()
        name = data["user"]
        pwd = data["passwd"]
        role = data["user_role"]
        create_sql = '''CREATE TABLE  IF NOT EXISTS user_admin (user_name varchar, user_pwd varchar, user_role varchar)'''
        conn = sqlite3.connect(user_dir+'user_data.db')
        c = conn.cursor()
        c.execute(create_sql)
        
        for r in c.execute("SELECT * FROM user_admin"):
            if r[0] == name:
                print "error"
                conn.close()
                return "there is a user name called" + name
                
        insert_sql = "INSERT INTO user_admin VALUES('%s', '%s', '%s')" % (name, pwd, role)
        context.session.username = name
        context.session.userrole = role
        c.execute(insert_sql)
        conn.commit()
        conn.close()  
        raise web.seeother("/")
