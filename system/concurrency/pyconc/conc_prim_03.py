#
# Event
#
import random, time
from threading import Event, Thread

event = Event()


def waiter(event, nloops):
    for i in range(nloops):
        print("{}. Waiting for the flag to be set".format(i + 1))
        event.wait()
        print("Wait complete at {}".format(time.ctime()))
        event.clear()
        print()


def setter(event, nloops):
    for i in range(nloops):
        time.sleep(random.randrange(2, 5))
        event.set()


threads = []
nloops = random.randrange(3, 6)

threads.append(Thread(target=waiter, args=(event, nloops)))
threads[-1].start()
threads.append(Thread(target=setter, args=(event, nloops)))
threads[-1].start()

for thread in threads:
    thread.join()

print("All done")
