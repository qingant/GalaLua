# -*- coding: utf-8 -*-
import web
from lxml import etree
from galaxy import utils
import os,sys


reload(sys)


os.chdir(os.path.dirname(__file__))
sys.setdefaultencoding("gbk")


render = web.template.render("templates/")
render_index = web.template.render("templates/", base="layout")
class AfaContext:
    pass

afa_context = AfaContext()
afa_context.render = render
afa_context.render_index = render_index

def_path = os.path.join(utils.getInstanceHome(), "cfg/ConfigDef.xml")
moniter_path = os.path.join(utils.getInstanceHome(), "cfg/MonitorDef.xml")

afa_context.define = etree.XML(open(def_path).read())

moniter_define = etree.XML(open(moniter_path).read())

import sys
sys.modules["afa_context"] = afa_context


# install handles
from config.admin import look as config_admin_look
from config.admin import editor as config_admin_edit
from config.groups import look as config_groups_look

from user_admin.check import checkout as check
from user_admin.check import login, logout
from user_admin.register import add_user, add_page

from monitor_sys.cpumem import cpu, mem, printt, resetall, meminfo, monitor
from monitor_sys.process import process, process_per_info, node_status
from monitor_sys.save_layout import current_layout, load_current_layout

from manage.admin import look as manage_admin_look
from manage.admin import start as manage_admin_start
from manage.admin import PltStartResult as manage_admin_PltStartResult
from manage.admin import stop as manage_admin_stop
from manage.admin import status as manage_admin_status

from manage_group.group_display import group_dis, group_info_display, group_control_start, group_control_stop, group_control_whole

from page_index.base_index import pageContent, index

urls = (
    #page index
    '/','index',
    '/index/','index',

    #user admin
    '/user/admin/login', 'login',
    '/user/admin/logout', 'logout',
    '/user/admin/check', 'check',
    '/user/admin/register_page', 'add_page',
    '/user/admin/register', 'add_user',

    #system monitor 
    '/monitor/sys/widget/cpu', 'cpu',
    '/monitor/sys/widget/mem', 'mem',
    '/monitor/sys/process', 'process',
    '/monitor/sys/process/(.*)', 'process',
    '/monitor/sys/processDetail', 'process_per_info',
    '/monitor/sys/record', 'printt',
    '/monitor/sys/resetall', 'resetall',
    '/monitor/sys/', 'monitor',
    '/monitor/sys/widget/meminfo', 'meminfo',
    '/monitor/sys/layout', 'current_layout',
    '/monitor/sys/load_current_layout', 'load_current_layout',
    '/monitor/sys/node_status', 'node_status',
    '/monitor/sys/node_status/(.*)', 'node_status',
 
    #system configuration
    '/cfg/admin/','config_admin_look',
    '/cfg/admin/edit/','config_admin_edit',
    '/cfg/edit/','config_editor',
    '/cfg/look/','config_look',
    '/cfg/groups/','config_groups_look',
    '/cfg/groups/(.*)/','config_groups_look',
    '/cfg/groups/(.*)/(.*)/','config_groups_look',
    '/pageContent', 'pageContent',

    #system manage
    '/manage/admin/start/','manage_admin_start',
    '/manage/admin/stop/','manage_admin_stop',
    '/manage/admin/status/','manage_admin_status',
    '/manage/admin/look/','manage_admin_look',
    '/manage/admin/startresult/','manage_admin_PltStartResult',

    #group display
    '/manage/group', 'group_dis',
    '/manage/group_info', 'group_info_display',
    '/manage/control_start', 'group_control_start',
    '/manage/control_stop', 'group_control_stop',
    '/manage/group_control', 'group_control_whole',
    )

app = web.application(urls, globals())

session_dir = os.path.join(os.getenv("HOME"), "var/sessions/")
session = None
if web.config.get('_session') is None:
    if not os.path.exists(session_dir):
        os.mkdir(session_dir)
    session = web.session.Session(app, web.session.DiskStore(session_dir), initializer={'login':"", 'username':'guest', 'userrole':''})
    web.config._session = session
else:
    session = web.config._session

afa_context.session = session
afa_context.app = app
def session_hook():
    web.ctx.session = session
app.add_processor(web.loadhook(session_hook))

web.template.Template.globals['session'] = session

if __name__ == "__main__": 
    app.run()        
