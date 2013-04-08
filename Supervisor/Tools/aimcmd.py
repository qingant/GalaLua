#!/usr/bin/env python

import getopt
import sys
import cmd
import front
import argparse
import urwid 
import sigar
import logging
import os
from galaxy import utils


utils.checkPathAndCreate()

LOGFILE=os.path.join(utils.getInstanceHome(),"log/aimcmd.log")
LOGLEVEL=logging.DEBUG

StatusPage=cmd.status.StatusPage
StartPage = cmd.group.StartPage
#RestartPage = cmd.group.RestartPage
StopPage = cmd.group.StopPage

def do_start(win,args):
    win.action=StartPage
    win.function=StartPage.start
    if args.group is not None:
        win.name=args.group[0]
    win._bigtext=args.bigtext
def do_stop(win,args):
    win.action=StopPage
    win.function=StopPage.start
    if args.group is not None:
        win.name=args.group[0]
    win._bigtext=args.bigtext
#def do_restart(win,args):
#    win.action=RestartPage
#    win.function=RestartPage.start
#    if args.group is not None:
#        win.name=args.group[0]
#    win._bigtext=args.bigtext
def do_status(win,args):
    win.action=StatusPage
    win.function=None
    if args.group is not None:
        win.name=args.group[0]
    win._bigtext=args.bigtext

#COMMANDS={"start":do_start,"stop":do_stop,"restart":do_restart,"status":do_status}
COMMANDS={"start":do_start,"stop":do_stop,"status":do_status}

def main(win):

    logging.basicConfig(filename=LOGFILE,format='%(asctime)s [%(levelname)s]:%(message)s',level=LOGLEVEL,datefmt='%Y-%m-%d %H:%M:%S')

    #logging.info("*********************************************************************")
    #logging.info("* Gala2.0 Supervisor Tool                                           *")
    #logging.info("*                                                                   *")
    #logging.info("* Agree Inc. (2012)    All Rights Reserved                          *")
    #logging.info("*********************************************************************")

    parser=argparse.ArgumentParser()
    parser.add_argument("command",default=None,nargs='?',choices=COMMANDS.keys(),help='start|stop|status target')
    parser.add_argument("-b","--bigtext",action='store_true',help="enable bigtext")
    parser.add_argument("-g","--group",type=int,nargs=1,help="the target group,-g 0 for group 0")

    args=parser.parse_args()

    win._bigtext=args.bigtext

    if args.command is None:
        return
    COMMANDS[args.command](win,args)

if __name__=='__main__':
    #urwid.set_encoding("GBK")
    win=front.MainLoop()
    main(win)
    win.sg=sigar.open()
    win.main()
    win.sg.close()
#    del win._mp.log
