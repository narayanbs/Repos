package main

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"fmt"
	"io"
	"log"
)

func main() {
	key := make([]byte, 32)
	if _, err := rand.Read(key); err != nil {
		log.Fatal(err)
	}

	block, err := aes.NewCipher(key)
	if err != nil {
		log.Fatal(err)
	}

	plaintext := []byte("Hello, friends")

	iv := make([]byte, aes.BlockSize)
	if _, err := io.ReadFull(rand.Reader, iv); err != nil {
		log.Fatal(err)
	}

	ciphertext := make([]byte, aes.BlockSize+len(plaintext))
	stream := cipher.NewCFBEncrypter(block, iv)
	stream.XORKeyStream(ciphertext[aes.BlockSize:], plaintext)

	fmt.Printf("Encrypted: %x\n", ciphertext)

	decrypted := make([]byte, len(plaintext))
	stream = cipher.NewCFBDecrypter(block, iv)
	stream.XORKeyStream(decrypted, ciphertext[aes.BlockSize:])

	fmt.Printf("Decrypted: %s\n", decrypted)
}
