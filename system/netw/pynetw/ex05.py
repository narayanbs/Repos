import socket
import sys


def getConn(host, port):
    sock = None
    ai_family = socket.AF_UNSPEC
    ai_socktype = socket.SOCK_DGRAM
    ai_protocol = 0
    ai_flags = socket.AI_PASSIVE

    hints = (ai_family, ai_socktype, ai_protocol, ai_flags)

    for response in socket.getaddrinfo(host, port, *hints):
        family, socktype, proto, canonname, sa = response
        try:
            sock = socket.socket(family, socktype, proto)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        except OSError as e:
            print(e)
            sock = None
            continue

        try:
            sock.bind(sa)
        except OSError as e:
            print(e)
            sock = None
            continue
        break

    if sock is None:
        print("Socket could not be bound")
        sys.exit(1)

    return sock


def main():
    HOST = None
    PORT = 8080
    conn = getConn(HOST, PORT)

    while True:
        print("Waiting to receive message")
        data, address = conn.recvfrom(1024)
        print(f"Received {len(data)} bytes from {address}")
        print(data)

        if data:
            sent = conn.sendto(data, address)
            print(f"Sent {sent} bytes back to {address}")


if __name__ == "__main__":
    main()
