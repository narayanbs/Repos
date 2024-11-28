package main

import (
	"crypto/hmac"
	"crypto/sha256"
	"fmt"
)

func main() {
	// Define our key
	key := []byte("password-1234-admin")

	// Message to be signed
	message := []byte("Hello Friends")

	// Creating a signature with HMAC-SHA256 algorithm
	hash := hmac.New(sha256.New, key)
	hash.Write(message)
	signature := hash.Sum(nil)

	// Print the signature
	fmt.Printf("Signature: %x\n", signature)

	// Verification process of the signature
	hash.Reset()
	hash.Write(message)
	newSignature := hash.Sum(nil)

	// comparing the signature
	if hmac.Equal(signature, newSignature) {
		fmt.Println("signature is valid")
	} else {
		fmt.Println("signature is not valid")
	}
}
