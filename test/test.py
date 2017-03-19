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
    for line in fileLog:
        #e.g. "01:30:15.958 [TEST] socket=1 queue message MessageEcho"
        m = regex.match(line)
        if m:
            lineLogTEST = fileLogTEST.readline()[20:].rstrip('\n')
            line = m.group(1)
            # ignore port number in "accept client socket=3 127.0.0.1:43337"
            if line.find('accept client socket=') == 0:
                line = line[:line.find(':')]
                lineLogTEST = lineLogTEST[:lineLogTEST.find(':')]
            if line == lineLogTEST:
                pass
            else:
                print "Not match:"
                print line 
                print lineLogTEST
                print "Fail: " + logFilename
                return False

    lineLogTEST = fileLogTEST.readline()  # TEST file should be EOF
    if lineLogTEST != '':
        print "Not match: more in TEST file: " + lineLogTEST.rstrip('\n')
        print "Fail: " + logFilename
        return False

    fileLog.close()
    fileLogTEST.close()
    print "Success: " + logFilename
    return True

# start
os.remove("/var/log/photona/log.photona.client");
os.remove("/var/log/photona/log.photona.server");

# run programs
procServer = subprocess.Popen(["../bin/photona.server", "10010"])
procClient = subprocess.Popen(["../bin/photona.client", "127.0.0.1", "10010"])

procClient.wait()
procServer.terminate()

# compare
compare("/var/log/photona/log.photona.client", "test.log.photona.client.TEST")
compare("/var/log/photona/log.photona.server", "test.log.photona.server.TEST")
