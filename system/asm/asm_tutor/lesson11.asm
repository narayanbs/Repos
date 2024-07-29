; program to add two numbers
%include "functions.asm"

global _start

section .text
_start:
  mov rax, 90
  mov rbx, 9
  add rax, rbx
  mov rdi, rax
  call iprintLF
  call quit
