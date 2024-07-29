import os
import socket
import sys


def main():
    server_address = "./uds_socket"

    try:
        os.unlink(server_address)
    except OSError:
        if os.path.exists(server_address):
            raise

    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

    print(f"starting up on {server_address}")
    sock.bind(server_address)
    sock.listen(1)

    while True:
        print("Waiting for a connection")
        connection, client_addr = sock.accept()
        try:
            print(f"Connection from {client_addr}")

            while True:
                data = connection.recv(16)
                print(f"Received {data}")
                if data:
                    print("Sending data back to client")
                    connection.sendall(data)
                else:
                    print(f"No data from {client_addr}")
                    break
        finally:
            connection.close()


if __name__ == "__main__":
    main()
