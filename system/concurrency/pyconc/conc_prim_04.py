#
# Condition
#
import random, time
from threading import Condition, Thread

condition = Condition()

box = []


def producer(box, nitems):
    for i in range(nitems):
        time.sleep(random.randrange(2, 5))
        condition.acquire()
        num = random.randint(1, 10)
        box.append(num)
        condition.notify()
        print("Produced:", num)
        condition.release()


def consumer(box, nitems):
    for i in range(nitems):
        condition.acquire()
        condition.wait()
        print("{}. Acquired {}".format(time.ctime(), box.pop()))
        condition.release()


threads = []

nloops = random.randrange(3, 6)

for func in [producer, consumer]:
    threads.append(Thread(target=func, args=(box, nloops)))
    threads[-1].start()
for thread in threads:
    thread.join()
print("All done")
