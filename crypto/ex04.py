# Secure Random Number Generators
# PRNG and CSPRNG
# In cryptography the randomness (entropy) plays very important role. 
# In many algorithms, we need random (i.e. unpredictable) numbers. 
# If these numbers are not unpredictable, the algorithms will be compromised. 
#
# To be secure, a PRNG (which is statistically random) should start by a 
# truly random initial seed, which is absolutely unpredictable. If the seed is predictable, 
# it will generate predictable sequence of random numbers and the entire random generation
# process will be insecure. That's why having unpredictable randomness at the start 
# (secure seed) is very important.
#
# use os.urandom or secrets library in python
#
import secrets
import os, binascii

if __name__ == "__main__":
    print(secrets.randbelow(int(1e50)))

    # 128 bit, 192 bit and 256 bit random
    key_128 = os.urandom(16)
    key_192 = os.urandom(24)
    key_256 = os.urandom(32)

    print(binascii.hexlify(key_128))
    print(binascii.hexlify(key_192))
    print(binascii.hexlify(key_256))
