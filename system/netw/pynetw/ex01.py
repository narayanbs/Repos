import socket
import sys

ai_family = socket.AF_UNSPEC
ai_socktype = socket.SOCK_STREAM
ai_prototype = 0
ai_flags = 0

hints = (ai_family, ai_socktype, ai_prototype, ai_flags)


def get_constants(prefix):
    return {getattr(socket, n): n for n in dir(socket) if n.startswith(prefix)}


def main(host):
    families = get_constants("AF_")
    socktypes = get_constants("SOCK_")
    prototypes = get_constants("IPPROTO_")

    for response in socket.getaddrinfo(host, 80, *hints):
        family, socktype, proto, canonname, sockaddr = response
        print(f"Family              : {families[family]}")
        print(f"Sock Type           : {socktypes[socktype]}")
        print(f"Protocol Type       : {prototypes[proto]}")
        print(f"Canonical Name      : {canonname}")
        print(f"Address             : {sockaddr}")
        try:
            print(f"Nameinfo: {socket.getnameinfo(sockaddr,0)}")
        except socket.error as e:
            print("Socket error", e)
        print()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage python pynetw/addrex <host>")
        sys.exit(1)

    main(sys.argv[1])
