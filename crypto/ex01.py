# Hash functions are deterministic, in that they 
# produce output of identical size regardless of the input size.
# digest refers to the fixed length output derived from some variable length input
# Hash functions are irreversible by design meaning there is no fast algorithm to restore
# the input message from hash value.
#
# Hash functions are used for document integrity, storing passwords, 
# generating unique ID, pseudorandom numbers
#
# Modern secure hash algorithms include 
# SHA-3 and BLAKE2. 
#
# SHA-2, SHA-256, SHA-512
# SHA-3, SHA3-256, SHA3-512,
# Keccak-256
#
# Unlike SHA-2 family SHA-3 is not vulnerable to length extension attack
#
# Keccak-256 requires third party library
# pip install pycryptodome 
#

import hashlib, binascii
from Crypto.Hash import keccak

def hashfunc():
    text = 'hello'
    data = text.encode("utf8")
    sha256hash = hashlib.sha256(data).digest()
    print("SHA-256:     ",binascii.hexlify(sha256hash))
    sha512hash = hashlib.sha512(data).digest()
    print("SHA-512:     ",binascii.hexlify(sha512hash))

    sha3_256hash = hashlib.sha3_256(data).digest()
    print("SHA3-256:    ",binascii.hexlify(sha3_256hash))
    sha3_512hash = hashlib.sha3_512(data).digest()
    print("SHA3-512:    ",binascii.hexlify(sha3_512hash))

    blake2s = hashlib.new("blake2s", data).digest()
    print("BLAKE2s:     ", binascii.hexlify(blake2s))

    ripemd160 = hashlib.new("ripemd160", data).digest()
    print("RIPEMD-160:  ", binascii.hexlify(ripemd160))

    keccak256 = keccak.new(data=data, digest_bits=256).digest()
    print("KECCAK256:   ", binascii.hexlify(keccak256))

if __name__ == "__main__":
    hashfunc()





