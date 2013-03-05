#!/usr/bin/python
# -*- coding: gbk -*-


"""

"""

import urwid
import urwid.raw_display
import time
import sys
from galaxy import language as lang
from galaxy import supervisor_api as monitor
from galaxy import configure as configure
from galaxy import const as const
import thread

def TimedSignal(obj, event, timeout, callback, work = None):
    urwid.register_signal(type(obj), event)
    urwid.connect_signal(obj, event, callback)
    thread.start_new(lambda:(time.sleep(timeout), work if work is None else work(), urwid.emit_signal(obj, event)), ())


