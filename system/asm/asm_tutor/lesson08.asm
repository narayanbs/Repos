; Program to get the input from command line

%include "functions.asm"

global _start

section .data
  msg1: db  "Please enter your name: ", 0h
  msg2: db  "Hello, ", 0h

section .bss
  sinput: resb 255

section .text
_start:
  mov rdi, msg1
  call sprintLF

  mov rax, 0
  mov rdi, 0
  mov rsi, sinput
  mov rdx, 255
  syscall

  mov rdi, msg2
  call sprint

  mov rdi, sinput
  call sprint

  call quit


