package main

import (
	"bytes"
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
)

func encryptFile(key []byte, filename string, outFilename string) (string, error) {
	if len(outFilename) == 0 {
		outFilename = filename + ".enc"
	}

	inFile, err := os.ReadFile(filename)
	if err != nil {
		log.Fatal(err)
	}
	plaintext, err := io.ReadAll(bytes.NewReader(inFile))
	if err != nil {
		return "", err
	}

	of, err := os.Create(outFilename)
	if err != nil {
		return "", err
	}
	defer of.Close()

	origSize := uint64(len(plaintext))
	if err = binary.Write(of, binary.LittleEndian, origSize); err != nil {
		return "", err
	}

	if len(plaintext)%aes.BlockSize != 0 {
		bytesToPad := aes.BlockSize - (len(plaintext) % aes.BlockSize)
		padding := make([]byte, bytesToPad)
		if _, err := rand.Read(padding); err != nil {
			return "", err
		}
		plaintext = append(plaintext, padding...)
	}

	iv := make([]byte, aes.BlockSize)
	if _, err := rand.Read(iv); err != nil {
		return "", err
	}
	if _, err = of.Write(iv); err != nil {
		return "", err
	}

	ciphertext := make([]byte, len(plaintext))
	block, err := aes.NewCipher(key)
	if err != nil {
		return "", err
	}
	mode := cipher.NewCBCEncrypter(block, iv)
	mode.CryptBlocks(ciphertext, plaintext)

	if _, err := of.Write(ciphertext); err != nil {
		return "", err
	}
	return outFilename, nil
}

func decryptFile(key []byte, filename string, outFilename string) (string, error) {
	if len(outFilename) == 0 {
		outFilename = filename + ".dec"
	}

	inFile, err := os.ReadFile(filename)
	if err != nil {
		log.Fatal(err)
	}
	ciphertext, err := io.ReadAll(bytes.NewReader(inFile))
	if err != nil {
		return "", err
	}

	of, err := os.Create(outFilename)
	if err != nil {
		return "", err
	}
	defer of.Close()

	var origSize uint64
	buf := bytes.NewReader(ciphertext)
	if err = binary.Read(buf, binary.LittleEndian, &origSize); err != nil {
		return "", err
	}

	iv := make([]byte, aes.BlockSize)
	if _, err := rand.Read(iv); err != nil {
		return "", err
	}

	paddedSize := len(ciphertext) - 8 - aes.BlockSize
	if paddedSize%aes.BlockSize != 0 {
		return "", fmt.Errorf("want padded plaintext size to be aligned to block size")
	}
	plaintext := make([]byte, paddedSize)

	block, err := aes.NewCipher(key)
	if err != nil {
		return "", err
	}
	mode := cipher.NewCBCDecrypter(block, iv)
	mode.CryptBlocks(plaintext, ciphertext[8+aes.BlockSize:])

	if _, err := of.Write(plaintext[:origSize]); err != nil {
		return "", err
	}
	return outFilename, nil
}

func main() {
	encFlag := flag.Bool("e", false, "encrypt")
	decFlag := flag.Bool("d", false, "decrypt")

	flag.Parse()
	filename := flag.Arg(0)

	key := bytes.Repeat([]byte("i"), 32)
	if *encFlag {
		outFilename, err := encryptFile(key, filename, "")
		if err != nil {
			log.Fatal(err)
		}
		fmt.Println("Encrypted output file:", outFilename)
	} else if *decFlag {
		outFilename, err := decryptFile(key, filename, "")
		if err != nil {
			log.Fatal(err)
		}
		fmt.Println("Decrypted output file:", outFilename)
	} else {
		flag.Usage()
		os.Exit(1)
	}

}
