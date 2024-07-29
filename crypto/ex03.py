# key derivation functions
# Passwords are easier to remember than binary keys.
# But when a certain algorithm needs a key (e.g for encryption or digtal signing)
# a key derivation function is needed. 
# Hash functions like SHA-256 are insecure for key derivations. It is vulnerable to
# many attacks like brute-force, dictionary attcks, rainbow attacks etc.
#
# PBKDF2, Bcrypt, Scrypt and Argon2 are significantly stronger key derivation functions 
#
# PBKDF2 - pip install backports.pbkdf2
# PBKDF2 is considered old-fashioned and less secure than modern KDF functions, 
# so it is recommended to use Bcrypt, Scrypt or Argon2
#
# scryt - pip install scrypt
# 
# argon2 - pip install argo2_cffi
#
import binascii
from backports.pbkdf2 import pbkdf2_hmac
import pyscrypt, argon2

def pbkdf2_hmac_kdf(passwd, salt, num_iterations, key_length):
    key = pbkdf2_hmac("sha256", passwd, salt, num_iterations, key_length)
    print("pbkdf2_hmac_Derived key:     ", binascii.hexlify(key))


def scrypt_kdf(passwd, salt, num_iterations, blk_size, parallelism_factor, key_length):
    key = pyscrypt.hash(passwd, salt, num_iterations, blk_size, parallelism_factor, key_length)
    print("Scrypt_Derived key:     ", binascii.hexlify(key))

def argon2_kdf():
    hash = argon2.hash_password_raw(
        time_cost=16, memory_cost=2**15, parallelism=2, hash_len=32,
        password=b'password', salt=b'some salt', type=argon2.low_level.Type.ID)
    print("Argon2 raw hash:", binascii.hexlify(hash))

    argon2Hasher = argon2.PasswordHasher(
        time_cost=16, memory_cost=2**15, parallelism=2, hash_len=32, salt_len=16)
    hash = argon2Hasher.hash("password")
    print("Argon2 hash (random salt):", hash)

    verifyValid = argon2Hasher.verify(hash, "password")
    print("Argon2 verify (correct password):", verifyValid)

    try:
        argon2Hasher.verify(hash, "wrong123")
    except:
        print("Argon2 verify (incorrect password):", False)


if __name__ == "__main__":
    salt = binascii.unhexlify('aaef2d3f4d77ac66e9c5a6c3d8f921d1')
    passwd = "p@$Sw0rD~1".encode("utf8")
    pbkdf2_hmac_kdf(passwd, salt, 50000, 32)
    scrypt_kdf(passwd, salt, 2048, 8, 1, 32)
    argon2_kdf()



