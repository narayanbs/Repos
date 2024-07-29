#
# Synchronization patterns
# Lightswitch
#
from dataclasses import dataclass
from threading import Lock, Semaphore, Thread, current_thread
import time


@dataclass
class ProgArg:
    lock: Lock
    sem: Semaphore
    counter: int


def enter(args):
    print("Entering")
    args.lock.acquire()
    args.counter += 1
    if args.counter == 1:
        args.sem.acquire()
    args.lock.release()


def leave(args):
    print("Leaving")
    args.lock.acquire()
    args.counter -= 1
    if args.counter == 0:
        args.sem.release()
    args.lock.release()


def conc_thread(args):
    print("Concurrent thread Starting", current_thread().name)
    enter(args)
    time.sleep(0.1)
    leave(args)
    print("Concurrent thread Finishing", current_thread().name)


def mutex_thread(args):
    print("Mutex thread Starting", current_thread().name)
    args.sem.acquire()
    time.sleep(0.1)
    args.sem.release()
    print("Mutex thread Finishing", current_thread().name)


if __name__ == "__main__":
    lock = Lock()
    sem = Semaphore(1)
    counter = 0
    args = ProgArg(lock, sem, counter)

    conc_thread = Thread(target=conc_thread, args=(args,))
    mutex_thread = Thread(target=mutex_thread, args=(args,))

    conc_thread.start()
    mutex_thread.start()

    conc_thread.join()
    mutex_thread.join()
