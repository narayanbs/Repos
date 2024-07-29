#
# Synchronization Patterns
# Turnstile
#
import threading
import time


def random_method(sem):
    print(threading.current_thread().name, "started")
    sem.acquire()
    sem.release()
    print(threading.current_thread().name, "completed")


if __name__ == "__main__":
    threads = []
    sem = threading.Semaphore(0)
    for i in range(1, 4):
        threads.append(threading.Thread(target=random_method, args=(sem,)))

    for thread in threads:
        thread.start()

    print("waiting...")
    time.sleep(0.5)

    sem.release()

    for thread in threads:
        thread.join()
