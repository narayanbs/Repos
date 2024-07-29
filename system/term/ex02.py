#
# program that receives reads input from stdin 
# sent by ex01 and prints it
#
# We are demonstrating how we can send a signal 
# to all the processes of a process group
# by using killgrp() or kill with negative PGID parameter
#
# first run the program as shown below 
#
# python ./ex01.py | python ./ex02.py
#
# then open another terminal and send sigterm signal to the PGID
# ex: kill -15 -123
#
import fileinput
import signal
import os
import sys

def signal_handler(signum, frame):
    print(f"[stdin] signal number: {signum}", file=sys.stderr)
    os._exit(signum)

signal.signal(signal.SIGTERM, signal_handler)

for i, line in enumerate(fileinput.input()):
    print(f"{i}: {line.rstrip()}")

