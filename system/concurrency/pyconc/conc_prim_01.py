#
# Locks and Reentrant Locks
#
# use RLock if you want reentrant Lock
# for example
# num = 0
# lock = Threading.Lock()
#
# lock.acquire()
# num += 1
# lock.acquire() # This will block.
# num += 2
# lock.release()
#
#  But With RLock, that problem doesn't happen.
#
# lock = Threading.RLock()
#
# lock.acquire()
# num += 3
# lock.acquire() # This won't block.
# num += 4
# lock.release()
# lock.release() # You need to call release once for each call to acquire.
#

from threading import Lock, RLock, Thread

lock = Lock()
g = 0


def add_one():
    global g
    lock.acquire()
    g += 1
    lock.release()


def add_two():
    global g
    lock.acquire()
    g += 2
    lock.release()


threads = []
for func in [add_one, add_two]:
    threads.append(Thread(target=func))
    threads[-1].start()

for thread in threads:
    thread.join()

print(g)
