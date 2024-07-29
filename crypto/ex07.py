# RSA 
#
# pip install pycryptodome
#


# generate the RSA keys (1024-bit) and print them on the console (as hex numbers and in the PKCS#8 PEM ASN.1 format):
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP
import binascii

keyPair = RSA.generate(3072)

pubKey = keyPair.publickey()
print(f"Public key:  (n={hex(pubKey.n)}, e={hex(pubKey.e)})")
pubKeyPEM = pubKey.exportKey()
print(pubKeyPEM.decode('ascii'))

print(f"Private key: (n={hex(pubKey.n)}, d={hex(keyPair.d)})")
privKeyPEM = keyPair.exportKey()
print(privKeyPEM.decode('ascii'))

# RSA Encryption
# Next, encrypt the message using RSA-OAEP encryption scheme (RSA with PKCS#1 OAEP padding) with the RSA public key:
msg = b'A message for encryption'
encryptor = PKCS1_OAEP.new(pubKey)
encrypted = encryptor.encrypt(msg)
print("Encrypted:", binascii.hexlify(encrypted))

# RSA Decryption
# decrypt the message using using RSA-OAEP with the RSA private key:
decryptor = PKCS1_OAEP.new(keyPair)
decrypted = decryptor.decrypt(encrypted)
print('Decrypted:', decrypted)



