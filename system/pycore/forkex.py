import os
import time


def child_process():
    print(f"Child pid {os.getpid()} started")
    time.sleep(2)
    print(f"Child pid {os.getpid()} finished")


def parent_process():
    children = []
    num_children = 5

    for _ in range(num_children):
        pid = os.fork()
        if pid == 0:
            child_process()
            os._exit(0)
        else:
            children.append(pid)

    print("Parent process started")
    while children:
        pid, _ = os.waitpid(-1, os.WNOHANG)
        if pid == 0:
            print("No child processes have exited. doing other work...")
            time.sleep(1)
        else:
            if pid in children:
                children.remove(pid)
                print(f"Child PID {pid} has finished")

    print("All child processes have finished")


if __name__ == "__main__":
    parent_process()
