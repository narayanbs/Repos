import os
import sys
import time


def child_process(child_id):
    print(f"Child process {os.getpid()} (ID: {child_id}) started.")
    time.sleep(5)
    print(f"Child process {os.getpid()} (ID: {child_id}) exiting")
    os._exit(child_id)


def parent_process(num_children):
    child_pids = []

    for i in range(num_children):
        try:
            pid = os.fork()
        except OSError as e:
            print(f"fork failed: {e}", file=sys.stdout)
            sys.exit(1)

        if pid == 0:
            try:
                child_process(i)
            except Exception as e:
                print(f"Error in child process {i}: {e}", file=sys.stdout)
                os._exit(1)
        else:
            child_pids.append(pid)
    print(f"Parent process {os.getpid()} waiting for {num_children} child processes.")

    remaining_children = num_children

    while remaining_children > 0:
        try:
            pid, status = os.waitpid(-1, os.WNOHANG)

            if pid == 0:
                print("No child process finised yet, checking again...")
                time.sleep(1)
            else:
                if pid in child_pids:
                    child_pids.remove(pid)
                    remaining_children -= 1
                    if os.WIFEXITED(status):
                        exit_code = os.WEXITSTATUS(status)
                        print(f"Child process {pid} exited with status {exit_code}")
                    elif os.WIFSIGNALED(status):
                        signal_number = os.WTERMSIG(status)
                        print(f"Child process {pid} was terminated by signal {signal_number}")
                    else:
                        print(f"Child process {pid} exited with unknown status {status}")

                else:
                    print(f"Unexpected child process PID {pid} reported")
        except OSError as e:
            print(f"Error while waiting for child process : {e}", file=sys.stdout)
            break


if __name__ == "__main__":
    num_children = 3
    parent_process(num_children)

