; program to multiply two numbers

%include "functions.asm"

global _start

section .text
_start:
  mov rax, 99
  mov rbx, 9
  mul rbx
  mov rdi, rax
  call iprintLF

  call quit
