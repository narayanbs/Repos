#
# Program that emulates the shell logic of 
# creating a process group for a pipeline
#
import os
import signal
import time

print(f"Parent: {os.getpid()}")
pgpid = os.fork()
if not pgpid:
    os.setpgid(os.getpid(), os.getpid())
    os.execve("./ex04.py", ["./ex04.py",], os.environ)

print(f"pgid: {pgpid}")

pid = os.fork()
if not pid:
    os.setpgid(os.getpid(), pgpid)
    os.execve("./ex04.py", ["./ex04.py",], os.environ)

pid = os.fork()
if not pid:
    os.setpgid(os.getpid(), pgpid)
    os.execve("./ex04.py", ["./ex04.py",], os.environ)

tty_fd = os.open("/dev/tty", os.O_RDONLY)

os.tcsetpgrp(tty_fd, pgpid)

for i in range(3):
    pid, status = os.waitpid(-1, 0)

h = signal.signal(signal.SIGTTOU, signal.SIG_IGN)
os.tcsetpgrp(tty_fd, os.getpgrp())
signal.signal(signal.SIGTTOU, h)

print("got foreground back")

time.sleep(99999)

