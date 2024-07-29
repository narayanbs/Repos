import socket
import sys


def getConn(host, port):
    ai_family = socket.AF_UNSPEC
    ai_socktype = socket.SOCK_STREAM
    ai_protocol = 0
    ai_flags = socket.AI_PASSIVE

    hints = (ai_family, ai_socktype, ai_protocol, ai_flags)

    sock = None

    for response in socket.getaddrinfo(host, port, *hints):
        family, socktype, proto, canonname, sa = response
        try:
            sock = socket.socket(family, socktype, proto)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        except OSError as msg:
            print(msg)
            sock = None
            continue
        try:
            sock.bind(sa)
            sock.listen(1)
        except OSError as msg:
            print(msg)
            sock = None
            continue
        break
    if sock is None:
        print("Could not open socket")
        sys.exit(1)

    return sock


def main():
    HOST = None
    PORT = 50007
    sock = getConn(HOST, PORT)

    while True:
        conn, addr = sock.accept()
        print(f"Client {addr} Connected")
        buf = b""
        with conn:
            while True:
                data = conn.recv(16)
                if not data:
                    break
                buf += data
                if len(data) < 16:
                    break
            if buf != b"":
                conn.sendall(buf)


if __name__ == "__main__":
    main()
