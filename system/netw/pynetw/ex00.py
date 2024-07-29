import socket
from urllib.parse import urlparse


HOSTS = [
    'amazon.in',
    'pymotw.com',
    'www.python.org',
    'facebook.com',
]
URLS = [
        'http://www.python.org',
        'https://www.mybank.com',
        'ftp://prep.ai.mit.edu',
        'gopher://gopher.micro.umn.edu',
        'smtp://mail.example.com',
        'imap://mail.example.com',
        'imaps://mail.example.com',
        'pop3://pop.example.com',
        'pop3s://pop.example.com',
]
ports = [80, 443, 21, 70, 25, 143, 993, 110, 995]
protocols= ['icmp', 'udp', 'tcp']

def main():
    print("Host--------------------------")
    print(f"Host : {socket.gethostname()}")
    print()

    print("Host By Name ---------------------------------")
    for host in HOSTS:
        try:
            print(f"{host} : {socket.gethostbyname(host)}")
        except socket.error as msg:
            print(msg)
    print()

    print("Host Name Ex ---------------------------------")
    for host in HOSTS:
        try:
            hostname, aliases, addresses = socket.gethostbyname_ex(host)
            print(f"{host} : {hostname}: {aliases}: {addresses}")
        except socket.error as msg:
            print(msg)
    print()

    print("Host by Addr------------------------------------")
    try:
        addr = "18.161.216.62"
        hostname, aliases, addresses = socket.gethostbyaddr(addr)
        print(f"{addr}  :   {hostname}, {aliases}, {addresses}")
    except socket.error as msg:
        print(msg)
    print()

    print("Fully qualified domain name ----------------------")
    try:
        print(f"{socket.getfqdn('www.skyscanner.net')}")
    except socket.error as msg:
        print(msg)
    print()

    print("Service by name ---------------------------------")
    for url in URLS:
        parsed_url = urlparse(url)
        port = socket.getservbyname(parsed_url.scheme)
        print(f"{parsed_url.scheme} --> {port}")
    print()

    print("Service by port ---------------------------------")
    for port in ports:
        name = socket.getservbyport(port)
        print(f"{port} --> {name}")
    print()

    print("Get proto by name -------------------------------")
    def get_constants(prefix):
        return { getattr(socket,n): n for n in dir(socket) if n.startswith(prefix)}
    
    prototypes = get_constants("IPPROTO_")

    for proto in protocols:
        proto_num = socket.getprotobyname(proto)
        const_name = prototypes[proto_num]
        print(f"{proto:>4} -> {proto_num:2d} (socket.{const_name:<12} = {getattr(socket, const_name):2d})")



if __name__ == "__main__":
    main()