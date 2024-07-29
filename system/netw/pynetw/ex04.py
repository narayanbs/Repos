import socket
import sys

def getConn(host, port):
    ai_family = socket.AF_UNSPEC
    ai_socktype = socket.SOCK_STREAM
    ai_protocol = 0
    ai_flags = 0

    sock = None

    hints = (ai_family, ai_socktype, ai_protocol, ai_flags)

    for response in socket.getaddrinfo(host, port, *hints):
        family,socktype, proto, canonname, sa = response
        try:
            sock = socket.socket(family, socktype, proto)
        except OSError as msg:
            print(msg)
            sock = None
            continue
        try:
            sock.connect(sa)
        except OSError as msg:
            print(msg)
            sock.close()
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

    conn = getConn(HOST, PORT)
    with conn:
        conn.sendall(b"Hello world. I am increasing")
        data = conn.recv(1024)

    print(f"Received {data}")

if __name__ == "__main__":
    main()
