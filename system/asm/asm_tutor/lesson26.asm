; file seek 

%include "functions.asm"

global _start

section .data
filename: db  "readme.txt", 0h
contents: db  "-updated-", 0h

section .text
_start:
  mov rax, 2
  mov rdi, filename
  mov rsi, 1
  xor rdx, rdx
  syscall

  mov rdi, rax

  mov rax, 8
  mov rsi, 0
  mov rdx, 2
  syscall

  mov rax, 1
  mov rsi, contents
  mov rdx, 9
  syscall

  call quit
