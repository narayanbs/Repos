#
# Synchronization patterns
# Rendezvous
#
import threading
import time


def download(sem_down, sem_up):
    print("Download started")
    time.sleep(0.5)
    sem_down.release()
    sem_up.acquire()
    print("Download ended")


def upload(sem_down, sem_up):
    print("Upload started")
    time.sleep(1.5)
    sem_up.release()
    sem_down.acquire()
    print("Upload ended")


if __name__ == "__main__":
    sem_down = threading.Semaphore(0)
    sem_up = threading.Semaphore(0)
    threads = []
    threads.append(threading.Thread(target=download, args=(sem_down, sem_up)))
    threads.append(threading.Thread(target=upload, args=(sem_down, sem_up)))

    for t in threads:
        t.start()

    for t in threads:
        t.join()
