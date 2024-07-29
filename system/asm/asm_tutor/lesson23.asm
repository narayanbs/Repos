; file open
%include "functions.asm"

global _start

section .data
filename: db  "readme.txt", 0h
contents: db  "Hello chunai", 0h

section .text
_start:

  mov rax, 85
  mov rdi, filename
  mov rsi, 0777o
  syscall

  mov rbx, rax

  mov rax,  1
  mov rdi, rbx
  mov rsi, contents
  mov rdx, 13
  syscall

  mov rax, 2
  mov rdi, filename
  mov rsi, 0 
  xor rdx, rdx
  syscall

  mov rdi, rax  ; open returns file descriptor in rax

  call iprintLF
  call quit

