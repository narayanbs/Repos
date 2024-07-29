import socket
import sys


def main(host, port):
    sock = None
    server_address = (host, port)
    message = b"This is a sample message"
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
        print(f"Sending {message}")
        sock.sendto(message, server_address)
        print(f"Waiting to receive")
        data, server_addr = sock.recvfrom(4096)
        print(f"Received {data}")
    except OSError as e:
        print(e)
        sys.exit(1)
    finally:
        if sock:
            sock.close()


if __name__ == "__main__":
    server_address = ("localhost", 8080)
    main(*server_address)
