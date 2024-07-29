import queue
import select
import socket
import sys


def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
    server.setblocking(False)

    server_address = ("localhost", 10000)
    print("Starting up on {}".format(*server_address))

    server.bind(server_address)
    server.listen(5)

    inputs = [server]

    outputs = []

    message_queues = {}

    while inputs:
        print("waiting for the next event")

        # The following lines are for selectslowclient
        timeout = 1
        readable, writable, exceptional = select.select(
            inputs, outputs, inputs, timeout
        )

        if not (readable or writable or exceptional):
            print("Timedout, do some other work here")
            continue

        # uncomment below and comment above for a normal selectclient
        # readable, writable, exceptional = select.select(inputs, outputs, inputs)

        for s in readable:
            if s is server:
                connection, client_addr = s.accept()
                print(f"Connection from {client_addr}")
                connection.setblocking(False)
                inputs.append(connection)

                message_queues[connection] = queue.Queue()

            else:
                data = s.recv(1024)
                if data:
                    print(f"Received {data} from {s.getpeername()}")
                    message_queues[s].put(data)

                    if s not in outputs:
                        outputs.append(s)

                else:
                    print(f"Closing socket")

                    if s in outputs:
                        outputs.remove(s)
                    inputs.remove(s)
                    s.close()

                    del message_queues[s]

        for s in writable:
            try:
                next_msg = message_queues[s].get_nowait()
            except queue.Empty:
                print(f"{s.getpeername()} queue empty")
                outputs.remove(s)
            else:
                print(f"sending {next_msg} to {s.getpeername()}")
                s.send(next_msg)

        for s in exceptional:
            print(f"exception condition {s.getpeername()}")
            inputs.remove(s)
            if s in outputs:
                outputs.remove(s)
            s.close()

            del message_queues[s]


if __name__ == "__main__":
    main()
