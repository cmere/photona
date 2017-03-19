#!/usr/bin/python

import os
import re
import subprocess
import sys
import time

# compare TEST line in log file and TEST log file (pre generated)
def compare(logFilename, TESTFilename):
    fileLog = open(logFilename, 'r')
    fileLogTEST = open(TESTFilename, 'r')
    regex = re.compile("^\d\d:\d\d:\d\d.\d\d\d \[TEST\] (.*)")
    lineLogTEST = fileLogTEST.readline()[21:]
    for line in fileLog:
        #e.g. "01:30:15.958 [TEST] socket=1 queue message MessageEcho"
        m = regex.match(line);
        if m and m.groups == 1:
            if m.group(1) == lineLogTEST:
                lineLogTEST = fileLogTEST.readline()[21:]
            else:
                print "Not match: \n" + line + lineLogTEST
                print "Fail: " + logFilename
                return False

    fileLog.close()
    fileLogTEST.close()
    print "Success: " + logFilename
    return True

# start
os.remove("/var/log/photona/log.client");
os.remove("/var/log/photona/log.server");

# run programs
procServer = subprocess.Popen(["../bin/server", "10010"])
procClient = subprocess.Popen(["../bin/client", "127.0.0.1", "10010"])

procClient.wait()
procServer.terminate()

# compare
compare("/var/log/photona/log.client", "test.log.client.TEST")
compare("/var/log/photona/log.server", "test.log.server.TEST")
