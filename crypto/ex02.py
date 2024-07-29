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
# Basically in HMAC, the hash function will be applied to the plain text message.
# But before applying, we have to compute S bits and then append it to plain text 
# and after that apply the hash function. 
# For generating those S bits we make use of a key that is shared between the sender and receiver.
# 
# Here are the steps in detail:  
# Message Pre-processing:
# The message (or data) is pre-processed to ensure compatibility with the hash function. 
# This typically involves padding and/or concatenating additional information to the message.
# Key Modification:
# The secret key is modified to create two different keys:
# Inner Key: Derived by XORing the secret key with a specific constant (usually a block of 0x36 bytes).
# Outer Key: Derived by XORing the secret key with another constant (usually a block of 0x5C bytes).
# Hash Calculation:
# Two hashes are calculated sequentially:
# First Hash: Compute the hash of the concatenation of the inner key and the pre-processed message.
# Second Hash: Compute the hash of the concatenation of the outer key and the result of the first hash.
# Output:
# The result of the second hash is the HMAC, which is a fixed-size hash value that serves as both a 
# message authentication code and a means of verifying the integrity of the message.
#
# Note: Difference between MAC and digital signatures is that assymetric keys are used
# in digital signatures.

import hashlib, hmac, binascii

def mac_func(key, msg):
    mac = hmac.new(key, msg, hashlib.sha256).digest()
    print("HMAC_SHA256:     ", binascii.hexlify(mac))




if __name__ == "__main__":
    key = b"12345"
    message = b"sample message"
    mac_func(key, message)


