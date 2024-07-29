#
# Barriers
#
from random import randrange
from threading import Barrier, Thread
from time import ctime, sleep

num = 4
b = Barrier(num)

names = ["John", "Mathew", "Luke", "George"]


def player():
    name = names.pop()
    sleep(randrange(2, 5))
    print("{} reached the barrier at: {}".format(name, ctime()))
    b.wait()


threads = []
print("Race starts now..")

for i in range(num):
    threads.append(Thread(target=player))
    threads[-1].start()

for thread in threads:
    thread.join()

print()
print("Race over!!")
