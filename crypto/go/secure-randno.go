/*
 * crypto/rand
 */
package main

import (
	"crypto/rand"
	"fmt"
)

func generateCryptoRandom(length int) []byte {
	key := make([]byte, length)
	_, err := rand.Read(key)
	if err != nil {
		panic(err)
	}
	return key
}

func generateRandomChars(input string, length int32) string {
	bytes := make([]byte, length)
	rand.Read(bytes)

	for index, element := range bytes {
		randomize := element % byte(len(input))
		bytes[index] = input[randomize]
	}
	return string(bytes)
}

func main() {
	fmt.Printf("key: %x\n", generateCryptoRandom(32))
	fmt.Printf("key: %x\n", generateCryptoRandom(64))
}
