import socket
import sys


def main():
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM, 0)
    server_address = "./uds_socket"
    print(f"Connecting to {server_address}")
    try:
        sock.connect(server_address)
    except socket.error as msg:
        print(msg)
        sys.exit(1)

    try:
        message = b"This is the message. It will be repeated"
        print(f"Sending {message}")
        sock.sendall(message)

        amount_received = 0
        amount_expected = len(message)

        while amount_received < amount_expected:
            data = sock.recv(16)
            amount_received += len(data)
            print(f"Received {data}")
    finally:
        print("Closing socket")
        sock.close()


if __name__ == "__main__":
    main()
