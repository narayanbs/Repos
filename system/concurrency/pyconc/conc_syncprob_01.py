# Synchronization problems
# 
# Producer consumer problem with BoundedQueue
#
import threading
import time


class IllegalArgumentError(ValueError):
    pass


class BoundedQueue:
    def __init__(self, capacity):
        if capacity < 1:
            raise IllegalArgumentError("Capacity should be 1 or higher")
        self.items = [0] * capacity
        self.front = 0
        self.back = 0
        self.len = 0
        self.capacity = capacity

    def add(self, item):
        if self.len == self.capacity:
            print("Queue is full")
            return
        self.items[self.back] = item
        self.back = (self.back + 1) % self.capacity
        self.len += 1

    def pop(self):
        if self.len == 0:
            print("Queue is empty")
            return
        result = self.items[self.front]
        self.front = (self.front + 1) % self.capacity
        self.len -= 1
        return result


def enqueue(queue, data, sem_space, sem_items, lock):
    sem_space.acquire()
    lock.acquire()
    queue.add(data)
    lock.release()
    sem_items.release()


def dequeue(queue, sem_space, sem_items, lock):
    sem_items.acquire()
    lock.acquire()
    result = queue.pop()
    lock.release()
    sem_space.release()
    return result


def produce(queue, sem_space, sem_items, lock):
    data = ["this", "is", "funny", "shit", "end"]
    for d in data:
        print(threading.current_thread().name, d)
        enqueue(queue, d, sem_space, sem_items, lock)
        time.sleep(0.5)


def consume(queue, sem_space, sem_items, lock):
    while True:
        result = dequeue(queue, sem_space, sem_items, lock)
        if result == "end":
            break
        print(threading.current_thread().name, ": -->", result)


if __name__ == "__main__":
    q = BoundedQueue(5)
    sem_space = threading.Semaphore(5)
    sem_items = threading.Semaphore(0)

    producerLock = threading.Lock()
    consumerLock = threading.Lock()

    threads = []

    for i in range(0, 2):
        threads.append(
            threading.Thread(
                target=produce, args=(q, sem_space, sem_items, producerLock)
            )
        )
        threads.append(
            threading.Thread(
                target=consume, args=(q, sem_space, sem_items, consumerLock)
            )
        )

    for t in threads:
        t.start()

    for t in threads:
        t.join()
