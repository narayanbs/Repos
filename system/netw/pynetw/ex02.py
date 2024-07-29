import binascii
import socket

for string_address in ["192.168.0.1", "127.0.0.1"]:
    packed = socket.inet_pton(socket.AF_INET, string_address)
    print(f"Original {string_address}")
    print(f"Packed: {binascii.hexlify(packed)}")
    print(f"Unpacked: {socket.inet_ntop(socket.AF_INET, packed)}")
