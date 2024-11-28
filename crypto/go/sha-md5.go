package main

import (
	"crypto/md5"
	"crypto/sha256"
	"encoding/hex"
	"fmt"
)

func shaHashing(input string) string {
	plainText := []byte(input)
	sha256Hash := sha256.Sum256(plainText)
	return hex.EncodeToString(sha256Hash[:])
}

func mdHashing(input string) string {
	byteInput := []byte(input)
	md5Hash := md5.Sum(byteInput)
	return hex.EncodeToString(md5Hash[:])
}

func main() {
	fmt.Println(mdHashing("abcdefghijklmnopqrstuvwxyz"))
	fmt.Println(mdHashing("abcdefghijklmnopqrstuvwxyz"))

	fmt.Println(shaHashing("abcdefghijklmnopqrstuvwxyz"))
	fmt.Println(shaHashing("abcdefghijklmnopqrstuvwxyz"))
}
