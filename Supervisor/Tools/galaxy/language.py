#!/usr/bin/python
# -*- coding: utf-8 -*-#

help_common = u"全局帮助"
help= u"帮助： esc:退出 q :回退/退出  r :刷新内容  m :回到主界面  方向键选择按钮"

welcom_info=u"\n欢迎使用\n\nGala2.0 Supervisor Tool \n\nAgree Inc. (2012)    All Rights Reserved\n"
exit_info=u"确定要退出吗? (y/n)[y]"

workgroup=u"工作组"
svc = u"处理器"
lsr = u"接入适配器"
trigger= u"清洗器"
ctr = u"管理器"

cnr = u"接出适配器"
ms_group=u'监控服务'
ms_guard=u'监控服务守护'
wg_guard = u"工作组守护"

ok = u"成功"
fail = u"失败"

start = u"启动"
restart = u"重启"
refresh=u"刷新"
stop = u"停止"
cat_status = u"查看"
state = u"状态"
#for popup
cancel=u"取消"
confirm=u"确定"
ask=u"确定要执行此操作吗？\n"
###############

starting=u"正在启动"
already_start = u"已启动"
stopping=u"正在停止"
already_stop = u"已停止"
running = u"..."

class LSRPage:
    tag= u"接入适配器"
    help_info=u"接入适配器帮助"

class CNRPage:
    tag= u"接出适配器"
    help_info=u"接出适配器帮助"

class SVCPage:
    tag=u"查看处理器"
    help_info=u"处理器帮助"

class SVCOSPage:
    tag=u"处理器"
    start=u"启动处理器"
    stop=u"停止处理器"
    status=u"查看处理器"
    add=u"增加处理器"
    help_info=u"处理器帮助"
    
class StartupPage:
    version = "2.0.1"
    compiler = "gcc 4.6"
    build_date = "2012-06-27"

    build_info =  u"版本号     : %s\n"%version
    build_info += u"编译器版本 : %s\n"%compiler
    build_info += u"构建时间   : %s\n"%build_date

    startup_tip = u"\n\n<按任意键继续...>"

    start_page_help = ""

    help=u"\n使用说明:"
    info=u"按ESC键退出\n其他键继续\n"
class StatusPage:
    tag=u"工作组管理"
    help_info=u"帮助"
    
class FirstPage:
    platform=u"平台管理"
    workgroup=u"工作组管理"
    monitor=u"服务管理"
    tag=u"主界面"

    platform_1=u"1.平台管理"
    workgroup_2=u"2.工作组管理"
    monitor_3=u"2.服务管理"
    help = u"%s\n%s\n%s"%(platform,workgroup,monitor)

class PlatformManage:
    tag=u"平台管理"
    start=u"1. 启动平台"
    stop=u"2. 停止平台"
    help_info=u"帮助"

class MgPage:
    tag=u"服务管理"
    help_info=u"帮助"
class StartPage:
    title = u"启动Gala平台        "
    tag=u"启动平台"
    help_info=u"帮助"


class StopPage:
    title= u"停止Gala平台         "
    tag=u"停止平台"
    help_info=u"停止页面帮助"

class ProcessPage:
    start_time=u"启动时间" 
    stop_time=u"结束时间" 
    uptime=u"运行时间" 
    process_name=u"名称"
    mem=u"虚拟内存"
    rss=u"物理内存"
    cpu=u"CPU"
    help_info=u"进程页面帮助"

class ProcessState:
    STOPPED = 0
    STARTING = 10
    RUNNING = 20
    BACKOFF = 30
    STOPPING = 40
    EXITED = 100
    FATAL = 200
    UNKNOWN = 1000

    fatal=u"启动失败"
    normal=u"运行中"
    unknown=u"未知"

    state_map={FATAL:fatal,RUNNING:normal}

class SummaryPage:
    err=u"  出现错误的工作组" 
    _tag=u"错误汇总"

class ErrorPage:
    tag=u"错误"
