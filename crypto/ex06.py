# AES symmetric encryption 
#
# Libraries that support this in python
# include pyaes, pycryptodome and scrypt
# Read documentation of pyaes, pycryptodome for
# more examples
#
# AES-CTR
# pip install pyaes
# pip install pbkdf2
#
# AES-256-GCM - pip install pycryptodome
# AES-256-GCM-Scrypt -- pip install scrypt
# 

import pyaes, pbkdf2, binascii, os, secrets, scrypt
from Crypto.Cipher import AES

def encrypt_AES_GCM(msg, secretKey):
    aesCipher = AES.new(secretKey, AES.MODE_GCM)
    ciphertext, authTag = aesCipher.encrypt_and_digest(msg)
    return (ciphertext, aesCipher.nonce, authTag)

def decrypt_AES_GCM(encryptedMsg, secretKey):
    (ciphertext, nonce, authTag) = encryptedMsg
    aesCipher = AES.new(secretKey, AES.MODE_GCM, nonce)
    plaintext = aesCipher.decrypt_and_verify(ciphertext, authTag)
    return plaintext

def encrypt_AES_GCM_Scrypt(msg, password):
    kdfSalt = os.urandom(16)
    secretKey = scrypt.hash(password, kdfSalt, N=16384, r=8, p=1, buflen=32)
    aesCipher = AES.new(secretKey, AES.MODE_GCM)
    ciphertext, authTag = aesCipher.encrypt_and_digest(msg)
    return (kdfSalt, ciphertext, aesCipher.nonce, authTag)

def decrypt_AES_GCM_Scrypt(encryptedMsg, password):
    (kdfSalt, ciphertext, nonce, authTag) = encryptedMsg
    secretKey = scrypt.hash(password, kdfSalt, N=16384, r=8, p=1, buflen=32)
    aesCipher = AES.new(secretKey, AES.MODE_GCM, nonce)
    plaintext = aesCipher.decrypt_and_verify(ciphertext, authTag)
    return plaintext


if __name__ == "__main__":
    # first lets get the symmetric key
    password = "s3cr3t*c0d3"
    passwordSalt = os.urandom(16)
    key = pbkdf2.PBKDF2(password, passwordSalt).read(32)
    print('AES encryption key:', binascii.hexlify(key))

    # Next, generate a random 256-bit initial vector (IV) for 
    # the AES CTR block mode and perform the AES-256-CTR encryption:
    # Encrypt the plaintext with the given key:
    # ciphertext = AES-256-CTR-Encrypt(plaintext, key, iv)
    print("AES-CTR ====================")
    iv = secrets.randbits(256)
    plaintext = "Text for encryption"
    print("Plain text: ", plaintext)
    aes = pyaes.AESModeOfOperationCTR(key, pyaes.Counter(iv))
    ciphertext = aes.encrypt(plaintext)
    print("Encrypted: ", binascii.hexlify(ciphertext))

    # Decryption
    # Decrypt the ciphertext with the given key:
    # plaintext = AES-256-CTR-Decrypt(ciphertext, key, iv)
    aes = pyaes.AESModeOfOperationCTR(key, pyaes.Counter(iv))
    decrypted = aes.decrypt(ciphertext)
    print('Decrypted:', decrypted)

    ### AES-256-GCM

    print("AES-256-GCM ========================")

    secretKey = os.urandom(32)
    print("Encryption Key: ", binascii.hexlify(secretKey))

    msg = b'Message for AES-256-GCM + Scrypt Encryption';
    encryptedMsg = encrypt_AES_GCM(msg, secretKey)
    print("encryptedMsg", {
    'ciphertext': binascii.hexlify(encryptedMsg[0]),
    'aesIV': binascii.hexlify(encryptedMsg[1]),
    'authTag': binascii.hexlify(encryptedMsg[2])
    })

    decryptedMsg = decrypt_AES_GCM(encryptedMsg, secretKey)
    print("decryptedMsg", decryptedMsg)

    ### AES-256-GCM-Scrypt
    print("AES-256-GCM-Scrypt ========================")
    msg = b'Message for AES-256-GCM + Scrypt encryption'
    password = b's3kr3tp4ssw0rd'
    encryptedMsg = encrypt_AES_GCM_Scrypt(msg, password)
    print("encryptedMsg", {
        'kdfSalt': binascii.hexlify(encryptedMsg[0]),
        'ciphertext': binascii.hexlify(encryptedMsg[1]),
        'aesIV': binascii.hexlify(encryptedMsg[2]),
        'authTag': binascii.hexlify(encryptedMsg[3])
    })

    decryptedMsg = decrypt_AES_GCM_Scrypt(encryptedMsg, password)
    print("decryptedMsg", decryptedMsg)





