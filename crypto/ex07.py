import hashlib

from Crypto.Cipher import AES

password = b"qwerty12345"
key = hashlib.sha256(password).digest()
IV = 16 * b"\x00"
mode = AES.MODE_CBC

encryptor = AES.new(key, mode, IV)

text = b"j" * 32 + b"i" * 64
ciphertext = encryptor.encrypt(text)

decryptor = AES.new(key, AES.MODE_CBC, IV)
plain = decryptor.decrypt(ciphertext)
print(plain)
