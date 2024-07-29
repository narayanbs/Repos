# long running process that prints
# integers to stdout 

import signal
import os
import time
import sys

def signal_handler(signum, frame):
    print(f"[print] signal number: {signum}", file=sys.stderr)
    os._exit(signum)

signal.signal(signal.SIGTERM, signal_handler)

print(f"PGID: {os.getpgrp()}", file=sys.stderr)

for i in range(9999):
    print(f"{i}")
    sys.stdout.flush()
    time.sleep(1)
    
