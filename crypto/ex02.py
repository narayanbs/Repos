# MAC, HMAC 
# 
# MAC is a string of bits sent along with the message. 
# The string of bits is created by a secret key which is shared by the 
# The sender and receiver. This string of bits is called Message Authentication Code
# or Cryptographic checksum. 
#
# There are different MAC algorithm like HMAC, KMAC, CMAC, GMAC etc
#
# HMAC is a type of MAC algorithm that uses a Cryptographic hash function 
# on the message and shared secret key. 
# The key and message are hashed in seperate steps making it secure.
#
# HMAC (Hash-based Message Authentication Code) is a cryptographic algorithm used to ensure data integrity and authenticity. It combines a cryptographic hash function with a secret key to provide both authentication and integrity verification of the data being transmitted or stored.
# How HMAC Works:
#
#     Input:
#         Message (M): The data to be hashed, like a file or string.
#         Secret Key (K): A private key known only to the sender and receiver.
#         Hash Function (H): A cryptographic hash function like SHA-256, MD5, or SHA-1.
#
#     Process: The HMAC algorithm works as follows:
#
#         Key Preparation:
#             If the key is longer than the hash function's block size, it is hashed first.
#             If the key is shorter than the block size, it is padded with zero bytes to the block size.
#
#         Inner Hashing:
#             The key is XOR'd with the inner padding (0x36 byte repeated).
#             This is then concatenated with the message and hashed using the chosen hash function:
#             H(K ⊕ 0x36 || M).
#
#         Outer Hashing:
#             The key is XOR'd with the outer padding (0x5C byte repeated).
#             The result from the inner hashing step is concatenated with the outer XORed key and hashed:
#             H(K ⊕ 0x5C || H(K ⊕ 0x36 || M)).
#
#     Output:
#         The final output is a fixed-size hash value that serves as the HMAC, which is used to verify the authenticity of the message. The receiver will perform the same process using the shared secret key and check if the HMAC matches.
#
# Example in Detail:
#
# Let's assume:
#
#     Key (K) = "key123"
#     Message (M) = "Hello, world!"
#     Hash function (H) = SHA-256
#
# Step-by-Step Process:
#
#     Prepare the key by ensuring it's the correct length (using padding or hashing if necessary).
#     Calculate the inner hash by XOR'ing the key with the inner padding (0x36 byte) and concatenating it with the message.
#     Hash the result using SHA-256.
#     Calculate the outer hash by XOR'ing the key with the outer padding (0x5C byte) and concatenating it with the result from the inner hash.
#     Hash the result again to produce the final HMAC.
#
import hashlib, hmac, binascii

def mac_func(key, msg):
    mac = hmac.new(key, msg, hashlib.sha256).digest()
    print("HMAC_SHA256:     ", binascii.hexlify(mac))




if __name__ == "__main__":
    key = b"12345"
    message = b"sample message"
    mac_func(key, message)


