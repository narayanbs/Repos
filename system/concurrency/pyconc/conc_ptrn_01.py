#
# Synchronization Patterns
# signalling
#
import threading
import time


def initialize(sem):
    time.sleep(1.0)
    print("Finished initialization work")
    sem.release()


if __name__ == "__main__":
    sem = threading.Semaphore(0)
    threading.Thread(target=initialize, args=(sem,)).start()
    print("Waiting")
    sem.acquire()
    print("Done")
