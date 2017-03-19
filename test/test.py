#!/usr/bin/python

import os
import subprocess
import time

procServer = subprocess.Popen(["../../bin/server", "10010"])
procClient = subprocess.Popen(["../../bin/client", "127.0.0.1", "10010"])

procClient.wait();
procServer.terminate()
