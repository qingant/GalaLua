import sigar
import time
import re
import afa_context as context

def read_cpu_usage():
    cpusg = sigar.open()
    cpu = cpusg.cpu()
    cpuidle = cpu.idle()
    cputotal = cpu.total()
    cpusg.close()
    return cpuidle, cputotal


def getCPU_info():
    idle, total = read_cpu_usage()
    cpuper = (float(1) - float(float(idle)/float(total)))*100
    return cpuper

def getMem_info():
    memsg = sigar.open()
    memory = memsg.mem()
    memfree = memory.free_percent()
    memsg.close()
    return memfree

def getMem_total_info():
    memsg = sigar.open()
    memory = memsg.mem()
    mem_ram = memory.ram()
    mem_total = memory.total()/(1024*1024)
    memsg.close()
    return mem_ram

cpuarray = []
memarray = []

class meminfo:
    def GET(self):
        memory_ram = getMem_total_info()
        inner_html = "<p>Memory Ram: %s MB</p><br />" % memory_ram
        return inner_html

class cpu:
    def GET(self):
        cper = getCPU_info()
        cpuarray.append(cper)
        return cper

class mem:
    def GET(self):
        mper = getMem_info()
        memarray.append(mper)
        return mper

class printt:
    def GET(self):
        pcpuarray = cpuarray
        pmemarray = memarray
        a = len(pcpuarray)
        rtime = time.strftime("%Y-%m-%d %H: %M: %S", time.localtime(time.time()))
        if a > 100:
            Rcpuarray = pcpuarray[0:100]
            Rmemarray = pmemarray[0:100]
        else: 
            Rcpuarray = pcpuarray
            Rmemarray = pmemarray
        print pcpuarray
        file = open('./history/record [' + rtime + '].txt', 'w')
        file.write('The CPU Record: \n')
        for cpurecord in Rcpuarray:
            file.write(str(round(cpurecord, 2)) + ' ')
        file.write('\n')
        file.write('The Mem Record: \n')
        for memrecord in Rmemarray:
            file.write(str(round(memrecord, 2)) + ' ')
        file.write('\n')
        file.close()
        print a
        print "testing"
        return "find it"

class resetall:
    def GET(self):
        del cpuarray[:]
        del memarray[:]
        return "clear all"

class monitor:
    def GET(self):
        return context.render.cpu_mem()
